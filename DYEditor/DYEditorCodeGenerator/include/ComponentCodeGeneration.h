#pragma once

#include <fmt/core.h>

#include <regex>

struct ComponentDescriptor
{
	std::string LocatedHeaderFile;
	std::string FullType;
	bool HasOptionalDisplayName;
	std::string OptionalDisplayName;
	std::vector<PropertyDescriptor> Properties;
	std::vector<std::string> FormerlyKnownNames;
	std::vector<std::string> UseWithComponentTypeHints;
};

char const *ComponentTypeRegistrationCallSourceStart =
R"(		TypeRegistry::RegisterComponentType<${COMPONENT_FULL_TYPE}>
			(
				NAME_OF(${COMPONENT_FULL_TYPE}),
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
						},
						.GetDisplayName = []() { return "${COMPONENT_DISPLAY_NAME}"; },
					}
			);
)";

char const* FormerlyKnownComponentTypeNameRegistrationCallSource =
R"(		TypeRegistry::RegisterFormerlyKnownTypeName("${FORMERLY_KNOWN_TYPE_NAME}", NAME_OF(${COMPONENT_FULL_TYPE}));
)";

std::string ComponentDescriptorToTypeRegistrationCallSource(ComponentDescriptor const& descriptor)
{
	auto const& componentDisplayName = descriptor.HasOptionalDisplayName? descriptor.OptionalDisplayName : descriptor.FullType;
	auto const& componentFullType = descriptor.FullType;

	std::string result = "\n\t\t// Component located in " + descriptor.LocatedHeaderFile + "\n";
	result.append(ComponentTypeRegistrationCallSourceStart);
	{
		result.append(SerializeLambdaSourceStart);
		{
			result.append(!descriptor.Properties.empty() ? SerializeGetComponent : SerializeGetEmptyComponent);
			for (auto const &propertyDescriptor: descriptor.Properties)
			{
				result.append(PropertyDescriptorToSerializeCallSource(descriptor.FullType, propertyDescriptor));
			}
		}
		result.append(SerializeLambdaSourceEnd);

		result.append(DeserializeLambdaSourceStart);
		{
			result.append(
				!descriptor.Properties.empty() ? DeserializeAddOrGetComponent : DeserializeAddOrGetEmptyComponent);
			for (auto const &propertyDescriptor: descriptor.Properties)
			{
				result.append(PropertyDescriptorToDeserializeCallSource(descriptor.FullType, propertyDescriptor));
			}
		}
		result.append(DeserializeLambdaSourceEnd);

		result.append(DrawInspectorLambdaSourceStart);
		{
			result.append(!descriptor.Properties.empty() ? DrawInspectorGetComponent : DrawInspectorGetEmptyComponent);
			for (auto const &propertyDescriptor: descriptor.Properties)
			{
				result.append(PropertyDescriptorToImGuiUtilControlCallSource(descriptor.FullType, propertyDescriptor));
			}
		}
	}
	result.append(ComponentTypeRegistrationCallSourceEnd);

	for (std::string const &formerlyKnownName : descriptor.FormerlyKnownNames)
	{
		std::regex const formerlyKnownNameKeywordPattern(R"(\$\{FORMERLY_KNOWN_TYPE_NAME\})");
		auto const &registerFormerlyKnownNameCall = std::regex_replace(FormerlyKnownComponentTypeNameRegistrationCallSource, formerlyKnownNameKeywordPattern, formerlyKnownName);
		result.append(registerFormerlyKnownNameCall);
	}

	std::regex const componentDisplayNameKeywordPattern(R"(\$\{COMPONENT_DISPLAY_NAME\})");
	std::regex const componentFullTypeKeywordPattern(R"(\$\{COMPONENT_FULL_TYPE\})");

	result = std::regex_replace(result, componentDisplayNameKeywordPattern, componentDisplayName);
	result = std::regex_replace(result, componentFullTypeKeywordPattern, componentFullType);

	return result;
}