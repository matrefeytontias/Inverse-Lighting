#ifndef INC_SHADER_PROGRAM
#define INC_SHADER_PROGRAM

#include <map>
#include <string>

#include <glad/glad.h>

using namespace std;

namespace invLight
{

class ShaderProgram
{
public:
	ShaderProgram(const char *vertexPath, const char *fragmentPath);
	~ShaderProgram();
	void use();
	void uniform1f(string name, float v);
	void uniform2f(string name, float v1, float v2);
	void uniform3f(string name, float v1, float v2, float v3);
	void uniform4f(string name, float v1, float v2, float v3, float v4);
	void uniformMatrix4fv(string name, GLuint count, const GLfloat *v);
	void uniformMatrix3fv(string name, GLuint count, const GLfloat *v);
	void vertexAttribPointer(string name, GLuint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	GLint ensureUniform(string name);
	GLint ensureAttrib(string name);
private:
	GLuint _vao, _program, _vertexShader, _fragmentShader;
	map<string, GLint> _uniforms;
	map<string, GLint> _attributes;
};

}

#endif
