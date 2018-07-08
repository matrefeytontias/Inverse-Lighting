// Heavily adapted from https://github.com/sasmaster/TrackballControls

#include <cmath>
#include "TrackballControls.h"

#define FloatInfinity std::numeric_limits<float>::infinity()
#define SQRT1_2 0.7071067811865476

#ifndef isnan
inline bool isnan(double num) { return num != num; }
#endif

using namespace Eigen;

namespace invLight
{

uint32_t TrackballControls::m_keys[3] = { GLFW_KEY_A , GLFW_KEY_S, GLFW_KEY_D };

static TrackballControls *s_instance = nullptr;

Camera3D::Camera3D(const Vector3f &pos) : m_eye(pos), m_up(0.f, 1.f, 0.f),
    m_viewMatr(Matrix4f::Constant(1.f)) { }

void Camera3D::lookAt(const Vector3f& target)
{
    Vector3f zAxis = m_eye - target,
        xAxis = m_up.cross(zAxis),
        yAxis = zAxis.cross(xAxis);
    xAxis.normalize();
    yAxis.normalize();
    zAxis.normalize();
    
    m_viewMatr << xAxis, yAxis, zAxis, Vector3f(0.f, 0.f, 0.f),
        -xAxis.dot(m_eye), -yAxis.dot(m_eye), -zAxis.dot(m_eye),  1.f;
    m_viewMatr.transposeInPlace();
}

TrackballControls& TrackballControls::getInstance(Camera3D *cam, Vector4f screenSize)
{
    static TrackballControls instance(cam, screenSize);
    s_instance = &instance;
    
    return instance;
}

//------------------------   Static callbacks -------------------------------------------------
static void mouseButtonCallBack(GLFWwindow *win, int button, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(win, &xpos, &ypos);
        s_instance->mouseDown(button, action, mods, (int)xpos, (int)ypos);
    }
    else if(action == GLFW_RELEASE)
        s_instance->mouseUp();

}
static void mouseMoveCallBack(GLFWwindow *, double xpos, double ypos)
{
    s_instance->mouseMove((int)xpos, (int)ypos);
}

static void mouseScrollCallBack(GLFWwindow *, double xpos, double ypos)
{
    s_instance->mouseWheel(xpos, ypos);
}

static void keyboardCallBack(GLFWwindow *, int key, int, int action, int)
{
    if(action == GLFW_PRESS)
        s_instance->keyDown(key);
    else if(action == GLFW_RELEASE)
        s_instance->keyUp();
}
//-----------------------------------------------------------------------------------------------


TrackballControls::TrackballControls(Camera3D *cam, Vector4f screenSize) :
    m_enabled(true),
    m_rotateSpeed(1.f),
    m_zoomSpeed(1.2f),
    m_panSpeed(.3f),
    m_noRotate(false),
    m_noPan(false),
    m_noZoom(false),
    m_noRoll(false),
    m_staticMoving(false),
    m_dynamicDampingFactor(.2f),
    m_minDistance(0.f),
    m_maxDistance(FloatInfinity),
    m_pCam(cam),
    m_screen(screenSize),
    m_target(0.f, 0.f, 0.f),
    m_lastPos(0.f, 0.f, 0.f),
    m_state(TCB_STATE::NONE),
    m_prevState(TCB_STATE::NONE),
    m_eye(0.f, 0.f, 0.f),
    m_rotStart(0.f, 0.f, 0.f),
    m_rotEnd(0.f, 0.f, 0.f),
    m_zoomStart(0.f, 0.f),
    m_zoomEnd(0.f, 0.f),
    m_panStart(0.f, 0.f),
    m_panEnd(0.f, 0.f) { }

void TrackballControls::init(GLFWwindow *win)
{
    glfwSetCursorPosCallback(win, mouseMoveCallBack);
    glfwSetMouseButtonCallback(win, mouseButtonCallBack);
    glfwSetScrollCallback(win, mouseScrollCallBack);
    glfwSetKeyCallback(win, keyboardCallBack);
}

void TrackballControls::update()
{
    m_eye = m_pCam->m_eye - m_target;
    if(!m_noRotate)
        rotateCamera();
    if(!m_noZoom)
        zoomCamera();
    if(!m_noPan)
        panCamera();
    
    m_pCam->m_eye = m_target + m_eye;
    
    checkDistances();
    
    m_pCam->lookAt(m_target);
    
    if((m_lastPos - m_pCam->m_eye).squaredNorm() > 0.0f)
        m_lastPos = m_pCam->m_eye;
}

void TrackballControls::rotateCamera()
{
    float angle = (float)acos(m_rotStart.dot(m_rotEnd) / m_rotStart.norm() / m_rotEnd.norm());
    
    if(!isnan(angle) && angle != 0.0f)
    {
        Vector3f axis = m_rotStart.cross(m_rotEnd);
        axis.normalize();
        
        if(isnan(axis[0]) || isnan(axis[1]) || isnan(axis[2]))
            return;
        
        angle *= m_rotateSpeed;
        Quaternionf quaternion(AngleAxisf(-angle, axis));
        
        m_eye = quaternion * m_eye;
        
        m_pCam->m_up = quaternion * m_pCam->m_up;
        m_rotEnd = quaternion * m_rotEnd;
        
        if(m_staticMoving)
            m_rotStart = m_rotEnd;
        else
        {
            quaternion = AngleAxisf(angle * (m_dynamicDampingFactor - 1.f), axis);
            m_rotStart = quaternion * m_rotStart;
        }
    }
}


