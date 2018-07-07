#define GLFW_DLL
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include <iostream>
#include <string>

#include <Eigen/Eigen>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GltfModel.h"
// Define these only in *one* .cpp file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include "utils.h"

#include "QuadRenderContext.h"
#include "ShaderProgram.h"

using namespace Eigen;
using namespace tinygltf;

static void glfw_error_callback(int error, const char *description)
{
    trace("Error " << error << " : " << description);
}

// Expects an identity matrix as input
void perspective(Matrix4f &p, float fov, float ratio, float near, float far)
{
    float d = 1 / tan(fov * M_PI / 180 / 2);
    float ir = 1. / (near - far);
    
    p(0, 0) = d;
    p(1, 1) = d * ratio;
    p(2, 2) = (near + far) * ir;
    p(3, 3) = 0;
    p(3, 2) = -1;
    p(2, 3) = 2 * near * far * ir;
}

inline void setAspectRatio(Matrix4f &p, float ratio)
{
    p(1, 1) = p(0, 0) * ratio;
}

/*
d,               0,                   0,                   0,
0, d * aspectRatio,                   0,                   0,
0,               0,   (near + far) * ir, 2 * near * far * ir,
0,               0,                  -1,                   0
*/

void keyCallback(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scanCode, action, mods);
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && !ImGui::GetIO().WantCaptureKeyboard)
        glfwSetWindowShouldClose(window, true);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

int main(int, char *argv[])
{
    setwd(argv);
    
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        trace("Couldn't initialize GLFW");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Inverse Lighting", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync
    
    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfwGL3_Init(window, true);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    
    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    
    // Load the GLTF model
    invLight::ShaderProgram modelProgram("shaders/modelVertex.glsl", "shaders/modelFragment.glsl");
    trace("Loading GLTF model ...");
    invLight::GltfModel model(modelProgram);
    TinyGLTF loader;
    std::string err;
    
    bool ret = loader.LoadASCIIFromFile(&model, &err, "DamagedHelmet/DamagedHelmet.gltf");
    // bool ret = loader.LoadBinaryFromFile(&model, &err, argv[1]); // for binary glTF(.glb)
    if (!err.empty())
        trace("Err: " << err);
    if (!ret)
    {
        trace("Failed to parse glTF");
        return -1;
    }
    
    model.initForRendering();
    model.armForRendering();
    
    trace("Model done loading");
    
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    
    Matrix4f p = Matrix4f::Identity();
    perspective(p, 90, (float)display_w / display_h, 0.1, 10);
    Matrix4f invP = p.inverse();
    
    modelProgram.use();
    modelProgram.uniformMatrix4fv("uP", 1, p.data());
    
    glViewport(0, 0, display_w, display_h);
    
    glClearDepth(1.f);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    
    // Test-display a quad with a solid color
    trace("Loading shader");
    invLight::ShaderProgram quadProgram("shaders/quadVertex.glsl", "shaders/quadFragment.glsl");
    trace("Loading context");
    invLight::QuadRenderContext quadContext(quadProgram);
    
    trace("Entering drawing loop");
    
    float ratio = (float)display_w / display_h;
    
    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &display_w, &display_h);
        float newRatio = (float)display_w / display_h;
        if(ratio != newRatio)
        {
            ratio = newRatio;
            setAspectRatio(p, ratio);
            invP = p.inverse();
            glViewport(0, 0, display_w, display_h);
        }
        
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        ImGui_ImplGlfwGL3_NewFrame();
        // ImGui::ShowDemoWindow();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        quadProgram.use();
        quadContext.render();
        
        modelProgram.use();
        modelProgram.uniform1f("uTime", glfwGetTime());
        model.render();
        
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    trace("Exiting drawing loop");
    
    model.cleanup();
    
    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    
    return 0;
}
