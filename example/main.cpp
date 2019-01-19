#include <emscripten.h>
#include <emscripten/html5.h>

#include "canvas.h"

void main_loop() noexcept
{
	canvasClear();
	canvasBeginPath();
	canvasMoveTo(0, 0);
	canvasLineTo(100, 100);
	canvasStroke();
}

int main(int argn, char ** args)
{
	canvasCreate();
	const char *test = "한글한글";
	EM_ASM_(alert(Pointer_stringify($0),4 ),test);
	emscripten_set_main_loop(main_loop, 0, true);
	return 0;
}
