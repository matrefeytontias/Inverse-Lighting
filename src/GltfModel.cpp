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

enum
{
    ARRAY_BUFFER,
    ELEMENT_ARRAY_BUFFER
};

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
        std::cerr << "Invalid number of components : " << components << std::endl;
        exit(1);
    }
}

void GltfModel::initForRendering()
{
    unsigned int n = textures.size();
    glGenBuffers(2, _buffers);
    _textureIds.resize(n);
    glGenTextures(n, &_textureIds[0]);
    std::cerr << "Filling textures" << std::endl;
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
    std::cerr << "Done initializing" << std::endl;
}

map<string, GLint> GltfModel::armForRendering(GLuint program)
{
    vector<AttributeBufferInfo> bufferFillingInfo;
    map<string, int> vertexAttribs;
    
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
    
    std::cerr << "Calculated total byte length of " << totalByteLength << std::endl;
    
    // Create the data store for the array buffer
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[ARRAY_BUFFER]);
    checkGLerror();
    glBufferData(GL_ARRAY_BUFFER, totalByteLength, NULL, GL_STATIC_DRAW);
    checkGLerror();
    
    // and fill it with the vertex attribute data
    totalByteLength = 0;
    
    for(AttributeBufferInfo &info : bufferFillingInfo)
    {
        std::cerr << "Filling attribute named " << info.name << std::endl;
        glBufferSubData(GL_ARRAY_BUFFER, totalByteLength, info.byteLength, &buffers[info.bufferIndex].data[info.byteOffset]);
        checkGLerror();
        GLint attrib = glGetAttribLocation(program, info.name.c_str());
        checkGLerror();
        // Register only if the attribute isn't unused
        if(attrib > -1)
        {
            std::cerr << "Attrib found in shader program ; registering" << std::endl;
            vertexAttribs[info.name] = attrib;
            glEnableVertexAttribArray(attrib);
            glVertexAttribPointer(attrib, info.componentsPerElement, info.componentType, GL_FALSE, 0, (const GLvoid *)totalByteLength);
            checkGLerror();
        }
        totalByteLength += info.byteLength;
    }
    
    // Fill the elements array buffer with the appropriate data
    Accessor &accessor = accessors[primitive.indices];
    BufferView &bufferView = bufferViews[accessor.bufferView];
    Buffer &buffer = buffers[bufferView.buffer];
    int bytesNb = accessor.count * GetComponentSizeInBytes(accessor.componentType) * GetTypeSizeInBytes(accessor.type);
    
    std::cerr << "Buffering index data of byte length " << bytesNb << std::endl;
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[ELEMENT_ARRAY_BUFFER]);
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
        std::cerr << "Found baseColorTexture" << std::endl;
        _textureLocations.push_back(glGetUniformLocation(program, "uAlbedoMap"));
        _activeTextures.push_back(material.values["baseColorTexture"].TextureIndex());
    }
    if(material.values.find("metallicRoughnessTexture") != material.values.end())
    {
        std::cerr << "Found metallicRoughnessTexture" << std::endl;
        _textureLocations.push_back(glGetUniformLocation(program, "uMetallicRoughness"));
        _activeTextures.push_back(material.values["metallicRoughnessTexture"].TextureIndex());
    }
    if(material.additionalValues.find("normalTexture") != material.additionalValues.end())
    {
        std::cerr << "Found normalTexture" << std::endl;
        _textureLocations.push_back(glGetUniformLocation(program, "uNormalMap"));
        _activeTextures.push_back(material.additionalValues["normalTexture"].TextureIndex());
    }
    if(material.additionalValues.find("emissiveTexture") != material.additionalValues.end())
    {
        std::cerr << "Found emissiveTexture" << std::endl;
        _textureLocations.push_back(glGetUniformLocation(program, "uEmissiveMap"));
        _activeTextures.push_back(material.additionalValues["emissiveTexture"].TextureIndex());
    }
    if(material.additionalValues.find("occlusionTexture") != material.additionalValues.end())
    {
        std::cerr << "Found occlusionTexture" << std::endl;
        _textureLocations.push_back(glGetUniformLocation(program, "uOcclusionMap"));
        _activeTextures.push_back(material.additionalValues["occlusionTexture"].TextureIndex());
    }
    
    return vertexAttribs;
}

void GltfModel::render() const
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
    glBindBuffer(GL_ARRAY_BUFFER, _buffers[ARRAY_BUFFER]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[ELEMENT_ARRAY_BUFFER]);
    glDrawElements(_drawingMode, _indicesCount, _indicesType, NULL);
}

void GltfModel::cleanup()
{
    glDeleteTextures(_textureIds.size(), &_textureIds[0]);
    glDeleteBuffers(2, _buffers);
}
