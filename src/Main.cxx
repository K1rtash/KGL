#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Mesh.h"
#include "Input.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <thread>
#include <csignal>

enum KGLenum {WINDOWMODE_RESIZABLE, WINDOWMODE_FULLSCREEN, WINDOWMODE_WINDOWED_BORDERLESS, WINDOWMODE_WINDOWED, CURSOR_FREE, CURSOR_DISABLED, CURSOR_LOCKED};


int delay_s = 0;
volatile int should_exit = 0;
std::string working_dir;

struct KGL_WindowConfig 
{
    int width = 1280, height = 720, windowMode = WINDOWMODE_WINDOWED, msaa = 16, vsync = 0, isFocused = 1;
    const float LOGICAL_WIDTH = 1920.0f, LOGICAL_HEIGHT = 1080.0f, LOGICAL_ASPECT = LOGICAL_WIDTH / LOGICAL_HEIGHT;
} windowConfig;

struct KGL_MouseState
{
    double scrollDelta;
    float dx = 0.0, dy = 0.0;
    int captured = 1;
} mouseState;

void resetMouseState(KGL_MouseState& m)
{
    m.dx = 0.0;
    m.dy = 0.0;
    m.scrollDelta = 0.0;
}


void error_callback(int error, const char* description);
void resize_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_callback(GLFWwindow* window, double xpos, double ypos);
void window_focus_callback(GLFWwindow* window, int focused);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


void setLogicalPresentation(int width, int height, const float LOGICAL_ASPECT);
void HSVtoRGB(float h, float s, float v, float &r, float &g, float &b);
void resolveArgs(int argc, char* argv[]);

void handle_sigint(int) {
    should_exit = 1;
}

std::string relativeDir(const char* relative)
{
    return working_dir + "/" + relative;
}

int main(int argc, char *argv[]) 
{
    std::signal(SIGINT, handle_sigint);
    resolveArgs(argc, argv);

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
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
    glfwSetKeyCallback(window, key_callback);

    if ( glfwRawMouseMotionSupported() ) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(windowConfig.vsync);

    setLogicalPresentation(windowConfig.width, windowConfig.height, windowConfig.LOGICAL_ASPECT);
        
    Shader shaderProgram{relativeDir("shaders/vert.glsl").c_str(), relativeDir("shaders/frag.glsl").c_str()};
    Shader lightShaderProgram{relativeDir("shaders/light/vert.glsl").c_str(), relativeDir("shaders/light/frag.glsl").c_str()};
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


    RawTexData texd1;
    int dw, dh, clrch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load(relativeDir("textures/planksSpec.png").c_str(), &dw, &dh, &clrch, 0);
    texd1 = getEmbeddedData(bytes, dw, dh, clrch);

    Texture textures[]
	{
		Texture(getDiscFileData(relativeDir("textures/planks.png").c_str()), 0, TextureType::DIFFUSE),
		Texture(texd1, 1, TextureType::SPECULAR)
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
    
    const int TARGET_TPS = 60;
    const double FIXED_TIMESTEP = 1.0 / (double)TARGET_TPS;
    const unsigned int MAX_STEPS = 5;
    double prevTime = glfwGetTime();
    double currentTime = 0.0;
    double accumulator = 0.0;
    float h = 0, s = 0.0, v = 1.0;
    double contador_ms = 0;
    int contador_ticks = 0;
    int contador_frames = 0;
    double contador_lag = 0.0;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    while(!glfwWindowShouldClose(window) && !should_exit) 
    {
        glfwPollEvents();
        
        if (!windowConfig.isFocused) {
            accumulator = 0.0;
            continue;
        }

        currentTime = glfwGetTime();
        double deltaTime = currentTime - prevTime;
        unsigned int steps = 0;
        prevTime = currentTime;
        accumulator += deltaTime;
        contador_lag += deltaTime;

        float mouseDX = mouseState.dx;
        float mouseDY = mouseState.dy;
        float mouseScroll = mouseState.scrollDelta;

        if(mouseState.captured) {
            camera.updateCursor(mouseDX, mouseDY);
            camera.updateScroll(mouseScroll);
        }

        while ( accumulator >= FIXED_TIMESTEP && steps < MAX_STEPS ) 
        {
            updateKeyboard(window);

            auto keyDown = [](KGL_KeyState key) -> bool {return (key == KGL_KeyState::Press || key == KGL_KeyState::Hold); };

            if(getKey(GLFW_KEY_ESCAPE) == KGL_KeyState::Press) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                if(!mouseState.captured) glfwSetWindowShouldClose(window, 1);
                mouseState.captured = 0;
            }

            if ( keyDown(getKey(GLFW_KEY_R)) ) 
            { 
                shaderProgram.Reload();
            }
            if(getKey(GLFW_KEY_X) == KGL_KeyState::Press) 
            {
                lightModelTrans.t = camera.getTransform()->pos;
            }
            if( keyDown(getKey(GLFW_KEY_UP)) ) 
            {
                s += 0.03f;
                if(s > 1.0f) s = 1.0f;  
            }        
            if(keyDown(getKey(GLFW_KEY_DOWN))) 
            {
                s -= 0.03f;
                if(s < 0.0f) s = 0.0f;  
            }
            if(keyDown(getKey(GLFW_KEY_LEFT))) {
                h -= 1.0f;
                if(h < 0.0f) h = 0.0f;  
            }        
            if(keyDown(getKey(GLFW_KEY_RIGHT))) 
            {
                h += 1.0f;
                if(h > 360.0f) h = 360.0f;  
            }
            if(keyDown(getKey(GLFW_KEY_O))) 
            {
                v -= 0.03f;
                if(v < 0.0f) v = 0.0f;  
            }        
            if(keyDown(getKey(GLFW_KEY_L))) 
            {
                v += 0.03f;
                if(v > 1.0f) v = 1.0f;  
            }
            if(getKey(GLFW_KEY_C) == KGL_KeyState::Press) h = 0.0f, s = 0.0f, v = 1.0f;
            
            if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
                std::cout << "HSV: " << h << ", " << s << ", " << v << std::endl;
                camera.setViewport(45.0f, 0.1f, 100.0f);
            }
            
            if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
                mouseState.captured = 1;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }

            camera.updateFixedInput(window, FIXED_TIMESTEP);

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

            glm::vec3 eje_rot{1.0f, 0.0f, 0.0f};
            glm::quat rot_aplicar{glm::angleAxis(glm::radians(10.0f), eje_rot)};
            glm::quat nueva_rot = rot_aplicar * rot_modelo;
            //rot_modelo = glm::normalize(nueva_rot);
            modelTrans.r = rot_modelo;

            std::this_thread::sleep_for(std::chrono::milliseconds(delay_s));

            accumulator -= FIXED_TIMESTEP;
            steps++;
            contador_ticks++;

            contador_lag -= FIXED_TIMESTEP;
        }        

        if (steps == MAX_STEPS) {
            accumulator = 0.0;
            contador_lag = 0.0;
        }

        resetMouseState(mouseState);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                
        double alpha = glm::clamp((accumulator / FIXED_TIMESTEP), 0.0, 1.0);  

        camera.updateMatrix(alpha);
        
        model.Draw(&shaderProgram, &camera, &modelTrans);
        lightModel.Draw(&lightShaderProgram, &camera, &lightModelTrans);
        
        glfwSwapBuffers(window);


        contador_frames++;
        contador_ms += deltaTime;
        if(contador_ms >= 1.0) {
            std::cout << "[DEBUG] in " << contador_ms << "s " << contador_ticks << " ticks and " << contador_frames << " frames were processed" << std::endl; 
            if( contador_ticks < TARGET_TPS ) 
            {
                double msBehind = contador_lag * 1000.0;
                int ticksBehind = (int)(contador_lag / FIXED_TIMESTEP);
                printf("[WARNING] Can't keep up! Is the server overloaded? Running %d ticks or %.2fms behind!\n", ticksBehind, msBehind);
            }
            contador_ms = 0.0;
            contador_frames = 0.0;
            contador_ticks = 0.0;
        }
    }
        
    shaderProgram.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    printf("\nProgram exited succesfully\n");
    return 0;
}
    
