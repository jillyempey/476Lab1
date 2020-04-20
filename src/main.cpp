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
    std::shared_ptr<Program> prog2;
    std::shared_ptr<Program> prog3;

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
    
    bool beginning = true;
    
    float speed = 2;
    
    shared_ptr<Shape> mesh;
    vector<shared_ptr<Shape>> allShapes;
    

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

	float camRot;
    
    float moveLight = 0;
    float sTheta = 0;


	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		keyToggles[key] = ! keyToggles[key];

        player.w = -(glm::normalize(player.gaze));
        player.u = glm::normalize(glm::cross(player.upVector, player.w));
        player.v = glm::normalize(glm::cross(player.w, player.u));
        
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
        if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
            moveLight -= 1;
        }
        if (key == GLFW_KEY_E && action == GLFW_PRESS) {
            moveLight += 1;
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
        player.phi = 0;// += .05*deltaY;
        player.theta -= .05*deltaX;
        
        if (player.phi > 1.39626) {
            player.phi = 1.39626;
        } else if (player.phi < -1.39626) {
            player.phi = -1.39626;
        }
        
        player.lookAtPoint.x = cos(player.phi)*cos(player.theta) + player.eye.x;
        player.lookAtPoint.y = sin(player.phi) + player.eye.y;
        player.lookAtPoint.z = cos(player.phi) * cos(1.57079632679 - player.theta) + player.eye.z;
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
        prog2 = make_shared<Program>();
        prog2->setVerbose(true);
        prog2->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
        if (! prog2->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        prog2->addUniform("P");
        prog2->addUniform("V");
        prog2->addUniform("M");
        prog2->addUniform("lightPos");
        prog2->addUniform("MatAmb");
        prog2->addUniform("MatDif");
        prog2->addUniform("MatSpec");
        prog2->addUniform("shine");
        prog2->addAttribute("vertPos");
        prog2->addAttribute("vertNor");
        prog2->addAttribute("vertTex");
        
        // Initialize the GLSL program for texture.
        prog3 = make_shared<Program>();
        prog3->setVerbose(true);
        prog3->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
        prog3->init();
        prog3->addUniform("P");
        prog3->addUniform("V");
        prog3->addUniform("M");
        prog3->addUniform("lightPos");
        prog3->addUniform("Texture0");
        prog3->addAttribute("vertPos");
        prog3->addAttribute("vertNor");
        prog3->addAttribute("vertTex");
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

	void initGeom(const std::string& resourceDirectory)
	{
        vector<tinyobj::shape_t> TOshapes;
        vector<tinyobj::material_t> objMaterials;
        string errStr;
        //load in the mesh and make the shape(s)

        bool rc2 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/cube.obj").c_str());
        
        if (!rc2) {
            cerr << errStr << endl;
        } else {
            // get length of shapes
            int length = TOshapes.size();
            
            for(int i = 0; i < length; i++) {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                allShapes.push_back(mesh);
            }
        }
        
        bool rc3 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/katiedog.obj").c_str());
        
        if (!rc3) {
            cerr << errStr << endl;
        } else {
            // get length of shapes
            int length = TOshapes.size();
            
            for(int i = 0; i < length; i++) {
                mesh = make_shared<Shape>();
                mesh->createShape(TOshapes[i]);
                mesh->measure();
                mesh->init();
                dogMin = mesh->min;
                dogMax = mesh->max;
                dogMiddle = vec3((dogMin.x + dogMax.x)/2 , (dogMin.y + dogMax.y)/2, (dogMin.z + dogMax.z)/2);
                allShapes.push_back(mesh);
            }
            
        }
        
		// generate the VAO
		CHECKED_GL_CALL(glGenVertexArrays(1, &VertexArrayID));
		CHECKED_GL_CALL(glBindVertexArray(VertexArrayID));

		// generate vertex buffer to hand off to OGL - using instancing
		CHECKED_GL_CALL(glGenBuffers(1, &pointsbuffer));
		// set the current state to focus on our vertex buffer
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer));
		// actually memcopy the data - only do this once
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW));

		CHECKED_GL_CALL(glGenBuffers(1, &colorbuffer));
		// set the current state to focus on our vertex buffer
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorbuffer));
		// actually memcopy the data - only do this once
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW));
       
	}
   
    float get_rotation(vec3 a, vec3 b){
        float dotProd = dot(a, b);
        float magnitudeA = length(a);
        float magnitudeB = length(b);
        return acos(dotProd / (magnitudeA * magnitudeB));
    }
    
    void drawMovableDog(std::shared_ptr<MatrixStack> Model, Dog *dog) {

        float dogx = dog->position.x;
        float dogz = dog->position.z;
        float theta = dog->isCollected ? 0 : sTheta;
        
        SetMaterial(10);
        Model->pushMatrix();

        Model->translate(vec3(dogx, 0, dogz));
        
        float rotation = get_rotation(vec3(0, 0, 1), dog->orientation) ;

        if(dog->orientation.x < 0){
            rotation *= -1;
        }

        Model->rotate(rotation, vec3(0, 1, 0));
        Model->scale(vec3(.5, .5, .5));
        Model->translate(-dogMiddle);
        setModel(prog2, Model);
        // head
        allShapes[1]->draw(prog2);
        //Model->pushMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, -1.5));
        Model->rotate(-.7*theta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, 1.5));
        Model->pushMatrix();
        setModel(prog2, Model);
        // back right leg bottom
        allShapes[3]->draw(prog2);
        Model->popMatrix();
        setModel(prog2, Model);
        //back right leg top
        allShapes[2]->draw(prog2);
        Model->popMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, 1.5));
        Model->rotate(.7*theta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, -1.5));
        Model->pushMatrix();
        setModel(prog2, Model);
        // front left leg bottom
        allShapes[5]->draw(prog2);
        Model->popMatrix();
        setModel(prog2, Model);
        // front left leg top
        allShapes[6]->draw(prog2);
        Model->popMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, -1.5));
        Model->rotate(.7*theta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, +1.5));
        Model->pushMatrix();
        setModel(prog2, Model);
        // back left leg bottom
        allShapes[7]->draw(prog2);
        Model->popMatrix();
        setModel(prog2, Model);
        // back left leg top
        allShapes[8]->draw(prog2);
        Model->popMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, 1.5));
        Model->rotate(-.7*theta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, -1.5));
        Model->pushMatrix();
        setModel(prog2, Model);
        // front right leg bottom
        allShapes[9]->draw(prog2);
        Model->popMatrix();
        setModel(prog2, Model);
        // front right leg top
        allShapes[10]->draw(prog2);
        Model->popMatrix();
        setModel(prog2, Model);
        // bottom
        allShapes[4]->draw(prog2);
        Model->popMatrix();
    }

    void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    }

	void render()
	{
		// Get current frame buffer size.
		int width, height;
        
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
        prog2->bind();
        CHECKED_GL_CALL(glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix())));
        CHECKED_GL_CALL(glUniformMatrix4fv(prog2->getUniform("V"), 1, GL_FALSE, value_ptr(look)));
        glUniform3f(prog2->getUniform("lightPos"), 0, 10, 0);
        
        SetMaterial(0);
        
        // Generate a dog at a random location if 7 seconds have
        // passed since the last dog was generated
        double curTime = glfwGetTime();
        if (curTime > gameModel.dogSpawnIntervalLow && (gameModel.dogs.size() + 1) <= gameModel.maxNumDogs) {
            glfwSetTime(0.0);
            gameModel.generateDogs(1);
        }
        
        for(int i = 0; i < gameModel.dogs.size(); i+=1){
            drawMovableDog(Model, &(gameModel.dogs[i]));
        }
        
        SetMaterial(4);
        Model->pushMatrix();
        Model->translate(vec3(0, -.75, 0));
        Model->scale(vec3(gameModel.gridWidth, .5, gameModel.gridLength));
        setModel(prog3, Model);
        
        allShapes[0]->draw(prog2);
      
        Model->popMatrix();
      
        prog2->unbind();
        
        gameModel.updateDogs();
        sTheta = sin(glfwGetTime());
		P->popMatrix();

	}
    void SetMaterial(int i) {
        switch (i) {
            case 0: //shiny blue plastic
                glUniform3f(prog2->getUniform("MatAmb"), 0.02, 0.04, 0.2);
                glUniform3f(prog2->getUniform("MatDif"), 0.0, 0.16, 0.9); glUniform3f(prog2->getUniform("MatSpec"), 0.14, 0.2, 0.8); glUniform1f(prog2->getUniform("shine"), 120.0);
                break;
            case 1: // flat grey
                glUniform3f(prog2->getUniform("MatAmb"), 0.13, 0.13, 0.14);
                glUniform3f(prog2->getUniform("MatDif"), 0.3, 0.3, 0.4);
                glUniform3f(prog2->getUniform("MatSpec"), 0.3, 0.3, 0.4);
                glUniform1f(prog2->getUniform("shine"), 4.0);
                break;
            case 2: //hot pink1
                glUniform3f(prog2->getUniform("MatAmb"), 1, 0.714, 0.757);
                glUniform3f(prog2->getUniform("MatDif"), 1, 0.714, 0.757);
                glUniform3f(prog2->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
                glUniform1f(prog2->getUniform("shine"), 27.9);
                break;
            case 3: // green / emerald - my unique material
                glUniform3f(prog2->getUniform("MatAmb"), 0.0215, 0.1745, 0.0215);
                glUniform3f(prog2->getUniform("MatDif"), 0.03568, 0.41424, 0.03568);
                glUniform3f(prog2->getUniform("MatSpec"), 0.633, 0.727811, 0.633);
                glUniform1f(prog2->getUniform("shine"), 0.6);
                break;
            case 4: //hot pink1
                glUniform3f(prog2->getUniform("MatAmb"), 1, 0.07843, 0.576);
                glUniform3f(prog2->getUniform("MatDif"), 1, 0.07843, 0.576);
                glUniform3f(prog2->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
                glUniform1f(prog2->getUniform("shine"), 27.9);
                break;
            case 5: // hot pink2
                glUniform3f(prog2->getUniform("MatAmb"), 0.35, 0.0725, 0.2725);
                glUniform3f(prog2->getUniform("MatDif"), 1.0, .78, .682);
                glUniform3f(prog2->getUniform("MatSpec"), 0.3, 0.1, 0.5);
                glUniform1f(prog2->getUniform("shine"), .078125);
                break;
            case 6: // neon green
                glUniform3f(prog2->getUniform("MatAmb"), 0.2, 0.40725, 0.10725);
                glUniform3f(prog2->getUniform("MatDif"), 0.78, 1.0, .396);
                glUniform3f(prog2->getUniform("MatSpec"), 0.4, 0.7, 0.4);
                glUniform1f(prog2->getUniform("shine"), .078125);
                break;
            case 7: //black rubber
                glUniform3f(prog2->getUniform("MatAmb"), 0.02, 0.02, 0.02);
                glUniform3f(prog2->getUniform("MatDif"), 0.01, 0.01, 0.01);
                glUniform3f(prog2->getUniform("MatSpec"), 0.4, 0.4, 0.4);
                glUniform1f(prog2->getUniform("shine"), .078125);
                break;
            case 8: //jade
                glUniform3f(prog2->getUniform("MatAmb"), 0.135, 0.2225, 0.1575);
                glUniform3f(prog2->getUniform("MatDif"), .54, .89, 0.63);
                glUniform3f(prog2->getUniform("MatSpec"), 0.316228, 0.316228, 0.316228);
                glUniform1f(prog2->getUniform("shine"), 0.1);
                break;
            case 9: //bronze
                glUniform3f(prog2->getUniform("MatAmb"), 0.25, 0.148, 0.06475);
                glUniform3f(prog2->getUniform("MatDif"), .4, .2368, 0.1036);
                glUniform3f(prog2->getUniform("MatSpec"), 0.774597, 0.458561, 0.200621);
                glUniform1f(prog2->getUniform("shine"), 76.8);
                break;
            case 10: //pearl
                glUniform3f(prog2->getUniform("MatAmb"), 0.25, 0.20725, 0.20725);
                glUniform3f(prog2->getUniform("MatDif"), 1.0, .829, 0.829);
                glUniform3f(prog2->getUniform("MatSpec"), 0.296648, 0.296648, 0.296648);
                glUniform1f(prog2->getUniform("shine"), 11.264);
                break;
            case 11: //ruby
                glUniform3f(prog2->getUniform("MatAmb"), 0.1745, 0.01175, 0.01175);
                glUniform3f(prog2->getUniform("MatDif"), .61424, .04136, 0.04136);
                glUniform3f(prog2->getUniform("MatSpec"), 0.727811, 0.626959, 0.626959);
                glUniform1f(prog2->getUniform("shine"), 76.8);
                break;
        }
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
	
	application->initGeom(resourceDir);
    cout << application->dogMiddle.x << " " << application->dogMiddle.y << " " << application->dogMiddle.z << " " << endl;

    application->gameModel.generateDogs(1);
    
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
