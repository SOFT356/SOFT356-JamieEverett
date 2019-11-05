#ifndef LOADOBJ_H
#define LOADOBJ_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

bool loadObj(const std::string path,		// model.obj filepath
	std::vector<glm::vec3>& out_vertices,	// ojb data vectors to be output
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals);

#endif