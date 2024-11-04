#include "Serialization/SerializedObjectFactory.h"

#include "Type/TypeRegistry.h"
#include "Core/Entity.h"
#include "Core/Scene.h"
#include "FileSystem/FileSystem.h"

#include <unordered_set>
#include <fstream>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
    std::optional<SerializedScene> SerializedObjectFactory::TryLoadSerializedSceneFromFile(const std::filesystem::path &path)
    {
        if (!FileSystem::FileExists(path))
        {
            DYE_LOG("Cannot find the scene file: %s", path.string().c_str());
        }

        auto result = toml::parse_file(path.string());
        if (!result)
        {
            return {};
        }

        return SerializedScene(std::move(result.table()));
    }

    std::optional<SerializedEntity> SerializedObjectFactory::TryLoadSerializedEntityFromFile(const std::filesystem::path &path)
    {
        auto result = toml::parse_file(path.string());
        if (!result)
        {
            return {};
        }

        return SerializedEntity(std::move(result.table()));
    }

    void SerializedObjectFactory::ApplySerializedSceneToEmptyScene(SerializedScene &serializedScene, Scene &scene)
    {
#if DYE_DEBUG
        bool const isEmptyScene = scene.World.IsEmpty() && scene.InitializeSystemDescriptors.empty();
        DYE_ASSERT(isEmptyScene && "The given scene is not empty!");
#endif

        auto serializedSystemHandles = serializedScene.GetSerializedSystemHandles();
        auto serializedEntityHandles = serializedScene.GetSerializedEntityHandles();

        auto tryGetSceneNameResult = serializedScene.TryGetName();
        if (tryGetSceneNameResult.has_value())
        {
            scene.Name = tryGetSceneNameResult.value();
        }

        // Populate systems.
        for (auto &serializedSystemHandle: serializedSystemHandles)
        {
            auto getTypeNameResult = serializedSystemHandle.TryGetTypeName();
            if (!getTypeNameResult.has_value())
            {
                continue;
            }

            auto getGroupResult = serializedSystemHandle.TryGetGroupName();
            bool const hasGroup = getGroupResult.has_value();
            SystemDescriptor systemDescriptor =
                {
                    .Name = getTypeNameResult.value(),
                    .Group = hasGroup ? scene.AddOrGetGroupID(getGroupResult.value()) : NoSystemGroupID,
                    .IsEnabled = serializedSystemHandle.GetIsEnabledOr(true)
                };

            auto tryGetSystemInstance = TypeRegistry::TryGetSystemInstance(getTypeNameResult.value());
            if (tryGetSystemInstance.Success)
            {
                SystemBase *pSystemInstance = tryGetSystemInstance.pInstance;
                systemDescriptor.Name = tryGetSystemInstance.FullTypeName;
                systemDescriptor.Instance = pSystemInstance;

                auto const phase = pSystemInstance->GetPhase();
                scene.GetSystemDescriptorsOfPhase(phase).emplace_back(systemDescriptor);
            }
            else
            {
                // Unrecognized system in TypeRegistry.
                scene.UnrecognizedSystems.emplace_back(systemDescriptor);
            }
        }

        // Populate entities.
        scene.World.Reserve(serializedEntityHandles.size());
        for (int i = 0; i < serializedEntityHandles.size(); i++)
        {
            auto &serializedEntityHandle = serializedEntityHandles[i];

            DYEditor::Entity entity = scene.World.createUntrackedEntity();
            auto result = ApplySerializedEntityToEmptyEntity(serializedEntityHandle, entity);

            if (!result.Success)
            {
                // If the deserialized entity has some issue during deserialization, add the result as component to the entity.
                entity.AddComponent<EntityDeserializationResult>(result);
            }

            auto tryGetGUID = entity.TryGetGUID();
            if (tryGetGUID.has_value())
            {
                scene.World.registerUntrackedEntityAtIndex(entity, i);
            }
            else
            {
                DYE_LOG("The entity at index %d doesn't have a GUID (IDComponent), it will not be tracked by the World.", i);
            }
        }

        scene.World.refreshAllHierarchyComponentEntityCache();
    }

    EntityDeserializationResult SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(SerializedEntity &serializedEntity,
                                                                                            DYEditor::Entity &entity)
    {
        EntityDeserializationResult result;

        std::vector<SerializedComponent> serializedComponentHandles = serializedEntity.GetSerializedComponentHandles();

#ifdef DYE_EDITOR
        std::vector<std::string> successfullyDeserializedComponentNames;
        successfullyDeserializedComponentNames.reserve(serializedComponentHandles.size());
#endif

        for (auto &serializedComponentHandle: serializedComponentHandles)
        {
            auto getTypeNameResult = serializedComponentHandle.TryGetTypeName();
            if (!getTypeNameResult.has_value())
            {
                // Garbage component element without a type name, skip it.
                continue;
            }

            auto &serializedTypeName = getTypeNameResult.value();
            auto getComponentTypeFunctionsResult = TypeRegistry::TryGetComponentTypeDescriptor(serializedTypeName);
            if (!getComponentTypeFunctionsResult.Success)
            {
                // Cannot find the given component type and its related functions,
                // add the component name to the unrecognized component list.
                DYE_LOG("Entity has an unrecognized component of type '%s'.", serializedTypeName.c_str());
                result.Success = false;
                result.UnrecognizedComponentTypeNames.push_back(serializedTypeName);
                result.UnrecognizedSerializedComponents.push_back(serializedComponentHandle.CloneAsNonHandle());
                continue;
            }

            char const *realFullTypeName = getComponentTypeFunctionsResult.FullTypeName;
            auto &componentTypeFunctions = getComponentTypeFunctionsResult.Descriptor;
            if (componentTypeFunctions.Deserialize == nullptr)
            {
                // The component type doesn't have a corresponding 'Deserialize' function.
                DYE_LOG("Component of type '%s' will not be deserialized because its Deserialize function is not provided.", realFullTypeName);
                continue;
            }

            DeserializationResult deserializeComponentResult;
#if defined(__EXCEPTIONS)
            try
            {
                deserializeComponentResult = componentTypeFunctions.Deserialize(serializedComponentHandle, entity);
            }
            catch (std::exception& exception)
            {
                DYE_LOG_ERROR(exception.what());
                DYE_ASSERT(false);
            }
#else
            deserializeComponentResult = componentTypeFunctions.Deserialize(serializedComponentHandle, entity);
#endif

#ifdef DYE_EDITOR
            if (deserializeComponentResult.Success)
            {
                successfullyDeserializedComponentNames.push_back(realFullTypeName);
            }
#endif
        }

#ifdef DYE_EDITOR
        entity.AddComponent<EntityEditorOnlyMetadata>().SuccessfullyDeserializedComponentNames = std::move(successfullyDeserializedComponentNames);
#endif

        return std::move(result);
    }

    SerializedScene SerializedObjectFactory::CreateSerializedScene(Scene &scene)
    {
        SerializedScene serializedScene;
        serializedScene.SetName(scene.Name);

        // Populate systems.
        scene.ForEachSystemDescriptor
            (
                [&serializedScene, &scene](SystemDescriptor const &systemDescriptor, ExecutionPhase phase)
                {
                    serializedScene.AddOrReplaceSystemHandle
                        (
                            SerializedScene::AddSystemParameters
                                {
                                    .SystemTypeName = systemDescriptor.Name,
                                    .HasGroup = systemDescriptor.Group != NoSystemGroupID,
                                    .SystemGroupName = (systemDescriptor.Group != NoSystemGroupID)
                                                       ? scene.SystemGroupNames[systemDescriptor.Group] : "",
                                    .IsEnabled = systemDescriptor.IsEnabled
                                }
                        );
                }
            );

        // Populate unrecognized/unknown systems.
        for (auto unrecognizedSystemDescriptor: scene.UnrecognizedSystems)
        {
            serializedScene.AddOrReplaceSystemHandle
                (
                    SerializedScene::AddSystemParameters
                        {
                            .SystemTypeName = unrecognizedSystemDescriptor.Name,
                            .HasGroup = unrecognizedSystemDescriptor.Group != NoSystemGroupID,
                            .SystemGroupName = (unrecognizedSystemDescriptor.Group != NoSystemGroupID)
                                               ? scene.SystemGroupNames[unrecognizedSystemDescriptor.Group] : "",
                            .IsEnabled = unrecognizedSystemDescriptor.IsEnabled
                        }
                );
        }

        // Populate entities and their components.
        scene.World.ForEachEntity
            (
                [&serializedScene](auto &entity)
                {
                    SerializedEntity serializedEntity = serializedScene.CreateAndAddEntityHandle();

                    std::unordered_set<std::string> serializedComponentTypeNames;

#ifdef DYE_EDITOR
                    // In editor build, we try to serialize entity's components in custom order first if the metadata is provided.

                    auto tryGetEntityMetadata = entity.template TryGetComponent<EntityEditorOnlyMetadata>();
                    DYE_ASSERT_LOG_WARN(tryGetEntityMetadata.has_value(),
                                        "In editor build, an entity should always have 'EntityEditorOnlyMetadata' component.");

                    auto &successfullyDeserializedComponentNames = tryGetEntityMetadata.value().get().SuccessfullyDeserializedComponentNames;
                    serializedComponentTypeNames.reserve(successfullyDeserializedComponentNames.size());

                    for (auto &deserializedTypeName: successfullyDeserializedComponentNames)
                    {
                        auto tryGetTypeDescriptor = TypeRegistry::TryGetComponentTypeDescriptor(deserializedTypeName);
                        DYE_ASSERT_LOG_WARN(tryGetTypeDescriptor.Success,
                                            "The component '%s' was successfully deserialized according to the metadata, but the type descriptor cannot be found in the TypeRegistry anymore.",
                                            deserializedTypeName.c_str());

                        char const *realFullTypeName = tryGetTypeDescriptor.FullTypeName;

                        auto typeDescriptor = tryGetTypeDescriptor.Descriptor;
                        if (!typeDescriptor.Has(entity))
                        {
                            DYE_LOG(
                                "The component '%s' is listed in the deserialized component names list, but the entity instance doesn't has the component (or component formerly known as the name).",
                                deserializedTypeName.c_str());
                            continue;
                        }

                        SerializedComponent serializedComponent = serializedEntity.AddOrGetComponentHandleOfType(realFullTypeName);
                        serializedComponentTypeNames.emplace(realFullTypeName);

                        DYE_ASSERT_LOG_WARN(typeDescriptor.Serialize != nullptr,
                                            "The component '%s' doesn't have a 'Serialize' function.",
                                            deserializedTypeName.c_str());

                        SerializationResult const result = typeDescriptor.Serialize(entity, serializedComponent);
                    }
#endif
                    // Even in editor build, we want to go through all the registered component types,
                    // just in case component types are not properly recorded in the metadata.

                    auto componentNamesAndTypeDescriptors = TypeRegistry::GetComponentTypesNamesAndDescriptors();
                    for (auto &[name, typeDescriptor]: componentNamesAndTypeDescriptors)
                    {
                        if (serializedComponentTypeNames.contains(name))
                        {
                            // The component of the given type has already been serialized.
                            // Skip it.
                            continue;
                        }

                        if (!typeDescriptor.Has(entity))
                        {
                            continue;
                        }

                        SerializedComponent serializedComponent = serializedEntity.AddOrGetComponentHandleOfType(name);
                        DYE_ASSERT_LOG_WARN(typeDescriptor.Serialize != nullptr,
                                            "The component '%s' doesn't have a 'Serialize' function.",
                                            name.c_str());

                        SerializationResult const result = typeDescriptor.Serialize(entity, serializedComponent);
                    }

                    if (entity.template HasComponent<EntityDeserializationResult>())
                    {
                        // Emplace serialized unrecognized component data back.
                        auto &deserializationResult = entity.template GetComponent<EntityDeserializationResult>();
                        for (auto &serializedComponent: deserializationResult.UnrecognizedSerializedComponents)
                        {
                            serializedEntity.PushSerializedComponent(serializedComponent);
                        }
                    }
                }
            );

        return serializedScene;
    }

    SerializedEntity SerializedObjectFactory::CreateSerializedEntity(DYE::DYEditor::Entity &entity)
    {
        SerializedEntity serializedEntity;

        std::unordered_set<std::string> serializedComponentTypeNames;

#ifdef DYE_EDITOR
        // In editor build, we try to serialize entity's components in custom order first if the metadata is provided.

        auto tryGetEntityMetadata = entity.TryGetComponent<EntityEditorOnlyMetadata>();
        DYE_ASSERT_LOG_WARN(tryGetEntityMetadata.has_value(),
                            "In editor build, an entity should always have 'EntityEditorOnlyMetadata' component.");

        auto &successfullyDeserializedComponentNames = tryGetEntityMetadata.value().get().SuccessfullyDeserializedComponentNames;
        serializedComponentTypeNames.reserve(successfullyDeserializedComponentNames.size());

        for (auto &deserializedTypeName: successfullyDeserializedComponentNames)
        {
            auto tryGetTypeDescriptor = TypeRegistry::TryGetComponentTypeDescriptor(deserializedTypeName);
            DYE_ASSERT_LOG_WARN(tryGetTypeDescriptor.Success,
                                "The component '%s' was successfully deserialized according to the metadata, but the type descriptor cannot be found in the TypeRegistry anymore.",
                                deserializedTypeName.c_str());

            char const *realFullTypeName = tryGetTypeDescriptor.FullTypeName;

            auto typeDescriptor = tryGetTypeDescriptor.Descriptor;
            if (!typeDescriptor.Has(entity))
            {
                DYE_LOG(
                    "The component '%s' is listed in the deserialized component names list according to the metadata, but the entity instance doesn't has the component (or component formerly known as the name).",
                    deserializedTypeName.c_str());
                continue;
            }

            SerializedComponent serializedComponent = serializedEntity.AddOrGetComponentHandleOfType(realFullTypeName);
            serializedComponentTypeNames.emplace(realFullTypeName);

            DYE_ASSERT_LOG_WARN(typeDescriptor.Serialize != nullptr,
                                "The component '%s' doesn't have a 'Serialize' function.",
                                deserializedTypeName.c_str());

            SerializationResult const result = typeDescriptor.Serialize(entity, serializedComponent);
        }
#endif
        // Even in editor build, we want to go through all the registered component types,
        // just in case component types are not properly recorded in the metadata.

        auto componentNamesAndTypeDescriptors = TypeRegistry::GetComponentTypesNamesAndDescriptors();
        for (auto &[name, typeDescriptor]: componentNamesAndTypeDescriptors)
        {
            if (serializedComponentTypeNames.contains(name))
            {
                // The component of the given type has already been serialized.
                // Skip it.
                continue;
            }

            if (!typeDescriptor.Has(entity))
            {
                continue;
            }

            SerializedComponent serializedComponent = serializedEntity.AddOrGetComponentHandleOfType(name);
            DYE_ASSERT_LOG_WARN(typeDescriptor.Serialize != nullptr,
                                "The component '%s' doesn't have a 'Serialize' function.",
                                name.c_str());

            SerializationResult const result = typeDescriptor.Serialize(entity, serializedComponent);
        }

        if (entity.HasComponent<EntityDeserializationResult>())
        {
            // Emplace serialized unrecognized component data back.
            auto &deserializationResult = entity.GetComponent<EntityDeserializationResult>();
            for (auto &serializedComponent: deserializationResult.UnrecognizedSerializedComponents)
            {
                serializedEntity.PushSerializedComponent(serializedComponent);
            }
        }

        return serializedEntity;
    }

    void SerializedObjectFactory::SaveSerializedEntityToFile(SerializedEntity &serializedEntity,
                                                             const std::filesystem::path &path)
    {
        std::ofstream fileStream(path, std::ios::trunc);
        if (serializedEntity.IsHandle())
        {
            // TODO: do some extra work to include children entities (i.e. nested prefab)
            fileStream << *serializedEntity.m_pEntityTableHandle;
        }
        else
        {
            fileStream << serializedEntity.m_EntityTable;
        }
    }

    void SerializedObjectFactory::SaveSerializedSceneToFile(SerializedScene &serializedScene,
                                                            const std::filesystem::path &path)
    {
        std::ofstream fileStream(path, std::ios::trunc);
        fileStream << serializedScene.m_SceneTable;
    }

    SerializedComponent SerializedObjectFactory::CreateSerializedComponentOfType(Entity &entity,
                                                                                 std::string const &componentTypeName,
                                                                                 ComponentTypeDescriptor componentTypeDescriptor)
    {
        // Default serialized component ctor creates an empty non-handle serialized component.
        SerializedComponent serializedComponent;

        serializedComponent.SetTypeName(componentTypeName);
        componentTypeDescriptor.Serialize(entity, serializedComponent);
        return serializedComponent;
    }

    SerializedEntity SerializedObjectFactory::CreateEmptySerializedEntity()
    {
        return {};
    }

    SerializedScene SerializedObjectFactory::CreateEmptySerializedScene()
    {
        return {};
    }

    SerializedComponent SerializedObjectFactory::CreateEmptySerializedComponent()
    {
        return {};
    }
}