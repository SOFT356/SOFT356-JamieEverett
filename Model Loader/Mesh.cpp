#include "Mesh.h"


Mesh::Mesh() {

}

Mesh::Mesh(MeshType meshType, std::string path, std::string materialName, ObjData objData, MtlData mtlData, DaeData daeData, std::vector<Texture> textures) {
	this->meshType = meshType;
	this->path = path;
	this->materialName = materialName;
	this->objData = objData;
	this->mtlData = mtlData;
	this->daeData = daeData;
	this->textures = textures;

	setupMesh();
}

void Mesh::draw(Shader shader) {
	unsigned int diffueNr	= 1;
	unsigned int specularNr = 1;
	unsigned int normalNr	= 1;
	unsigned int heightNr	= 1;

	if (textures.empty())	
		glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), false);
	else 
		glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), true);
	

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

	if (meshType == MeshType::OBJ) {
		//shader.setVec3("material.ambient", mtlData.Ka);

		// convert RGB to RGBA
		shader.setVec4("material.diffuse", mtlData.Kd);

		//shader.setVec3("material.specular", mtlData.Ks);

		//shader.setFloat("material.shininess", mtlData.Ni);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, objData.vertices.size());
		glBindVertexArray(0);
	} 
	else {
		if (!daeData.colour.empty())
			shader.setVec4("material.diffuse", daeData.colour[0]);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, daeData.vertices.size());
		glBindVertexArray(0);
	}

	// reset active texture
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
	///////////////////////////////////////////////////////////
	// Setup Buffers
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(NUM_VERTEX_BUFFERS, vertexBuffers);

	// position buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[VertexBufferValue::TRIANGLES]);

	if (meshType == MeshType::OBJ)
		glBufferData(GL_ARRAY_BUFFER, objData.vertices.size() * sizeof(glm::vec3), &objData.vertices[0], GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, daeData.vertices.size() * sizeof(glm::vec3), &daeData.vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);


	// normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[VertexBufferValue::NORMALS]);

	if (meshType == MeshType::OBJ)
		glBufferData(GL_ARRAY_BUFFER, objData.normals.size() * sizeof(glm::vec3), &objData.normals[0], GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, daeData.normals.size() * sizeof(glm::vec3), &daeData.normals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);


	// texture buffer
	if (!daeData.uvs.empty() || !objData.uvs.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[VertexBufferValue::TEXTURES]);

		if (meshType == MeshType::OBJ)
			glBufferData(GL_ARRAY_BUFFER, objData.uvs.size() * sizeof(glm::vec2), &objData.uvs[0], GL_STATIC_DRAW);
		else if (meshType == MeshType::DAE && !daeData.uvs.empty())
			glBufferData(GL_ARRAY_BUFFER, daeData.uvs.size() * sizeof(glm::vec2), &daeData.uvs[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);
	}


	// colour buffer
	if (meshType == MeshType::DAE && !daeData.colour.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[VertexBufferValue::COLOUR]);

		glBufferData(GL_ARRAY_BUFFER, daeData.colour.size() * sizeof(glm::vec4), &daeData.colour[0], GL_STATIC_DRAW);

		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(3);
	}
}