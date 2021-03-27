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
            case ShaderType::NumOfType:
                break;
            case ShaderType::Invalid:
                break;
        }
        return 0;
    }

    /// A static helper function that compiles a single shader and return GL Shader ID
    /// \param type
    /// \param source
    /// \return Shader ID, return 0 if the compilation failed
    static unsigned int compileShader(unsigned int type, const std::string& source)
    {
        const char* src = source.c_str();

        unsigned int shaderID = glCreateShader(type);
        glShaderSource(shaderID, 1, &src, nullptr);
        glCompileShader(shaderID);

        int compileResult;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileResult);
        if (compileResult == GL_FALSE)
        {
            int length;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);

            char *msg = (char *) alloca(length * sizeof(char));
            glGetShaderInfoLog(shaderID, length, &length, msg);

            DYE_LOG_ERROR("Shader Compilation Error - %s", msg);

            /// reset ID to zero, which represents error
            shaderID = 0;
        }

        return shaderID;
    }

    ShaderProgram* ShaderProgram::s_pCurrentShaderProgramInUse = nullptr;

    std::shared_ptr<ShaderProgram> ShaderProgram::CreateFromFile(const std::string& name, const std::string &filepath)
    {
        auto program = std::make_shared<ShaderProgram>(name);
        bool success = program->createProgramFromSourceFile(filepath);

        if (success)
        {
            return program;
        }
        else
        {
            DYE_LOG_ERROR("Failed to compile shader %s name at %s", name.c_str(), filepath.c_str());
            return std::shared_ptr<ShaderProgram>();
        }
    }

    DYE::ShaderProgram::ShaderProgram(std::string name) : m_ID(0), m_Name(std::move(name))
    {

    }

    DYE::ShaderProgram::~ShaderProgram()
    {
        // ensure this program is not being used before deletion
        if (s_pCurrentShaderProgramInUse == this)
        {
            s_pCurrentShaderProgramInUse = nullptr;
            glCall(glUseProgram(0));
        }

        glCall(glDeleteProgram(m_ID));
    }

    void ShaderProgram::Bind()
    {
        glCall(glUseProgram(m_ID));

        s_pCurrentShaderProgramInUse = this;
    }

    bool ShaderProgram::createProgramFromSourceFile(const std::string &filepath)
    {
        std::ifstream fs(filepath);
        std::string content((std::istreambuf_iterator<char>(fs)),
                             (std::istreambuf_iterator<char>()));

        return createProgramFromSource(content);
    }

    bool ShaderProgram::createProgramFromSource(const std::string &source)
    {
        /// Parse source
        /// if the program has certain types of shader
        bool hasShadersOfType[(int)ShaderType::NumOfType];

        /// source string stream for each currScopeType of shader
        std::stringstream shaderSS[(int)ShaderType::NumOfType];

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
                    DYE_LOG_WARN("Unknown shader type - %s", line.c_str());
                    return false;
                }

                /// The shader of the type has already existed!
                if (hasShadersOfType[(int) currScopeType])
                {
                    DYE_LOG_WARN("Duplicate shader type - %s", line.c_str());
                    return false;
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

        /// Compile source
        ShaderProgramID programID = glCreateProgram();
        glCheckAfterCall(glCreateProgram());

        std::vector<ShaderID> createdShaderIDs;

        for (int typeIndex = 0; typeIndex < (int) ShaderType::NumOfType; typeIndex++)
        {
            bool hasShaderType = hasShadersOfType[typeIndex];
            if (hasShaderType)
            {
                unsigned int glShaderType = shaderTypeEnumToGLShaderType((ShaderType) typeIndex);
                ShaderID shaderID = compileShader(glShaderType, shaderSS[typeIndex].str());

                if (shaderID != 0)
                {
                    glCall(glAttachShader(programID, shaderID));
                    createdShaderIDs.push_back(shaderID);

                    m_ID = programID;
                }
                /// Compile error!
                else
                {
                    m_HasCompileError = true;
                }
            }
        }

        if (m_ID != 0)
        {
            /// Link the shaders specified by m_ID with the corresponding GPU processors
            glCall(glLinkProgram(m_ID));
            glCall(glValidateProgram(m_ID));
        }

        /// Clean up shaders
        for (auto shaderID : createdShaderIDs)
        {
            glCall(glDeleteShader(shaderID));
        }

        return true;
    }
}