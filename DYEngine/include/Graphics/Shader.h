#pragma once

#include "Graphics/UniformType.h"

#include <string>
#include <memory>
#include <vector>
#include <filesystem>

namespace DYE
{
    using ShaderProgramID = std::uint32_t;
    using ShaderID = std::uint32_t;

	class ShaderProcessorBase;

	namespace ShaderConstants
	{
		constexpr int NumberOfShaderTypes = 3; // Vertex, Fragment, Geometry
		constexpr const char* ShaderTypeSpecifier = "#shader";
	}
    enum class ShaderType
    {
        Vertex = 0,
        Geometry,
        Fragment,
    };


    /// A Shader Program wraps the GL ShaderProgramID and functions.
    /// Current implementation is in OpenGL
    class ShaderProgram
    {
    public:
		struct ShaderTypeParseResult
		{
			bool Success;
			std::vector<std::pair<ShaderType, std::string>> ShaderSources;
		};

		struct ShaderCompilationResult
		{
			bool Success;
			ShaderID CompiledShaderID;
		};

		/// The constructor should not be called directly, use ShaderProgram::CreateFromFile instead
        explicit ShaderProgram(std::string name);
        ~ShaderProgram();

        ShaderProgramID GetID() const { return m_ID; }
        std::string GetName() const { return m_Name; }
        inline bool HasCompileError() const { return m_HasCompileError; }

        /// Use the shader program to the rendering pipeline
        void Use();
        void Unbind();

		std::vector<UniformInfo> GetAllUniformInfo();
		const std::vector<PropertyInfo>& GetAllPropertyInfo() const { return m_Properties; }
		void AddPropertyInfo(const PropertyInfo& propertyInfo) { m_Properties.emplace_back(propertyInfo); }
		void ClearPropertyInfo() { m_Properties.clear(); }

        /// A factory function that creates a shader program with the given shader source file
        /// \param filepath
        /// \return a shared pointer to the newly created ShaderProgram, return shared_ptr(nullptr) if failed
        static std::shared_ptr<ShaderProgram> CreateFromFile(const std::string& name, const std::filesystem::path& filepath);
    private:
        /// A raw pointer to the shader program that is currently used/bound to the GPU
        //static ShaderProgram* s_pCurrentShaderProgramInUse;

        /// Compile and create shader program in the GPU driver
        /// \param source the source code of the shader
        /// \return true if no compile error, else false
        bool initializeProgramFromSource(std::string& source,  const std::vector<std::unique_ptr<ShaderProcessorBase>>& shaderProcessors);

		/// Parse the given shader program source code into multiple shader sources.
		/// \param programSource
		/// \return If .Success is true, the result is a vector of pair (ShaderType -> ShaderSource), with all the shader sources stored in it.
		static ShaderTypeParseResult parseShaderProgramSourceIntoShaderSources(const std::string& programSource, const std::vector<std::string>& directivesToIgnore);

		/// Compile a shader of the given type with the given source code and attach it to the given shader program.
		/// \param programId The id of the shader program to attach the compiled shader to.
		/// \param type Shader type.
		/// \param source Shader source code.
		/// \return result. When .Success is false, ShaderID is set to 0 (which also means the default shader in the render API).
		static ShaderCompilationResult compileShaderForProgram(ShaderProgramID programId, ShaderType type, const std::string& source);

	private:
        /// ShaderProgramName (debugging)
        std::string m_Name;
        /// ShaderProgramID
        ShaderProgramID m_ID {};

		std::vector<PropertyInfo> m_Properties {};

        bool m_HasCompileError = false;
    };
}