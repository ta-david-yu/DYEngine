#pragma once

#include "Graphics/Uniform.h"

#include <string>
#include <memory>
#include <vector>

namespace DYE
{
    using ShaderProgramID = std::uint32_t;
    using ShaderID = std::uint32_t;

    enum class ShaderType
    {
        Vertex = 0,
        Geometry,
        Fragment,

        NumOfType,

        Invalid
    };

    /// A Shader Program wraps the GL ShaderProgramID and functions.
    /// Current implementation is in OpenGL
    class ShaderProgram
    {
    public:
        /// The constructor should not be called directly, use ShaderProgram::CreateFromFile instead
        explicit ShaderProgram(std::string name);
        ~ShaderProgram();

        ShaderProgramID GetID() const { return m_ID; }
        std::string GetName() const { return m_Name; }
        inline bool HasCompileError() const { return m_HasCompileError; }

        /// Use the shader program to the rendering pipeline
        void Use();
        void Unbind();

        /// A factory function that creates a shader program with the given shader source file
        /// \param filepath
        /// \return a shared pointer to the newly created ShaderProgram, return shared_ptr(nullptr) if failed
        static std::shared_ptr<ShaderProgram> CreateFromFile(const std::string& name, const std::string& filepath);
    private:
        /// A raw pointer to the shader program that is currently used/bound to the GPU
        //static ShaderProgram* s_pCurrentShaderProgramInUse;

        /// Compile and create shader program in the GPU driver
        /// \param filepath the source filepath of the shader
        /// \return true if succeed, else false
        bool createProgramFromSourceFile(const std::string& filepath);

        /// Compile and create shader program in the GPU driver
        /// \param source the source code of the shader
        /// \return true if succeed, else false
        bool createProgramFromSource(const std::string& source);

		/// Populate uniform infos vector with the shader uniforms information.
		void updateUniformInfos();

	private:
        /// ShaderProgramName (debugging)
        std::string m_Name;
        /// ShaderProgramID
        ShaderProgramID m_ID{};

		std::vector<UniformInfo> m_UniformInfos{};

        bool m_HasCompileError = false;
    };
}