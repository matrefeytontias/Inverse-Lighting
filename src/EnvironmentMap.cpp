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
	checkGLerror();
	_map = _skyboxProgram.getTexture("uEnvironment");
	checkGLerror();
	glBindTexture(GL_TEXTURE_2D, _map.id);
	checkGLerror();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, environmentMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(environmentMap);
}

EnvironmentMap::~EnvironmentMap()
{
	
}

void EnvironmentMap::render(Camera3D &cam, Matrix4f &invProjMat)
{
	_skyboxProgram.use();
	_skyboxProgram.uniformMatrix4fv("uInvP", 1, invProjMat.data());
	_skyboxProgram.uniformMatrix4fv("uV", 1, cam.m_viewMatr.data());
	_skyboxContext.render();
}
