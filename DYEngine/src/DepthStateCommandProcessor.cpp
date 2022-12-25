#include "Graphics/Processor/DepthStateCommandProcessor.h"

namespace DYE::ShaderProcessor
{
	void DepthStateCommandProcessor::OnBegin(ShaderProgram &shaderProgram)
	{

	}

	void DepthStateCommandProcessor::OnPreShaderTypeParse(std::string &programSource)
	{
		m_ShaderProgramSourceCache = programSource;
	}

	void DepthStateCommandProcessor::OnEnd(ShaderProgram &shaderProgram)
	{
		// Parse property information from cached shader program source.
		std::stringstream stream(m_ShaderProgramSourceCache);

		RenderState renderState = shaderProgram.GetDefaultRenderState();

		std::string line;
		while (std::getline(stream, line))
		{
			auto firstToken = getFirstTokenFromLine(line);
			if (firstToken == ZTestCommandDirective)
			{
				renderState.DepthState.IsEnabled = true;
				renderState.DepthState.CompareFunction = parseZTestLine(line);
				continue;
			}

			if (firstToken == ZWriteCommandDirective)
			{
				renderState.DepthState.IsEnabled = true;
				renderState.DepthState.IsWriteEnabled = parseZWriteLine(line);
				continue;
			}
		}

		shaderProgram.setDefaultRenderState(renderState);
	}

	CompareFunction DepthStateCommandProcessor::parseZTestLine(const std::string &line)
	{
		std::vector<std::string> const tokens = splitLineIntoTokensBySpace(line);
		if (tokens.size() < 2)
		{
			// There should be at least 2 tokens!
			DYE_LOG("ZTest directive '%s' is presented but no parameters follow. ZTest::Always is returned instead", line.c_str());
			return CompareFunction::Always;
		}

		auto compareFunction = StringToCompareFunction(tokens[1]);
		if (!compareFunction.has_value())
		{
			DYE_LOG("ZTest directive '%s' is presented but with invalid parameter '%s'. ZTest::Always is returned instead", line.c_str(), tokens[1].c_str());
			return CompareFunction::Always;
		}

		return compareFunction.value();
	}

	bool DepthStateCommandProcessor::parseZWriteLine(const std::string &line)
	{
		std::vector<std::string> const tokens = splitLineIntoTokensBySpace(line);
		if (tokens.size() < 2)
		{
			// There should be at least 2 tokens!
			DYE_LOG("ZWrite directive '%s' is presented but no parameter follows. ZWrite is enabled by default", line.c_str());
			return true;
		}

		if (tokens[1] == "Off" || tokens[1] == "off")
		{
			return false;
		}

		if (tokens[1] == "On" || tokens[1] == "on")
		{
			return true;
		}

		DYE_LOG("ZWrite directive '%s' is presented but with invalid parameter. ZWrite is enabled by default", line.c_str());
		return true;
	}
}
