#include "Mesh.h"


Mesh::Mesh() {

}

void Mesh::draw(Shader shader) {
	unsigned int diffueNr	= 1;
	unsigned int specularNr = 1;
	unsigned int normalNr	= 1;
	unsigned int heightNr	= 1;	

	shader.use();

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

	shader.setVec4("material.diffuse", mtlData.Kd);

	//shader.setVec3("material.ambient", mtlData.Ka);

	//shader.setVec3("material.specular", mtlData.Ks);

	//shader.setFloat("material.shininess", mtlData.Ni);


	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vecData.vertices.size());
	glBindVertexArray(0);

	// reset active texture
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh(Shader shader) {
	///////////////////////////////////////////////////////////
	// Setup Buffers
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(NUM_VERTEX_BUFFERS, vertexBuffers);

	// position buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[VertexBufferValue::TRIANGLES]);

	glBufferData(GL_ARRAY_BUFFER, vecData.vertices.size() * sizeof(glm::vec3), &vecData.vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);


	// normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[VertexBufferValue::NORMALS]);

	glBufferData(GL_ARRAY_BUFFER, vecData.normals.size() * sizeof(glm::vec3), &vecData.normals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);


	// texture buffer
	if (!vecData.uvs.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[VertexBufferValue::TEXTURES]);

		glBufferData(GL_ARRAY_BUFFER, vecData.uvs.size() * sizeof(glm::vec2), &vecData.uvs[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);
	}

	shader.use();

	if (textures.empty())
		glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), false);
	else
		glUniform1i(glGetUniformLocation(shader.ID, "hasTexture"), true);
}