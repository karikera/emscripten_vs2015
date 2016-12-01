#pragma once

#include "texture.h"

namespace kr
{
	namespace gl
	{
		class Window3D;

		class TextureFile :public HasSampler
		{
			friend Window3D;
		public:
			TextureFile() noexcept;
			TextureFile(const char * filename) noexcept;
			TextureFile(const char * filename, bool cubemap) noexcept;
			~TextureFile() noexcept;
			TextureFile(const TextureFile& copy) noexcept;
			TextureFile(TextureFile&& move) noexcept;
			TextureFile& operator =(const TextureFile& tex) noexcept;
			TextureFile& operator =(TextureFile&& tex) noexcept;

			void use(int slot) const noexcept;
			bool exists() const noexcept;

#ifndef KRGL_PRIVATE
		private:
#endif
			class TextureFileRef;
			Keep<TextureFileRef>				m_tex;
		};
	}

}