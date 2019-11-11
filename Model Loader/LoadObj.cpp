#include "ModelLoader.h"
#include "LoadObj.h"


///////////////////////////////////////////////////
// Forward Declarations
void addMeshToCollection(std::vector<glm::vec3> tmpVertices,
	std::vector<glm::vec2> tmpUvs,
	std::vector<glm::vec3> tmpNormals,
	std::vector<unsigned int>& vertexIndices,
	std::vector<unsigned int>& uvIndices,
	std::vector<unsigned int>& normalIndices,
	std::string path,
	std::string currMaterialName);

objData processObjectData(
	std::vector<glm::vec3> tmpVertices,
	std::vector<glm::vec2> tmpUvs,
	std::vector<glm::vec3> tmpNormals,
	std::vector<unsigned int>& vertexIndices,
	std::vector<unsigned int>& uvIndices,
	std::vector<unsigned int>& normalIndices);

mtlData processMaterialData(std::string path, std::string currMaterialName);

///////////////////////////////////////////////////
// Global variables
std::vector<mesh> meshCollection; // Collection of all meshes to be returned


std::vector<mesh> loadObj(const std::string path) // Model.obj filepath
{
	std::vector<glm::vec3> tmpVertices;
	std::vector<glm::vec2> tmpUvs;
	std::vector<glm::vec3> tmpNormals;
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

	mesh theMesh;

	///////////////////////////////////////////////////
	// 1. Read file
	std::string line, linePeek, currMaterialName;
	std::ifstream objFile(path, std::ios::binary);
	if (objFile.is_open()) {
		while (std::getline(objFile, line)) {
			// TODO: 2. Validate correct format
			// http://paulbourke.net/dataformats/obj/ -> Error checks
			// only read strings if file is in plaintext encoding

			// store the current file position
			int filePos = objFile.tellg();
			// peek ahead one line and store its value
			std::getline(objFile, linePeek);
			// jump back to the line we started on
			objFile.seekg(filePos);

			///////////////////////////////////////////////////
			// 3. Create data structures
			std::string objDataType = line.substr(0, line.find(" "));
			//std::getline(objFile, linePeek);

			if (objDataType == "usemtl") {
				currMaterialName = line.substr(line.find(" ") + 1, line.length());
				auto youWillStop = "here";
			}

			if (objDataType == "v") {
				std::istringstream sstr(line.substr(2));
				glm::vec3 vertex;
				sstr >> vertex.x;
				sstr >> vertex.y;
				sstr >> vertex.z;
				tmpVertices.push_back(vertex);
			}
			else if (objDataType == "vt") {
				std::istringstream sstr(line.substr(3));
				glm::vec2 vertex;
				sstr >> vertex.x;
				sstr >> vertex.y;
				tmpUvs.push_back(vertex);
			}
			else if (objDataType == "vn") {
				std::istringstream sstr(line.substr(3));
				glm::vec3 vertex;
				sstr >> vertex.x;
				sstr >> vertex.y;
				sstr >> vertex.z;
				tmpNormals.push_back(vertex);
			}
			else if (objDataType == "f") {
				std::string buf;
				std::stringstream lineStream(line.substr(2));

				char delim = '/';
				auto indicesPerFace = (std::count(line.begin(), line.end(), delim))/2; // 2 delims per index
				
				std::vector<std::string> faceElements;
				std::vector<unsigned int> tmpVertIndices, tmpUvIndices, tmpNormalIndices;

				while (lineStream >> buf) {
					std::string token;
					std::stringstream faceStream(buf);

					while (getline(faceStream, token, delim)) {
						faceElements.push_back(token);
					}

					tmpVertIndices.push_back(std::stoi(faceElements[0]));
					tmpUvIndices.push_back(std::stoi(faceElements[1]));
					tmpNormalIndices.push_back(std::stoi(faceElements[2]));

					faceElements.clear();
				}

				if (indicesPerFace == 3) {
					vertexIndices.insert(vertexIndices.end(), { tmpVertIndices[0], tmpVertIndices[1], tmpVertIndices[2] });
					uvIndices.insert(uvIndices.end(), { tmpUvIndices[0], tmpUvIndices[1], tmpUvIndices[2] });
					normalIndices.insert(normalIndices.end(), { tmpNormalIndices[0], tmpNormalIndices[1], tmpNormalIndices[2] });
				}
				else if (indicesPerFace == 4) {
					// convert quads to triangles
					vertexIndices.insert(vertexIndices.end(), {
						tmpVertIndices[0], tmpVertIndices[1], tmpVertIndices[2],
						tmpVertIndices[2], tmpVertIndices[3], tmpVertIndices[0] });
					uvIndices.insert(uvIndices.end(), {
						tmpUvIndices[0], tmpUvIndices[1], tmpUvIndices[2],
						tmpUvIndices[2], tmpUvIndices[3], tmpUvIndices[0] });
					normalIndices.insert(normalIndices.end(), {
						tmpNormalIndices[0], tmpNormalIndices[1], tmpNormalIndices[2],
						tmpNormalIndices[2], tmpNormalIndices[3], tmpNormalIndices[0] });
				}				
			}

			if (objDataType == "f") {
				if (linePeek.find("usemtl") != -1 || linePeek.find("o ") != -1 || linePeek.empty()) {
					// must be at the next mesh or at EOF -> create a new mesh with the stored data
					addMeshToCollection(tmpVertices, tmpUvs, tmpNormals, vertexIndices, uvIndices, normalIndices, path, currMaterialName);
				}
			}
		}
	}
	else {
	std::cout << std::endl;
	std::cout << "ERROR->" << __FUNCTION__ << ": Unable to open obj file, the file may not exist or be corrupt" << std::endl;
	}

	///////////////////////////////////////////////////
	// 5. Deallocate resources

	return meshCollection;
}


