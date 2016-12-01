#pragma once

#include <KR3/main.h>
#include "../image.h"

namespace kr
{
	namespace gl
	{
		class Png;
	}
}

class kr::gl::Png
{
public:
	static ImageData load(FILE * fp) noexcept;
	static bool save(const ImageData & image, pcwstr filename) noexcept;
};