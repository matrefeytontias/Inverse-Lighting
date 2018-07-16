#ifndef INC_MODEL_RENDER_CONTEXT
#define INC_MODEL_RENDER_CONTEXT

#include <map>
#include <string>
#include <vector>

#include <glad/glad.h>
#include "tiny_gltf.h"

#include "RenderContext.h"
#include "ShaderProgram.h"

using namespace std;
using namespace tinygltf;

namespace invLight
{

class ModelRenderContext : public Model, public RenderContext
{
private:
    vector<GLuint> _textureIds;
    vector<GLint> _textureLocations;
    vector<GLuint> _activeTextures;
    GLuint _indicesCount;
    GLenum _indicesType;
    GLenum _drawingMode;
    
public:
    
    ModelRenderContext(ShaderProgram &_program) : Model(), RenderContext(_program) { }
    
    /**
     * Creates textures necessary for the rendering.
     */
    void initForRendering();
    
    /**
     * Fills the internal buffers for rendering. Also creates various
     * vertex attributes according to the underlying glTF file.
     */
    void armForRendering();
    
    GLuint indicesCount() const { return _indicesCount; }
    GLenum indicesType() const { return _indicesType; }
    
    /**
     * Draws the model using the currently bound shader program.
     */
    void render() override;
    
    int activeTexturesCount() const { return _activeTextures.size(); }
    
    /**
     * Clears up resources after use.
     */
    void cleanup();
};

}

#endif
