#include "Base.h"
#include "Graphics/Shader.h"
#include "Graphics/OpenGL.h"

#include <utility>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <glad/glad.h>

namespace DYE
{
    /// Map the ShaderType enum to GL_(TYPE)_SHADER
    /// \param type enum ShaderType
    /// \return unsigned int (GL Shader Type)
    static unsigned int shaderTypeEnumToGLShaderType(ShaderType type)
    {
        switch (type)
        {
            case ShaderType::Vertex:
                return GL_VERTEX_SHADER;
                break;
            case ShaderType::Geometry:
                return GL_GEOMETRY_SHADER;
                break;
            case ShaderType::Fragment:
                return GL_FRAGMENT_SHADER;
                break;
            case ShaderType::Invalid:
                break;
        }
        return 0;
    }

    //ShaderProgram* ShaderProgram::s_pCurrentShaderProgramInUse = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::CreateFromFile(const std::string& name, const std::filesystem::path &filepath)
	{
		DYE_LOG("-- Start creating shader \"%s\" from %s --", name.c_str(), filepath.c_str());

		auto program = std::make_shared<ShaderProgram>(name);
		bool success = program->initializeProgramFromSourceFile(filepath);

		if (program->HasCompileError())
		{
			DYE_LOG("There are compile errors in the shader!");
		}

		if (!success)
		{
			DYE_LOG("-- Failed to create shader [%d] \"%s\" from %s --", program->m_ID, name.c_str(), filepath.c_str());
			DYE_ASSERT(false);
			return std::shared_ptr<ShaderProgram>();
		}

		DYE_LOG("-- Successfully create shader [%d] \"%s\" from %s --", program->m_ID, name.c_str(), filepath.c_str());
		return program;
	}

    DYE::ShaderProgram::ShaderProgram(std::string name) : m_ID(0), m_Name(std::move(name))
    {

    }

    DYE::ShaderProgram::~ShaderProgram()
    {
        Unbind();

        DYE_LOG("Delete Program [%d] %s", m_ID, m_Name.c_str());
        glCall(glDeleteProgram(m_ID));
    }

    void ShaderProgram::Use()
    {
        glCall(glUseProgram(m_ID));

        //s_pCurrentShaderProgramInUse = this;
    }

    void ShaderProgram::Unbind()
    {
        //if (s_pCurrentShaderProgramInUse == this)
        {
            glCall(glUseProgram(0));
            //s_pCurrentShaderProgramInUse = nullptr;
        }
    }

    bool ShaderProgram::initializeProgramFromSourceFile(const std::filesystem::path &filepath)
    {
        std::ifstream fs(filepath);
        std::string content((std::istreambuf_iterator<char>(fs)),
                             (std::istreambuf_iterator<char>()));

        return initializeProgramFromSource(content);
    }

    bool ShaderProgram::initializeProgramFromSource(const std::string &source)
    {
		/// TODO: Processors.OnBegin

		/// TODO: Processors.OnPreShaderTypeKeyword

        /// Parse source
        /// if the program has certain types of shader
        bool hasShadersOfType[ShaderConstants::NumberOfShaderTypes];
        for (bool& hasShaderOfType : hasShadersOfType)
        {
			hasShaderOfType = false;
        }

        /// source string stream for each currScopeType of shader
        std::stringstream shaderSS[ShaderConstants::NumberOfShaderTypes];

        /// open the file and read the source from it
        std::stringstream stream(source);

        /// the type of the current shader that is being parsed
        ShaderType currScopeType = ShaderType::Invalid;


        std::string line;
        while (std::getline(stream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                {
                    /// TO Vertex
                    currScopeType = ShaderType::Vertex;
                }
                else if (line.find("geometry") != std::string::npos)
                {
                    /// To Geometry
                    currScopeType = ShaderType::Geometry;
                }
                else if (line.find("fragment") != std::string::npos)
                {
                    /// To Fragment
                    currScopeType = ShaderType::Fragment;
                }
                else
                {
                    DYE_LOG("Unknown shader type - %s", line.c_str());
                    m_HasCompileError = true;
                }

                /// The shader of the type has already existed!
                if (hasShadersOfType[(int) currScopeType])
                {
                    DYE_LOG("Duplicate shader type - %s", line.c_str());
                    m_HasCompileError = true;
                }
                else
                {
                    hasShadersOfType[(int) currScopeType] = true;
                }
            }
            else
            {
                if (currScopeType != ShaderType::Invalid)
                {
                    shaderSS[(int) currScopeType] << line << '\n';
                }
            }
        }

		/// TODO: Processors.OnPostShaderTypeKeyword

        /// Compile source
        m_ID = glCreateProgram();
        glCheckAfterCall(glCreateProgram());

        std::vector<ShaderID> createdShaderIDs;

        for (int typeIndex = 0; typeIndex < ShaderConstants::NumberOfShaderTypes; typeIndex++)
        {
            bool hasShaderType = hasShadersOfType[typeIndex];
            if (hasShaderType)
            {
				/// TODO: Processors.OnPreShaderCompilation
                auto shaderSource = shaderSS[typeIndex].str();
                auto type = (ShaderType) typeIndex;

				ShaderCompilationResult result = compileShaderForProgram(m_ID, type, shaderSource);
				if (result.Success)
				{
					glCall(glAttachShader(m_ID, result.CompiledShaderID));
					createdShaderIDs.push_back(result.CompiledShaderID);
				}
				else
				{
					/// Compile error!
					m_HasCompileError = true;
				}
				/// TODO: Processors.OnPostShaderCompilation
            }
        }

        /// Link the shaders specified by m_ID with the corresponding GPU processors
        glCall(glLinkProgram(m_ID));
        glCall(glValidateProgram(m_ID));

        /// Clean up shaders
        for (auto shaderID : createdShaderIDs)
        {
            glCall(glDeleteShader(shaderID));
        }

		/// Update uniforms info
		updateUniformInfos();

        return true;
    }

