#define STB_IMAGE_IMPLEMENTATION

#include "ModelLoader.h"
#include "LoadObj.h"
#include "Shader.h"
#include "stb_image.h"


/*******************************************************
FILETYPES:

.obj - Wavefront OBJ & MTL
.dae - Collada DAE
.fbx - Autodesk FBX (proprietary)
.3ds - Autodesk 3DS (proprietary)

********************************************************/


///////////////////////////////////////////////////
// DataTypes
enum BufferValue { 
	TRIANGLES,
	COLOUR,
	TEXTURES,
	NUM_BUFFERS = 3
};

struct displayObj {
	unsigned int VAO;
	unsigned int texture;
};


///////////////////////////////////////////////////
// Forward Declarations
bool loadObj(
	const std::string path,
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs, 
	std::vector<glm::vec3>& normals);

void display(
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void setUniformMatrix(Shader shaders, glm::mat4 matrix, const char* uniformName);
displayObj displayInit(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals);
void onWindowResize(GLFWwindow* window, int width, int height);
void printWelcomeAscii();


///////////////////////////////////////////////////
// Global Vars (Default Program Behaviour)
const char* DEFAULT_SCR_TITLE = "JE - ModelLoader";
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;
bool displayAscii = true;
GLuint buffers[1];


int main()
{
	if (displayAscii)
		printWelcomeAscii();
	displayAscii = false;
	
	///////////////////////////////////////////////////
	// Get user input (file/folder directory)
	std::cout << "Enter the location of a model file or folder of models to continue..." << std::endl;
	std::cout << "(press 'Escape' to close an open model)" << std::endl;
	std::string modelPath; std::cin >> modelPath;
	
	//std::string modelPath = "C:\\Users\\Jamie\\Downloads\\Test Files (Model Loader)-20191105\\Creeper-obj\\creeper.obj";

	///////////////////////////////////////////////////
	// Read Model
	std::regex pattern(".[a-z0-9]+$", std::regex_constants::icase);
	std::smatch fileExtension;
	std::regex_search(modelPath, fileExtension, pattern);
	
	std::vector<glm::vec3> vertices, normals;
	std::vector<glm::vec2> uvs;

	if (fileExtension[0] == ".obj") {
		if (!loadObj(modelPath, vertices, uvs, normals))
			exit(EXIT_FAILURE);
	} 
	else if (fileExtension[0] == ".dae") {
		// TODO: load a .dae file
		// loadDae(modelPath);
	}
	else if (fileExtension[0] == ".fbx") {
		// TODO: load a .fbx file
		// loadFbx(modelPath);
	}
	else if (fileExtension[0] == ".3ds") {
		// TODO: load a .3ds file
		// load3ds(modelPath);
	}
	else {
		system("cls");

		printWelcomeAscii();
		std::cout << std::endl;
		std::cout << "ERROR->" << __FUNCTION__ << ": Unsupported file type" << std::endl;
		std::cout << "Try using the supported file types:" << std::endl;
		std::cout << "  - .obj" << std::endl;
		std::cout << std::endl;

		main();
	}

	display(vertices, uvs, normals);

	// BEGIN DEBUG
	system("pause");
	// END DEBUG
}


void display(
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals)
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, DEFAULT_SCR_TITLE, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, onWindowResize);

	glfwSetKeyCallback(window, keyCallback);

	glewInit();

	glEnable(GL_DEPTH_TEST);

	// Build and compile Shader program
	Shader shaders("shaders/shader.vs", "shaders/shader.fs");

	displayObj displayVals = displayInit(vertices, uvs, normals);
	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Model transformations
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		float angleDelta = (float)glfwGetTime() * 0.4f;

		model = glm::rotate(model, angleDelta, glm::vec3(1.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)(SCR_WIDTH/SCR_HEIGHT), 0.1f, 100.0f);

		// bind textures
		glBindTexture(GL_TEXTURE_2D, displayVals.texture);

		// Select shaders
		shaders.use();
		setUniformMatrix(shaders, model, "model");
		setUniformMatrix(shaders, view, "view");
		setUniformMatrix(shaders, projection, "projection");
		glUniform1i(glGetUniformLocation(shaders.ID, "theTexture"), 0);

		glBindVertexArray(displayVals.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (GLint)vertices.size());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &displayVals.VAO);
	glDeleteBuffers(BufferValue::NUM_BUFFERS, buffers);

	glfwTerminate();

	main();
}


void keyCallback(GLFWwindow* window, int key, int scancode, int actions, int mods) {
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		displayAscii = true;
		system("cls");
		break;
	}
}


void setUniformMatrix(Shader shaders, glm::mat4 matrix, const char* uniformName) {
	unsigned int loc = glGetUniformLocation(shaders.ID, uniformName);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}


displayObj displayInit(std::vector<glm::vec3> vertices,
	std::vector<glm::vec2> uvs,
	std::vector<glm::vec3> normals
) {
	// add textures
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	GLint width, height, nrChannels;
	unsigned char* data = stbi_load("Texture.png", &width, &height, &nrChannels, 0);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR->" << __FUNCTION__ << ": Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	// setup buffers
	unsigned int VAO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(BufferValue::NUM_BUFFERS, buffers);

	// position buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[TRIANGLES]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// colour buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[COLOUR]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	// texture buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXTURES]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	// set return values
	displayObj vals;
	vals.VAO = VAO;
	vals.texture = texture;

	return vals;
}


void onWindowResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


void printWelcomeAscii() {
	std::cout << R"(
	##################################################################################################
	#                                                                                                #
	#          ,___          .-;'                                                                    #
	#         `"-.`\_...._/`.`                                                                       #
	#      ,      \        /      __  __           _      _   _                     _                #
	#   .-' ',    / ()   ()\     |  \/  |         | |    | | | |                   | |               #
	#  `'._   \  /()    .  (|    | \  / | ___   __| | ___| | | |     ___   __ _  __| | ___ _ __      #
	#      > .' ;,     -'-  /    | |\/| |/ _ \ / _` |/ _ \ | | |    / _ \ / _` |/ _` |/ _ \ '__|     #
	#     / <   |;,     __.;     | |  | | (_) | (_| |  __/ | | |___| (_) | (_| | (_| |  __/ |        #
	#     '-.'-.|  , \    , \    |_|  |_|\___/ \__,_|\___|_| |______\___/ \__,_|\__,_|\___|_|        #
	#        `>.|;, \_)    \_)                                                                       #
	#         `-;     ,    /                                                                         #
	#            \    /   <                                                                          #
	#             '. <`'-,_)                                                                         #
	#          jgs '._)                                                                              #
	#                                                                                                #
	##################################################################################################
	)" << '\n';
}