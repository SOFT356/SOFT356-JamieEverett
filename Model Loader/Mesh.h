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
enum MeshType {
	OBJ,
	DAE
};

enum VertexBufferValue {
	TRIANGLES,
	NORMALS,
	TEXTURES,
	COLOUR,
	NUM_VERTEX_BUFFERS
};

struct VecData {
	std::string materialName;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

struct MtlData {
	std::string materialName;						// Name of the material
	float Ns = NULL;								// Specular exponent
	glm::vec4 Ka = glm::vec4(0.0, 0.0, 0.0, 0.0);	// Ambient colour
	glm::vec4 Kd = glm::vec4(0.0, 0.0, 0.0, 0.0);	// Diffuse colour
	glm::vec4 Ks = glm::vec4(0.0, 0.0, 0.0, 0.0);	// Specular colour
	glm::vec4 Ke = glm::vec4(0.0, 0.0, 0.0, 0.0);	// Emissive coefficient
	float Ni = NULL;								// Optical density (index of refraction)
	float d = NULL;									// Dissolved value
	int illum = NULL;								// Illumination model
	std::string map_d;								// Alpha texture map
	std::string map_Kd;								// Diffuse texture map
};

struct Texture {
	unsigned int id;
	std::string type;
};


class Mesh {
public:
	MeshType meshType;

	std::string path;

	VecData vecData;
	MtlData mtlData;

	std::vector<Texture> textures;

	Mesh();

	void draw(Shader shader);
	void setupMesh(Shader shader);
private:
	unsigned int VAO = NULL;

	GLuint vertexBuffers[NUM_VERTEX_BUFFERS];
};

#endif