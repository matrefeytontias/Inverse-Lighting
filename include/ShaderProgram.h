#ifndef INC_SHADER_PROGRAM
#define INC_SHADER_PROGRAM

#include <map>
#include <string>

#include <glad/glad.h>

using namespace std;

namespace invLight
{

struct Texture
{
    GLuint id = -1;
    GLenum target = GL_TEXTURE_2D;
    bool persistent = false;
};

class ShaderProgram
{
public:
    ShaderProgram(const char *vertexPath, const char *fragmentPath);
    ~ShaderProgram();
    void use();
    void uniform1f(const string &name, float v);
    void uniform2f(const string &name, float v1, float v2);
    void uniform3f(const string &name, float v1, float v2, float v3);
    void uniform4f(const string &name, float v1, float v2, float v3, float v4);
    void uniformMatrix4fv(const string &name, GLuint count, const GLfloat *v);
    void uniformMatrix3fv(const string &name, GLuint count, const GLfloat *v);
    void vertexAttribPointer(const string &name, GLuint size, GLenum type, GLsizei stride, const GLvoid *pointer);
    Texture &getTexture(const string &name);
    Texture &registerTexture(const string &name, const Texture &tex);
    unsigned int getTexturesAmount() const { return _textures.size(); }
    GLint ensureUniform(const string &name);
    GLint ensureAttrib(const string &name);
private:
    GLuint _vao, _program, _vertexShader, _fragmentShader;
    map<string, GLint> _uniforms;
    map<string, GLint> _attributes;
    map<string, Texture> _textures;
};

}

#endif
