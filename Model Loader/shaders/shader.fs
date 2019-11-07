#version 330 core
out vec4 fragColour;

in vec3 vecColour;
in vec2 texCoord;

uniform sampler2D theTexture;

void main()
{
    fragColour = texture(theTexture, texCoord);
}