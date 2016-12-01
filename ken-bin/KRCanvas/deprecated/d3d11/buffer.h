#pragma once

#include <KR3/main.h>
#include <cstddef>

#ifdef KRGL_USE_D3D11

struct ID3D11Buffer;

#else
#error Need implement
#endif

namespace kr
{
	namespace gl
	{
		using vindex_t = unsigned short;

		class VertexBuffer
		{
		public:
			VertexBuffer() noexcept;
			~VertexBuffer() noexcept;
			VertexBuffer(std::size_t size) noexcept;
			VertexBuffer(const void * data, std::size_t size) noexcept;
			template <typename T, std::size_t size> VertexBuffer(const T(&data)[size]) noexcept
				:VertexBuffer(data, size * sizeof(T))
			{
			}
			VertexBuffer(const VertexBuffer& copy) noexcept;
			VertexBuffer(VertexBuffer&& move) noexcept;
			VertexBuffer& operator =(const VertexBuffer& tex) noexcept;
			VertexBuffer& operator =(VertexBuffer&& tex) noexcept;

			void use(std::size_t stride, std::size_t offset = 0) const noexcept;
			void* lock() noexcept;
			void unlock() noexcept;

		private:
			Keep<ID3D11Buffer> m_buffer;
		};
		class IndexBuffer
		{
		public:
			IndexBuffer() noexcept;
			~IndexBuffer() noexcept;
			IndexBuffer(const void * data, std::size_t size) noexcept;
			IndexBuffer(const IndexBuffer& copy) noexcept;
			IndexBuffer(IndexBuffer&& move) noexcept;
			IndexBuffer& operator =(const IndexBuffer& tex) noexcept;
			IndexBuffer& operator =(IndexBuffer&& tex) noexcept;
			template <typename T, std::size_t size> IndexBuffer(const T(&data)[size]) noexcept
				:IndexBuffer(data, size * sizeof(T))
			{
			}

			void use(unsigned int offset = 0) noexcept;

		private:
			Keep<ID3D11Buffer> m_buffer;
		};
	}

}