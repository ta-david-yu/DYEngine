#include "Util/Macro.h"
#include "Graphics/Shader.h"
#include "Graphics/OpenGL.h"

#include "Graphics/ShaderProcessorBase.h"
#include "Graphics/Processor/UniformPropertyProcessor.h"
#include "Graphics/Processor/BlendStateCommandProcessor.h"
#include "Graphics/Processor/DepthStateCommandProcessor.h"

#include <utility>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <glad/glad.h>

namespace DYE
{
	namespace
	{
		static const char* s_FallbackMVPShaderName = "Fallback MVP Shader";

		static const std::string s_FallbackMVPShaderSource =
			R"(
@Blend Off Off
@ZWrite On
@ZTest Less

#Shader Vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;

uniform mat4 _ModelMatrix;
uniform mat4 _ViewMatrix;
uniform mat4 _ProjectionMatrix;

void main()
{
    vec4 point4 = vec4(position, 1.0);
    gl_Position = _ProjectionMatrix * _ViewMatrix * _ModelMatrix * point4;
};

#Shader Fragment
#version 330 core

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(1, 0, 1, 1);
};
)";

		static std::shared_ptr<ShaderProgram> s_FallbackMVPShader;
	}

    /// Map the ShaderType enum to GL_(TYPE)_SHADER
    /// \param type enum ShaderType
    /// \return unsigned int (GL Shader Type)
    static unsigned int ShaderTypeEnumToGLShaderType(ShaderType type)
    {
        switch (type)
        {
            case ShaderType::Vertex:
                return GL_VERTEX_SHADER;
            case ShaderType::Geometry:
                return GL_GEOMETRY_SHADER;
            case ShaderType::Fragment:
                return GL_FRAGMENT_SHADER;
        }
        return 0;
    }

	static std::string ShaderTypeEnumToShaderTypeKeyword(ShaderType type)
	{
		switch (type)
		{
			case ShaderType::Vertex:
				return "vertex";
			case ShaderType::Geometry:
				return "geometry";
			case ShaderType::Fragment:
				return "fragment";
		}
		return "invalid-type";
	}

	/// Check if the given line contains a valid shader type keyword. The contained shader type will be return through outShaderType parameter.
	/// \param shaderSourceLine
	/// \param outShaderType The output shader type if the return value is true.
	/// \return true if the line includes a shader type keyword.
	static bool HasValidShaderTypeKeywordInLine(const std::string& shaderSourceLine, ShaderType& outShaderType)
	{
		if (shaderSourceLine.find("Vertex") != std::string::npos)
		{
			outShaderType = ShaderType::Vertex;
			return true;
		}

		if (shaderSourceLine.find("Geometry") != std::string::npos)
		{
			outShaderType = ShaderType::Geometry;
			return true;
		}

		if (shaderSourceLine.find("Fragment") != std::string::npos)
		{
			outShaderType = ShaderType::Fragment;
			return true;
		}

		return false;
	}

    std::shared_ptr<ShaderProgram> ShaderProgram::CreateFromFile(const std::string& name, const std::filesystem::path &filepath)
	{
		DYE_LOG("<< Start creating shader \"%s\" from \"%s\" -", name.c_str(), filepath.string().c_str());

		auto program = std::make_shared<ShaderProgram>(name);

		bool success = true;

		// Populate shader processors!
		std::vector<std::unique_ptr<ShaderProcessor::ShaderProcessorBase>> shaderProcessors {};
		shaderProcessors.emplace_back(std::make_unique<ShaderProcessor::UniformPropertyProcessor>());
		shaderProcessors.emplace_back(std::make_unique<ShaderProcessor::BlendStateCommandProcessor>());
		shaderProcessors.emplace_back(std::make_unique<ShaderProcessor::DepthStateCommandProcessor>());

		if (!std::filesystem::exists(filepath))
		{
			DYE_LOG("There is no file at the given path!");
			success = false;
		}
		else
		{
			std::ifstream fs(filepath);
			std::string shaderProgramSource((std::istreambuf_iterator<char>(fs)),
											(std::istreambuf_iterator<char>()));

			success = program->initializeProgramFromSource(shaderProgramSource, shaderProcessors);
			if (program->HasCompileError())
			{
				DYE_LOG("There are compile errors in the shader!");
			}
		}

		if (!success)
		{
			DYE_LOG("- Failed to create shader [%d] \"%s\" from \"%s\", returning fallback MVP shader >>", program->m_ID, name.c_str(), filepath.string().c_str());
			DYE_ASSERT(false);

			// We will return the fallback purple MVP shader.
			if (!s_FallbackMVPShader)
			{
				DYE_LOG("<< Start creating fallback MVP shader -");
				// If the fallback mvp shader is not loaded yet, we will do that first.
				s_FallbackMVPShader = std::make_shared<ShaderProgram>(s_FallbackMVPShaderName);
				s_FallbackMVPShader->initializeProgramFromSource(s_FallbackMVPShaderSource, shaderProcessors);
			}

			return s_FallbackMVPShader;
		}

		DYE_LOG("- Successfully create shader (%d) \"%s\" from \"%s\" >>", program->m_ID, name.c_str(), filepath.string().c_str());

#ifdef DYE_DEBUG
		glObjectLabel(GL_PROGRAM, program->m_ID, -1, name.c_str());
#endif

		return program;
	}

    DYE::ShaderProgram::ShaderProgram(std::string name) : m_ID(0), m_Name(std::move(name))
    {

    }

    DYE::ShaderProgram::~ShaderProgram()
    {
        Unbind();

        DYE_LOG("Delete Program [%d] %s", m_ID, m_Name.c_str());
        glDeleteProgram(m_ID);
    }

    void ShaderProgram::Use() const
    {
        glCall(glUseProgram(m_ID));
    }

    void ShaderProgram::Unbind() const
    {
        glUseProgram(0);
    }

	std::optional<UniformInfo> ShaderProgram::TryGetUniformInfo(const std::string &name) const
	{
		for (const auto& info : m_Uniforms)
		{
			if (info.Name == name)
			{
				return info;
			}
		}
		return std::nullopt;
	}

	std::optional<UniformInfo> ShaderProgram::TryGetUniformInfoFromLocation(UniformLocation location) const
	{
		for (const auto& info : m_Uniforms)
		{
			if (info.Location == location)
			{
				return info;
			}
		}
		return std::nullopt;
	}

	bool ShaderProgram::HasUniform(const std::string &name) const
	{
		return std::ranges::any_of
					(
						m_Uniforms,
						[&](UniformInfo const &uniformInfo) { return uniformInfo.Name == name; }
					);
	}

    bool ShaderProgram::initializeProgramFromSource(std::string source, const std::vector<std::unique_ptr<ShaderProcessor::ShaderProcessorBase>>& shaderProcessors)
    {
		bool hasCompileError = false;

		// Processors.OnBegin
		for (auto& processor : shaderProcessors)
		{
			processor->OnBegin(*this);
		}

		// Processors.OnPreShaderTypeParse
		for (auto& processor : shaderProcessors)
		{
			processor->OnPreShaderTypeParse(source);
		}

		std::vector<std::string> processorDirectivesToIgnore {};
		for (auto& processor : shaderProcessors)
		{
			// Collect all the directives to ignore into one list.
			auto directives = processor->GetDirectivesToIgnoreInShaderTypeParsePhase();
			processorDirectivesToIgnore.insert(processorDirectivesToIgnore.end(), directives.begin(), directives.end());
		}

		// Parse ShaderProgram into Shader Sources of different types
		ShaderTypeParseResult shaderTypeParseResult = parseShaderProgramSourceIntoShaderSources(source, processorDirectivesToIgnore);
		if (!shaderTypeParseResult.Success)
		{
			hasCompileError = true;
		}

		// Processors.OnPostShaderTypeParse
		for (auto& processor : shaderProcessors)
		{
			processor->OnPostShaderTypeParse(shaderTypeParseResult);
		}

        // Create shader program on GPU
        m_ID = glCreateProgram();
        glCheckAfterCall(glCreateProgram());

		// Actually compile shader(s)
        std::vector<ShaderID> createdShaderIDs;
		for (auto& shaderSourcePair : shaderTypeParseResult.ShaderSources)
		{
			auto shaderType = shaderSourcePair.first;
			auto shaderSource = shaderSourcePair.second;

			// Processors.OnPreShaderCompilation
			for (auto& processor : shaderProcessors)
			{
				processor->OnPreShaderCompilation(shaderType, shaderSource);
			}

			ShaderCompilationResult compileShaderResult = compileShaderForProgram(m_ID, shaderType, shaderSource);
			if (compileShaderResult.Success)
			{
				glCall(glAttachShader(m_ID, compileShaderResult.CompiledShaderID));
				createdShaderIDs.push_back(compileShaderResult.CompiledShaderID);
			}
			else
			{
				/// Compile error!
				hasCompileError = true;
			}

			// Processors.OnPostShaderCompilation
			for (auto& processor : shaderProcessors)
			{
				processor->OnPostShaderCompilation(shaderType, compileShaderResult);
			}
		}

        // Link the shaders specified by m_ID with the corresponding GPU processors
        glCall(glLinkProgram(m_ID));
        glCall(glValidateProgram(m_ID));

        // Clean up shaders
        for (auto shaderID : createdShaderIDs)
        {
            glCall(glDeleteShader(shaderID));
        }

		// Processors.OnEnd
		for (auto& processor : shaderProcessors)
		{
			processor->OnEnd(*this);
		}

		m_HasCompileError = hasCompileError;
        return !hasCompileError;
    }

	ShaderProgram::ShaderTypeParseResult ShaderProgram::parseShaderProgramSourceIntoShaderSources(const std::string &programSource, const std::vector<std::string>& directivesToIgnore)
	{
		bool hasParseError = false;

		std::vector<std::pair<ShaderType, std::string>> shaderSources {};

		/// Parse source into multiple shader sources,
		/// if the program has certain types of shader.
		bool hasShadersOfType[ShaderConstants::NumberOfShaderTypes];
		for (bool& hasShaderOfType : hasShadersOfType)
		{
			hasShaderOfType = false;
		}

		/// source string stream for each currScopeType of shader
		std::stringstream shaderSS[ShaderConstants::NumberOfShaderTypes];

		/// open the file and read the source from it
		std::stringstream stream(programSource);

		/// the type of the current shader that is being parsed
		bool isInValidTypeScope = false;
		ShaderType currScopeType = ShaderType::Vertex;

		std::string line;
		while (std::getline(stream, line))
		{
			if (line.find(ShaderConstants::ShaderTypeSpecifier) != std::string::npos)
			{
				// Found #shader preprocessor directive! Parse the shader type.
				bool const hasShaderTypeKeyword = HasValidShaderTypeKeywordInLine(line, currScopeType);
				if (hasShaderTypeKeyword)
				{
					isInValidTypeScope = true;
				}
				else
				{
					DYE_LOG("Unknown shader type - %s", line.c_str());
					hasParseError = true;
					isInValidTypeScope = false;
				}

				/// The shader of the type has already existed!
				int const currScopeTypeIndex = (int) currScopeType;
				if (hasShadersOfType[currScopeTypeIndex])
				{
					DYE_LOG("Duplicate shader type - %s", line.c_str());
					hasParseError = true;
					continue;
				}

				hasShadersOfType[currScopeTypeIndex] = true;
				continue;
			}

			auto firstToken = line.substr(0, line.find(' '));
			if (std::find(directivesToIgnore.begin(), directivesToIgnore.end(), firstToken) != std::end(directivesToIgnore))
			{
				// This line is marked as ignored by one of the processors' directives. Skip it!
				continue;
			}

			if (!isInValidTypeScope)
			{
				// The current line is not under a specific shader type scope. Skip it!
				continue;
			}

			// Add the line to the corresponding shader string stream if the type is valid.
			shaderSS[(int) currScopeType] << line << '\n';
		}

		for (int typeIndex = 0; typeIndex < ShaderConstants::NumberOfShaderTypes; typeIndex++)
		{
			bool const hasShaderType = hasShadersOfType[typeIndex];
			if (!hasShaderType)
			{
				continue;
			}

			auto shaderType = (ShaderType) typeIndex;
			shaderSources.emplace_back(shaderType, shaderSS[typeIndex].str());
		}

		return ShaderTypeParseResult { .Success = !hasParseError, .ShaderSources = std::move(shaderSources) };
	}

	ShaderProgram::ShaderCompilationResult ShaderProgram::compileShaderForProgram(DYE::ShaderProgramID programId, DYE::ShaderType type, const std::string &source)
	{
		ShaderID shaderID = 0;

		const char *shaderSource = source.c_str();

		unsigned int const glShaderType = ShaderTypeEnumToGLShaderType(type);

		shaderID = glCreateShader(glShaderType);
		glCall(glShaderSource(shaderID, 1, &shaderSource, nullptr));
		glCall(glCompileShader(shaderID));

		int compileResult;
		glCall(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileResult));
		if (compileResult == GL_FALSE)
		{
			int length;
			glCall(glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length));

			char *msg = (char *) alloca(length * sizeof(char));
			glCall(glGetShaderInfoLog(shaderID, length, &length, msg));

			DYE_LOG("Shader[%d] Compilation Error - %s", (int) type, msg);

			return ShaderCompilationResult { .Success = false, .CompiledShaderID = 0 };
		}

		return ShaderCompilationResult { .Success = true, .CompiledShaderID = shaderID };
	}
}