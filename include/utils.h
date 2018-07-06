#ifndef INC_UTILS
#define INC_UTILS

#include <string>

#include <glad/glad.h>

void setwd(char **argv);
std::string getFileContents(const char *path);
int createShaderFromSource(int type, const char *path);
void printShaderLog(GLuint shader);

#define checkGLerror() _checkGLerror(__FILE__, __LINE__)
void _checkGLerror(const char *file, int line);

#define trace(s) std::cerr << __FILE__ << ":" << __LINE__ << " : " << s << std::endl

void displayTexture(GLint texture, float dx = 0.f, float dy = 0.f);

#endif
