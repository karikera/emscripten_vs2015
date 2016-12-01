#pragma once

#include <KR3/main.h>
#include <cstdint>

struct ID3D11ShaderReflection;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11GeometryShader;


namespace kr
{
	namespace gl
	{

		enum class ShaderType
		{
			Vertex,
			Fragment,
			Geometry
		};
		template <ShaderType type> class Shader;
		template <ShaderType type> struct ShaderObject;
		template <> struct ShaderObject<ShaderType::Vertex>{ using type = Keep<ID3D11VertexShader>; };
		template <> struct ShaderObject<ShaderType::Fragment>{ using type = Keep<ID3D11PixelShader>; };
		template <> struct ShaderObject<ShaderType::Geometry>{ using type = Keep<ID3D11GeometryShader>; };

		using VertexShader = Shader<ShaderType::Vertex>;
		using FragmentShader = Shader<ShaderType::Fragment>;
		using GeometryShader = Shader<ShaderType::Geometry>;
		class Program;
		class UniformImpl;

		template <ShaderType type> class Shader
		{
		public:
			Shader(const void * data, std::size_t size) noexcept;
			template <std::size_t size> Shader(const std::byte(&data)[size]) noexcept
				:Shader(data, size)
			{
			}
			~Shader() noexcept;
			Shader(const Shader & _copy) noexcept;
			Shader(Shader && _move) noexcept;
			Shader& operator =(const Shader & _copy) noexcept;
			Shader& operator =(Shader && _move) noexcept;

	#ifndef KRGL_PRIVATE
		private:
	#endif

			const void * m_data;
			std::size_t m_size;
			Keep<ID3D11ShaderReflection> m_reflect;
			typename ShaderObject<type>::type m_shader;
		};
	

	}
}