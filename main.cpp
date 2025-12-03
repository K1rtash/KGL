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

#include <iostream>

void error_callback(int error, const char* description);

int main() {
    int width = 800, height = 800, msaa = 16;
    bool fullscreen = true;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_SAMPLES, msaa);

    GLFWmonitor *monitor = NULL;

    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // Hinting these properties lets us use "borderless full screen" mode.
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        width = mode->width;  // Use our 'desktop' resolution for window size
        height = mode->height; // to get a 'full screen borderless' window.
    }

    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Context", monitor, NULL);

    glfwMakeContextCurrent(window);

    glfwSetErrorCallback(error_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

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
    
    ShaderProgram shaderProgram{"../shaders/basic.vert", "../shaders/basic.frag"};

    VAO vao;

    VBO vbo{vertices, sizeof(vertices), GL_STATIC_DRAW};
    EBO ebo{indices, sizeof(indices), GL_STATIC_DRAW};

    vao.LinkAttrib(&vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    vao.LinkAttrib(&vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    vao.LinkAttrib(&vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
    
    Texture texture{"../textures/verduras.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE};
    texture.texUnit(&shaderProgram, "tex0", 0);
    texture.Unbind();
    
    VAO cubeVao;
    VBO cubeVbo{cubeVertices, sizeof(cubeVertices), GL_STATIC_DRAW};
    EBO cubeEbo{cubeIndices, sizeof(cubeIndices), GL_STATIC_DRAW};
    cubeVao.LinkAttrib(&cubeVbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    cubeVao.LinkAttrib(&cubeVbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    cubeVao.LinkAttrib(&cubeVbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    
    cubeVao.Unbind();
    cubeVbo.Unbind();
    cubeEbo.Unbind();
    
    Texture cubeTex{"../textures/bricks.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE};
    cubeTex.texUnit(&shaderProgram, "tex0", 0);
    cubeTex.Unbind();
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    Camera camera{width, height, glm::vec3(0.0f, 0.0f, 2.0f)};

    // Pirámide fija en (-2, 0, -3)
    glm::mat4 modelPyramid = glm::mat4(1.0f);
    modelPyramid = glm::translate(modelPyramid, glm::vec3(-1.0f, 0.0f, -3.0f));

    // Cubo fijo en (2, 0, -3)
    glm::mat4 modelCube = glm::mat4(1.0f);
    modelCube = glm::translate(modelCube, glm::vec3(1.0f, 0.0f, -3.0f));


    double prev_s = glfwGetTime();
    double fps_update_countdown = 0.1;
    while(!glfwWindowShouldClose(window)) 
    {
        double current_s = glfwGetTime();
        double delta_time = current_s - prev_s;
        prev_s = current_s;

        //Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) shaderProgram.Reload();

        //FPS Counter
        fps_update_countdown -= delta_time;
        if(fps_update_countdown <= 0.0 && delta_time > 0.0) {
            double fps = 1.0 / delta_time;
            const char* title = (std::format("OpenGL Context | {} FPS", (int)fps)).c_str();
            glfwSetWindowTitle(window, title);
            fps_update_countdown = 0.1;
        }

        //Update viewport
        glfwGetWindowSize( window, &width, &height );
		glViewport( 0, 0, width, height );
        camera.Aspect(width, height);

        //Render {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shaderProgram.Activate();        

        camera.Inputs(window);
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");
        int modelLoc = glGetUniformLocation(shaderProgram.id, "model");

        /*texture.Bind();
        vao.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);*/

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPyramid));
        vao.Bind();
        texture.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCube));
        cubeVao.Bind();
        cubeTex.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices)/sizeof(int), GL_UNSIGNED_INT, 0);
        
        // } Render
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    vbo.Delete();
    ebo.Delete();
    vao.Delete();
    shaderProgram.Delete();
    texture.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}