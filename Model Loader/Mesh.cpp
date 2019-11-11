#include "Mesh.h"


Mesh::Mesh() {

}

Mesh::Mesh(std::string path, std::string materialName, ObjData objData, MtlData mtlData) {
	this->path = path;
	this->materialName = materialName;
	this->objData = objData;
	this->mtlData = mtlData;
}

void Mesh::draw(Shader shader) {

}

void Mesh::setupMesh() {

}