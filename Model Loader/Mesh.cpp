#include "Mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Mesh::Mesh() {

}

Mesh::Mesh(std::string path, std::string materialName, ObjData objData, MtlData mtlData, unsigned int VAO) {
	this->path = path;
	this->materialName = materialName;
	this->objData = objData;
	this->mtlData = mtlData;

	setupMesh();
}

void Mesh::draw(Shader shader) {
	unsigned int diffueNr	= 1;
	unsigned int specularNr = 1;
	unsigned int normalNr	= 1;
	unsigned int heightNr	= 1;

	for (unsigned int i = 0; i < textures.size(); i++)	{
		glActiveTexture(GL_TEXTURE0 + i);
		// get texture number (diffuseNr)
		std::string number;
		std::string type = textures[i].type;
		if (type == "texture_diffuse")
			number = std::to_string(diffueNr++);
		else if (type == "texture_specular")
			number = std::to_string(specularNr++);
		else if (type == "texture_normal")
			number = std::to_string(normalNr++);

		glUniform1i(glGetUniformLocation(shader.ID, (type + number).c_str()), i);
		
		// bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, objData.vertices.size());
	glBindVertexArray(0);

	// reset active texture
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
	///////////////////////////////////////////////////////////
	// Setup Textures
	//   textures;

	//glGenTextures(NUM_TEXTURE_BUFFERS, textureBuffers);

	//for (int i = MAP_D; i < NUM_TEXTURE_BUFFERS; i++) {
	//	// start at map_d and iterate over all texture values
	//	glBindTexture(GL_TEXTURE_2D, textureBuffers[i]);

	//	// texture params
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//	stbi_set_flip_vertically_on_load(true);

	//	// for each map texture (3 or 4) set it up and add to the texture vector 
	//	GLint width, height, nrChannels;
	//	std::string texturePath = path + "\\" + mtlData.map_d;
	//	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

	//	if (data) {
	//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//		glGenerateMipmap(GL_TEXTURE_2D);
	//	}
	//	else {
	//		std::cout << "ERROR->" << __FUNCTION__ << ": Failed to load texture (texture file may not exist)" << std::endl;
	//	}

	//	stbi_image_free(data);

	//	textures.push_back(2, 3);
	//}	


	///////////////////////////////////////////////////////////
	// Setup Buffers
	glBindVertexArray(VAO);

	glGenBuffers(NUM_VERTEX_BUFFERS, vertexBuffers);

	// position buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[VertexBufferValue::TRIANGLES]);
	glBufferData(GL_ARRAY_BUFFER, objData.vertices.size() * sizeof(glm::vec3), &objData.vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// colour buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[VertexBufferValue::COLOUR]);
	glBufferData(GL_ARRAY_BUFFER, objData.normals.size() * sizeof(glm::vec3), &objData.normals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	//texture buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[VertexBufferValue::TEXTURES]);
	glBufferData(GL_ARRAY_BUFFER, objData.uvs.size() * sizeof(glm::vec2), &objData.uvs[0], GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);
}
