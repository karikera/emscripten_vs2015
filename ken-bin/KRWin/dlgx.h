#pragma once

#include "handle.h"

namespace kr
{
	namespace win
	{
		bool loadFileDialog(Window* hWnd, const char* Title, Text Type, Text TypeName, char* strFileName) noexcept;
		bool saveFileDialog(Window* hWnd, const char* Title, Text Type, Text TypeName, char* strFileName) noexcept;

		bool loadFileDialog(Window* hWnd, const wchar* Title, TextW Type, TextW TypeName, wchar* strFileName) noexcept;
		bool saveFileDialog(Window* hWnd, const wchar* Title, TextW Type, TextW TypeName, wchar* strFileName) noexcept;
	}
}