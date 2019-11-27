#include "LoadDae.h"

///////////////////////////////////////////////////
// Forward Declarations
void addMeshToModel(Model& model, std::vector<VecData> daeVec, std::vector<MtlData> mtlVec, std::string texturePath);
void splitIntoVector(std::vector<glm::vec2>& targetVector, std::string values);
void splitIntoVector(std::vector<glm::vec3>& targetVector, std::string values);
void splitIntoVector(std::vector<glm::vec4>& targetVector, std::string values);
void processDaeData(
	std::string modelPath,
	VecData& vecData,
	std::vector<glm::vec2> tmpUvs,
	std::vector<glm::vec3> tmpPositions,
	std::vector<glm::vec3> tmpNormals,
	std::vector<unsigned int> uvIndices,
	std::vector<unsigned int> positionIndices,
	std::vector<unsigned int> normalIndices);
std::vector<Texture> processTextures(std::string path, std::string texturePath);
bool coloursAreDifferent(glm::vec4 colour1, glm::vec4 colour2);



void loadDae(Model& model) {
	///////////////////////////////////////////////////
	// 1. Read file
	std::string line;
	std::ifstream objFile(model.path, std::ios::binary);

	std::vector<glm::vec2> tmpUvs;
	std::vector<glm::vec3> tmpVertices;
	std::vector<glm::vec3> tmpNormals;

	std::vector<std::string> indexNames;
	std::vector<unsigned int> uvIndices, vertexIndices, normalIndices;

	std::vector<MtlData> mtlVector;
	std::vector<VecData> daeVector;

	std::string texturePath; // texture location for the dae

	bool readIndices = false; // flag to detect when indices will be available to read in
	bool readEffectData = false; // as above, but for colours/lighting data
	bool readTexturePath = false; // as above, but for the texture file

	bool processEffect = false; // flag to detect when to export data to a new effect
	bool endOfDaeData = false; // as above, but for a new mesh

	MtlData tempMtlData; // used to store effect data before it gets processed
	VecData tempDaeData; // used to store dae data before it gets processed

	int npos = std::string::npos;

	if (objFile.is_open()) {
		while (getline(objFile, line)) {
			// VERTICES ////////////////////////////////////////////////////
			if (line.find("<float_array") != npos) {
				int floatStart = line.find_first_of(">") + 1;
				int floatEnd = line.find_last_of("<");
				std::string values = std::string(line.c_str() + floatStart, line.c_str() + floatEnd);

				int asda = std::string::npos;

				if (line.find("positions-array") != std::string::npos) {
					splitIntoVector(tmpVertices, values);
				}
				else if (line.find("normals-array") != std::string::npos) {
					splitIntoVector(tmpNormals, values);
				}
				else if (line.find("map") != std::string::npos) {
					splitIntoVector(tmpUvs, values);
				}
			}
			// EFFECTS //////////////////////////////////////////////////////
			else if (line.find("<effect") != npos) {
				int start = line.find("id=\"") + 4;
				int end = line.find("-effect\"");
				std::string effectName = line.substr(start, end - start);

				tempMtlData.materialName = effectName;

				readEffectData = true;
			}
			else if ((line.find("<color") != npos || line.find("<float") != npos) && readEffectData) {
				int effectStart = line.find_first_of(">") + 1;
				int effectEnd = line.find_last_of("<");
				std::string values = std::string(line.c_str() + effectStart, line.c_str() + effectEnd);

				std::istringstream sstrEffect(values);
				std::string token;
				char delim = ' ';

				// emission
				if (line.find("sid=\"emission\"") != npos) {
					sstrEffect >> tempMtlData.Ke.r;
					sstrEffect >> tempMtlData.Ke.g;
					sstrEffect >> tempMtlData.Ke.b;
					sstrEffect >> tempMtlData.Ke.a;
				}
				// diffuse
				else if (line.find("sid=\"diffuse\"") != npos) {
					sstrEffect >> tempMtlData.Kd.r;
					sstrEffect >> tempMtlData.Kd.g;
					sstrEffect >> tempMtlData.Kd.b;
					sstrEffect >> tempMtlData.Kd.a;
				}
				// specular (reflectivity)
				else if (line.find("sid=\"specular\"") != npos) {
					sstrEffect >> tempMtlData.Ks.r;
				}
				// optical density (index of refraction)
				else if (line.find("sid=\"ior\"") != npos) {
					sstrEffect >> tempMtlData.Ni;
				}
			}
			else if (line.find("</effect>") != npos) {
				readEffectData = false;
				processEffect = true;
			}
			// TEXTURES /////////////////////////////////////////////////////
			else if (line.find("<image") != npos) {
				readTexturePath = true;
			}
			else if (line.find("<init_from>") != npos && readTexturePath) {
				int floatStart = line.find_first_of(">") + 1;
				int floatEnd = line.find_last_of("<");
				texturePath = std::string(line.c_str() + floatStart, line.c_str() + floatEnd);
			}
			else if (line.find("</image>") != npos) {
				readTexturePath = false;
			}
			// INDICES //////////////////////////////////////////////////////
			else if (line.find("<triangles") != npos) {
				int start = line.find("material=\"") + 10;
				int end = line.find("-material\"");
				std::string effectName = line.substr(start, end - start);

				tempDaeData.materialName = effectName;

				readIndices = true;
			}
			else if (line.find("<input") != npos && readIndices) {
				std::stringstream ss(line);
				std::string token;
				char delim = ' ';

				// create vector of index names
				while (std::getline(ss, token, delim)) {
					if (token.find("semantic") != npos) {
						int start = token.find_first_of("\"") + 1;
						int end = token.find_last_of("\"");
						indexNames.push_back(std::string(token.c_str() + start, token.c_str() + end));
					}
				}
			}
			else if (line.find("<p>") != npos && readIndices) {
				// process index values
				int start = line.find_first_of(">") + 1;
				int end = line.find_last_of("<");
				std::string valueString = std::string(line.c_str() + start, line.c_str() + end);

				std::stringstream ss(valueString);
				std::string token;
				char delim = ' ';

				std::vector<unsigned int> tmpVector1, tmpVector2, tmpVector3; // generic names as their value types (pos/uv/norm) are not yet known
				int counter = 0;

				while (std::getline(ss, token, delim)) {
					if (counter == 0)
						tmpVector1.push_back(std::stoi(token));
					else if (counter == 1)
						tmpVector2.push_back(std::stoi(token));
					else if (counter == 2)
						tmpVector3.push_back(std::stoi(token));

					if (counter == 2)
						counter = 0;
					else
						counter++;
				}
				// assign generic vectors to their appropriate types
				if (indexNames[0].compare("TEXCOORD") == 0)
					uvIndices = tmpVector1;
				else if (indexNames[0].compare("VERTEX") == 0)
					vertexIndices = tmpVector1;
				else if (indexNames[0].compare("NORMAL") == 0)
					normalIndices = tmpVector1;

				if (indexNames[1].compare("TEXCOORD") == 0)
					uvIndices = tmpVector2;
				else if (indexNames[1].compare("VERTEX") == 0)
					vertexIndices = tmpVector2;
				else if (indexNames[1].compare("NORMAL") == 0)
					normalIndices = tmpVector2;

				if (indexNames[2].compare("TEXCOORD") == 0)
					uvIndices = tmpVector3;
				else if (indexNames[2].compare("VERTEX") == 0)
					vertexIndices = tmpVector3;
				else if (indexNames[2].compare("NORMAL") == 0)
					normalIndices = tmpVector3;
			}
			else if (line.find("</triangles>") != npos) {
				readIndices = false;
				endOfDaeData = true;
			}
			else if (line.find("</mesh>") != npos) {
				// new mesh so clear temp vectors
				tmpUvs.clear();
				tmpVertices.clear();
				tmpNormals.clear();
			}


			// DATA PROCESSING //////////////////////////////////////////////////////
			if (processEffect) {
				// add the material to the material vector
				mtlVector.push_back(tempMtlData);

				// clear the temporary material struct
				tempMtlData = {};
				processEffect = false;
			}
			else if (endOfDaeData) {
				// create a DaeData object
				processDaeData(model.path, tempDaeData, tmpUvs, tmpVertices, tmpNormals,
					uvIndices, vertexIndices, normalIndices);

				daeVector.push_back(tempDaeData);

				// clear the temporary dae struct
				tempDaeData = {};
				
				indexNames.clear();

				uvIndices.clear();
				vertexIndices.clear();
				normalIndices.clear();

				endOfDaeData = false;
			}
		}

		addMeshToModel(model, daeVector, mtlVector, texturePath);
	}
	else {
		std::cout << std::endl;
		std::cout << "ERROR->" << __FUNCTION__ << ": Unable to open dae file, the file may not exist or be corrupt" << std::endl;
	}
}


