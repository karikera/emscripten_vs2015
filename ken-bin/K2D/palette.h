#pragma once

#include <KR3/main.h>
#include <KR3/math/coord.h>
#include "common.h"

class kr::gl::Palette :public kr::Bufferable<Palette, kr::BufferInfo<color, true, false, false> >
{
	using Super = WRefArray<color>;
public:
	static constexpr int PALETTE_SIZE = 256;

	Palette() noexcept = default;

	color * begin() noexcept;
	const color * begin() const noexcept;
	color * end() noexcept;
	const color * end() const noexcept;
	size_t size() const noexcept;
	bool empty() const noexcept;

	void fill(color _color) noexcept;

	int getNearstColor(color _c) const noexcept;

	void setEntireAlpha(byte _alpha) noexcept;

	static thread_local const Palette * defaultPalette;

private:
	color m_color[PALETTE_SIZE];
};
