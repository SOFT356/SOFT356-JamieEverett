#include "ModelLoader.h"
#include "LoadObj.h"
#include "LoadDae.h"
#include "Shader.h"
#include "Model.h"


/*******************************************************
FILETYPES:

.obj - Wavefront OBJ & MTL
.dae - Collada DAE
.fbx - Autodesk FBX (proprietary)
.3ds - Autodesk 3DS (proprietary)

********************************************************/


///////////////////////////////////////////////////
// DataTypes
enum class WindowStatus {
	FOCUSED,
	NOT_FOCUSED
};

struct displayObj {
	unsigned int VAO;
	unsigned int texture;
};


///////////////////////////////////////////////////
// Forward Declarations
bool getModelPaths(std::vector<std::string>& modelPaths);
void clearInput();
bool loadModels(std::vector<std::string>& modelPaths, std::vector<Model>& models);
void display(GLFWwindow* window, std::vector<Model> models);
void processInput(GLFWwindow* window, std::vector<Model>& models);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void setUniformMatrix(Shader shaders, glm::mat4 matrix, const char* uniformName);
void onWindowResize(GLFWwindow* window, int width, int height);
void printWelcomeAscii();


///////////////////////////////////////////////////
// Global Vars (Default Program Behaviour)
// window properties
const char* DEFAULT_SCR_TITLE = "JE - ModelLoader";
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
double lastX = 400;
double lastY = 300;
double yaw = -90.0f;
double pitch = 0.0f;
double fov = 45.0f;
bool firstMouse = true;
WindowStatus windowStatus = WindowStatus::FOCUSED;

// rendering
bool captureMouse = true;
bool wireframe = false;

// timing
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
bool awaitingRelease = false;

// user feedback
bool displayAscii = true;


int main()
{
	std::vector<std::string> modelPaths;

	// Ask user for model paths (keep asking until they enter valid strings)
	while (!getModelPaths(modelPaths))
		getModelPaths(modelPaths);

	// Init opengl
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, DEFAULT_SCR_TITLE, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, onWindowResize);

	glewInit();

	// Load the models (includes setting up meshes)
	std::vector<Model> models;
	if (!loadModels(modelPaths, models)) {
		std::cout << "ERROR->" << __FUNCTION__ << ": Could not load models";
		exit(EXIT_FAILURE);
	}

	display(window, models); // TODO: make this compatable with the DAE loader return
}

bool getModelPaths(std::vector<std::string>& modelPaths) {
	if (displayAscii)
		printWelcomeAscii();
	displayAscii = false;

	///////////////////////////////////////////////////
	// Get user input (file/folder directory)

	int numModels;
	std::cout << "Enter the number of models you wish to load" << std::endl;
	std::cin >> numModels;
	std::cout << std::endl;
	if (!std::cin.good()) {
		std::cout << "ERROR->" << __FUNCTION__ << ": Input is not a valid number" << std::endl << std::endl;
		clearInput();
		return false;
	}
	else if (numModels == 0) {
		std::cout << "ERROR->" << __FUNCTION__ << ": Number of models cannot be zero" << std::endl << std::endl;
		clearInput();
		return false;
	}

	clearInput();

	std::ifstream fileTester;
	std::string currPath;

	for (int i = 0; i < numModels; i++)	{
		std::cout << "Enter the location of model " << i + 1 << std::endl;

		std::getline(std::cin, currPath);

		fileTester.open(currPath);
		if (fileTester.fail()) {
			std::cout << "ERROR->" << __FUNCTION__ << ": '" << currPath << "' is not a valid file path" << std::endl << std::endl;
			modelPaths.clear();
			return false;
		}
		fileTester.close();

		modelPaths.push_back(currPath);
	}

	// ******************************************************************************************************************
	// DEBUG USE ONLY
	//modelPaths.push_back("D:\\source\\repos\\SOFT356\\Model Loader\\Test Files\\Creeper-obj\\creeper.obj");
	//modelPaths.push_back("D:\\source\\repos\\SOFT356\\Model Loader\\Test Files\\LowPolyBoat-obj\\low_poly_boat.obj");
	//modelPaths.push_back("D:\\source\\repos\\SOFT356\\Model Loader\\Test Files\\LowPolyBoat-blender.dae");
	//modelPaths.push_back("D:\\source\\repos\\SOFT356\\Model Loader\\Test Files\\Creeper-dae\\Creeper.dae");
	//modelPaths.push_back("D:\\source\\repos\\SOFT356\\Model Loader\\Test Files\\Pouf.dae");
	// ******************************************************************************************************************

	return true;
}

void clearInput() {
	std::cin.clear();
	std::cin.ignore(512, '\n');
}

