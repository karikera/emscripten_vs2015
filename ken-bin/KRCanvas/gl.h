#pragma once

#include "canvas.h"

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>

namespace kr
{
	class WebCanvasGL: public WebCanvas
	{
	public:
		WebCanvasGL(int width, int height) noexcept;
		~WebCanvasGL() noexcept;
		void swap() noexcept;

		void onResize(int width, int height) noexcept override;

	private:
		EGLContext m_eglContext;
		EGLSurface m_eglSurface;
		EGLDisplay m_eglDisplay;
	};

	namespace gl
	{
		class Shader
		{
		public:
			Shader() = default;
			Shader(GLenum type, const GLchar * shaderSrc) noexcept;
			void remove() noexcept;
			GLuint getId() noexcept;

		private:
			GLuint m_id;
		};

		class Program
		{
		public:
			Program() = default;
			static Program create() noexcept;
			void remove() noexcept;
			GLuint getId() noexcept;
			void attach(Shader shader) noexcept;
			void bindAttribLocation(GLuint index, const GLchar * name) noexcept;
			void link() noexcept;
			void use() noexcept;
			GLint getInt(GLenum pname) noexcept;
			GLint getLinkState() noexcept;
			GLint getInfoLogLength() noexcept;
			void getInfoLog(GLsizei bufSize, GLsizei * length, GLchar * infoLog) noexcept;
			AText getInfoLog() noexcept;

		private:
			GLuint m_id;
		};


	}

}
