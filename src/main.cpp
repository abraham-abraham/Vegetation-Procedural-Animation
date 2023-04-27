
/*
Base code written by Luiza Nogueira Costa
ID 40124771
Concordia University
Winter 2022 for COMP 371

COMP 477 Final Project

Written by

Team #3
Ibrahim Ibrahim (40158162)
Calvin Lee (40128163)
Luiza Nogueira Costa (40124771)
Julien Picard (40158060)

Concordia University
Fall 2022 for COMP 477
*/


#include <iostream>

//Including OpenGL Libraries:

#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  
#include "glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <imgui.h> 
#include <imgui_impl_opengl3.h> 
#include <imgui_impl_glfw.h>

#include <glm/gtx/rotate_vector.hpp>

#include "Shader.h"
#include "Camera.h"
#include "ObjectLoader.h"
#include "Animator.h"
#include "Animation.h"
#include "Wind.h"

#include "Rain.h"
#include "Grass.h"



using namespace std;

#define WINDOW_WIDTH 1600

#define WINDOW_HEIGHT 900

Camera camera; 

float prevx = WINDOW_WIDTH / 2.0;
float prevy = WINDOW_HEIGHT / 2.0;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int skyBoxCounter = 0;

int scatter_light = -5; 

float useShadow = 10;

Wind wind;
Rain rain; 


bool toggleCursor = false; 

bool show_tree = false; 

float sceneModelScaleFactor = 1.0f; 

bool show_test_anim = false; 

bool toggleRain = false; 

bool toggleDetailBending = false; 


bool toggleCoconutTree = true; 
bool toggleRegularTree = false; 
bool toggleGrass = false; 



float quadVertices[] = { //Quad that fills the screen for FBO use
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
};

//Debugger for OpenGL calls:

void opengl_debug(int x) {
	GLenum e = glGetError();
	if (e != GL_NO_ERROR) {
		cout << "An error was found! " << x << " " << (void*)e << endl;
	}
}
//Function for automatically adjusting the viewport when window size changes:

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Input processing:
void processInput(GLFWwindow* window)
{
	ImGuiIO& io = ImGui::GetIO();
	
	if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
		return;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.processKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.processKeyboard(FOWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.processKeyboard(BACKWARD, deltaTime);
	}


	
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Change light mode:
	if (action == GLFW_RELEASE) return; //only handle press events
	if (key == GLFW_KEY_R) {
		scatter_light *= -1;
	}

	//UI toggle:
	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		toggleCursor = !toggleCursor;
	}
	
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() //DEBUG rendering function from LearnOpenGL
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void mouse_position_callback(GLFWwindow* window, double x, double y) {

	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(x, y); 
	if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
		return;
	}


	float xposition = static_cast<float>(x);
	float yposition = static_cast<float>(y);
	float xoff, yoff; 

	if (firstMouse) {
		prevx = xposition;
		prevy = yposition;
		firstMouse = false; 
	}
	xoff = prevx - xposition ;
	yoff = prevy - yposition;

	prevx = xposition;
	prevy = yposition; 

	
	if (!toggleCursor) {
		camera.processMouseMovement(xoff, yoff);
	}
	

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(button, action == GLFW_PRESS);
	if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
		return;
	}
}

void scroll_position_callback(GLFWwindow* window, double x, double y) {
	float yposition = static_cast<float>(y);
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
		return;
	}
	camera.processMouseScroll(yposition); 
}



