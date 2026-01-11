#version 330 core

out vec4 FragColor; // Outputs colors in RGBA

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

vec4 direcLight() 
{
    float ambient = 0.2;

    //diffuse lightning
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(1.0, 1.0, 0.0));
    float diffuse = max(abs(dot(normal, lightDirection)), 0.0); // absolute helps with grass shading

    // discards all fragments with alpha less than 0.1
	if (texture(diffuse0, texCoord).a < 0.1) discard;

    // specular lighting
	float specularLight = 0.50;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0), 16.0);
	float specular = specAmount * specularLight;

    return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

void main()
{
    FragColor = direcLight();
}