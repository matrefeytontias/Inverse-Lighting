// Heavily adapted from https://github.com/sasmaster/TrackballControls

#ifndef INC_TRACKBALL_CONTROLS
#define INC_TRACKBALL_CONTROLS


#include <stdint.h>

#include <Eigen/Eigen>
#include <GLFW/glfw3.h>

using namespace Eigen;

namespace invLight
{

class Camera3D
{
public:
    Vector3f m_eye, m_up;
    Matrix4f m_viewMatr;
    
    Camera3D(const Vector3f &pos);
    void lookAt(const Vector3f &target);
};

/**
* This class is a singleton.
*/
class TrackballControls
{
public:
    /**
    * @param cam    pointer to active camera object.
    * @param screenSize size of the window screen.
    */
    static TrackballControls &getInstance(Camera3D *cam, Vector4f screenSize);
    
    void init(GLFWwindow *win);
    void update();
    void mouseUp();
    void keyUp();
    void mouseDown(int button, int action, int mods, int xpos, int ypos);
    void mouseMove(int xpos, int ypos);
    void keyDown(int key);
    void mouseWheel(double xoffset, double yoffset);
    
    bool m_enabled;
    float m_rotateSpeed;
    float m_zoomSpeed;
    float m_panSpeed;
    bool m_noRotate;
    bool m_noPan;
    bool m_noZoom;
    bool m_noRoll;
    bool m_staticMoving;
    float m_dynamicDampingFactor;
    float m_minDistance;
    float m_maxDistance;
    
private:
    TrackballControls(Camera3D *cam, Vector4f screenSize);
    Vector3f getMouseProjectionOnBall(int clientX, int clientY);
    Vector2f getMouseOnScreen(int clientX, int clientY);
    
    void rotateCamera();
    void zoomCamera();
    void panCamera();
    void checkDistances();
    
    enum class TCB_STATE : uint8_t
    {
        NONE,
        ROTATE,
        ZOOM,
        PAN
    };
    
    Camera3D *m_pCam;
    Vector4f m_screen;
    
    Vector3f m_target;
    Vector3f m_lastPos;
    TCB_STATE m_state;
    TCB_STATE m_prevState;
    Vector3f m_eye;
    Vector3f m_rotStart;
    Vector3f m_rotEnd;
    Vector2f m_zoomStart;
    Vector2f m_zoomEnd;
    Vector2f m_panStart;
    Vector2f m_panEnd;
    
    static uint32_t m_keys[3];
};

inline Vector2f TrackballControls::getMouseOnScreen(int clientX, int clientY)
{
    return Vector2f(
        (float)(clientX - m_screen[0]) / m_screen[2],
        (float)(clientY - m_screen[1]) / m_screen[3]
    );
}


inline void TrackballControls::mouseUp()
{
    if(!m_enabled)
        return;
    m_state = TCB_STATE::NONE;
}

inline void TrackballControls::keyUp()
{
    if (!m_enabled)
        return;
    m_state = m_prevState;
}

}

#endif//INC_TRACKBALL_CONTROLS
