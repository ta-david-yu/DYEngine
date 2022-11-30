#pragma once

#include <string>

#include <glad/glad.h>

namespace DYE
{
	namespace DefaultUniformNames
	{
		static const char* TransformMatrix = "_TransformMatrix";
		static const char* ModelMatrix = "_ModelMatrix";
		static const char* ViewMatrix = "_ViewMatrix";
		static const char* ProjectionMatrix = "_ProjectionMatrix";
	}

	using UniformLocation = std::uint32_t;
	using UniformSize = GLsizei;
	using GLUniformEnum = GLenum;

	enum class UniformType
	{
		Invalid = -1,
		Float,
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
				break;
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

	/// Represent the information about an uniform variable in the Shader
	struct UniformInfo
	{
		std::string Name;
		UniformType Type;
		UniformLocation Location;
	} __attribute__((aligned(64)));
}
