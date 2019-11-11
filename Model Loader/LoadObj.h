#ifndef LOADOBJ_H
#define LOADOBJ_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


///////////////////////////////////////////////////
// DataTypes
struct objData {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

struct mtlData {
	float Ns = NULL;	// Specular exponent
	glm::vec3 Ka;		// Ambient colour
	glm::vec3 Kd;		// Diffuse colour
	glm::vec3 Ks;		// Specular colour
	glm::vec3 Ke;		// Emissive coefficient
	float Ni = NULL;	// Optical density (index of refraction)
	float d = NULL;		// Dissolved value
	int illum = NULL;	// Illumination model
	std::string map_Kd;	// Diffuse texture map
	std::string map_d;	// Alpha texture map
};

struct mesh { 
	std::string path;
	std::string materialName;
	objData objData;
	mtlData mtlData;
};

std::vector<mesh> loadObj(const std::string path);

#endif