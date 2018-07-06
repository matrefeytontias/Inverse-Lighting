#ifndef INC_QUAD_RENDER_CONTEXT
#define INC_QUAD_RENDER_CONTEXT

#include "RenderContext.h"
#include "ShaderProgram.h"

namespace invLight
{

class QuadRenderContext : public RenderContext
{
public:
	QuadRenderContext(ShaderProgram &_program);
	virtual void render() override;
};

}

#endif
