#include "Graphics/Processor/BlendStateCommandProcessor.h"

namespace DYE::ShaderProcessor
{
	void BlendStateCommandProcessor::OnBegin(ShaderProgram &shaderProgram)
	{
		// Init BlendState cache.
		m_BlendStateCache = BlendState {};
	}

	void BlendStateCommandProcessor::OnPreShaderTypeParse(std::string &programSource)
	{
		// Parse property information from shader program source.
		std::stringstream stream(programSource);

		std::string line;
		while (std::getline(stream, line))
		{
			auto firstToken = getFirstTokenFromLine(line);
			if (firstToken == BlendFunctionCommandDirective)
			{
				m_BlendStateCache = parseBlendFunctionLine(line);
				continue;
			}

			if (firstToken == BlendOperationCommandDirective)
			{
				parseBlendOperationLine(line);
				continue;
			}
		}
	}

	void BlendStateCommandProcessor::OnEnd(ShaderProgram &shaderProgram)
	{
		// Set cached BlendState to Shader's render state.
		RenderState renderState = shaderProgram.GetDefaultRenderState();
		renderState.BlendState = m_BlendStateCache;
		shaderProgram.setDefaultRenderState(renderState);
	}

	BlendState BlendStateCommandProcessor::parseBlendFunctionLine(std::string const&line)
	{
		std::vector<std::string> const tokens = splitLineIntoTokensBySpace(line, 3);
		if (tokens.size() < 2)
		{
			// There should be at least 2 tokens!
			DYE_LOG("BlendFunc directive '%s' is presented but no parameters follow. BlendFunc directive is ignored and disabled.", line.c_str());
			return BlendState { .IsEnabled = false };
		}

		std::string const& srcFactorStr = tokens[1];
		std::string const dstFactorStr = tokens.size() >= 3? tokens[2] : "";
		if (srcFactorStr == "Off" || srcFactorStr == "off")
		{
			return BlendState { .IsEnabled = false };
		}

		auto srcFactor = stringToBlendFactor(srcFactorStr);
		if (!srcFactor.has_value())
		{
			DYE_LOG("Unrecognized BlendFunc Src Factor '%s'. BlendFunc is disabled.", srcFactorStr.c_str());
			return BlendState { .IsEnabled = false };
		}

		auto dstFactor = stringToBlendFactor(dstFactorStr);
		if (!dstFactor.has_value())
		{
			DYE_LOG("Unrecognized BlendFunc Dst Factor '%s'. BlendFunc is disabled.", dstFactorStr.c_str());
			return BlendState { .IsEnabled = false };
		}

		return BlendState { .IsEnabled = true, .SourceFactor = srcFactor.value(), .DestinationFactor = dstFactor.value() };
	}

	void BlendStateCommandProcessor::parseBlendOperationLine(std::string const&line)
	{
		// Do nothing now!
		// std::vector<std::string> const tokens = splitLineIntoTokensBySpace(line);
	}

	std::optional<BlendState::BlendFactor> BlendStateCommandProcessor::stringToBlendFactor(const std::string &input)
	{
		if (input == "Zero")
		{
			return BlendState::BlendFactor::Zero;
		}

		if (input == "One")
		{
			return BlendState::BlendFactor::One;
		}

		if (input == "SrcColor")
		{
			return BlendState::BlendFactor::SrcColor;
		}

		if (input == "OneMinusSrcColor")
		{
			return BlendState::BlendFactor::OneMinusSrcColor;
		}

		if (input == "SrcAlpha")
		{
			return BlendState::BlendFactor::SrcAlpha;
		}

		if (input == "OneMinusSrcAlpha")
		{
			return BlendState::BlendFactor::OneMinusSrcAlpha;
		}

		if (input == "DstAlpha")
		{
			return BlendState::BlendFactor::DstAlpha;
		}

		if (input == "OneMinusDstAlpha")
		{
			return BlendState::BlendFactor::OneMinusDstAlpha;
		}

		if (input == "DstColor")
		{
			return BlendState::BlendFactor::DstColor;
		}

		if (input == "OneMinusDstColor")
		{
			return BlendState::BlendFactor::OneMinusDstColor;
		}

		return {};
	}
}