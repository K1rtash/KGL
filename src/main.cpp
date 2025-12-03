#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VBO.h"
#include "EBO.h"
#include "VAO.h"
#include "Vector.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"
#include "Model2D.h"

#include <iostream>

void error_callback(int error, const char* description);
bool hasWindowChanged(GLFWwindow* window, int* width, int* height);

int main() {
    enum WindowMode {WINDOWMODE_FULLSCREEN, WINDOWMODE_WINDOWED, WINDOWMODE_RESIZABLE};
    int win_w = 800, win_h = 800, msaa = 16, windowMode = WINDOWMODE_WINDOWED, vsync = 1;
    glfwInit();

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* video = glfwGetVideoMode(monitor);
    GLFWwindow* window;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_REFRESH_RATE, video->refreshRate);
    glfwWindowHint(GLFW_SAMPLES, msaa);
    glfwWindowHint(GLFW_RESIZABLE, windowMode);


    switch (windowMode) 
    {
        case WINDOWMODE_FULLSCREEN:
            glfwWindowHint(GLFW_RED_BITS, video->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, video->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, video->blueBits);
            win_w = video->width;  // Use our 'desktop' resolution for window size
            win_h = video->height; // to get a 'full screen borderless' window.
            window = glfwCreateWindow(win_w, win_h, "OpenGL Context", monitor, NULL);
            break;
        case WINDOWMODE_WINDOWED:
            int x, y, w, h;
            glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);
            window = glfwCreateWindow(w, h, "OpenGL Context", NULL, NULL);
            glfwSetWindowPos(window, x, y);
            break;
        default:
                window = glfwCreateWindow(win_w, win_h, "OpenGL Context", NULL, NULL);
                break;
    }

    glfwMakeContextCurrent(window);

    glfwSetErrorCallback(error_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(vsync);

    GLfloat vertices[] =
    { //     COORDINATES     /        COLORS      /   TexCoord  //
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
        0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
        0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
        0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
    };

    // Indices para ordenar los vertices en el ebo
    GLuint indices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    GLfloat cubeVertices[] = {
        //   Posición         // Color         // TexCoord
        -0.5f,-0.5f,-0.5f,   1.0f,0.0f,0.0f,  0.0f,0.0f,
        0.5f,-0.5f,-0.5f,   0.0f,1.0f,0.0f,  1.0f,0.0f,
        0.5f, 0.5f,-0.5f,   0.0f,0.0f,1.0f,  1.0f,1.0f,
        -0.5f, 0.5f,-0.5f,   1.0f,1.0f,0.0f,  0.0f,1.0f,

        -0.5f,-0.5f, 0.5f,   1.0f,0.0f,1.0f,  0.0f,0.0f,
        0.5f,-0.5f, 0.5f,   0.0f,1.0f,1.0f,  1.0f,0.0f,
        0.5f, 0.5f, 0.5f,   1.0f,1.0f,1.0f,  1.0f,1.0f,
        -0.5f, 0.5f, 0.5f,   0.3f,0.7f,0.9f,  0.0f,1.0f
    };

    GLuint cubeIndices[] = {
        // Cara trasera
        0,1,2,  2,3,0,
        // Cara delantera
        4,5,6,  6,7,4,
        // Cara izquierda
        0,4,7,  7,3,0,
        // Cara derecha
        1,5,6,  6,2,1,
        // Cara inferior
        0,1,5,  5,4,0,
        // Cara superior
        3,2,6,  6,7,3
    };

    GLfloat triangle2Dvert[] =
    { //     COORDINATES     /        COLORS      /     TexCoord
        -0.5f, 0.0f,  0.0f,     0.83f, 0.70f, 0.44f,	-0.5f, 0.0f,
        0.5f, 0.0f, 0.0f,     0.83f, 0.70f, 0.44f,	    0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,     0.83f, 0.70f, 0.44f,	    0.0f, 0.5f,
    };

    // Indices para ordenar los vertices en el ebo
    GLuint triangle2Dind[] =
    {
        0, 1, 2,
    };
    
    ShaderProgram shaderProgram{"../shaders/basic.vert", "../shaders/basic.frag"};

    Model pyramid{vertices, sizeof(vertices), indices, sizeof(indices), "../textures/verduras.png"};
    pyramid.setPosition(glm::vec3(-1.0f, 0.0f, -2.0f));

    Model cube{cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices), "../textures/bricks.png"};
    cube.setPosition(glm::vec3(1.0f, 0.0f, -2.0f));
    cube.setScale(glm::vec3(0.5f));


    ShaderProgram shaderProgram2D{"../shaders/2d.vert", "../shaders/2d.frag"};
    Model2D modelo2d{triangle2Dvert, sizeof(triangle2Dvert), triangle2Dind, sizeof(triangle2Dind), "../textures/verduras.png"};
    

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    if (msaa) glEnable(GL_MULTISAMPLE);

    Camera camera{win_w, win_h, glm::vec3(0.0f, 0.0f, 2.0f)};

    double prev_s = glfwGetTime();
    double fps_update_countdown = 0.0;
    while(!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();
        if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) continue;

        double current_s = glfwGetTime();
        double delta_time = current_s - prev_s;
        prev_s = current_s;

        //Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) shaderProgram.Reload();

        fps_update_countdown -= delta_time;
        if(fps_update_countdown <= 0.0 && delta_time > 0.0) {
            double fps = 1.0 / delta_time;
            const char* title = (std::format("OpenGL Context | {} FPS", (int)fps)).c_str();
            glfwSetWindowTitle(window, title);
            fps_update_countdown = 0.1;
        }

        //Update viewport
            glfwGetWindowSize(window, &win_w, &win_h);
            glViewport(0, 0, win_w, win_h);
            camera.Aspect(win_w, win_h);
        
        //Render {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            shaderProgram.Activate();        
            
            camera.Inputs(window, delta_time);
            camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");
            int modelLoc = glGetUniformLocation(shaderProgram.id, "model");
            
            pyramid.Draw(shaderProgram);
            cube.Draw(shaderProgram);

            //shaderProgram2D.Activate();
            //modelo2d.Draw(shaderProgram2D);
            
        // } Render
            glfwSwapBuffers(window);
        }
        
        pyramid.Delete();
        cube.Delete();
        shaderProgram.Delete();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
    
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
    
bool hasWindowChanged(GLFWwindow* window, int* width, int* height)
{
    int w = *width, h = *height;
    glfwGetWindowSize(window, width, height);
    if( w != *width || h != *height ) return true;
    return false;
}