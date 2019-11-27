#include "Model.h"


Model::Model() {
}

void Model::draw() {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].draw(shader);
	}
}
