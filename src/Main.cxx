#define GLFW_INCLUDE_NONE
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Model.h"
#include "Mesh.h"

#include <iostream>
#include <algorithm>

enum KGLenum {WINDOWMODE_RESIZABLE, WINDOWMODE_FULLSCREEN, WINDOWMODE_WINDOWED_BORDERLESS, WINDOWMODE_WINDOWED};

struct KGL_WindowConfig {
    int width = 1280, height = 720, windowMode = WINDOWMODE_WINDOWED, msaa = 16, vsync = 1;
    const float LOGICAL_WIDTH = 1920.0f, LOGICAL_HEIGHT = 1080.0f, LOGICAL_ASPECT = LOGICAL_WIDTH / LOGICAL_HEIGHT;
};

double mouseScrollDelta;

void error_callback(int error, const char* description);
void resize_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setLogicalPresentation(int width, int height, const float LOGICAL_ASPECT);
void HSVtoRGB(float h, float s, float v, float &r, float &g, float &b);

int main(void) {
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
    glfwSetScrollCallback(window, scroll_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(windowConfig.vsync);

    setLogicalPresentation(windowConfig.width, windowConfig.height, windowConfig.LOGICAL_ASPECT);
        
    Shader shaderProgram{"../shaders/vert.glsl", "../shaders/frag.glsl"};
    Shader lightShaderProgram{"../shaders/light/vert.glsl", "../shaders/light/frag.glsl"};
    Camera camera{windowConfig.LOGICAL_WIDTH, windowConfig.LOGICAL_HEIGHT, glm::vec3(0.0f, 0.0f, 1.0f)};

    Vertex vertices[] =
    { //               COORDINATES           /            COLORS          /           NORMALS         /       TEXTURE COORDINATES    //
        Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
    };

    // Indices for vertices order
    GLuint indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    RawTexData td0, td1;
    td0.file = "../textures/planks.png";    
    td1.file = "../textures/planksSpec.png";

    if (Texture::resolveData(&td0) && Texture::resolveData(&td1) ) {
        printf("tex data resolved!");
    } else {
        printf("TEX DATA CANT BE RESOLVED");
        return 1;
    }

    Texture textures[]
	{
		Texture(&td0, 0, TextureType::DIFFUSE),
		Texture(&td1, 1, TextureType::SPECULAR)
	};

    std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
    std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));

    Mesh model{verts, ind, tex};    
    glm::quat rot_modelo = glm::quat{glm::angleAxis(glm::radians(270.0f), glm::vec3(0,1,0))};
    Transform modelTrans;
    modelTrans.r = rot_modelo;
    modelTrans.s = glm::vec3{1.0f, 1.0f, 1.0f};
    modelTrans.t = glm::vec3{1.0, 1.0, 1.0};

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

    std::vector <Vertex> Lverts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> Lind(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));

    Mesh lightModel{Lverts, Lind, tex};    
    Transform lightModelTrans;
    lightModelTrans.r = glm::quat{glm::angleAxis(glm::radians(270.0f), glm::vec3(0,1,0))};
    lightModelTrans.s = glm::vec3{1.0f, 1.0f, 1.0f};
    lightModelTrans.t = glm::vec3{0.5, 1.5, 0.5};
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (windowConfig.msaa) glEnable(GL_MULTISAMPLE);   
    
    const double FIXED_TIMESTEP = 1.0 / 60.0;
    const unsigned int MAX_STEPS = 5;
    double prevTime = glfwGetTime();
    double currentTime = 0.0;
    double deltaTime = 0.0;
    double accumulator = 0.0;
    float h = 0, s = 0.0, v = 1.0;
    while(!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();
        if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            accumulator = 0.0;
            continue;
        }
                
        currentTime = glfwGetTime();
        deltaTime = std::min((currentTime - prevTime), 0.25);
        prevTime = currentTime;
        accumulator += deltaTime;
        unsigned int steps = 0;

        camera.captureMouse(window);

        while ( accumulator >= FIXED_TIMESTEP && steps < MAX_STEPS ) {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
            if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { 
                shaderProgram.Reload();
            }
            if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
                lightModelTrans.t = camera.getTransform()->pos;
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
            if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
                std::cout << "HSV: " << h << ", " << s << ", " << v << std::endl;
                camera.setViewport(45.0f, 0.1f, 100.0f);
            }

            double fps = 1.0 / deltaTime; 
            double msPerFrame = deltaTime * 1000.0;
            string title = std::format("OpenGL Context | {} FPS | Ms/frame: {}", (int)fps, (float)msPerFrame);
            glfwSetWindowTitle(window, title.c_str());

            float lr = 0, lg = 0, lb =0;
            HSVtoRGB(h/360.0f, s, v, lr, lg, lb);
    
            shaderProgram.Activate();
            glUniform3fv(glGetUniformLocation(shaderProgram.id, "lightPos"), 1, glm::value_ptr(lightModelTrans.t));
            glUniform4f(shaderProgram.GetUniformLoc("lightColor"), lr, lg, lb, 1.0f);            
            lightShaderProgram.Activate();
            glUniform4f(lightShaderProgram.GetUniformLoc("lightColor"), lr, lg, lb, 1.0f);
            //outlineShader.Activate();
            //glUniform1f(outlineShader.GetUniformLoc("outline"), 0.08f);

            camera.fixedInput(window, FIXED_TIMESTEP);

            glm::vec3 eje_rot{1.0f, 0.0f, 0.0f};
            glm::quat rot_aplicar{glm::angleAxis(glm::radians(10.0f), eje_rot)};
            glm::quat nueva_rot = rot_aplicar * rot_modelo;
            //rot_modelo = glm::normalize(nueva_rot);
            modelTrans.r = rot_modelo;

            accumulator -= FIXED_TIMESTEP;
            steps++;
        }        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                
        double alpha = glm::clamp((accumulator / FIXED_TIMESTEP), 0.0, 1.0);        
        
        camera.updateScroll(mouseScrollDelta);
        camera.update(alpha);
        
        model.Draw(&shaderProgram, &camera, &modelTrans);
        lightModel.Draw(&lightShaderProgram, &camera, &lightModelTrans);
        
        glfwSwapBuffers(window);
        mouseScrollDelta = 0;
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mouseScrollDelta += yoffset;
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