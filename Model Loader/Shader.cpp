#include "Shader.h"

unsigned int ID; // Program ID
int success;
char infoLog[512];

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
	///////////////////////////////////////////////////////
	// Read shader files
	std::string vertexCode, fragmentCode;
	std::ifstream vertexFile, fragmentFile;
	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try	{
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);

		std::stringstream vertexStream, fragmentStream;
		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();

		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();
	}
	catch (const std::exception&) {
		std::cout << "ERROR->" << __FUNCTION__ << ": Error reading shader files" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	///////////////////////////////////////////////////////
	// Compile shaders
	unsigned int vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	compileShader(vertex, vShaderCode);

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	compileShader(fragment, fShaderCode);		
		
	///////////////////////////////////////////////////////
	// Create shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR->" << __FUNCTION__ << ": Could not link program" << std::endl << infoLog << std::endl;;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
};


// Activates shader program
void Shader::use() {
	glUseProgram(ID);
};


void Shader::compileShader(unsigned int& shader, const char* shaderCode) {
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR->" << __FUNCTION__ << ": Could not load shader" << std::endl << infoLog << std::endl;
	}
}