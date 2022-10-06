#pragma once
// the OpenGL wrangler library for
// later versions of OpenGL
#include <GL/glew.h>

// GLFW3 graphics library
// exposing some native stuff to 
// make OpenCL OpenGL interop work
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <string>

#include "file_io.h"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	void use_program();
private:
	static bool compile_shader(const std::string& name, GLuint* shader, GLuint type);

private:
	GLuint frag_shader, vert_shader;
	GLuint program;
};