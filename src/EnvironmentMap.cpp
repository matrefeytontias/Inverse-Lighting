#include "EnvironmentMap.h"

#include <stdexcept>
#include <string>

#include "stb_image.h"

#include "utils.h"

using namespace std;
using namespace invLight;

EnvironmentMap::EnvironmentMap(const string &path) :
    _skyboxProgram("shaders/quadVertex.glsl", "shaders/skyboxFragment.glsl"),
    _skyboxContext(_skyboxProgram)
{
    int width, height, bpp;
    float *environmentMap = stbi_loadf(path.c_str(), &width, &height, &bpp, 3);
    if(!environmentMap)
        fatal("Couldn't load image " << path);
    glGenTextures(1, &_map.id);
    _map = _skyboxProgram.registerTexture("uEnvironment");
    glBindTexture(GL_TEXTURE_2D, _map.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, environmentMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    stbi_image_free(environmentMap);
}

EnvironmentMap::~EnvironmentMap()
{
    glDeleteTextures(1, &_map.id);
    glDeleteTextures(1, &_irradianceMap.id);
    glDeleteTextures(1, &_specularMap.id);
    glDeleteTextures(1, &_brdfMap.id);
}

void EnvironmentMap::precomputeIrradiance(int width, int height)
{
    ShaderProgram precompProgram("shaders/precompVertex.glsl", "shaders/precompIrradianceFragment.glsl");
    precompProgram.registerTexture("uEnvironment", _map);
    QuadRenderContext quadContext(precompProgram);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _map.id);
    if(width <= 0)
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    if(height <= 0)
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGenTextures(1, &_irradianceMap.id);
    glBindTexture(GL_TEXTURE_2D, _irradianceMap.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    GLuint fbo, rbo;
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _irradianceMap.id, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        precompProgram.use();
        quadContext.render();
    }
    else
        trace("Framebuffer is incomplete !");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
}

void EnvironmentMap::render(Camera3D &cam, Matrix4f &invProjMat)
{
    _skyboxProgram.use();
    _skyboxProgram.uniformMatrix4fv("uInvP", 1, invProjMat.data());
    _skyboxProgram.uniformMatrix4fv("uV", 1, cam.m_viewMatr.data());
    _skyboxContext.render();
}
