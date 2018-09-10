#ifndef INC_ENVIRONMENT_MAP
#define INC_ENVIRONMENT_MAP

#include <string>

#include <Eigen/Eigen>
#include <glad/glad.h>

#include "QuadRenderContext.h"
#include "ShaderProgram.h"
#include "TrackballControls.h" // Camera3D

using namespace std;

namespace invLight
{

class EnvironmentMap
{
public:
	EnvironmentMap(const string &path);
	~EnvironmentMap();
	void render(Camera3D &cam, Matrix4f &invProjMat);
private:
	Texture _map, _irradianceMap, _specularMap;
	ShaderProgram _skyboxProgram;
	QuadRenderContext _skyboxContext;
};

}

#endif
