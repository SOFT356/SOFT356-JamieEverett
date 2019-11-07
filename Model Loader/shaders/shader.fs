#version 330 core
out vec4 fragColour;
in vec3 vecColour;

void main()
{
    fragColour = vec4(vecColour, 1.0f);
}