void error_callback(int error, const char* description)
{
    fprintf(stderr, "[GLFW ERROR] %s\n", description);
}

void resize_callback(GLFWwindow *window, int width, int height)
{
    KGL_WindowConfig* wcfg = (KGL_WindowConfig*)glfwGetWindowUserPointer(window);
    wcfg->width = width, wcfg->height = height;

    setLogicalPresentation(width, height, wcfg->LOGICAL_ASPECT);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mouseState.scrollDelta += yoffset;
}

void cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
    static bool first = true;
    static double lastX, lastY;

    if(first) {
        lastX = xpos;
        lastY = ypos;
        first = false;
        return;
    }

    mouseState.dx += float(xpos - lastX);
    mouseState.dy += float(ypos - lastY);

    lastX = xpos;
    lastY = ypos;
}

void window_focus_callback(GLFWwindow* window, int focused)
{
    if(focused) {
        if(mouseState.captured) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        windowConfig.isFocused = 1;
        printf("[DEBUG] Window is focused!\n");
    }
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        windowConfig.isFocused = 0;
        mouseState.captured = 0;
        printf("[DEBUG] Window focus lost!\n");
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key < GLFW_KEY_SPACE || key >= GLFW_KEY_LAST) return;

    if (action == GLFW_PRESS) 
        keyboard[key] = true;
    else if (action == GLFW_RELEASE) 
        keyboard[key] = false;
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

void resolveArgs(int argc, char* argv[])
{
    if(argc <= 0) return;
    try {
        printf("[INFO] Resolving %d arguments\n", argc-1);

        for( int i = 1; i < argc; i++ ) 
        {
            std::string token = argv[i];
            std::cout << "Token " << i << ": " << token << std::endl;

            if(token.find("-vsync") == 0) 
                windowConfig.vsync = 1;
            
            if(token.find("-winmod") == 0)
                windowConfig.windowMode = std::stoi(token.substr(7));

            if(token.find("-D") == 0)
                delay_s = std::stoi(token.substr(2, 3));
            
            if(token.find("-wd") == 0)
                working_dir = token.substr(3);
        }
    } catch(const std::exception& e) {
        printf("[ERROR] while resolving arguments\n");
    }
}