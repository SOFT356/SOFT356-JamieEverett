#ifndef SHADER_H
#define SHADER_H
	
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class Shader {
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	void use();
private:
	void compileShader(unsigned int& shader, const char* shaderCode);
};

#endif