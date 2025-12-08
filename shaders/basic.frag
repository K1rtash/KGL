#version 410 core

out vec4 FragColor; // Outputs colors in RGBA

in vec3 color;
in vec2 texCoord;

uniform sampler2D tex0;
//uniform bool useTexture;
uniform vec4 lightColor;

void main()
{
    /*if(useTexture) {
        FragColor = texture(tex0, texCoord);
    }
    else {
        FragColor = vec4(color, 1.0);
    }*/
    FragColor = texture(tex0, texCoord) * lightColor;
}