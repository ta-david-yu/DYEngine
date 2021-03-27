#pragma once

#include <string>
#include <memory>

namespace DYE
{
    using ShaderProgramID = unsigned int;
    using ShaderID = unsigned int;

    enum class ShaderType
    {
        Vertex = 0,
        Geometry,
        Fragment,

        NumOfType,

        Invalid
    };

    /// A Shader Program wraps the GL ShaderProgramID and functions
    class ShaderProgram
    {
    public:
        /// The constructor should not be called directly, use ShaderProgram::CreateFromFile instead
        explicit ShaderProgram(std::string name);
        ~ShaderProgram();

        ShaderProgramID GetID() const { return m_ID; }
        inline bool HasCompileError() const { return m_HasCompileError; }

        /// Bind and use the shader program to the rendering pipeline
        void Bind();

        /// A factory function that creates a shader program with the given shader source file
        /// \param filepath
        /// \return a shared pointer to the newly created ShaderProgram, return shared_ptr(nullptr) if failed
        static std::shared_ptr<ShaderProgram> CreateFromFile(const std::string& name, const std::string& filepath);
    private:
        /// A raw pointer to the shader program that is currently used/bound to the GPU
        static ShaderProgram* s_pCurrentShaderProgramInUse;

        /// Compile and create shader program in the GPU driver
        /// \param filepath the source filepath of the shader
        /// \return true if succeed, else false
        bool createProgramFromSourceFile(const std::string& filepath);

        /// Compile and create shader program in the GPU driver
        /// \param source the source code of the shader
        /// \return true if succeed, else false
        bool createProgramFromSource(const std::string& source);

        /// ShaderProgramName (debugging)
        std::string m_Name;
        /// ShaderProgramID
        ShaderProgramID m_ID{};
        bool m_HasCompileError = false;
    };
}