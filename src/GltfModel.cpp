#include "GltfModel.h"

#include <iostream>

#include "utils.h"

using namespace invLight;
using namespace std;

typedef struct
{
    string name;
    unsigned int bufferIndex;
    unsigned int byteOffset;
    unsigned int elements;
    unsigned int componentsPerElement;
    unsigned int bytesPerComponent;
    unsigned int byteLength;
    unsigned int componentType;
} AttributeBufferInfo;

GLenum getTextureFormatFromComponents(int components)
{
    switch(components)
    {
    case 1:
        return GL_RED;
    case 2:
        return GL_RG;
    case 3:
        return GL_RGB;
    case 4:
        return GL_RGBA;
    default:
        trace("Invalid number of components : " << components);
        exit(1);
    }
}

void GltfModel::initForRendering()
{
    unsigned int n = textures.size();
    _textureIds.resize(n);
    glGenTextures(n, &_textureIds[0]);
    trace("Filling textures");
    for(unsigned int i = 0; i < n; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        Image &image = images[textures[i].source];
        GLuint format = getTextureFormatFromComponents(image.component);
        Sampler &sampler = samplers[textures[i].sampler];
        glBindTexture(GL_TEXTURE_2D, _textureIds[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, &image.image[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    trace("Done initializing");
}

void GltfModel::armForRendering()
{
    vector<AttributeBufferInfo> bufferFillingInfo;
    
    _program.use();
    
    if (defaultScene < 0)
        defaultScene = 0;
    Scene &scene = scenes[defaultScene];
    // TODO : support more than one scene / mesh / primitive
    Node &node = nodes[scene.nodes[0]];
    Mesh &mesh = meshes[node.mesh];
    Primitive &primitive = mesh.primitives[0];
    
    _drawingMode = primitive.mode > -1 ? primitive.mode : GL_TRIANGLES;
    
    // Compute buffer info prior to filling
    // Ridiculous size for casting to void* with no warning later
    unsigned long long int totalByteLength = 0;
    
    for (auto it : primitive.attributes)
    {
        AttributeBufferInfo info;
        Accessor &accessor = accessors[it.second];
        BufferView &bufferView = bufferViews[accessor.bufferView];
        
        info.name = it.first;
        info.bufferIndex = bufferView.buffer;
        info.byteOffset = bufferView.byteOffset + accessor.byteOffset;
        info.elements = accessor.count;
        info.componentsPerElement = GetTypeSizeInBytes(accessor.type);
        info.bytesPerComponent = GetComponentSizeInBytes(accessor.componentType);
        info.byteLength = info.elements * info.componentsPerElement
            * info.bytesPerComponent;
        info.componentType = accessor.componentType;
        
        totalByteLength += info.byteLength;
        
        bufferFillingInfo.push_back(info);
    }
    
    trace("Calculated total byte length of " << totalByteLength);
    
    // Create the data store for the array buffer
    glBindBuffer(GL_ARRAY_BUFFER, _vbos[VERTEX_ARRAY_BUFFER]);
    checkGLerror();
    glBufferData(GL_ARRAY_BUFFER, totalByteLength, NULL, GL_STATIC_DRAW);
    checkGLerror();
    
    // and fill it with the vertex attribute data
    totalByteLength = 0;
    
    for(AttributeBufferInfo &info : bufferFillingInfo)
    {
        trace("Filling attribute named " << info.name);
        glBufferSubData(GL_ARRAY_BUFFER, totalByteLength, info.byteLength, &buffers[info.bufferIndex].data[info.byteOffset]);
        checkGLerror();
        _program.vertexAttribPointer(info.name, info.componentsPerElement, info.componentType, 0, (const GLvoid *)totalByteLength);
        checkGLerror();
        totalByteLength += info.byteLength;
    }
    
    // Fill the elements array buffer with the appropriate data
    Accessor &accessor = accessors[primitive.indices];
    BufferView &bufferView = bufferViews[accessor.bufferView];
    Buffer &buffer = buffers[bufferView.buffer];
    int bytesNb = accessor.count * GetComponentSizeInBytes(accessor.componentType) * GetTypeSizeInBytes(accessor.type);
    
    trace("Buffering index data of byte length " << bytesNb);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[ELEMENT_ARRAY_BUFFER]);
    checkGLerror();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytesNb, &buffer.data[bufferView.byteOffset + accessor.byteOffset], GL_STATIC_DRAW);
    checkGLerror();
    
    _indicesCount = accessor.count;
    _indicesType = accessor.componentType;
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // Get the uniform locations of the various textures
    Material &material = materials[primitive.material];
    
    if(material.values.find("baseColorTexture") != material.values.end())
    {
        trace("Found baseColorTexture");
        _textureLocations.push_back(_program.ensureUniform("uAlbedoMap"));
        _activeTextures.push_back(material.values["baseColorTexture"].TextureIndex());
    }
    if(material.values.find("metallicRoughnessTexture") != material.values.end())
    {
        trace("Found metallicRoughnessTexture");
        _textureLocations.push_back(_program.ensureUniform("uMetallicRoughness"));
        _activeTextures.push_back(material.values["metallicRoughnessTexture"].TextureIndex());
    }
    if(material.additionalValues.find("normalTexture") != material.additionalValues.end())
    {
        trace("Found normalTexture");
        _textureLocations.push_back(_program.ensureUniform("uNormalMap"));
        _activeTextures.push_back(material.additionalValues["normalTexture"].TextureIndex());
    }
    if(material.additionalValues.find("emissiveTexture") != material.additionalValues.end())
    {
        trace("Found emissiveTexture");
        _textureLocations.push_back(_program.ensureUniform("uEmissiveMap"));
        _activeTextures.push_back(material.additionalValues["emissiveTexture"].TextureIndex());
    }
    if(material.additionalValues.find("occlusionTexture") != material.additionalValues.end())
    {
        trace("Found occlusionTexture");
        _textureLocations.push_back(_program.ensureUniform("uOcclusionMap"));
        _activeTextures.push_back(material.additionalValues["occlusionTexture"].TextureIndex());
    }
}

void GltfModel::render()
{
    for(unsigned int i = 0; i < _activeTextures.size(); i++)
    {
        if(_textureLocations[i] > -1)
        {
            int j = _activeTextures[i];
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, _textureIds[j]);
            glUniform1i(_textureLocations[i], i);
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, _vbos[VERTEX_ARRAY_BUFFER]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[ELEMENT_ARRAY_BUFFER]);
    glDrawElements(_drawingMode, _indicesCount, _indicesType, NULL);
}

void GltfModel::cleanup()
{
    glDeleteTextures(_textureIds.size(), &_textureIds[0]);
    glDeleteBuffers(2, _vbos);
}