void addMeshToCollection(std::vector<glm::vec3> tmpVertices,
	std::vector<glm::vec2> tmpUvs,
	std::vector<glm::vec3> tmpNormals,
	std::vector<unsigned int>& vertexIndices,
	std::vector<unsigned int>& uvIndices,
	std::vector<unsigned int>& normalIndices,
	std::string path,
	std::string currMaterialName)
{
	mesh mesh;
	mesh.path = path.substr(0, path.find_last_of("\\/"));
	mesh.materialName = currMaterialName;
	mesh.objData = processObjectData(tmpVertices, tmpUvs, tmpNormals, vertexIndices, uvIndices, normalIndices);
	mesh.mtlData = processMaterialData(path, currMaterialName);
	meshCollection.push_back(mesh);
}


objData processObjectData(
	std::vector<glm::vec3> tmpVertices,
	std::vector<glm::vec2> tmpUvs,
	std::vector<glm::vec3> tmpNormals,
	std::vector<unsigned int>& vertexIndices, 
	std::vector<unsigned int>& uvIndices, 
	std::vector<unsigned int>& normalIndices) 
{
	///////////////////////////////////////////////////
	// Process data
	objData objData;

	// Process Vertices
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = tmpVertices[vertexIndex - 1];
		objData.vertices.push_back(vertex);
	}

	// Process UVs
	for (unsigned int i = 0; i < uvIndices.size(); i++) {
		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = tmpUvs[uvIndex - 1];
		objData.uvs.push_back(uv);
	}

	// Process Normals
	for (unsigned int i = 0; i < normalIndices.size(); i++) {
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = tmpNormals[normalIndex - 1];
		objData.normals.push_back(normal);
	}

	vertexIndices.clear();
	uvIndices.clear();
	normalIndices.clear();
	
	return objData;
}


mtlData processMaterialData(std::string path, std::string currMaterialName) {
	///////////////////////////////////////////////////
	// Process data
	mtlData mtlData;
	bool targetMaterial = false;

	std::string line;
	std::string mtlFileName = path.substr(0, path.find_last_of(".")) + ".mtl";
	std::ifstream mtlFile(mtlFileName, std::ios::binary);

	if (mtlFile.is_open()) {
		while (std::getline(mtlFile, line)) {
			std::string mtlDataType = line.substr(0, line.find(" "));

			if (mtlDataType == "newmtl" || mtlDataType.empty())
				if (targetMaterial)
					// start of the next 'newmtl', time to return
					return mtlData;

			if (line == "newmtl " + currMaterialName) {
				targetMaterial = true;
			}

			if (targetMaterial) {
				// Populate material data
				if (mtlDataType == "Ns") {
					std::istringstream sstr(line.substr(3));
					sstr >> mtlData.Ns;
				} 
				else if (mtlDataType == "Ka") {
					std::istringstream sstr(line.substr(3));
					sstr >> mtlData.Ka.x;
					sstr >> mtlData.Ka.y;
					sstr >> mtlData.Ka.z;
				}
				else if (mtlDataType == "Kd") {
					std::istringstream sstr(line.substr(3));
					sstr >> mtlData.Kd.x;
					sstr >> mtlData.Kd.y;
					sstr >> mtlData.Kd.z;
				}
				else if (mtlDataType == "Ks") {
					std::istringstream sstr(line.substr(3));
					sstr >> mtlData.Ks.x;
					sstr >> mtlData.Ks.y;
					sstr >> mtlData.Ks.z;
				}
				else if (mtlDataType == "Ke") {
					std::istringstream sstr(line.substr(3));
					sstr >> mtlData.Ke.x;
					sstr >> mtlData.Ke.y;
					sstr >> mtlData.Ke.z;
				}
				else if (mtlDataType == "Ni") {
					std::istringstream sstr(line.substr(3));
					sstr >> mtlData.Ni;
				}
				else if (mtlDataType == "d") {
					std::istringstream sstr(line.substr(2));
					sstr >> mtlData.d;
				}
				else if (mtlDataType == "illum") {
					std::istringstream sstr(line.substr(6));
					sstr >> mtlData.illum;
				}
				else if (mtlDataType == "map_Kd") {
					std::istringstream sstr(line.substr(7));
					sstr >> mtlData.map_Kd;
				}
				else if (mtlDataType == "map_d") {
					std::istringstream sstr(line.substr(6));
					sstr >> mtlData.map_d;
				}
			}
		}
	}

	// target is the last material in the file
	return mtlData;
}