#pragma once

#include <KR3/main.h>
#include "../image.h"

namespace kr
{
	namespace gl
	{
		class Jpeg;
	}
}

class kr::gl::Jpeg
{
public:
	static constexpr int QUALITY_MAX = 100;
	static ImageData load(FILE * infile) noexcept;
	static bool save(const ImageData & data, pcwstr filename, int quality) noexcept;
};