#pragma once

#ifdef WIN32
#include <KR3/wl/windows.h>
#endif

#include <EGL/egl.h>

namespace kr
{
	class WebCanvas
	{
	public:
		WebCanvas(int width, int height) noexcept;
		~WebCanvas() noexcept;
		int getScreenWidth() noexcept;
		int getScreenHeight() noexcept;

		virtual void onResize(int width, int height) noexcept;

	protected:
		EGLNativeWindowType m_window;
		EGLNativeDisplayType m_display;
#ifdef WIN32
		int m_width, m_height;
#endif
	};
}

