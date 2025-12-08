#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VBO.h"
#include "EBO.h"
#include "VAO.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"
#include "Sprite.h"

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
        -0.5f,-0.5f,-0.5f,   1.0f,1.0f,0.3f,  0.0f,0.0f,
        0.5f,-0.5f,-0.5f,   1.0f,1.0f,1.0f,  1.0f,0.0f,
        0.5f, 0.5f,-0.5f,   0.6f,0.7f,1.0f,  1.0f,1.0f,
        -0.5f, 0.5f,-0.5f,   1.0f,1.0f,1.0f,  0.0f,1.0f,

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
        -0.5f, 0.0f,  0.0f,     1.0f, 0.70f, 0.44f,	-0.5f, 0.0f,
        0.5f, 0.0f, 0.0f,     0.83f, 0.70f, 0.44f,	    0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,     0.83f, 0.70f, 0.44f,	    0.0f, 0.5f,
    };

    // Indices para ordenar los vertices en el ebo
    GLuint triangle2Dind[] =
    {
        0, 1, 2,
    };
    
    GLfloat lightVertices[] =
    { //     COORDINATES     //
        -0.1f, -0.1f,  0.1f,
        -0.1f, -0.1f, -0.1f,
        0.1f, -0.1f, -0.1f,
        0.1f, -0.1f,  0.1f,
        -0.1f,  0.1f,  0.1f,
        -0.1f,  0.1f, -0.1f,
        0.1f,  0.1f, -0.1f,
        0.1f,  0.1f,  0.1f
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

    ShaderProgram shaderProgram{"../shaders/basic.vert", "../shaders/basic.frag"};
    shaderProgram.AddUniform("model");
    shaderProgram.AddUniform("view");
    shaderProgram.AddUniform("proj");
    shaderProgram.AddUniform("tex0");
    shaderProgram.AddUniform("useTexture");
    /* NUEVO <.--.> */    
    VAO ligthVao;
    ligthVao.Bind();
    VBO lightVbo{lightVertices, sizeof(lightVertices), GL_STATIC_DRAW};
    EBO lightEbo{lightIndices, sizeof(lightIndices), GL_STATIC_DRAW};
    ligthVao.LinkAttrib(&lightVbo, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (void*)0);
    ligthVao.Unbind();
    lightEbo.Unbind();
    lightVbo.Unbind();

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

    ShaderProgram lightShader{"../shaders/light.vert", "../shaders/light.frag"};
    lightShader.AddUniform("model");
    lightShader.AddUniform("cameraMatrix");
    lightShader.AddUniform("lightColor");
    glUniformMatrix4fv(lightShader.GetUniformLoc("model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(lightShader.GetUniformLoc("lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    /* NUEVO <.xx.> */

    Texture verdurasTex{"../textures/verduras.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE};
    verdurasTex.texUnit(&shaderProgram, "tex0", 0);
    Texture brickTex{"../textures/bricks.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE};
    brickTex.texUnit(&shaderProgram, "tex0", 1);
    
    Model pyramid{vertices, sizeof(vertices), indices, sizeof(indices), &shaderProgram, &brickTex};
    pyramid.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    
    Model cube{cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices), &shaderProgram, nullptr};
    cube.setPosition(glm::vec3(1.0f, 0.0f, -2.0f));
    cube.setScale(glm::vec3(0.5f));

    Sprite modelo2d{triangle2Dvert, sizeof(triangle2Dvert), triangle2Dind, sizeof(triangle2Dind), &shaderProgram, &verdurasTex, 100, 170, windowConfig.LOGICAL_WIDTH, windowConfig.LOGICAL_HEIGHT};
    modelo2d.setPosition(-900.0f, 370.0f);
    modelo2d.setRotation(10.0f);
    modelo2d.setScale(2.0f);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    if (windowConfig.msaa) glEnable(GL_MULTISAMPLE);

    Camera camera{windowConfig.LOGICAL_WIDTH, windowConfig.LOGICAL_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f)};

    double prev_s = glfwGetTime();
    double fps_update_countdown = 0.0;
    int temp_time_passed = 0;
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
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) shaderProgram.Reload();

        fps_update_countdown -= delta_time;
        if(fps_update_countdown <= 0.0 && delta_time > 0.0) {
            double fps = 1.0 / delta_time;
            const char* title = (std::format("OpenGL Context | {} FPS | Time: {}", (int)fps, temp_time_passed).c_str());
            glfwSetWindowTitle(window, title);
            fps_update_countdown = 0.1;
        }

        //Render {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            camera.Inputs(window, delta_time);
            camera.updateMatrix(45.0f, 0.1f, 100.0f);
            
            shaderProgram.Activate();       
            camera.Matrix(&shaderProgram, "cameraMatrix");
            pyramid.Draw();
            cube.setRotation(cos((double)temp_time_passed * 0.05), glm::vec3(0.5, 0.5, 0.5));
            cube.Draw();
            modelo2d.setRotation(temp_time_passed * 0.5f);
            modelo2d.setScale(std::max(0.5, sin((double)temp_time_passed * 0.2)));
            modelo2d.Draw();

            /* nuevo */
            lightShader.Activate(); 
            camera.Matrix(&lightShader, "cameraMatrix");
            ligthVao.Bind();
            glm::mat4 lightModelMatrix = glm::mat4(1.0f);
            lightModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3{1.0f, 1.0f, 1.0f});
            glm::vec4 lightVec = glm::vec4{1.0f, 1.0f, 0.3f, 1.0f};
            glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
            /* nuevo*/
            
        // } Render
            glfwSwapBuffers(window);
        }
        
        pyramid.Delete();
        //cube.Delete();
        shaderProgram.Delete();
        lightShader.Delete();
        ligthVao.Delete();
        lightEbo.Delete();
        lightVbo.Delete();

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