int main() {

	//Initializing the window:

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "VegetationAnimation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//Initializing GLEW:
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW could not be initialized." << std::endl;
		glfwTerminate();
		return -1;
	}

	//Initialize ImGui 
	// Setup Dear ImGui context 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings 
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	// Setup Dear ImGui style 
	ImGui::StyleColorsDark();

	//stbi_set_flip_vertically_on_load(true);

	//Creating the viewport:
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT); //Lower left corner set to (0, 0).

	//Alert GLFW of function used for resizing the viewport when window size changes:

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Alert GLFW of mouse input:
	glfwSetCursorPosCallback(window, mouse_position_callback);
	glfwSetScrollCallback(window, scroll_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	

	glfwSetKeyCallback(window, key_callback);

	//Z-Buffer:

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_LIGHTING);

	//Setting the shaders:
	Shader default;

	Shader fire;

	Shader rainShader; 

	Shader screen("../../../shaders/screenshader_vertex.glsl", "../../../shaders/screenshader_fragment.glsl");

	Shader depthShader("../../../shaders/depthshader_vertex.glsl", "../../../shaders/depthshader_fragment.glsl");

	Shader debugDepthQuad("../../../shaders/depthDebug_vertex.glsl", "../../../shaders/depthDebug_fragment.glsl");

	Shader occShader("../../../shaders/occulsion_vertex.glsl", "../../../shaders/occlusion_fragment.glsl");

	//Shader animationShader;

	//Shader animationShader("../../../shaders/baseanimation_vertex.glsl", "../../../shaders/baseanimation_fragment.glsl");

	Shader animationShader("../../../shaders/mainbending_vertex.glsl", "../../../shaders/baseanimation_fragment.glsl"); //Assume fragment shading is the same

	//Shader detailBendingShader("../../../shaders/detailbending_vertex.glsl", "../../../shaders/baseanimation_fragment.glsl"); //Assume fragment shading is the same



	Shader animationShader2("../../../shaders/mainbending_vertex.glsl", "../../../shaders/baseanimation_fragment.glsl"); //Assume fragment shading is the same

	Shader grassShader("../../../shaders/mainbending_vertex.glsl", "../../../shaders/baseanimation_fragment.glsl"); //Assume fragment shading is the same

	//Occlusion FBO:
	unsigned int occframebuffer;
	glGenFramebuffers(1, &occframebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, occframebuffer);

	//Color attachment texture:
	unsigned int occcolorbuffer;
	glGenTextures(1, &occcolorbuffer);
	glBindTexture(GL_TEXTURE_2D, occcolorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, occcolorbuffer, 0);


	//Depth Map FBO:
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//Framebuffer configuration:
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//Color attachment texture:
	unsigned int colorbuffer;
	glGenTextures(1, &colorbuffer);
	glBindTexture(GL_TEXTURE_2D, colorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);

	//Render Buffer Object:
	unsigned int renderbuffer;
	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "ERROR: FrameBuffer is incomplete." << endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Screen VAO for Multipass rendering:
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



	//Model loading:

	Model room("../../../assets/project_room_4/exports/2.obj"); //"../../../assets/project_room_4/project_scene_export/projects_scene.obj"
	Model sky("../../../assets/project_room_4/sky.obj");
	//Model skyRed("../../../assets/project_room_4/skyRed.obj");
	//Model skyNight("../../../assets/project_room_4/skyNight.obj");
	//Model fireball("../../../assets/project_room_4/fireball.obj");


	//Animation Modelling:
	Model animationModelSample("../../../assets/project_room_4/coconut_tree/coconut2.dae"); //tree2.dae
	Animation sampleAnimation("../../../assets/project_room_4/coconut_tree/coconut2.dae", &animationModelSample);
	Animator animator(&sampleAnimation);


	Model regularTree("../../../assets/project_room_4/coconut_tree/tree6.dae"); //tree2.dae
	Animation animationRegularTree("../../../assets/project_room_4/coconut_tree/tree6.dae", &regularTree);
	Animator animatorRegularTree(&animationRegularTree);

	Model animationModelSample2("../../../assets/project_room_4/Capoeira/Capoeira.dae");
	Animation sampleAnimation2("../../../assets/project_room_4/Capoeira/Capoeira.dae", &animationModelSample2);
	Animator animator2(&sampleAnimation2);

	Model grass("../../../assets/project_room_4/coconut_tree/grass.dae");
	Animation animationgrass("../../../assets/project_room_4/coconut_tree/grass.dae", &grass);
	Animator animatorgrass(&animationgrass);


	Model raindrop("../../../assets/project_room_4/coconut_tree/raindrop.obj");


	//Light Sources:
	float spherePosition[5] = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f };
	float candle_modifier[5] = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f };
	int multiplier = 1;

	//vec3 sun_position = glm::vec3(-5.0f, 3.0f, 0.0f);

	vec3 sun_position = glm::vec3(-100.0f, 20.0f, 100.0f);

	float exposure = 0.1f;
	float decay = 0.97f;
	float density = 0.5f;
	float weight = 0.25f;

	float modelScaleTree = 1;

	//Set GUI window size:

	ImGui::SetNextWindowSize(ImVec2((float)350, (float)450));


	//Skybox:

	Shader skyShader("../../../shaders/cubemap_vertex.glsl", "../../../shaders/cubemap_fragment.glsl");

	// Grass Renderer:

	Grass grassRenderer;
	grassRenderer.start = glm::vec2(0, 0);
	grassRenderer.stop = glm::vec2(15, 15);
	grassRenderer.scarcity = 1;
	grassRenderer.grass = &grass;
	grassRenderer.animator = &animatorgrass;
	grassRenderer.animation = &animationgrass;

	grassRenderer.initialize(); 

	//Render loop:

	while (!glfwWindowShouldClose(window)) //If window has not been closed...
	{

		//Update wind parameters into glm format:

		wind.vWind = vec4(wind.direction[0] + wind.vWindPosition.x, wind.direction[1] + +wind.vWindPosition.y, wind.direction[2] + +wind.vWindPosition.z, 0);

		//float currentFrame = static_cast<float>(glfwGetTime());

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window); //Processing input for loop iteration. 

		if (!toggleCursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		// UI 
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		
	


		//Light properties:
		vec3 ambient, ambient_fire;
		vec3 color, color_fire;
		vec3 diffuse, diffuse_fire;
		vec3 specular, specular_fire;
		float scatter = -5;

		//Coloring:
		if (skyBoxCounter % 3 == 0) { //Normal
			ambient = vec3(0.50, 0.25, 0.25);
			color = vec3(1, 1, 1);
			diffuse = vec3(0.75, 0.75, 0.75);
			specular = vec3(1, 1, 1);

			ambient_fire = vec3(0.50, 0.25, 0.25);
			color_fire = vec3(1, 1, 1);
			diffuse_fire = vec3(0.75, 0.75, 0.75);
			specular_fire = vec3(1, 1, 1);

			if (scatter_light > 0) {
				scatter = 5;
			}
			else {
				scatter = -5;
			}

		}
		else if (skyBoxCounter % 3 == 1) {  //Red
			ambient = vec3(0.50, 0.25, 0.25);
			color = vec3(0.75, 0.25, 0.05);
			diffuse = vec3(0.5, 0.5, 0.5);
			specular = vec3(0.75, 0.75, 0.75);

			ambient_fire = vec3(0.50, 0.25, 0.25);
			color_fire = vec3(1, 1, 1);
			diffuse_fire = vec3(0.75, 0.75, 0.75);
			specular_fire = vec3(1, 1, 1);

			scatter = -5;
		}
		else { //Night
			ambient = vec3(0.5, 0.5, 0.5);
			color = vec3(0.2, 0.2, 1);
			diffuse = vec3(0.05, 0.05, 0.05);
			specular = vec3(0.1, 0.1, 0.1);

			ambient_fire = vec3(0.50, 0.25, 0.25);
			color_fire = vec3(1, 1, 1);
			diffuse_fire = vec3(0.75, 0.75, 0.75);
			specular_fire = vec3(1, 1, 1);

			scatter = -5;
		}

		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_FRONT);

		vec3 lightDir = glm::vec3(0.0f, 0.0f, 0.0f) - sun_position;

		//Shadow Mapping:
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = -10.0f, far_plane = 20.0f;
		//Light-space transform:
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(sun_position, glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		//Matrix creation:
		glm::mat4 viewMatrix = camera.getViewMatrix();

		//Projection matrix:

		glm::mat4 projectionMatrix = glm::perspective(
			glm::radians(camera.zoom),
			(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
			0.1f,
			100.0f
		);

		projectionMatrix = glm::scale(projectionMatrix, glm::vec3(1.0f, -1.0f, 1.0f));

		

		//Loading the room:

		//Set shader uniforms:
		depthShader.setMat4("projection", projectionMatrix);
		depthShader.setMat4("view", viewMatrix);

		//Model matrix:
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.25f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

		depthShader.setMat4("model", modelMatrix);

		//Render loaded room model:

		room.draw(depthShader);

		//Render tree:

		if (show_tree) {

		
			//Model matrix:
			glm::mat4 modelMatrixAnim = glm::mat4(1.0f);
			modelMatrixAnim = glm::translate(modelMatrixAnim, glm::vec3(0.0f, 1.0f, 0.0f));
		
			modelMatrixAnim = glm::scale(modelMatrixAnim, glm::vec3(1.0f, 1.0f, 0.5f));
			


			depthShader.setMat4("model", modelMatrixAnim);

			if (toggleCoconutTree) {
				animationModelSample.draw(depthShader);
			}


			modelMatrixAnim = glm::mat4(1.0f);
			modelMatrixAnim = glm::translate(modelMatrixAnim, glm::vec3(3.0f, 1.0f, 1.0f));

			modelMatrixAnim = glm::scale(modelMatrixAnim, glm::vec3(1.0f, 1.0f, 1.0f));
			if (toggleRegularTree) {
				regularTree.draw(depthShader);
			}

			


			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}


		//Occlusion Pass:
		glBindFramebuffer(GL_FRAMEBUFFER, occframebuffer);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		occShader.use();


		//Set shader uniforms:
		occShader.setMat4("projection", projectionMatrix);
		occShader.setMat4("view", viewMatrix);

		//Model matrix:
		occShader.setMat4("model", modelMatrix);

		//Render loaded room model:

		room.draw(occShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Binding the FBO:
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);

		//Rendering commands:
		glClearColor(0.5f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Activate Shader Program:

		default.use();
		glUniform1i(glGetUniformLocation(default.id, "shadowMap"), 2);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		default.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		default.setFloat("useShadow", useShadow);

		//Matrix creation:
		viewMatrix = camera.getViewMatrix();

		//Projection matrix:

		projectionMatrix = glm::perspective(
			glm::radians(camera.zoom),
			(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
			0.1f,
			100.0f
		);

		projectionMatrix = glm::scale(projectionMatrix, glm::vec3(1.0f, -1.0f, 1.0f));

		//Loading the room:

		//Set shader uniforms:
		default.setMat4("projection", projectionMatrix);
		default.setMat4("view", viewMatrix);

		//Model matrix:
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.25f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));

		default.setMat4("model", modelMatrix);

		//Light properties:


		default.setVec3("light.position", sun_position);
		default.setVec3("light.direction", lightDir);  //camera.front
		default.setVec3("viewPos", camera.position);
		default.setVec3("light.color", color);
		default.setVec3("light.ambient", ambient);
		default.setVec3("light.diffuse", diffuse);
		default.setVec3("light.specular", specular);


		
		//Render loaded room model:

		room.draw(default);

		//Render tree with colors:

		if (show_tree && toggleCoconutTree) {

			//Updating Animation:
			if (toggleDetailBending) {
				animator.origin = glm::vec3(1.0f, 0.0, 0.0f); 
				animator.windModel = &wind; 
				animator.UpdateAnimationTree(deltaTime);
			}
			

			//Model matrix:
			glm::mat4 modelMatrixAnim = glm::mat4(1.0f);
			modelMatrixAnim = glm::translate(modelMatrixAnim, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrixAnim = glm::scale(modelMatrixAnim, glm::vec3(1.0f, 1.0f, 1.0f));

			animationShader.use();
			animationShader.setMat4("model", modelMatrixAnim);

			//Light properties:


			animationShader.setVec3("light.position", sun_position);
			animationShader.setVec3("light.direction", lightDir);  //camera.front
			animationShader.setVec3("viewPos", camera.position);
			animationShader.setVec3("light.color", color);
			animationShader.setVec3("light.ambient", ambient);
			animationShader.setVec3("light.diffuse", diffuse);
			animationShader.setVec3("light.specular", specular);


			//Set shader uniforms:
			animationShader.setMat4("projection", projectionMatrix);
			animationShader.setMat4("view", viewMatrix);

			//Wind parameters:


			//float noise = wind.generatePerlin(glm::vec2(dot(wind.vWind, vec4(1.0f)), glfwGetTime()));
			
			float noise = cos(glfwGetTime() * wind.fTimeScale);
			//noise = clamp(noise, 0.0f, 1.0f);

			animationShader.setVec4("vWind", wind.vWind);
			animationShader.setFloat("fBendScale", noise * wind.fBendScale);
			animationShader.setFloat("fLength", wind.fLength);
			animationShader.setBool("useWeights", true);


			animationShader.setFloat("branchSwayPowerA",1);
			animationShader.setFloat("branchSwayPowerB", 1);
			animationShader.setFloat("branchMovementRandomization", 1);
			animationShader.setFloat("time", deltaTime);
			animationShader.setFloat("timeWithDelay", deltaTime);
			animationShader.setFloat("branchSuppressPower", 1);
			animationShader.setFloat("delayedWindPower", 1);

			

			auto transforms = animator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i) {
				animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}


			glUniform1i(glGetUniformLocation(animationShader.id, "shadowMap"), 2);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			animationShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			animationShader.setFloat("useShadow", useShadow);

			animationModelSample.draw(animationShader);


			
		

			modelMatrixAnim = glm::translate(modelMatrixAnim, glm::vec3(-9.0f, -0.25f, -5.0f));
			animationShader.setMat4("model", modelMatrixAnim);
			animationModelSample.draw(animationShader);

		
			
			modelMatrixAnim = glm::translate(modelMatrixAnim, glm::vec3(-5.0f, 0.0f, 3.0f));
			//modelMatrixAnim = glm::rotate(modelMatrixAnim, 1.5708f, glm::vec3(0, 1, 0));
			animationShader.setMat4("model", modelMatrixAnim);
			if (toggleDetailBending) {
				animator.origin = glm::vec3(1.0f, 0.0, 0.0f);
				animator.windModel = &wind;
				//animator.modelRotation = glm::rotate(glm::mat4(1.0f), 1.5708f, glm::vec3(0, 1, 0));;
				animator.UpdateAnimationTree(deltaTime);
			}
			transforms = animator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i) {
				animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}
			animationModelSample.draw(animationShader);
			animator.modelRotation = glm::mat4(1.0f);
		}

		if (show_tree && toggleRegularTree) {

			//Updating Animation:
			if (toggleDetailBending) {
				animatorRegularTree.windModel = &wind;
				animatorRegularTree.UpdateAnimationTree(deltaTime);
			}


			//Model matrix:
			glm::mat4 modelMatrixAnim = glm::mat4(1.0f);
			modelMatrixAnim = glm::translate(modelMatrixAnim, glm::vec3(0.0f, 1.0f, 5.0f));
			modelMatrixAnim = glm::scale(modelMatrixAnim, glm::vec3(1.0f, 1.0f, 1.0f));

			animationShader.use();
			animationShader.setMat4("model", modelMatrixAnim);

			//Light properties:


			animationShader.setVec3("light.position", sun_position);
			animationShader.setVec3("light.direction", lightDir);  //camera.front
			animationShader.setVec3("viewPos", camera.position);
			animationShader.setVec3("light.color", color);
			animationShader.setVec3("light.ambient", ambient);
			animationShader.setVec3("light.diffuse", diffuse);
			animationShader.setVec3("light.specular", specular);


			//Set shader uniforms:
			animationShader.setMat4("projection", projectionMatrix);
			animationShader.setMat4("view", viewMatrix);

			//Wind parameters:


			//float noise = wind.generatePerlin(glm::vec2(dot(wind.vWind, vec4(1.0f)), glfwGetTime()));

			float noise = cos(glfwGetTime() * wind.fTimeScale);
			//noise = clamp(noise, 0.0f, 1.0f);

			animationShader.setVec4("vWind", wind.vWind);
			animationShader.setFloat("fBendScale", noise * wind.fBendScale);
			animationShader.setFloat("fLength", wind.fLength);
			animationShader.setBool("useWeights", true);


			animationShader.setFloat("branchSwayPowerA", 1);
			animationShader.setFloat("branchSwayPowerB", 1);
			animationShader.setFloat("branchMovementRandomization", 1);
			animationShader.setFloat("time", deltaTime);
			animationShader.setFloat("timeWithDelay", deltaTime);
			animationShader.setFloat("branchSuppressPower", 1);
			animationShader.setFloat("delayedWindPower", 1);



			auto transforms = animatorRegularTree.GetFinalBoneMatrices(); 
			for (int i = 0; i < transforms.size(); ++i) {
				animationShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}


			glUniform1i(glGetUniformLocation(animationShader.id, "shadowMap"), 2);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			animationShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			animationShader.setFloat("useShadow", useShadow);

			regularTree.draw(animationShader);
		}

		if (toggleGrass) {

			//Updating Animation:
			if (toggleDetailBending) {
				animatorgrass.windModel = &wind;
				animatorgrass.UpdateAnimationTree(deltaTime);
			}


			//Model matrix:
			glm::mat4 modelMatrixAnim = glm::mat4(1.0f);
			modelMatrixAnim = glm::translate(modelMatrixAnim, glm::vec3(7.0f, 1.0f, 5.0f));
			modelMatrixAnim = glm::scale(modelMatrixAnim, glm::vec3(0.01f, 0.01f, 0.01f));

			grassShader.use();
			grassShader.setMat4("model", modelMatrixAnim);

			//Light properties:


			grassShader.setVec3("light.position", sun_position);
			grassShader.setVec3("light.direction", lightDir);  //camera.front
			grassShader.setVec3("viewPos", camera.position);
			grassShader.setVec3("light.color", color);
			grassShader.setVec3("light.ambient", ambient);
			grassShader.setVec3("light.diffuse", diffuse);
			grassShader.setVec3("light.specular", specular);


			//Set shader uniforms:
			grassShader.setMat4("projection", projectionMatrix);
			grassShader.setMat4("view", viewMatrix);

			//Wind parameters:


			//float noise = wind.generatePerlin(glm::vec2(dot(wind.vWind, vec4(1.0f)), glfwGetTime()));

			float noise = cos(glfwGetTime() * wind.fTimeScale);
			//noise = 1; 
			//noise = clamp(noise, 0.0f, 1.0f);

			grassShader.setVec4("vWind", wind.vWind);
			grassShader.setFloat("fBendScale", noise * wind.fBendScale);
			grassShader.setFloat("fLength", wind.fLength);
			grassShader.setBool("useWeights", true);
			grassShader.setBool("isGrass", true);


			grassShader.setFloat("branchSwayPowerA", 1);
			grassShader.setFloat("branchSwayPowerB", 1);
			grassShader.setFloat("branchMovementRandomization", 1);
			grassShader.setFloat("time", deltaTime);
			grassShader.setFloat("timeWithDelay", deltaTime);
			grassShader.setFloat("branchSuppressPower", 1);
			grassShader.setFloat("delayedWindPower", 1);



			auto transforms = animatorgrass.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i) {
				grassShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}


			glUniform1i(glGetUniformLocation(grassShader.id, "shadowMap"), 2);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			grassShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			grassShader.setFloat("useShadow", useShadow);

		
			grassRenderer.windDir = wind.vWind;
			grassRenderer.bendScale = noise * wind.fBendScale; 
			grassRenderer.render(camera, grassShader); 
		}


	
		//DEBUG : Render Test animation

		if (show_test_anim) {
			

			animator2.UpdateAnimation(deltaTime);

			//Model matrix:
			glm::mat4 modelMatrixAnim = glm::mat4(1.0f);
			modelMatrixAnim = glm::translate(modelMatrixAnim, glm::vec3(1.0f, 1.0f, 0.0f));
			modelMatrixAnim = glm::scale(modelMatrixAnim, glm::vec3(1.0f, 1.0f, 1.0f));

			animationShader2.use();
			animationShader2.setMat4("model", modelMatrixAnim);

			//Light properties:


			animationShader2.setVec3("light.position", sun_position);
			animationShader2.setVec3("light.direction", lightDir);  //camera.front
			animationShader2.setVec3("viewPos", camera.position);
			animationShader2.setVec3("light.color", color);
			animationShader2.setVec3("light.ambient", ambient);
			animationShader2.setVec3("light.diffuse", diffuse);
			animationShader2.setVec3("light.specular", specular);


			//Set shader uniforms:
			animationShader2.setMat4("projection", projectionMatrix);
			animationShader2.setMat4("view", viewMatrix);

			//Wind parameters:


			float noise = wind.generatePerlin(glm::vec2(dot(wind.vWind, vec4(1.0f)), glfwGetTime()));
			noise = clamp(noise, 0.0f, 1.0f);
			noise = cos(glfwGetTime() * wind.fTimeScale)+1;


			animationShader2.setVec4("vWind", wind.vWind);
			animationShader2.setFloat("fBendScale", noise * wind.fBendScale);
			animationShader2.setFloat("fLength", wind.fLength);
			animationShader2.setBool("useWeights", true);


			animationShader2.setFloat("branchSwayPowerA", 1);
			animationShader2.setFloat("branchSwayPowerB", 1);
			animationShader2.setFloat("branchMovementRandomization", 1);
			animationShader2.setFloat("time", deltaTime);
			animationShader2.setFloat("timeWithDelay", deltaTime);
			animationShader2.setFloat("branchSuppressPower", 1);
			animationShader2.setFloat("delayedWindPower", 1);



			auto transforms = animator2.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i) {
				animationShader2.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}


			glUniform1i(glGetUniformLocation(animationShader2.id, "shadowMap"), 2);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			animationShader2.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			animationShader2.setFloat("useShadow", useShadow);

			animationModelSample2.draw(animationShader2);
		}

		if (toggleRain) {
			
			//Model matrix:
			glm::mat4 modelMatrixRain = glm::mat4(1.0f);
			modelMatrixRain = glm::translate(modelMatrixRain, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrixRain = glm::scale(modelMatrixRain, glm::vec3(0.05f, 0.05f, 0.05f));

			rainShader.use();
			rainShader.setMat4("model", modelMatrixRain);

			//Light properties:


			rainShader.setVec3("light.position", sun_position);
			rainShader.setVec3("light.direction", lightDir);  //camera.front
			rainShader.setVec3("viewPos", camera.position);
			rainShader.setVec3("light.color", color);
			rainShader.setVec3("light.ambient", ambient);
			rainShader.setVec3("light.diffuse", diffuse);
			rainShader.setVec3("light.specular", specular);


			//Set shader uniforms:
			rainShader.setMat4("projection", projectionMatrix);
			rainShader.setMat4("view", viewMatrix);

			glUniform1i(glGetUniformLocation(rainShader.id, "shadowMap"), 2);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			rainShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
			rainShader.setFloat("useShadow", false);
			
			rain.drawRain(raindrop, rainShader);

			//raindrop.draw(rainShader);
			
		}


		//Drawing the skybox

		
		glDepthFunc(GL_LEQUAL);
		//glDisable(GL_CULL_FACE);
		
		skyShader.use();
		viewMatrix = glm::mat4(glm::mat3(camera.getViewMatrix())); //Remove the translation from view
		skyShader.setMat4("view", viewMatrix);
		skyShader.setMat4("projection", projectionMatrix);

		//Model matrix:
		glm::mat4 modelMatrix2 = glm::mat4(1.0f);
		modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(0.0f, 0.0f, 0.0f));
		modelMatrix2 = glm::scale(modelMatrix2, glm::vec3(1000.0f, 1000.0f, 1000.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, (float)glm::radians(130.0f), glm::vec3(0, 1, 0));
		skyShader.setMat4("model", modelMatrix2);
		skyShader.setFloat("useShadow", -100);

		sky.draw(skyShader);

		glDepthFunc(GL_LESS);

		//Binding back the FBO and render to screen quad:
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		screen.use();
		screen.setFloat("exposure", exposure);
		screen.setFloat("decay", decay);
		screen.setFloat("density", density);
		screen.setFloat("weight", weight);


		//Light screen position for use in volumetric rendering.
		vec3 projected_light = glm::project(sun_position, glm::mat4(1.0f), glm::scale(projectionMatrix, glm::vec3(1.0f, -1.0f, 1.0f)), vec4(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));

		vec4 clip_space_pos = projectionMatrix * viewMatrix * vec4(sun_position, 1.0);
		clip_space_pos /= clip_space_pos.w;
		vec3 snd_space_pos = vec3(clip_space_pos.x, clip_space_pos.y, clip_space_pos.z);
		vec2 window_space_pos;

		window_space_pos.x = snd_space_pos.x;
		window_space_pos.y = snd_space_pos.y;


		screen.setVec2("light_position", window_space_pos);
		screen.setFloat("scatter", scatter);


		glBindVertexArray(quadVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, occcolorbuffer);
		glUniform1i(glGetUniformLocation(screen.id, "screenTexture"), 0);

		glActiveTexture(GL_TEXTURE1);

		glBindTexture(GL_TEXTURE_2D, colorbuffer);
		glUniform1i(glGetUniformLocation(screen.id, "screenTexture2"), 1);

		glActiveTexture(GL_TEXTURE2);

		glBindTexture(GL_TEXTURE_2D, depthMap);
		glUniform1i(glGetUniformLocation(screen.id, "shadowMap"), 2);

		glDrawArrays(GL_TRIANGLES, 0, 6);



		//DEBUG - Depth testing
		debugDepthQuad.use();
		debugDepthQuad.setFloat("near_plane", near_plane);
		debugDepthQuad.setFloat("far_plane", far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		//renderQuad();

		// UI 
		
		ImGui::Begin("Wind Parameters");
		ImGui::SliderFloat("Bend Scale", &wind.fBendScale, 0, 1);
		ImGui::SliderFloat("Time Scale", &wind.fTimeScale, 0, 10);
		ImGui::SliderFloat("Length", &wind.fLength, 0, 20);
		ImGui::SliderFloat("Direction - X", &wind.direction[0], -100, 100);
		ImGui::SliderFloat("Direction - Y", &wind.direction[1], -100, 100);
		ImGui::SliderFloat("Direction - Z", &wind.direction[2], -100, 100);

		ImGui::SliderFloat("Position - X", &wind.vWindPosition.x, -10, 10);
		ImGui::SliderFloat("Position - Y", &wind.vWindPosition.y, -10, 10);
		ImGui::SliderFloat("Position - Z", &wind.vWindPosition.z, -10, 10);

		ImGui::SliderFloat("Sun Position - X", &sun_position.x, -1000, 1000);
		ImGui::SliderFloat("Sun Position - Y", &sun_position.y, -1000, 1000);
		ImGui::SliderFloat("Sun Position - Z", &sun_position.z, -1000, 1000);


		
		int prevParticles = rain.maxParticles; 
		ImGui::SliderInt("Rain Particles", &rain.maxParticles, 1, MAX_PARTICLES);
		
		if (prevParticles != rain.maxParticles) {
			rain.initialize(); 
		}
		
		
		ImGui::Checkbox("Show Trees", &show_tree);
		ImGui::Checkbox("Toggle Coconut Tree", &toggleCoconutTree);
		ImGui::Checkbox("Toggle Regular Tree", &toggleRegularTree);
		ImGui::Checkbox("Show Test Animation", &show_test_anim);
		ImGui::Checkbox("Toggle Rain", &toggleRain);
		ImGui::Checkbox("Toggle Detail Bending", &toggleDetailBending);
		ImGui::Checkbox("Toggle Grass", &toggleGrass);
		
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		


		glfwSwapBuffers(window); // Swap the buffer for the frame and show it as an output. Double buffer is used to avoid flickering. 
		glfwPollEvents(); //Checks for events triggered.

	}

	// Terminating ImGui 
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	//Terminating GLFW:

	glfwTerminate();

	return 0;
}