bool loadModels(std::vector<std::string>& modelPaths, std::vector<Model>& models) {
	///////////////////////////////////////////////////
	// Read Model

	for (int i = 0; i < modelPaths.size(); i++) {
		std::regex pattern(".[a-z0-9]+$", std::regex_constants::icase);
		std::smatch fileExtension;
		std::regex_search(modelPaths[i], fileExtension, pattern);

		Model model;

		if (fileExtension[0] == ".obj") {
			model.path = modelPaths[i];
			loadObj(model);
		}
		else if (fileExtension[0] == ".dae") {
			model.path = modelPaths[i];
			loadDae(model);
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
			std::cout << "  - .dae" << std::endl;
			std::cout << std::endl;

			glfwTerminate();

			return false;
		}

		models.push_back(model);
	}
	
	return true;
}

void display(GLFWwindow* window, std::vector<Model> models) {
	// Attach input callbacks
	glfwSetKeyCallback(window, keyCallback); // only used to check key releases
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// Record mouse input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glEnable(GL_DEPTH_TEST);

	// Build and compile Shader program
	Shader shaders("shaders/shader.vs", "shaders/shader.fs");
	
	while (!glfwWindowShouldClose(window))
	{
		// Frame timer logic
		float currFrame = (float)glfwGetTime();
		deltaTime = currFrame - lastFrame;
		lastFrame = currFrame;

		// Check inputs
		processInput(window, models);

		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Model transformations
		glm::mat4 modelTrans = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		float angleDelta = (float)glfwGetTime() * 0.4f;

		//modelTrans = glm::rotate(modelTrans, angleDelta, glm::vec3(1.0f, 1.0f, 0.0f));
		//modelTrans = glm::scale(modelTrans, glm::vec3(0.008f, 0.008f, 0.008f));
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		projection = glm::perspective(glm::radians((float)fov), (float)(SCR_WIDTH/SCR_HEIGHT), 0.1f, 250.0f);

		// Select shaders
		shaders.use();
		setUniformMatrix(shaders, modelTrans, "model");
		setUniformMatrix(shaders, view, "view");
		setUniformMatrix(shaders, projection, "projection");
		glUniform1i(glGetUniformLocation(shaders.ID, "theTexture"), 0);
		
		// Draw the models
		for (int i = 0; i < models.size(); i++)	{
			models[i].draw(shaders);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//glDeleteBuffers(BufferValue::NUM_BUFFERS, buffers);

	glfwTerminate();
}


void processInput(GLFWwindow* window, std::vector<Model>& models) {
	if ((glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) && !awaitingRelease) {
		firstMouse = true;
		captureMouse = !captureMouse;
		captureMouse ? windowStatus = WindowStatus::FOCUSED : windowStatus = WindowStatus::NOT_FOCUSED;
		captureMouse ? glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED) : glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		awaitingRelease = true;
	}

	if (windowStatus == WindowStatus::NOT_FOCUSED)
		return;

	float cameraSpeed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		displayAscii = true;
		system("cls");
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraPos += cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		cameraPos -= cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !awaitingRelease) {
		wireframe = !wireframe;
		wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		awaitingRelease = true;
	}
	if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS && !awaitingRelease) {
		if (models.size() > 0)
			models.pop_back();
		awaitingRelease = true;
	}
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (GLFW_KEY_1 && action == GLFW_RELEASE)
		awaitingRelease = false;
	if (GLFW_KEY_2 && action == GLFW_RELEASE)
		awaitingRelease = false;
	if (GLFW_KEY_BACKSPACE && action == GLFW_RELEASE)
		awaitingRelease = false;
}


void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
	if (windowStatus == WindowStatus::NOT_FOCUSED)
		return;

	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	// Calculate mouse offset
	double xOffset = xPos - lastX;
	double yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	float sensitivity = 0.05f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	// Add offset values to camera values
	yaw += xOffset;
	pitch += yOffset;

	// Add pitch constraints
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Calculate direction vector
	glm::vec3 cameraDir;
	cameraDir.x = cos(glm::radians((float)pitch)) *  cos(glm::radians((float)yaw));
	cameraDir.y = sin(glm::radians((float)pitch));
	cameraDir.z = cos(glm::radians((float)pitch)) * sin(glm::radians((float)yaw));
	cameraFront = glm::normalize(cameraDir);
}


void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	// Limit fov between 1.0f and 45.0f
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yOffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}


void setUniformMatrix(Shader shaders, glm::mat4 matrix, const char* uniformName) {
	unsigned int loc = glGetUniformLocation(shaders.ID, uniformName);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
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