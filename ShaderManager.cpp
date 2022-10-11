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

		char* errorLog = (char*)_alloca(logSize * sizeof(char));
		glGetShaderInfoLog(*shader, logSize, &logSize, errorLog);

		const std::string erroInfo = errorLog;
		std::cout << "Failed to compile shader: " + erroInfo << std::endl;

		glDeleteShader(*shader);

		return false;
	}
}

ShaderManager::ShaderManager()
{
	compile_shader(readFile("./vertex_shader.glsl"), &vert_shader, GL_VERTEX_SHADER);
	compile_shader(readFile("./fragment_shader.glsl"), &frag_shader, GL_FRAGMENT_SHADER);

	program = glCreateProgram();

	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);

	// error checking
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char* errorLog = (char*)_alloca(maxLength * sizeof(char));
		glGetProgramInfoLog(program, maxLength, &maxLength, errorLog);

		const std::string erroInfo = errorLog;
		std::cout << "Failed to link shaders: " + erroInfo << std::endl;

		glDeleteProgram(program);
	}

}

void ShaderManager::use_program()
{
	glUseProgram(program);
}

void ShaderManager::add_uniform(UNIFORM_TYPE uni_type,const std::string& name, const void * value)
{
	GLuint uni_id = glGetUniformLocation(program, name.c_str());
	
	switch (uni_type)
	{
		case UNIFORM_TYPE::MAT4: 
			glUniformMatrix4fv(uni_id, 1, GL_FALSE, (GLfloat*)value);
			break;
		case UNIFORM_TYPE::TEXTURE:
			glUniform1i(uni_id, (GLint)value);
			break;
		default: break;
	}
}

ShaderManager::~ShaderManager() 
{
	glDeleteProgram(program);
}