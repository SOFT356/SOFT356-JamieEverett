#include "ModelLoader.h"
#include "LoadObj.h"

bool loadObj(
	const std::string path,					// model.obj filepath
	std::vector<glm::vec3>& out_vertices,	// ojb data vectors to be output
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
)
{
	std::vector<glm::vec3> tmpVertices;
	std::vector<glm::vec2> tmpUvs;
	std::vector<glm::vec3> tmpNormals;
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

	///////////////////////////////////////////////////
	// 1. Read file
	std::string line;
	std::ifstream objFile(path, std::ios::binary);

	if (objFile.is_open()) {
		while (std::getline(objFile, line)) {
			// TODO: 2. Validate correct format
			// http://paulbourke.net/dataformats/obj/ -> Error checks
			// only read strings if file is in plaintext encoding

			///////////////////////////////////////////////////
			// 3. Create data structures
			std::string objDataType = line.substr(0, line.find(" "));

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
		}

		///////////////////////////////////////////////////
		// 4. Process data

		// Process Vertices
		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			unsigned int vertexIndex = vertexIndices[i];
			glm::vec3 vertex = tmpVertices[vertexIndex - 1];
			out_vertices.push_back(vertex);
		}

		// Process UVs
		for (unsigned int i = 0; i < uvIndices.size(); i++) {
			unsigned int uvIndex = uvIndices[i];
			glm::vec2 uv = tmpUvs[uvIndex - 1];
			out_uvs.push_back(uv);
		}

		// Process Normals
		for (unsigned int i = 0; i < normalIndices.size(); i++) {
			unsigned int normalIndex = normalIndices[i];
			glm::vec3 normal = tmpNormals[normalIndex - 1];
			out_normals.push_back(normal);
		}
	}
	else {
		std::cout << std::endl;
		std::cout << "ERROR->" << __FUNCTION__ << ": Unable to open obj file, the file may not exist or be corrupt" << std::endl;
		return false;
	}


	///////////////////////////////////////////////////
	// 4. Process material file
	std::string mtlFileName = path.substr(0, path.find_last_of(".")) + ".mtl";
	std::ifstream mtlFile(mtlFileName, std::ios::binary);
	line.erase();

	if (mtlFile.is_open()) {
		while (std::getline(mtlFile, line)) {
			std::string mtlDataType = line.substr(0, line.find(" "));
			
			if (mtlDataType == "") {

			}
		}
	}
	else {
		std::cout << std::endl;
		std::cout << "ERROR->" << __FUNCTION__ << ": Unable to open mtl file, the file may not exist or be corrupt" << std::endl;
		return false;
	}


	///////////////////////////////////////////////////
	// 5. Deallocate resources

	return true;
}