void addMeshToModel(Model& model, std::vector<VecData> daeVector, std::vector<MtlData> mtlVec, std::string texturePath) {
	// for each dae in daeVec, find its material and add them both to a mesh
	for (unsigned int vertIndex = 0; vertIndex < daeVector.size(); vertIndex++) {
		for (unsigned int matIndex = 0; matIndex < mtlVec.size(); matIndex++) {
			if (strcmp(daeVector[vertIndex].materialName.c_str(), mtlVec[matIndex].materialName.c_str()) == 0) {
				Mesh tempMesh;

				tempMesh.meshType = MeshType::DAE;
				tempMesh.path = model.path.substr(0, model.path.find_last_of("\\/"));
				tempMesh.textures = processTextures(tempMesh.path, texturePath);
				
				tempMesh.vecData = daeVector[vertIndex];
				tempMesh.mtlData = mtlVec[matIndex];

				tempMesh.setupMesh(model.shader);
				model.meshes.push_back(tempMesh);
			}
		}
	}
}


void splitIntoVector(std::vector<glm::vec2>& targetVector, std::string values) {
	std::stringstream posStream(values);
	std::string token;
	char delim = ' ';

	glm::vec2 tmpVector = glm::vec2(0.0f, 0.0f);
	int counter = 0;

	while (getline(posStream, token, delim)) {
		if (!tmpVector.x)
			tmpVector.x = std::stof(token);
		else if (!tmpVector.y)
			tmpVector.y = std::stof(token);

		counter++;

		if (counter == 2) {
			// push to vector if stride length is reached
			targetVector.push_back(tmpVector);

			tmpVector = glm::vec2(0.0f, 0.0f);
			counter = 0;
		}
	}
}

