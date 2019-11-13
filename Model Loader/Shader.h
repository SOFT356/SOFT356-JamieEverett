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
	unsigned int ID; // Program ID

	Shader();
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	
	void use();
	void setVec3(const std::string &name, glm::vec3 &value);
	void setFloat(const std::string &name, float value);
private:
	void compileShader(unsigned int& shader, const char* shaderCode);
};

#endif