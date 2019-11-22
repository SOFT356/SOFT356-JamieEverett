#version 330 core
out vec4 fragColour;

struct Material {       
    float shininess;    // Ns
    vec3 ambient;       // Ka
    vec4 diffuse;       // Kd
    vec3 specular;      // Ks
    float transparency; // d
}; 

//in vec3 vecColour;
in vec2 texCoord;

uniform sampler2D texture_diffuse1;
uniform bool hasTexture;
uniform Material material;

void main()
{
    if(hasTexture) {
        fragColour = texture(texture_diffuse1, texCoord);
    }
    else {
        fragColour = vec4(material.diffuse);
    }
}