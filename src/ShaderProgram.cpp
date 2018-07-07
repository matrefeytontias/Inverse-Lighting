#include <cstdio>
#include <iostream>

#include "ShaderProgram.h"
#include "utils.h"

using namespace invLight;

ShaderProgram::ShaderProgram(const char *vertex, const char *fragment)
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	
	_program = glCreateProgram();
	_vertexShader = createShaderFromSource(GL_VERTEX_SHADER, vertex);
	printShaderLog(_vertexShader);
	_fragmentShader = createShaderFromSource(GL_FRAGMENT_SHADER, fragment);
	printShaderLog(_fragmentShader);
	glAttachShader(_program, _vertexShader);
	glAttachShader(_program, _fragmentShader);
	glLinkProgram(_program);
}

ShaderProgram::~ShaderProgram()
{
	glBindVertexArray(_vao);
	
	for(auto cpl : _attributes)
		glDisableVertexAttribArray(cpl.second);
	
	glDetachShader(_program, _vertexShader);
	glDetachShader(_program, _fragmentShader);
	glDeleteShader(_vertexShader);
	glDeleteShader(_fragmentShader);
	glDeleteProgram(_program);
	glDeleteVertexArrays(1, &_vao);
}

void ShaderProgram::use()
{
	glBindVertexArray(_vao);
	glUseProgram(_program);
}

void ShaderProgram::uniform1f(string name, float v)
{
    glUniform1f(ensureUniform(name), v);
}

void ShaderProgram::uniform2f(string name, float v1, float v2)
{
    glUniform2f(ensureUniform(name), v1, v2);
}

void ShaderProgram::uniform3f(string name, float v1, float v2, float v3)
{
    glUniform3f(ensureUniform(name), v1, v2, v3);
}

void ShaderProgram::uniform4f(string name, float v1, float v2, float v3, float v4)
{
    glUniform4f(ensureUniform(name), v1, v2, v3, v4);
}

void ShaderProgram::uniformMatrix4fv(string name, GLuint count, const GLfloat *v)
{
    glUniformMatrix4fv(ensureUniform(name), count, GL_FALSE, v);
}

void ShaderProgram::uniformMatrix3fv(string name, GLuint count, const GLfloat *v)
{
    glUniformMatrix3fv(ensureUniform(name), count, GL_FALSE, v);
}

void ShaderProgram::vertexAttribPointer(string name, GLuint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
	glEnableVertexAttribArray(ensureAttrib(name));
    glVertexAttribPointer(_attributes[name], size, type, GL_FALSE, stride, pointer);
}

GLint ShaderProgram::ensureUniform(string name)
{
	if(_uniforms.find(name) == _uniforms.end())
		_uniforms[name] = glGetUniformLocation(_program, name.c_str());
	return _uniforms[name];
}

GLint ShaderProgram::ensureAttrib(string name)
{
	if(_attributes.find(name) == _attributes.end())
		_attributes[name] = glGetAttribLocation(_program, name.c_str());
	return _attributes[name];
}
