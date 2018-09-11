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
    void precomputeIrradiance(int width = 0, int height = 0);
    void precomputeSpecular();
    void render(Camera3D &cam, Matrix4f &invProjMat);
    
    const Texture& getMap() { return _map; }
    const Texture& getIrradianceMap() { return _irradianceMap; }
    const Texture& getSpecularMap() { return _specularMap; }
    const Texture& getBRDFMap() { return _brdfMap; }
private:
    Texture _map, _irradianceMap, _specularMap, _brdfMap;
    ShaderProgram _skyboxProgram;
    QuadRenderContext _skyboxContext;
};

}

#endif