void splitIntoVector(std::vector<glm::vec3>& targetVector, std::string values) {
	std::stringstream posStream(values);
	std::string token;
	char delim = ' ';

	glm::vec3 tmpVector = glm::vec3(0.0f, 0.0f, 0.0f);
	int counter = 0;

	while (getline(posStream, token, delim)) {
		if (!tmpVector.x)
			tmpVector.x = std::stof(token);
		else if (!tmpVector.y)
			tmpVector.y = std::stof(token);
		else if (!tmpVector.z)
			tmpVector.z = std::stof(token);

		counter++;

		if (counter == 3) {
			// push to vector if stride length is reached
			targetVector.push_back(tmpVector);
			
			tmpVector = glm::vec3(0.0f, 0.0f, 0.0f);			
			counter = 0;
		}
	}
}

void splitIntoVector(std::vector<glm::vec4>& targetVector, std::string values) {
	std::stringstream posStream(values);
	std::string token;
	char delim = ' ';

	glm::vec4 tmpVector = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	int counter = 0;

	while (getline(posStream, token, delim)) {
		if (!tmpVector.x)
			tmpVector.x = std::stof(token);
		else if (!tmpVector.y)
			tmpVector.y = std::stof(token);
		else if (!tmpVector.z)
			tmpVector.z = std::stof(token);
		else if (!tmpVector.a)
			tmpVector.a = std::stof(token);

		counter++;

		if (counter == 4) {
			// push to vector if stride length is reached
			targetVector.push_back(tmpVector);

			tmpVector = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			counter = 0;
		}
	}
}

void processDaeData(
	std::string modelPath,
	VecData& vecData,
	std::vector<glm::vec2> tmpUvs,
	std::vector<glm::vec3> tmpVertices,
	std::vector<glm::vec3> tmpNormals,
	std::vector<unsigned int> uvIndices,
	std::vector<unsigned int> vertexIndices,  
	std::vector<unsigned int> normalIndices)
{
	int highestVertexIndex = *std::max_element(vertexIndices.begin(), vertexIndices.end());
	int highestUvIndex = *std::max_element(uvIndices.begin(), uvIndices.end());
	int highestNormalIndex = *std::max_element(normalIndices.begin(), normalIndices.end());

	if (highestVertexIndex > tmpVertices.size() || highestUvIndex > tmpUvs.size() || highestNormalIndex > tmpNormals.size()) {
		// something doesn't add up.. probably a corrupt file
		std::cout << std::endl;
		std::cout << "ERROR->" << __FUNCTION__ << ": Unable to process obj file, the file may be corrupt" << std::endl;
		std::cout << "More Info: " << modelPath << " is missing vertices that are required by the files indices" << std::endl;
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < uvIndices.size(); i++) {
		unsigned int uvIndex = uvIndices[i];
		vecData.uvs.push_back(tmpUvs[uvIndex]);
	}

	// process position data
	for (int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		vecData.vertices.push_back(tmpVertices[vertexIndex]);
	}

	// process normal data
	for (int i = 0; i < normalIndices.size(); i++) {
		unsigned int normalIndex = normalIndices[i];
		vecData.normals.push_back(tmpNormals[normalIndex]);
	}
}

std::vector<Texture> processTextures(std::string path, std::string texturePath) {
	///////////////////////////////////////////////////////////
	// Setup Textures (if a texture file exists)
	std::vector<Texture> textures;
	GLuint textureBuffer[1];

	if (!texturePath.empty()) {
		glGenTextures(1, textureBuffer);

		glBindTexture(GL_TEXTURE_2D, textureBuffer[0]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(true);

		GLint width, height, nrChannels;
		std::string totalPath = path + "\\" + texturePath;
		
		unsigned char* data = stbi_load(totalPath.c_str(), &width, &height, &nrChannels, 0);

		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			Texture texture;
			texture.id = 1;
			texture.type = "texture_map";

			textures.push_back(texture);
		}
		else {
			std::cout << "WARN->" << __FUNCTION__ << ": Could not load texture (texture file may not exist)" << std::endl;
		}

		stbi_image_free(data);
	}

	return textures;
}

bool coloursAreDifferent(glm::vec4 colour1, glm::vec4 colour2) {
	if (colour1.r == colour2.r && colour1.g == colour2.g && colour1.b == colour2.b && colour1.a == colour2.a)
		return false;
	else 
		return true;
}