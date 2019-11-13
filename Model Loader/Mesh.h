#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Shader.h"


///////////////////////////////////////////////////
// DataTypes
enum VertexBufferValue {
	TRIANGLES,
	COLOUR,
	TEXTURES,
	NUM_VERTEX_BUFFERS
};

struct ObjData {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

struct MtlData {
	float Ns = NULL;							// Specular exponent
	glm::vec3 Ka = glm::vec3(0.0, 0.0, 0.0);	// Ambient colour
	glm::vec3 Kd = glm::vec3(0.0, 0.0, 0.0);	// Diffuse colour
	glm::vec3 Ks = glm::vec3(0.0, 0.0, 0.0);	// Specular colour
	glm::vec3 Ke = glm::vec3(0.0, 0.0, 0.0);	// Emissive coefficient
	float Ni = NULL;							// Optical density (index of refraction)
	float d = NULL;								// Dissolved value
	int illum = NULL;							// Illumination model
	std::string map_d;							// Alpha texture map
	std::string map_Kd;							// Diffuse texture map
};

struct Texture {
	unsigned int id;
	std::string type;
};


class Mesh {
public:
	std::string path;
	std::string materialName;

	ObjData objData;
	MtlData mtlData;

	std::vector<Texture> textures;

	Mesh();
	Mesh(std::string path, std::string materialName, ObjData objData, MtlData mtlData, std::vector<Texture> textures);

	void draw(Shader shader);
private:
	unsigned int VAO = NULL;

	GLuint vertexBuffers[NUM_VERTEX_BUFFERS];

	void setupMesh();
};

#endif