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

struct DaeData {
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> colour;
};

struct Texture {
	unsigned int id;
	std::string type;
};


class Mesh {
public:
	MeshType meshType;

	std::string path;
	std::string materialName;

	ObjData objData;
	MtlData mtlData;

	DaeData daeData;

	std::vector<Texture> textures;

	Mesh();
	Mesh(MeshType meshType,
		std::string path,
		std::string materialName,
		ObjData objData,
		MtlData mtlData,
		DaeData daeData,
		std::vector<Texture> textures);

	void draw(Shader shader);
	void setupMesh();
private:
	unsigned int VAO = NULL;

	GLuint vertexBuffers[NUM_VERTEX_BUFFERS];
};

#endif