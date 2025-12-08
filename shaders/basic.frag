#version 410 core

out vec4 FragColor; // Outputs colors in RGBA

in vec3 color; // Inputs the color from the Vertex Shader
in vec2 texCoord; // Inputs the texture coordinates from the Vertex Shader

uniform sampler2D tex0; // Gets the Texture Unit
uniform bool useTexture;

void main()
{
    if(useTexture) {
        FragColor = texture(tex0, texCoord);
    }
    else {
        FragColor = vec4(color, 1.0);
    }
    //FragColor = vec4(color, 1.0f) * texture(tex0, texCoord);
}