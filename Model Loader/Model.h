#ifndef MODEL_H
#define MODEL_H

#include <vector>;

#include "Mesh.h";
#include "Shader.h";

class Model {
public:
	Shader shader;
	std::string path;
	std::vector<Mesh> meshes;

	Model();

	void draw();
};

#endif