void TrackballControls::zoomCamera()
{
    float factor = 1.f + (float)(m_zoomEnd[1] - m_zoomStart[1]) * m_zoomSpeed;
    
    if(factor != 1.f && factor > 0.f)
    {
        m_eye *= factor;
        
        if(m_staticMoving)
            m_zoomStart = m_zoomEnd;
        else
            m_zoomStart[1] += (float)(m_zoomEnd[1] - m_zoomStart[1]) * m_dynamicDampingFactor;
    }
}

void TrackballControls::panCamera()
{
    Vector2f mouseChange = m_panEnd - m_panStart;
    
    if(mouseChange.norm() != 0.f)
    {
        mouseChange *= m_eye.norm() * m_panSpeed;
        
        Vector3f pan = m_eye.cross(m_pCam->m_up);
        pan.normalize();
        
        pan *= mouseChange[0];
        
        Vector3f camUpClone = m_pCam->m_up.normalized();
        
        camUpClone *= mouseChange[1];
        pan += camUpClone;
        
        m_pCam->m_eye += pan;
        m_target += pan;
        
        if(m_staticMoving)
            m_panStart = m_panEnd;
        else
            m_panStart += (m_panEnd - m_panStart) * m_dynamicDampingFactor;
    }
}

void TrackballControls::checkDistances()
{
    if(!m_noZoom || !m_noPan)
    {
        if(m_pCam->m_eye.squaredNorm() > m_maxDistance * m_maxDistance)
        {
            m_pCam->m_eye.normalize();
            m_pCam->m_eye *= m_maxDistance;
        }

        if(m_eye.squaredNorm() < m_minDistance * m_minDistance)
        {
            m_eye.normalize();
            m_eye *= m_minDistance;
            m_pCam->m_eye = m_target + m_eye;
        }
    }
}

Vector3f TrackballControls::getMouseProjectionOnBall(int clientX, int clientY)
{
    Vector3f mouseOnBall(
        ((float)clientX - (float)m_screen[2] * .5f) / (float)(m_screen[2] * .5f),
        ((float)m_screen[3] * .5f - (float)clientY) / (float)(m_screen[3] * .5f),
        0.f
    );
    
    double length = (double)mouseOnBall.norm();
    
    if(m_noRoll)
    {
        if(length < SQRT1_2)
            mouseOnBall[2] = (float)sqrt(1. - length * length);
        else
            mouseOnBall[2] = (float)(.5 / length);
    }
    else if(length > 1.)
        mouseOnBall.normalize();
    else
        mouseOnBall[2] = (float)sqrt(1. - length * length);
    
    m_eye = m_target - m_pCam->m_eye;
    
    Vector3f upClone = m_pCam->m_up.normalized();
    Vector3f projection = upClone * mouseOnBall[1];
    
    Vector3f cross = m_pCam->m_up.cross(m_eye);
    cross.normalize();
    
    cross *= mouseOnBall[0];
    projection += cross;
    
    Vector3f eyeClone = m_eye.normalized();
    projection += eyeClone * mouseOnBall[2];
    
    return projection;
}

void TrackballControls::mouseDown(int button, int, int, int xpos, int ypos)
{
    if(!m_enabled)
        return;
    
    if(m_state == TCB_STATE::NONE)
    {
        if(button == GLFW_MOUSE_BUTTON_RIGHT)
            m_state = TCB_STATE::PAN;
        else
            m_state = TCB_STATE::ROTATE;
    }
    
    if(m_state == TCB_STATE::ROTATE && !m_noRotate)
    {
        m_rotStart = getMouseProjectionOnBall(xpos, ypos);
        m_rotEnd = m_rotStart;
    }
    else if(m_state == TCB_STATE::ZOOM && !m_noZoom)
    {
        m_zoomStart = getMouseOnScreen(xpos, ypos);
        m_zoomEnd = m_zoomStart;
    }
    else if(m_state == TCB_STATE::PAN && !m_noPan)
    {
        m_panStart = getMouseOnScreen(xpos, ypos);
        m_panEnd = m_panStart;
    }
}

void TrackballControls::keyDown(int key)
{
    if(!m_enabled)
        return;
    
    m_prevState = m_state;
    
    if(m_state != TCB_STATE::NONE)
        return;
    else if(key == GLFW_KEY_A /* event.keyCode == keys[ STATE.ROTATE ]*/ && !m_noRotate)
        m_state = TCB_STATE::ROTATE;
    else if(key == GLFW_KEY_S /* event.keyCode == keys[ STATE.ZOOM ]*/ && !m_noZoom)
        m_state = TCB_STATE::ZOOM;
    else if(key == GLFW_KEY_D /* event.keyCode == keys[ STATE.PAN ]*/ && !m_noPan)
        m_state = TCB_STATE::PAN;
}

void TrackballControls::mouseWheel(double, double yoffset)
{
    if(!m_enabled)
        return;
    
    float delta = 0.f;
    
    if(yoffset != 0.)
        delta = (float)yoffset / 3.f;

    m_zoomStart[1] += delta * .05f;
}

void TrackballControls::mouseMove(int xpos, int ypos)
{
    if(!m_enabled)
        return;
    
    if(m_state == TCB_STATE::ROTATE && !m_noRotate)
        m_rotEnd = getMouseProjectionOnBall(xpos, ypos);
    else if(m_state == TCB_STATE::ZOOM && !m_noZoom)
        m_zoomEnd = getMouseOnScreen(xpos, ypos);
    else if(m_state == TCB_STATE::PAN && !m_noPan)
        m_panEnd = getMouseOnScreen(xpos, ypos);
}

}
