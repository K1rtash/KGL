#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"

#include <iostream>
#include <algorithm>

enum KGLenum {WINDOWMODE_RESIZABLE, WINDOWMODE_FULLSCREEN, WINDOWMODE_WINDOWED_BORDERLESS, WINDOWMODE_WINDOWED};

struct KGL_WindowConfig {
    int width = 1280, height = 720, windowMode = WINDOWMODE_WINDOWED, msaa = 16, vsync = 1;
    const float LOGICAL_WIDTH = 1920.0f, LOGICAL_HEIGHT = 1080.0f, LOGICAL_ASPECT = LOGICAL_WIDTH / LOGICAL_HEIGHT;
};

void error_callback(int error, const char* description);
void resize_callback(GLFWwindow *window, int width, int height);
void setLogicalPresentation(int width, int height, const float LOGICAL_ASPECT);
void HSVtoRGB(float h, float s, float v, float &r, float &g, float &b);

int main() {
    KGL_WindowConfig windowConfig;
    windowConfig.windowMode = WINDOWMODE_WINDOWED;
    windowConfig.vsync = 1;
    glfwInit();

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* video = glfwGetVideoMode(monitor);
    GLFWwindow* window;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_REFRESH_RATE, video->refreshRate);
    glfwWindowHint(GLFW_SAMPLES, windowConfig.msaa);
    glfwWindowHint(GLFW_RESIZABLE, !windowConfig.windowMode);


    switch (windowConfig.windowMode) 
    {
        case WINDOWMODE_FULLSCREEN:
            glfwWindowHint(GLFW_RED_BITS, video->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, video->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, video->blueBits);
            windowConfig.width = video->width;  // Use our 'desktop' resolution for window size
            windowConfig.height = video->height; // to get a 'full screen borderless' window.
            window = glfwCreateWindow(windowConfig.width, windowConfig.height, "OpenGL Context", monitor, NULL);
            break;
        case WINDOWMODE_WINDOWED_BORDERLESS:
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
            int x, y, w, h;
            glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);
            window = glfwCreateWindow(w, h, "OpenGL Context", NULL, NULL);
            glfwSetWindowPos(window, x, y);
            break;
        case WINDOWMODE_WINDOWED:
            glfwWindowHint(GLFW_RESIZABLE, 1);
            window = glfwCreateWindow(windowConfig.width, windowConfig.height, "OpenGL Context", NULL, NULL);
            glfwMaximizeWindow(window);
            break;
        default:
            window = glfwCreateWindow(windowConfig.width, windowConfig.height, "OpenGL Context", NULL, NULL);
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, &windowConfig);
    glfwGetWindowSize(window, &windowConfig.width, &windowConfig.height);

    glfwSetErrorCallback(error_callback);
    glfwSetWindowSizeCallback(window, resize_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(windowConfig.vsync);

    setLogicalPresentation(windowConfig.width, windowConfig.height, windowConfig.LOGICAL_ASPECT);

    /*Vertex vertices[] =
    { //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
        Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
    };

    // Indices for vertices order
    GLuint indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    Vertex lightVertices[] =
    { //     COORDINATES     //
        Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
        Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
        Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
        Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
        Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
        Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
        Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
        Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
    };

    GLuint lightIndices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 4, 7,
        0, 7, 3,
        3, 7, 6,
        3, 6, 2,
        2, 6, 5,
        2, 5, 1,
        1, 5, 4,
        1, 4, 0,
        4, 5, 6,
        4, 6, 7
    };

    Texture textures[] 
    {
        Texture{"../textures/planks.png", "diffuse", 0},
        Texture{"../textures/planksSpec.png", "specular", 1}
    };

    Shader lightShader{"../shaders/light/vert.glsl", "../shaders/light/frag.glsl"};
    
	std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	//std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	//Mesh floor(&verts, &ind, &tex);
    
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
    Mesh light(&lightVerts, &lightInd, &tex);    
    
    glm::vec3 objectPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 objectModel = glm::mat4(1.0f);
    objectModel = glm::translate(objectModel, objectPos);*/

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    
    Shader shaderProgram{"../shaders/vert.glsl", "../shaders/frag.glsl"};
    Shader outlineShader{"../shaders/outline/vert.glsl", "../shaders/outline/frag.glsl"};
    Model model_ground{"../models/ground/scene.gltf"};
    Model model_tree{"../models/trees/scene.gltf"};
    Model model{"../models/statue/scene.gltf"};    

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
    if (windowConfig.msaa) glEnable(GL_MULTISAMPLE);
    
    
    Camera camera{windowConfig.LOGICAL_WIDTH, windowConfig.LOGICAL_HEIGHT, glm::vec3(0.0f, 0.0f, 1.0f)};
    camera.SetAttr(10.0, 2.0, 100.0);
    
    double prev_s = glfwGetTime();
    double fps_update_countdown = 0.0;
    int temp_time_passed = 0;
    float h = 0, s = 0.0, v = 1.0;
    while(!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();
        if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) continue;
        
        
        double current_s = glfwGetTime();
        double delta_time = current_s - prev_s;
        prev_s = current_s;
        temp_time_passed++; //TEMPORAL
        
        //Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { 
            shaderProgram.Reload();
            outlineShader.Reload();
        }
        if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            lightPos = camera.Position;
            lightModel = glm::translate(glm::mat4(1.0f), camera.Position);
        }
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            s += 0.03f;
            if(s > 1.0f) s = 1.0f;  
        }        
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            s -= 0.03f;
            if(s < 0.0f) s = 0.0f;  
        }
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            h -= 1.0f;
            if(h < 0.0f) h = 0.0f;  
        }        
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            h += 1.0f;
            if(h > 360.0f) h = 360.0f;  
        }
        if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
            v -= 0.03f;
            if(v < 0.0f) v = 0.0f;  
        }        
        if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            v += 0.03f;
            if(v > 1.0f) v = 1.0f;  
        }
        if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) h = 0.0f, s = 0.0f, v = 1.0f;
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) std::cout << "HSV: " << h << ", " << s << ", " << v << std::endl;


        fps_update_countdown -= delta_time;
        if(fps_update_countdown <= 0.0 && delta_time > 0.0) {
            double fps = 1.0 / delta_time;
            const char* title = (std::format("OpenGL Context | {} FPS | Time: {}", (int)fps, temp_time_passed).c_str());
            glfwSetWindowTitle(window, title);
            fps_update_countdown = 0.1;
        }
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        float lr = 0, lg = 0, lb =0;
        HSVtoRGB(h/360.0f, s, v, lr, lg, lb);

        shaderProgram.Activate();
        glUniform3f(shaderProgram.GetUniformLoc("lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform4f(shaderProgram.GetUniformLoc("lightColor"), lr, lg, lb, 1.0f);
        outlineShader.Activate();
        glUniform1f(outlineShader.GetUniformLoc("outline"), 0.08f);
        
        camera.Inputs(window, delta_time);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);
        
        //model_ground.Draw(&shaderProgram, &camera, glm::vec3{1.0, 1.0, 1.0}, glm::quat{glm::angleAxis(glm::radians(0.0f), glm::vec3(0,0,0))}, glm::vec3{1.0, 1.0, 1.0});
        //model_tree.Draw(&shaderProgram, &camera, glm::vec3{1.0, 1.0, 1.0}, glm::quat{glm::angleAxis(glm::radians(0.0f), glm::vec3(0,0,0))}, glm::vec3{1.0, 1.0, 1.0});

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        model.Draw(&shaderProgram, &camera, glm::vec3{1.0, 2.0, 1.0}, glm::quat{glm::angleAxis(glm::radians(0.0f), glm::vec3(0,0,0))}, glm::vec3{1.0, 1.0, 1.0});
        
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        
        model.Draw(&outlineShader, &camera, glm::vec3{1.0, 2.0, 1.0}, glm::quat{glm::angleAxis(glm::radians(0.0f), glm::vec3(0,0,0))}, glm::vec3{1.0, 1.0, 1.0});        
        
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
        

        glfwSwapBuffers(window);
    }
        
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
    
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void resize_callback(GLFWwindow *window, int width, int height)
{
    KGL_WindowConfig* wcfg = (KGL_WindowConfig*)glfwGetWindowUserPointer(window);
    wcfg->width = width, wcfg->height = height;

    setLogicalPresentation(width, height, wcfg->LOGICAL_ASPECT);
}

void setLogicalPresentation(int width, int height, const float LOGICAL_ASPECT) 
{
    float aspect = (float)width / (float)height;
    int viewportX, viewportY, viewportW, viewportH;

    if (aspect > LOGICAL_ASPECT) {
        viewportH = height;
        viewportW = (int)(height * LOGICAL_ASPECT);
        viewportX = (width - viewportW) / 2;
        viewportY = 0;
    } else {
        viewportW = width;
        viewportH = (int)(width / LOGICAL_ASPECT);
        viewportX = 0;
        viewportY = (height - viewportH) / 2;
    }

    glViewport(viewportX, viewportY, viewportW, viewportH);
}

void HSVtoRGB(float h, float s, float v, float &r, float &g, float &b) {
    int i = int(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch(i % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
}