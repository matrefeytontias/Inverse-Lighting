#ifndef DEF_GLTF_MODEL
#define DEF_GLTF_MODEL

#include <map>
#include <string>
#include <vector>

#include <glad/glad.h>
#include "tiny_gltf.h"

using namespace std;
using namespace tinygltf;

namespace invLight
{

class GltfModel : public Model
{
private:
    GLuint _buffers[2];
    vector<GLuint> _textureIds;
    vector<GLint> _textureLocations;
    vector<GLuint> _activeTextures;
    GLuint _indicesCount;
    GLenum _indicesType;
    GLenum _drawingMode;
    
public:
    
    /**
     * Creates VBOs and textures necessary for the rendering.
     */
    void initForRendering();
    
    /**
     * Fills the internal buffers for rendering. Also creates various
     * vertex attributes according to the underlying glTF file.
     * @param   program a shader program for creating vertex attribs
     * @return  a map of vertex attribute names to indices. Doesn't include unused vertex attribs
     */
    map<string, GLint> armForRendering(GLuint program);
    
    GLuint indicesCount() const { return _indicesCount; }
    GLenum indicesType() const { return _indicesType; }
    
    /**
     * Draws the model using the currently bound shader program.
     */
    void render() const;
    
    int activeTexturesCount() const { return _activeTextures.size(); }
    
    /**
     * Clears up resources after use.
     */
    void cleanup();
};

}

#endif
