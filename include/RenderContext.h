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
	RenderContext(ShaderProgram &_program) : program(_program)
	{
		glGenBuffers(ARRAY_BUFFERS, vbos);
	}
	~RenderContext()
	{
		glDeleteBuffers(ARRAY_BUFFERS, vbos);
	}
	
	virtual void render() = 0;
protected:
	GLuint vbos[ARRAY_BUFFERS];
	ShaderProgram &program;
};

}

#endif
