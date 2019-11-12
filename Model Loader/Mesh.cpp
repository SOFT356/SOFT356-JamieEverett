#include "Mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Mesh::Mesh() {

}

Mesh::Mesh(std::string path, std::string materialName, ObjData objData, MtlData mtlData) {
	this->path = path;
	this->materialName = materialName;
	this->objData = objData;
	this->mtlData = mtlData;

	setupMesh();
}

void Mesh::draw(Shader shader) {
	unsigned int diffueNr = 1;
	unsigned int specularNr = 1;

	/*for (unsigned int i = 0; i < textures go here; i++)
	{

	}*/
}

void Mesh::setupMesh() {
	///////////////////////////////////////////////////////////
	// Setup Textures
        std::vector(Texture) textures;

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

        // for each map texture (3 or 4) set it up and add to the texture vector 
	GLint width, height, nrChannels;
	std::string texturePath = path + "\\" + mtlData.map_d;
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR->" << __FUNCTION__ << ": Failed to load texture (texture file may not exist)" << std::endl;
	}

	stbi_image_free(data);


	///////////////////////////////////////////////////////////
	// Setup Buffers
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(BufferValue::NUM_BUFFERS, buffers);

	// position buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[TRIANGLES]);
	glBufferData(GL_ARRAY_BUFFER, objData.vertices.size() * sizeof(glm::vec3), &objData.vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// colour buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[COLOUR]);
	glBufferData(GL_ARRAY_BUFFER, objData.normals.size() * sizeof(glm::vec3), &objData.normals, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	//texture buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXTURES]);
	glBufferData(GL_ARRAY_BUFFER, objData.uvs.size() * sizeof(glm::vec2), &objData.uvs, GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);
}
