#include <cstdio>
#include <iostream>

#include "ShaderProgram.h"
#include "utils.h"

using namespace invLight;

ShaderProgram::ShaderProgram(const char *vertex, const char *fragment)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	program = glCreateProgram();
	vertexShader = createShaderFromSource(GL_VERTEX_SHADER, vertex);
	printShaderLog(vertexShader);
	fragmentShader = createShaderFromSource(GL_FRAGMENT_SHADER, fragment);
	printShaderLog(fragmentShader);
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
}

ShaderProgram::~ShaderProgram()
{
	glBindVertexArray(vao);
	
	for(auto cpl : attributes)
		glDisableVertexAttribArray(cpl.second);
	
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vao);
}

void ShaderProgram::use()
{
	glBindVertexArray(vao);
	glUseProgram(program);
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
    glVertexAttribPointer(attributes[name], size, type, GL_FALSE, stride, pointer);
}

GLint ShaderProgram::ensureUniform(string name)
{
	if(uniforms.find(name) == uniforms.end())
		uniforms[name] = glGetUniformLocation(program, name.c_str());
	return uniforms[name];
}

GLint ShaderProgram::ensureAttrib(string name)
{
	if(attributes.find(name) == attributes.end())
		attributes[name] = glGetAttribLocation(program, name.c_str());
	return attributes[name];
}
