#include "ShaderManager.h"
#include <iostream>

bool ShaderManager::compile_shader(const std::string &name, GLuint *shader, GLuint type)
{
	*shader = glCreateShader(type);
	const char* shaders[1] = { name.c_str() };
	const int size = name.size();
	glShaderSource(*shader, 1, shaders, &size);
	glCompileShader(*shader);

	GLint success = 0;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		GLint logSize = 0;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logSize);

		char* errorLog = (char*)alloca(logSize);
		glGetShaderInfoLog(*shader, logSize, &logSize, errorLog);

		const std::string erroInfo = errorLog;
		std::cout << "Failed to compile shader: " + erroInfo << std::endl;

		glDeleteShader(*shader);

		return false;
	}
}

ShaderManager::ShaderManager()
{
	compile_shader("./vertex_shader", &vert_shader, GL_VERTEX_SHADER);
	compile_shader("./fragment_shader", &frag_shader, GL_FRAGMENT_SHADER);

	program = glCreateProgram();

	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);

	glLinkProgram(program);
}

void ShaderManager::use_program()
{
	glUseProgram(program);
}

ShaderManager::~ShaderManager() {}