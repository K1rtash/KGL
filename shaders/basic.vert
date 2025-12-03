#version 410 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aColor;
// Texture Coordinates
layout (location = 2) in vec2 aTex;


// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;

uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
	gl_Position = camMatrix * model * vec4(aPos, 1.0); 	// Outputs the positions/coordinates of all vertices
	color = aColor; 	// Assigns the colors from the Vertex Data to "color"
	texCoord = aTex; 	// Assigns the texture coordinates from the Vertex Data to "texCoord"
}