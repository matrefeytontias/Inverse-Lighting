#include "QuadRenderContext.h"

using namespace invLight;

static const GLfloat quadVertices[] = {
	-1.f, -1.f,
	1.f, -1.f,
	1.f, 1.f,
	-1.f, 1.f
};

static const GLuint quadIndices[] = {
	0, 1, 2,
	0, 2, 3
};

QuadRenderContext::QuadRenderContext(ShaderProgram &program) : RenderContext(program)
{
	glBindBuffer(GL_ARRAY_BUFFER, _vbos[VERTEX_ARRAY_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[ELEMENT_ARRAY_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
	_program.use();
	_program.vertexAttribPointer("POSITION", 2, GL_FLOAT, 0, 0);
}

void QuadRenderContext::render()
{
	glBindBuffer(GL_ARRAY_BUFFER, _vbos[VERTEX_ARRAY_BUFFER]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[ELEMENT_ARRAY_BUFFER]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}
