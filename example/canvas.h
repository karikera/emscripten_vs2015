#pragma once

extern "C" void canvasCreate();
extern "C" void canvasClear();
extern "C" void canvasBeginPath();
extern "C" void canvasMoveTo(int x, int y);
extern "C" void canvasLineTo(int x, int y);
extern "C" void canvasStroke();
extern "C" void showDevTools();
