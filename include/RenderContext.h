#ifndef INC_RENDER_CONTEXT
#define INC_RENDER_CONTEXT

#include "ShaderProgram.h"

namespace invLight
{

enum
{
	VERTEX_ARRAY_BUFFER,
	ELEMENT_ARRAY_BUFFER,
	ARRAY_BUFFERS
};

class RenderContext
{
public:
	RenderContext(ShaderProgram &program) : _program(program)
	{
		glGenBuffers(ARRAY_BUFFERS, _vbos);
	}
	~RenderContext()
	{
		glDeleteBuffers(ARRAY_BUFFERS, _vbos);
	}
	
	virtual void render() = 0;
protected:
	GLuint _vbos[ARRAY_BUFFERS];
	ShaderProgram &_program;
};

}

#endif
