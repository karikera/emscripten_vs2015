#include "stdafx.h"

#include <emscripten/html5.h>

#include <math.h>
#include <time.h>
#include <assert.h>
#include <iostream>

#include <KRCanvas/gl.h>
#include <KR3/math/coord.h>

using namespace kr;

WebCanvasGL * webgl;
gl::Program program;

void initProgram() noexcept
{
	static const char vShaderStr[] =
		"attribute vec4 vPosition;    \n"
		"void main()                  \n"
		"{                            \n"
		"   gl_Position = vPosition;  \n"
		"}                            \n";

	static const char fShaderStr[] =
		"precision mediump float;\n"
		"void main()                                  \n"
		"{                                            \n"
		"  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
		"}                                            \n";

	program = gl::Program::create();

	gl::Shader vertexShader(GL_VERTEX_SHADER, vShaderStr);
	gl::Shader fragmentShader(GL_FRAGMENT_SHADER, fShaderStr);

	program.attach(vertexShader);
	program.attach(fragmentShader);

	program.bindAttribLocation(0, "vPosition");
	program.link();

	// Check the link status

	if (!program.getLinkState())
	{
		cerr << program.getInfoLog() << endl;
		program.remove();
	}
}

void main_loop()
{
	GLfloat vVertices[] = {
		0.0f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};

	// No clientside arrays, so do this in a webgl-friendly manner
	GLuint vertexPosObject;
	glGenBuffers(1, &vertexPosObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
	glBufferData(GL_ARRAY_BUFFER, 9 * 4, vVertices, GL_STATIC_DRAW);
	
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Use the program object
	program.use();

	// Load the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
	glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	webgl->swap();
}

int CT_CDECL main(int argn, char ** args)
{
	WebCanvasGL webglimpl(256, 256);
	webgl = &webglimpl;
	
	initProgram();
	emscripten_set_main_loop(main_loop, 0, true);
	return 0;
}
