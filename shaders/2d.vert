#version 410 core

layout (location = 0) in vec2 aPos;  
layout (location = 1) in vec2 aTex; 

out vec2 texCoord;

uniform mat4 proj; // matriz ortográfica para 2D
uniform mat4 model;      // transformaciones del sprite (traslación, escala, rotación)

void main()
{
    gl_Position = proj * model * vec4(aPos, 0.0, 1.0);
    texCoord = aTex;
}
