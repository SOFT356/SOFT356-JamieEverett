#ifndef LOADOBJ_H
#define LOADOBJ_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Mesh.h"
#include "Model.h"


const std::vector<std::string> textureTypes = {
	"texture_alpha",	// map_d
	"texture_diffuse",	// map_Kd
	"texture_specular"	// map_Ks
};

const int numTextureTypes = 3;


void loadObj(Model& model);


#endif