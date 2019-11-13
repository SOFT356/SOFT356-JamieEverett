#include "Model.h"


Model::Model() {
	glGenVertexArrays(1, &VAO);
}

void Model::draw(Shader shader) {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].draw(shader);
	}
}
