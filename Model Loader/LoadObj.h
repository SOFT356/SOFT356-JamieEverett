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


Model loadObj(Model& model);


#endif