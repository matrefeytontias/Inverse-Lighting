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
    
    for(auto tex : _textures)
        glDeleteTextures(1, &tex.second.id);
    
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
    int i = 0;
    for(auto tex : _textures)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(tex.second.target, tex.second.id);
        glUniform1i(ensureUniform(tex.first), i++);
    }
}

void ShaderProgram::uniform1f(const string name, float v)
{
    glUniform1f(ensureUniform(name), v);
}

void ShaderProgram::uniform2f(const string name, float v1, float v2)
{
    glUniform2f(ensureUniform(name), v1, v2);
}

void ShaderProgram::uniform3f(const string name, float v1, float v2, float v3)
{
    glUniform3f(ensureUniform(name), v1, v2, v3);
}

void ShaderProgram::uniform4f(const string name, float v1, float v2, float v3, float v4)
{
    glUniform4f(ensureUniform(name), v1, v2, v3, v4);
}

void ShaderProgram::uniformMatrix4fv(const string name, GLuint count, const GLfloat *v)
{
    glUniformMatrix4fv(ensureUniform(name), count, GL_FALSE, v);
}

void ShaderProgram::uniformMatrix3fv(const string name, GLuint count, const GLfloat *v)
{
    glUniformMatrix3fv(ensureUniform(name), count, GL_FALSE, v);
}

void ShaderProgram::vertexAttribPointer(const string name, GLuint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    glEnableVertexAttribArray(ensureAttrib(name));
    glVertexAttribPointer(_attributes[name], size, type, GL_FALSE, stride, pointer);
}

Texture &ShaderProgram::getTexture(const string name)
{
    if(_textures.find(name) == _textures.end())
    {
        Texture tex;
        glGenTextures(1, &tex.id);
        tex.target = GL_TEXTURE_2D;
        _textures[name] = tex;
    }
    return _textures[name];
}

GLint ShaderProgram::ensureUniform(const string &name)
{
    if(_uniforms.find(name) == _uniforms.end())
        _uniforms[name] = glGetUniformLocation(_program, name.c_str());
    return _uniforms[name];
}

GLint ShaderProgram::ensureAttrib(const string &name)
{
    if(_attributes.find(name) == _attributes.end())
        _attributes[name] = glGetAttribLocation(_program, name.c_str());
    return _attributes[name];
}
