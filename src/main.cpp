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
    { //     COORDINATES     /        COLORS        /    TexCoord    /       NORMALS     //
        -1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
        1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f
    };

    // Indices for vertices order
    GLuint indices[] =
    {
        0, 1, 2,
        0, 2, 3
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
        /*shaderProgram.AddUniform("model");
        shaderProgram.AddUniform("view");
        shaderProgram.AddUniform("proj");
        shaderProgram.AddUniform("tex0");
        //shaderProgram.AddUniform("useTexture");
        //glUniform1i(shaderProgram.GetUniformLoc("useTexture"), true);
        shaderProgram.AddUniform("lightColor");*/
    ShaderProgram lightShader{"../shaders/light.vert", "../shaders/light.frag"};
        /*lightShader.AddUniform("model");
        lightShader.AddUniform("cameraMatrix");
        lightShader.AddUniform("lightColor");*/
     
    VAO ligthVao;
        VBO lightVbo{lightVertices, sizeof(lightVertices), GL_STATIC_DRAW};
        EBO lightEbo{lightIndices, sizeof(lightIndices), GL_STATIC_DRAW};
        ligthVao.LinkAttrib(&lightVbo, 0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (void*)0);
        ligthVao.Unbind();
        lightEbo.Unbind();
        lightVbo.Unbind();
    
        glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
    
    VAO modelVao;
        VBO modelVbo{vertices, sizeof(vertices), GL_STATIC_DRAW};
        EBO modelEbo{indices, sizeof(indices), GL_STATIC_DRAW};
        modelVao.LinkAttrib(&modelVbo, 0, 3, GL_FLOAT, 11 * sizeof(GLfloat), (void*)0);
        modelVao.LinkAttrib(&modelVbo, 1, 3, GL_FLOAT, 11 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        modelVao.LinkAttrib(&modelVbo, 2, 2, GL_FLOAT, 11 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        modelVao.LinkAttrib(&modelVbo, 3, 3, GL_FLOAT, 11 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));
        modelVao.Unbind();
        modelEbo.Unbind();
        modelVbo.Unbind();
        
        glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, modelPos);

	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.id, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniform4f(glGetUniformLocation(shaderProgram.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.id, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    if (windowConfig.msaa) glEnable(GL_MULTISAMPLE);
    
    Texture planksTex{"../textures/planks.png", GL_TEXTURE_2D, 0, GL_UNSIGNED_BYTE};
    planksTex.texUnit(&shaderProgram, "tex0", 0);
    Texture planksSpec{"../textures/planksSpec.png", GL_TEXTURE_2D, 1, GL_UNSIGNED_BYTE};
    planksSpec.texUnit(&shaderProgram, "tex1", 1);

    Camera camera{windowConfig.LOGICAL_WIDTH, windowConfig.LOGICAL_HEIGHT, glm::vec3(0.0f, 0.0f, 1.0f)};

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
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { 
            shaderProgram.Reload();
            lightShader.Reload();
        }

        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) lightPos.y += 0.2f;
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) lightPos.y -= 0.2f;
        if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) lightPos = camera.Position;

        fps_update_countdown -= delta_time;
        if(fps_update_countdown <= 0.0 && delta_time > 0.0) {
            double fps = 1.0 / delta_time;
            const char* title = (std::format("OpenGL Context | {} FPS | Time: {}", (int)fps, temp_time_passed).c_str());
            glfwSetWindowTitle(window, title);
            fps_update_countdown = 0.1;
        }

        lightModel = glm::translate(glm::mat4(1.0f), lightPos);
        modelMatrix = glm::translate(modelMatrix, modelPos);

        camera.Inputs(window, delta_time);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        //Render {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //pasar datos al shader principal
            shaderProgram.Activate();      // PARA UNIFORMS EL SHADER DEBE ESTAR ACTIVO ANTES SIEMPRE
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix)); //objetos
            glUniform4f(glGetUniformLocation(shaderProgram.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w); //color de la luz
            glUniform3f(glGetUniformLocation(shaderProgram.id, "lightPos"), lightPos.x, lightPos.y, lightPos.z);  //fuente de la luz
            glUniform3f(glGetUniformLocation(shaderProgram.id, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z); //camara del jugador
            camera.Matrix(&shaderProgram, "cameraMatrix");

            //render modelo 1
            modelVao.Bind();
            planksTex.Bind();
            planksSpec.Bind();
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
            
            //render luz
            lightShader.Activate(); 
            glUniformMatrix4fv(glGetUniformLocation(lightShader.id, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	        glUniform4f(glGetUniformLocation(lightShader.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
            camera.Matrix(&lightShader, "cameraMatrix");
            ligthVao.Bind();
            glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
            
        // } Render
            glfwSwapBuffers(window);
        }
        
        shaderProgram.Delete();
        lightShader.Delete();
        ligthVao.Delete();
        lightEbo.Delete();
        lightVbo.Delete();
        modelVao.Delete();
        modelEbo.Delete();
        modelVbo.Delete();
        planksTex.Delete();
        planksSpec.Delete();

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