/**
 * Base code
 * Draws two meshes and one ground plane, one mesh has textures, as well
 * as ground plane.
 * Must be fixed to load in mesh with multiple shapes (dummy.obj)
 */

#include <iostream>
#include <algorithm>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "Texture.h"
#include "Particle.h"
#include "WindowManager.h"
#include "Dog.h"
#include "Player.h"
#include "GameModel.h"

 // value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:


	WindowManager * windowManager = nullptr;

	// Our shader program
    std::shared_ptr<Program> bf_prog;
    std::shared_ptr<Program> tex_prog;
	std::shared_ptr<Program> toon_prog;


    Player player = Player();
    GameModel gameModel = GameModel();

	// CPU array for particles - redundant with particle structure
	// but simple
	int numP = 300;
	GLfloat points[900];
	GLfloat pointColors[1200];

	GLuint pointsbuffer;
	GLuint colorbuffer;
    

    vec3 dogMin;
    vec3 dogMax;
    vec3 dogMiddle;
    float dogRad = 0;
	float planeRad = 0;

    bool beginning = true;
    
    float speed = 2;
    
    shared_ptr<Shape> mesh;
    vector<shared_ptr<Shape>> allShapesDog;
    vector<shared_ptr<Shape>> allShapesPlane;


	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// OpenGL handle to texture data
    shared_ptr<Texture> texture0;

	int gMat = 0;

	// Display time to control fps
	float t0_disp = 0.0f;
	float t_disp = 0.0f;

	bool keyToggles[256] = { false };
	float t = 0.0f; //reset in init
	float h = 0.01f;
	glm::vec3 g = glm::vec3(0.0f, -0.01f, 0.0f);
	float curFrameRate, lastFrameRate;
	float framespeed = .01;
	float camRot;
    
    float moveLight = 0;


	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		keyToggles[key] = ! keyToggles[key];
        player.updateLocalOrientation();
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            player.moveLeft();
        }
        if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            player.moveRight();
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            player.moveBackwards();
        }
        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            player.moveForward();
        }
        
        if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        }
        if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
        if (key == GLFW_KEY_O && action == GLFW_PRESS)
		{
			speed += 1;
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
        player.scrollCallback(deltaX, deltaY);
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
		}
	}

	void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		CHECKED_GL_CALL(glViewport(0, 0, width, height));
	}

	//code to set up the two shaders - a diffuse shader and texture mapping
	void init(const std::string& resourceDirectory)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		// Set background color.
		CHECKED_GL_CALL(glClearColor(.12f, .34f, .56f, 1.0f));

		// Enable z-buffer test.
		CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
		CHECKED_GL_CALL(glEnable(GL_BLEND));
		CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		CHECKED_GL_CALL(glPointSize(14.0f));
        
        // Initialize the GLSL program for lighting.
        bf_prog = make_shared<Program>();
        bf_prog->setVerbose(true);
        bf_prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
        if (! bf_prog->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        bf_prog->addUniform("P");
        bf_prog->addUniform("V");
        bf_prog->addUniform("M");
        bf_prog->addUniform("lightPos");
        bf_prog->addUniform("MatAmb");
        bf_prog->addUniform("MatDif");
        bf_prog->addUniform("MatSpec");
        bf_prog->addUniform("shine");
        bf_prog->addAttribute("vertPos");
        bf_prog->addAttribute("vertNor");
        bf_prog->addAttribute("vertTex");
        
        // Initialize the GLSL program for texture.
        tex_prog = make_shared<Program>();
        tex_prog->setVerbose(true);
        tex_prog->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
        tex_prog->init();
        tex_prog->addUniform("P");
        tex_prog->addUniform("V");
        tex_prog->addUniform("M");
        tex_prog->addUniform("lightPos");
        tex_prog->addUniform("Texture0");
        tex_prog->addAttribute("vertPos");
        tex_prog->addAttribute("vertNor");
        tex_prog->addAttribute("vertTex");


		toon_prog = make_shared<Program>();
        toon_prog->setVerbose(true);
        toon_prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/cartoon_frag.glsl");
        if (! toon_prog->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        toon_prog->addUniform("P");
        toon_prog->addUniform("V");
        toon_prog->addUniform("M");
        toon_prog->addUniform("lightPos");
        toon_prog->addUniform("MatAmb");
        toon_prog->addUniform("MatDif");
        toon_prog->addUniform("MatSpec");
        toon_prog->addUniform("shine");
        toon_prog->addAttribute("vertPos");
        toon_prog->addAttribute("vertNor");
        toon_prog->addAttribute("vertTex");
	}

	// Code to load in the three textures
	void initTex(const std::string& resourceDirectory)
	{
        texture0 = make_shared<Texture>();
        texture0->setFilename(resourceDirectory + "/grass.jpg");
        texture0->init();
        texture0->setUnit(2);
        texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	}

	void initGeom(const std::string& resourceDirectory, vector<shared_ptr<Shape>> &meshes, vec3 &meshMiddle, float &radius)
	{
		// Initialize mesh
		// Load geometry
 		// Some obj files contain material information.We'll ignore them for this assignment.
		vector<tinyobj::shape_t> TOshapes;
		vector<tinyobj::material_t> objMaterials;
		string errStr;
        vec3 gMax = vec3(float(INT32_MIN), float(INT32_MIN), float(INT32_MIN));
        vec3 gMin = vec3(float(INT32_MAX), float(INT32_MAX), float(INT32_MAX));

		printf("drawing: %s\n", resourceDirectory.c_str());
		//load in the mesh and make the shape(s)
		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory).c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			cout << "number of shapes: " << int(TOshapes.size()) << endl;
			for (int i = 0; i < int(TOshapes.size()); i++) {
				mesh = make_shared<Shape>();

				mesh->createShape(TOshapes[i]);
				mesh->measure();
				mesh->init();

				gMin.x = glm::min(mesh->min.x, gMin.x);
				gMin.y = glm::min(mesh->min.y, gMin.y);
				gMin.z = glm::min(mesh->min.z, gMin.z);

				gMax.x = glm::max(mesh->max.x, gMax.x);
				gMax.y = glm::max(mesh->max.y, gMax.y);
				gMax.z = glm::max(mesh->max.z, gMax.z);
				meshes.push_back(mesh);

			}
            meshMiddle.x = (gMin.x + gMax.x)/2;
            meshMiddle.y = (gMin.y + gMax.y)/2;
            meshMiddle.z = (gMin.z + gMax.z)/2;

			radius = length(gMax - meshMiddle);

		}
	}

	double getElapsedTime()
	{
		static double lasttime = glfwGetTime();
		double actualtime = glfwGetTime();
		double difference = actualtime - lasttime;
		lasttime = actualtime;
		return difference;
	}
	float max(float a, float b){
		if(a > b) return a;
		return b;
	}
	float min(float a, float b){
		if(a < b) return a;
		return b;
	}
	void render()
	{
		// Get current frame buffer size.
		int width, height;
		curFrameRate = glfwGetTime();
		
		//cout << framespeed << endl;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        mat4 look = glm::lookAt(player.eye, player.lookAtPoint, player.upVector);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float aspect = width / (float) height;
        
        auto Model = make_shared<MatrixStack>();
        auto View = make_shared<MatrixStack>();

        gameModel.checkForCollisions(-1, 2, player.eye);
		// Create the matrix stacks
		auto P = make_shared<MatrixStack>();
		auto MV = make_shared<MatrixStack>();
		// Apply perspective projection.
		P->pushMatrix();
		P->perspective(45.0f, aspect, 0.01f, 100.0f);

		MV->pushMatrix();
		MV->loadIdentity();
        
		// camera rotate
		MV->rotate(camRot, vec3(0, 1, 0));
        toon_prog->bind();
        CHECKED_GL_CALL(glUniformMatrix4fv(toon_prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix())));
        CHECKED_GL_CALL(glUniformMatrix4fv(toon_prog->getUniform("V"), 1, GL_FALSE, value_ptr(look)));
        glUniform3f(toon_prog->getUniform("lightPos"), 0, 10, 0);

        // Generate a dog at a random location if 7 seconds have
        // passed since the last dog was generated
        double curTime = glfwGetTime();
        if (curTime > gameModel.dogSpawnIntervalLow && (gameModel.dogs.size() + 1) <= gameModel.maxNumDogs) {
            glfwSetTime(0.0);
            gameModel.generateDogs(1, allShapesDog);
        }
        
        for(int i = 0; i < gameModel.dogs.size(); i+=1){
            gameModel.dogs[i].draw(Model, bf_prog);
        }
        glUniform3f(toon_prog->getUniform("lightPos"), 0, 1, 0);

        gameModel.plane.draw(Model, bf_prog);
        toon_prog->unbind();
        
        gameModel.updateDogs(framespeed);
        
		P->popMatrix();
		lastFrameRate = glfwGetTime();
		float tmp = lastFrameRate - curFrameRate;
		framespeed = max(0, min(.0007, tmp));//getElapsedTime();
		//if(tmp != framespeed) cout << "changed framespeed from " << tmp << endl;

	}
    
};

int main(int argc, char **argv)
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(512, 512);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initTex(resourceDir);
	vec3 planeMid;
	application->initGeom(resourceDir + "/katiedog.obj", application->allShapesDog, application->dogMiddle, application->dogRad);
    application->initGeom(resourceDir + "/cube.obj", application->allShapesPlane, planeMid, application->planeRad);
    application->gameModel.plane.allShapes = application->allShapesPlane;

    application->gameModel.dogMiddle = application->dogMiddle;
	application->gameModel.dogRad = application->dogRad;
    application->gameModel.generateDogs(1, application->allShapesDog);
    
    application->scrollCallback(windowManager->getHandle(), 0, 0);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
