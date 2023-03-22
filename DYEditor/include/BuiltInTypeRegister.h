#pragma once

namespace DYE::DYEntity
{
	class Entity;
}

namespace DYE::DYEditor
{
	struct SerializedEntity;
	struct SerializedComponentHandle;

	void RegisterBuiltInTypes();

	// Hide built-in component functions in a nameless namespace
	namespace
	{
		void AddNameComponent(DYE::DYEntity::Entity& entity);
		bool DrawInspectorOfNameComponent(DYE::DYEntity::Entity& entity);
		void SerializeNameComponent(DYE::DYEntity::Entity& entity, SerializedEntity& serializedEntity);
		void DeserializeNameComponent(SerializedComponentHandle& serializedComponent, DYE::DYEntity::Entity& entity);
		bool DrawInspectorOfTransformComponent(DYE::DYEntity::Entity& entity);
	}
}