	ShaderTypeParseResult ShaderProgram::parseShaderProgramSourceIntoShaderSources(const std::string &programSource)
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
		ShaderType currScopeType = ShaderType::Invalid;

		std::string line;
		while (std::getline(stream, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
				{
					/// TO Vertex
					currScopeType = ShaderType::Vertex;
				}
				else if (line.find("geometry") != std::string::npos)
				{
					/// To Geometry
					currScopeType = ShaderType::Geometry;
				}
				else if (line.find("fragment") != std::string::npos)
				{
					/// To Fragment
					currScopeType = ShaderType::Fragment;
				}
				else
				{
					DYE_LOG("Unknown shader type - %s", line.c_str());
					hasParseError = true;
				}

				/// The shader of the type has already existed!
				if (hasShadersOfType[(int) currScopeType])
				{
					DYE_LOG("Duplicate shader type - %s", line.c_str());
					hasParseError = true;
				}
				else
				{
					hasShadersOfType[(int) currScopeType] = true;
				}
			}
			else
			{
				if (currScopeType != ShaderType::Invalid)
				{
					shaderSS[(int) currScopeType] << line << '\n';
				}
			}
		}

		for (int typeIndex = 0; typeIndex < ShaderConstants::NumberOfShaderTypes; typeIndex++)
		{
			bool hasShaderType = hasShadersOfType[typeIndex];
			if (!hasShaderType)
			{
				continue;
			}

			auto shaderType = (ShaderType) typeIndex;
			shaderSources.emplace_back(shaderType, shaderSS[typeIndex].str());
		}

		return ShaderTypeParseResult { .Success = !hasParseError, .ShaderSources = std::move(shaderSources) };
	}

	ShaderCompilationResult ShaderProgram::compileShaderForProgram(DYE::ShaderProgramID programId, DYE::ShaderType type, const std::string &source)
	{
		ShaderID shaderID = 0;

		const char *shaderSource = source.c_str();

		unsigned int glShaderType = shaderTypeEnumToGLShaderType(type);

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

	void ShaderProgram::updateUniformInfos()
	{
		/// Init vector.
		m_UniformInfos.clear();

		/// Parse uniform variables and cache the information.
		std::int32_t numberOfUniforms = 0;
		glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &numberOfUniforms);

		if (numberOfUniforms <= 0)
		{
			return;
		}

		std::int32_t maxUniformNameLength = 0;
		UniformSize uniformNameLength = 0;
		UniformSize uniformSize = 0;
		GLUniformEnum uniformType = GL_NONE;

		glGetProgramiv(m_ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
		auto uniformName = std::make_unique<char[]>(maxUniformNameLength);

		/// Use this program before reading/writing uniforms.
		Use();

		int textureUnitSlotCounter = 0;
		for (std::int32_t i = 0; i < numberOfUniforms; i++)
		{
			glGetActiveUniform(m_ID, i, maxUniformNameLength, &uniformNameLength, &uniformSize, &uniformType,
							   uniformName.get());

		 	UniformInfo info{};
			info.Name = std::string(uniformName.get(), uniformNameLength);
			info.Type = GLTypeToUniformType(uniformType);
			info.Location = glGetUniformLocation(m_ID, uniformName.get());

			if (info.Type == UniformType::Texture2D)
			{
				// If the variable is a texture,
				// we want to bind texture unit slot to the uniform.
				glCall(glUniform1i(info.Location, textureUnitSlotCounter));

				textureUnitSlotCounter++;
			}
			// TODO: add more texture type here (sampler)

			m_UniformInfos.push_back(info);
		}

		DYE_LOG("There are [%d] uniform variables", m_UniformInfos.size());
	}
}