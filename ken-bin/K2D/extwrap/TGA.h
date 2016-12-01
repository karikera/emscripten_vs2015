#pragma once

#include <KR3/main.h>
#include "../image.h"

namespace kr
{

	class Tga
	{
	public:
		static gl::ImageData load(FILE * _fp, gl::Palette * _palette) noexcept;
		static bool save(const char * sz_filename, bool blCompress) noexcept;
	};
}
