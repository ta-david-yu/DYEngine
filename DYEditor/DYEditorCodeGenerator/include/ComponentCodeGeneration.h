#pragma once

#include <fmt/core.h>

#include <regex>

struct ComponentDescriptor
{
	std::string LocatedHeaderFile;
	std::string CustomName;
	std::string FullType;
	std::vector<PropertyDescriptor> Properties;
};

char const *ComponentTypeRegistrationCallSourceStart =
R"(		TypeRegistry::RegisterComponentType<${COMPONENT_FULL_TYPE}>
			(
				"${COMPONENT_NAME}",
				ComponentTypeDescriptor
					{
)";

char const* SerializeLambdaSourceStart =
R"(						.Serialize = [](Entity& entity, SerializedComponent& serializedComponent)
						{
)";

char const* SerializeGetComponent =
R"(							auto const& component = entity.GetComponent<${COMPONENT_FULL_TYPE}>();
)";
char const* SerializeGetEmptyComponent =
R"(
)";

char const* SerializeLambdaSourceEnd =
R"(							return SerializationResult {};
						},
)";

char const* DeserializeLambdaSourceStart =
R"(						.Deserialize = [](SerializedComponent& serializedComponent, DYE::DYEditor::Entity& entity)
						{
)";

char const* DeserializeAddOrGetComponent =
R"(							auto& component = entity.AddOrGetComponent<${COMPONENT_FULL_TYPE}>();
)";
char const* DeserializeAddOrGetEmptyComponent =
R"(							entity.AddOrGetComponent<${COMPONENT_FULL_TYPE}>();
)";

char const* DeserializeLambdaSourceEnd =
R"(							return DeserializationResult {};
						},
)";

char const* DrawInspectorLambdaSourceStart =
R"(						.DrawInspector = [](DrawComponentInspectorContext &drawInspectorContext, Entity &entity)
						{
							bool changed = false;
)";

char const* DrawInspectorGetComponent =
R"(							auto& component = entity.GetComponent<${COMPONENT_FULL_TYPE}>();
)";
char const* DrawInspectorGetEmptyComponent =
R"(							ImGui::Indent();
							ImGui::TextUnformatted("The component doesn't have any properties (i.e. DYE_PROPERTY).");
							ImGui::Unindent();
)";

char const* ComponentTypeRegistrationCallSourceEnd =
R"(							return changed;
						}
					}
			);

)";

std::string ComponentDescriptorToTypeRegistrationCallSource(ComponentDescriptor const& descriptor)
{
	auto const& componentName = descriptor.CustomName;
	auto const& componentFullType = descriptor.FullType;

	std::string result = "\t\t// Component located in " + descriptor.LocatedHeaderFile + "\n";
	result.append(ComponentTypeRegistrationCallSourceStart);
	result.append(SerializeLambdaSourceStart);
	result.append(!descriptor.Properties.empty()? SerializeGetComponent : SerializeGetEmptyComponent);
	for (auto const& propertyDescriptor : descriptor.Properties)
	{
		result.append(PropertyDescriptorToSerializeCallSource(descriptor.FullType, propertyDescriptor));
	}
	result.append(SerializeLambdaSourceEnd);
	result.append(DeserializeLambdaSourceStart);
	result.append(!descriptor.Properties.empty()? DeserializeAddOrGetComponent : DeserializeAddOrGetEmptyComponent);
	for (auto const& propertyDescriptor : descriptor.Properties)
	{
		result.append(PropertyDescriptorToDeserializeCallSource(descriptor.FullType, propertyDescriptor));
	}
	result.append(DeserializeLambdaSourceEnd);
	result.append(DrawInspectorLambdaSourceStart);
	result.append(!descriptor.Properties.empty()? DrawInspectorGetComponent : DrawInspectorGetEmptyComponent);
	for (auto const& propertyDescriptor : descriptor.Properties)
	{
		result.append(PropertyDescriptorToImGuiUtilControlCallSource(descriptor.FullType, propertyDescriptor));
	}
	result.append(ComponentTypeRegistrationCallSourceEnd);

	std::regex const componentNameKeywordPattern(R"(\$\{COMPONENT_NAME\})");
	std::regex const componentFullTypeKeywordPattern(R"(\$\{COMPONENT_FULL_TYPE\})");

	result = std::regex_replace(result, componentNameKeywordPattern, componentName);
	result = std::regex_replace(result, componentFullTypeKeywordPattern, componentFullType);

	return result;
}