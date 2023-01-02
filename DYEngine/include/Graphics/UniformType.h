#pragma once

#include "Base.h"

#include <string>
#include <glad/glad.h>

namespace DYE
{
	namespace DefaultUniformNames
	{
		constexpr const char* TransformMatrix = "_TransformMatrix";
		constexpr const char* ModelMatrix = "_ModelMatrix";
		constexpr const char* ViewMatrix = "_ViewMatrix";
		constexpr const char* ProjectionMatrix = "_ProjectionMatrix";
	}

	using UniformLocation = std::uint32_t;
	using UniformSize = GLsizei;
	using GLUniformEnum = GLenum;

	namespace UniformConstants
	{
		constexpr int NumberOfUniformTypes = 9;
	}
	enum class UniformType
	{
		Invalid = -1,
		Float = 0,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Boolean,
		Texture2D
	};

	static GLUniformEnum UniformTypeToGLType(UniformType type)
	{
		switch (type)
		{
			case UniformType::Float: return GL_FLOAT;
			case UniformType::Float2: return GL_FLOAT_VEC2;
			case UniformType::Float3: return GL_FLOAT_VEC3;
			case UniformType::Float4: return GL_FLOAT_VEC4;
			case UniformType::Mat3: return GL_FLOAT_MAT3;
			case UniformType::Mat4: return GL_FLOAT_MAT4;
			case UniformType::Int: return GL_INT;
			case UniformType::Boolean: return GL_BOOL;
			case UniformType::Texture2D: return GL_SAMPLER_2D;

			case UniformType::Invalid:
			default:
				break;
		}
		return GL_NONE;
	}

	static UniformType GLTypeToUniformType(GLUniformEnum glEnum)
	{
		switch (glEnum)
		{
			case GL_FLOAT: return UniformType::Float;
			case GL_FLOAT_VEC2: return UniformType::Float2;
			case GL_FLOAT_VEC3: return UniformType::Float3;
			case GL_FLOAT_VEC4: return UniformType::Float4;
			case GL_FLOAT_MAT3: return UniformType::Mat3;
			case GL_FLOAT_MAT4: return UniformType::Mat4;
			case GL_INT: return UniformType::Int;
			case GL_BOOL: return UniformType::Boolean;
			case GL_SAMPLER_2D: return UniformType::Texture2D;

			default:
				break;
		}
		return UniformType::Invalid;
	}

	static std::string UniformTypeToPropertyTypeQualifier(UniformType type)
	{
		switch (type)
		{
			case UniformType::Float: return "float";
			case UniformType::Float2: return "vec2";
			case UniformType::Float3: return "vec3";
			case UniformType::Float4: return "vec4";
			case UniformType::Mat3: return "mat3";
			case UniformType::Mat4: return "mat4";
			case UniformType::Int: return "int";
			case UniformType::Boolean: return "bool";
			case UniformType::Texture2D: return "sampler2D";

			case UniformType::Invalid:
			default:
				break;
		}
		return "invalid-type";
	}

	static bool HasValidPropertyTypeKeywordInLine(const std::string& line, UniformType& outUniformType)
	{
		bool result = false;

		for (int i = 0; i < UniformConstants::NumberOfUniformTypes; i++)
		{
			auto uniformType = (UniformType) i;
			auto typeQualifier = UniformTypeToPropertyTypeQualifier(uniformType);
			if (line.find(typeQualifier) != std::string::npos)
			{
				outUniformType = uniformType;
				result = true;
				break;
			}
		}

		return result;
	}

	// Represent the information about an uniform variable in the Shader
	struct UniformInfo
	{
		std::string Name;
		UniformType Type;
		UniformLocation Location;
		// The texture unit slot this uniform is tied to If the type is a texture.
		// Pass this value in Texture.Bind(int slot) function.
		int TextureUnitSlotIfTexture;
	};

	// Represent the information about a (uniform) property defined in the Shader
	struct PropertyInfo
	{
		std::string UniformName;
		UniformType Type;
		std::string DisplayName;
	};
}
