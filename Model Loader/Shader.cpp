#include "Shader.h"

int success;
char infoLog[512];

Shader::Shader() {};

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

void Shader::setVec3(const std::string& name, glm::vec3& value) {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, glm::vec4& value) {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::compileShader(unsigned int& shader, const char* shaderCode) {
	glShaderSource(shader, 1, &shaderCode, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR->" << __FUNCTION__ << ": Could not load shader" << std::endl << infoLog << std::endl;
	}
}