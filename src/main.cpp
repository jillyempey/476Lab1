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

 // value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:
    float randFloat(float l, float h)
    {
        float r = rand() / (float) RAND_MAX;
        return (1.0f - r) * l + r * h;
    }
    
    struct TreeStruct {
        float x;
        float y = 15;
        float z;
        float thetaOffset;
        int material;
        bool rain = false;
    } t1, dummy1;
    
	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
    std::shared_ptr<Program> prog2;
    std::shared_ptr<Program> prog3;
    vector<Dog> dogs;
    vector<shared_ptr<Shape>> dummy;
    float dummyTheta = 0;
    
    float gridWidth = 50;
    float gridLength = 50;
    
    double dogSpawnIntervalLow = 7.0;
    
    int maxNumDogs = 1;
    
    int dogsCollected = 0;
    
    vector<std::vector<std::shared_ptr<Particle>>> allParticles;
    std::vector<std::shared_ptr<Particle>> particles;
	

	// CPU array for particles - redundant with particle structure
	// but simple
	int numP = 300;
	GLfloat points[900];
	GLfloat pointColors[1200];

	GLuint pointsbuffer;
	GLuint colorbuffer;
    
    bool isRaining = false;
    
    float phi = -.3;
    float theta = -1.58;
    float xTrans = 4;
    //float dogTheta = 0;
    vec3 eye = glm::vec3(0, 15, 0);
    vec3 lookAtPoint = glm::vec3(0, 0, 1);
    vec3 upVector = glm::vec3(0, 1, 0);
    
    vec3 gaze = lookAtPoint - eye;
    vec3 w = -(glm::normalize(gaze));
    vec3 u = glm::normalize(glm::cross(upVector, w));
    vec3 v = glm::normalize(glm::cross(w, u));
    vec3 dogMin;
    vec3 dogMax;
    vec3 dogMiddle;
    
    bool move = true;
    
    bool beginning = true;
    
    float speed = 2;
    
    shared_ptr<Shape> mesh;
    vector<shared_ptr<Shape>> allShapes;
    
    vector<TreeStruct> trees;
    vector<TreeStruct> dummies;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// OpenGL handle to texture data
	shared_ptr<Texture> texture;
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

        w = -(glm::normalize(gaze));
        u = glm::normalize(glm::cross(upVector, w));
        v = glm::normalize(glm::cross(w, u));
        
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            eye -= speed * u;
            lookAtPoint -= speed * u;
            gaze = lookAtPoint - eye;
        }
        if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            eye += speed * u;
            lookAtPoint += speed * u;
            gaze = lookAtPoint - eye;
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            eye += speed * w;
            lookAtPoint += speed * w;
            gaze = lookAtPoint - eye;
        }
        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            eye -= speed * w;
            lookAtPoint -= speed * w;
            gaze = lookAtPoint - eye;
        }
        if (key == GLFW_KEY_X && action == GLFW_PRESS) {
            
            if(move == false) {
                move = true;
                sTheta = 0;
            } else {
                move = false;
            }
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
		if (key == GLFW_KEY_K && action == GLFW_PRESS)
		{
			//camRot -= 0.314f;
		}
		if (key == GLFW_KEY_L && action == GLFW_PRESS)
		{
			camRot += 0.314f;
		}
        if (key == GLFW_KEY_O && action == GLFW_PRESS)
		{
			speed += 1;
		}

        // dog orientation debug:
        if (key == GLFW_KEY_I && action == GLFW_PRESS)
		{
			dogs[0].orientation = vec3(0, 0, 1);
		}
        if (key == GLFW_KEY_K && action == GLFW_PRESS)
		{
			dogs[0].orientation = vec3(1, 0, 0);
		}
        if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			dogs[0].orientation = vec3(0, 0, -1);
		}
        if (key == GLFW_KEY_J && action == GLFW_PRESS)
		{
			dogs[0].orientation = vec3(-1, 0, 0);
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
        phi += .05*deltaY;
        theta -= .05*deltaX;
        
        if (phi > 1.39626) {
            phi = 1.39626;
        } else if (phi < -1.39626) {
            phi = -1.39626;
        }
        
        lookAtPoint.x = cos(phi)*cos(theta) + eye.x;
        lookAtPoint.y = sin(phi) + eye.y;
        lookAtPoint.z = cos(phi) * cos(1.57079632679 - theta) + eye.z;
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			//cout << "Pos X " << posX << " Pos Y " << posY << endl;
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

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(
			resourceDirectory + "/lab10_vert.glsl",
			resourceDirectory + "/lab10_frag.glsl");
		if (! prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("MV");
		prog->addUniform("alphaTexture");
		prog->addAttribute("vertPos");
        
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
		texture = make_shared<Texture>();
		texture->setFilename(resourceDirectory + "/dollarbills.jpg");
		texture->init();
		texture->setUnit(0);
		texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture0 = make_shared<Texture>();
        texture0->setFilename(resourceDirectory + "/whiteTex.jpg");
        texture0->init();
        texture0->setUnit(0);
        texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	}

	void initParticles()
	{
        int n = numP;
        
        for (int i = 0; i < n; ++ i)
        {
            auto particle = make_shared<Particle>();
            particles.push_back(particle);
            for (int i = 0; i < trees.size(); i++) {
                particle->load(trees[i].x, trees[i].y, trees[i].z);
            }
        }
	}

	void initGeom(const std::string& resourceDirectory)
	{
        vector<tinyobj::shape_t> TOshapes;
        vector<tinyobj::material_t> objMaterials;
        string errStr;
        //load in the mesh and make the shape(s)
        
        bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/sphere.obj").c_str());
        
        if (!rc) {
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
        
        bool rc4 = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/dummy.obj").c_str());
        
        if (!rc4) {
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
        t1.x = 9.71789;
        t1.z = -2.35851;
        t1.thetaOffset = .1;
        t1.material = 4;
        trees.push_back(t1);
        for (int i = 0; i < 10; i++) {
            t1.x = randFloat(-20., 20.);
            t1.z = randFloat(-20., 20.);
            t1.thetaOffset = randFloat(-.2, .20);
            t1.material = (int)randFloat(5.0, 6.999);
            trees.push_back(t1);
            
            dummy1.x = randFloat(-20., 20.);
            dummy1.y = randFloat(.1, 30.);
            dummy1.z = randFloat(-20., 20.);
            dummy1.material = (int)randFloat(1.0, 11.999);
            dummies.push_back(dummy1);
        }
	}

	// Note you could add scale later for each particle - not implemented
	void updateGeom()
	{
		glm::vec3 pos;
		glm::vec4 col;

		// go through all the particles and update the CPU buffer
        for (int i = 0; i < numP; i++)
		{
			pos = particles[i]->getPosition();
			col = particles[i]->getColor();
			points[i * 3 + 0] = pos.x;
			points[i * 3 + 1] = pos.y;
			points[i * 3 + 2] = pos.z;
			pointColors[i * 4 + 0] = col.r + col.a / 10.f;
			pointColors[i * 4 + 1] = col.g + col.g / 10.f;
			pointColors[i * 4 + 2] = col.b + col.b / 10.f;
			pointColors[i * 4 + 3] = col.a;
		}

		// update the GPU data
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW));
		CHECKED_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * numP * 3, points));

		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorbuffer));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW));
		CHECKED_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * numP * 4, pointColors));

		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	/* note for first update all particles should be "reborn"
	 * which will initialize their positions */
	void updateParticles()
	{
        // update the particles
        for (int i = 0; i < trees.size(); i++) {
            if (trees[i].rain) {
                for (auto particle : particles)
                {
                    particle->update(t, h, g, keyToggles, trees[i].x, trees[i].y, trees[i].z);
                }
                t += h;

                // Sort the particles by Z
                auto temp = make_shared<MatrixStack>();
                temp->rotate(camRot, vec3(0, 1, 0));

                ParticleSorter sorter;
                sorter.C = temp->topMatrix();
                std::sort(particles.begin(), particles.end(), sorter);
            }
        }
	}
    void drawTree(double x, double y, double z, float thetaOffset, int material, std::shared_ptr<MatrixStack> Model) {
        SetMaterial(material);
        Model -> pushMatrix();
            // draw trunk of tree
            sTheta += thetaOffset;
            Model->translate(vec3(x, y, z));
            Model->pushMatrix();
                Model->rotate(.2*sTheta, vec3(0, 0, 1));
                Model->translate(vec3(0, 2, 0));
                Model->pushMatrix();
                    Model->rotate(.15*sTheta, vec3(0, 0, 1));
                    Model->translate(vec3(0, 1.8,0));
                    Model->pushMatrix();
                        Model->rotate(.10*sTheta, vec3(0, 0, 1));
                        Model->translate(vec3(0,1.6,0));
                        Model->pushMatrix();
                            Model->translate(vec3(0, 1.6,0));
                            Model->pushMatrix();
                                Model->rotate(.3*sTheta, vec3(0, 0, 1));
                                Model->translate(vec3(0, 1.4,0));
                                Model->rotate(0.05, vec3(0, 0, 1));
                                Model->scale(vec3(2, .4, 2));
                                setModel(prog2, Model);
                                allShapes[0]->draw(prog2);
                            Model->popMatrix();
                            Model->rotate(-0.15, vec3(0, 0, 1));
                            Model->scale(vec3(3, .6, 3));
                            setModel(prog2, Model);
                            allShapes[0]->draw(prog2);
                        Model->popMatrix();
                        Model->rotate(0.05, vec3(0, 0, 1));
                        Model->scale(vec3(4, 1, 4));
                        setModel(prog2, Model);
                        allShapes[0]->draw(prog2);
                    Model->popMatrix();
                    Model->rotate(-0.15, vec3(0, 0, 1));
                    Model->scale(vec3(5, 1.4, 5));
                    setModel(prog2, Model);
                    allShapes[0]->draw(prog2);
                Model->popMatrix();
                Model->rotate(0.05, vec3(0, 0, 1));
                Model->scale(vec3(6, 1.4, 6));
                setModel(prog2, Model);
                allShapes[0]->draw(prog2);
            Model->popMatrix();
            Model->scale(vec3(1, 2, 2));
            setModel(prog2, Model);
            allShapes[0]->draw(prog2);
        Model->popMatrix();
    }
    float get_rotation(vec3 a, vec3 b){
        // theta = arccos(dot(a, b) / (magnitude(a) * magnitude(b))
        float dotProd = dot(a, b);
        float magnitudeA = length(a);
        float magnitudeB = length(b);
        return acos(dotProd / (magnitudeA * magnitudeB));
    }
    
//    float rotateDog(vec3 currentOrientation, vec3 goalOrientation) {
//
//
//    goalOrientation = goalOrientation.subtract(tM.position);
//    Return -Math.atan2(lAt.z, lAt.x) - Math.PI/2;
//    }
    
    void drawMovableDog(std::shared_ptr<MatrixStack> Model, Dog *dog) {
        //dog->position.x = cos(dog->theta) * dog->position.x + dog->offset;
        //dog->position.z = sin(dog->theta) * dog->position.z + dog->offset;
        //cout << dog->offset << endl;
        float dogx = dog->position.x;
        float dogz = dog->position.z;
        // eye.x = dogX;
        // eye.z = dogZ + 14;
        for(int j = 0; j < trees.size(); j++){
            float num = pow((trees[j].x - dogx),2);
            //            num += pow((positions[k].y - positions[i].y),2);
            num += pow((trees[j].z - dogz),2);
            float d = pow(num,0.5);
            float rad1 = .5;
            float rad2 = 1;
            
            // if (d < (rad1 + rad2)) {
            //     trees[j].rain = true;
            //     isRaining = true;
            // }
            
        }
        
        SetMaterial(10);
        Model->pushMatrix();
        //Model->rotate(5, vec3(0, 1, 0));
        //Model->translate(vec3(0, 0, -10));
        Model->translate(vec3(dogx, 0, dogz));
        
        //float rotation = 180 * get_rotation(vec3(0, 0, 1), dog->orientation) / 3.14159 ;//- 90.0;
        float rotation = get_rotation(vec3(0, 0, 1), dog->orientation) ;//- 90.0;

        if(dog->orientation.x > 0){
            rotation *= -1;
        }
        //cout << rotation << endl;

        if(dog->orientation.z > 0){
            //cout << "positive z" << endl;
            rotation += 1.5708;
        }
        //cout << rotation << endl;
        //Model->translate(vec3(1.15945 ,-1.17137 ,1.34475));

        Model->rotate(rotation, vec3(0, 1, 0));
        Model->scale(vec3(.5, .5, .5));
        Model->translate(-dogMiddle);
        setModel(prog2, Model);
        // head
        allShapes[2]->draw(prog2);
        //Model->pushMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, -1.5));
        Model->rotate(-.7*sTheta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, 1.5));
        Model->pushMatrix();
        setModel(prog2, Model);
        // back right leg bottom
        allShapes[4]->draw(prog2);
        Model->popMatrix();
        setModel(prog2, Model);
        //back right leg top
        allShapes[3]->draw(prog2);
        Model->popMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, 1.5));
        Model->rotate(.7*sTheta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, -1.5));
        Model->pushMatrix();
        setModel(prog2, Model);
        // front left leg bottom
        allShapes[6]->draw(prog2);
        Model->popMatrix();
        setModel(prog2, Model);
        // front left leg top
        allShapes[7]->draw(prog2);
        Model->popMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, -1.5));
        Model->rotate(.7*sTheta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, +1.5));
        Model->pushMatrix();
        setModel(prog2, Model);
        // back left leg bottom
        allShapes[8]->draw(prog2);
        Model->popMatrix();
        setModel(prog2, Model);
        // back left leg top
        allShapes[9]->draw(prog2);
        Model->popMatrix();
        Model->pushMatrix();
        Model->translate(vec3(0, -.9, 1.5));
        Model->rotate(-.7*sTheta, vec3(1, 0, 0));
        Model->translate(vec3(0, 1.1, -1.5));
        Model->pushMatrix();
        setModel(prog2, Model);
        // front right leg bottom
        allShapes[10]->draw(prog2);
        Model->popMatrix();
        setModel(prog2, Model);
        // front right leg top
        allShapes[11]->draw(prog2);
        Model->popMatrix();
        setModel(prog2, Model);
        // bottom
        allShapes[5]->draw(prog2);
        Model->popMatrix();
        //Model->popMatrix();
    }
    void drawDummy(double x, double y, double z, int material, std::shared_ptr<MatrixStack> Model) {
        // draw dummy
        SetMaterial(material);
        Model->pushMatrix();
            Model->rotate(.5, vec3(0, 0, 1));
            Model->rotate(dummyTheta, vec3(0, 1, 0));
            Model->translate(vec3(x, y, z));
            Model->scale(vec3(.03, .03, .03));
            setModel(prog2, Model);
            for (int i = 12; i < allShapes.size(); i++) {
                allShapes[i]->draw(prog2);
            }
        Model->popMatrix();
    }
    
    void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    }
    void generateDogs(int numDogs){
        for(int i = 0; i < numDogs; i++){
            Dog newDog;
            newDog.theta = 0;
            newDog.offset = randFloat(0, 10);
            newDog.position = vec3(randFloat(-gridLength/2 + 2, gridWidth/2 - 2), 0, randFloat(-gridLength/2 + 2, gridWidth/2 - 2));
            //newDog.position = vec3(0,0,0);
            //cout << newDog.position.x << " " << newDog.position.y << " " << newDog.position.z << endl;
            newDog.speed = .3;
            newDog.id = i;
            newDog.pathRadius = randFloat(5, 10);
            newDog.modelRadius = 2;
            newDog.isCollected = false;
            newDog.orientation = glm::normalize(vec3(randFloat(-1, 1), 0, randFloat(-1, 1)));
            //newDog.orientation = vec3(0,0,1);
            dogs.push_back(newDog);
        }
    }
    void updateDogs(){
        for(int i = 0; i < dogs.size(); i++){
            checkForCollisions(i, dogs[i].modelRadius, dogs[i].position);
            dogs[i].move();
        }
    }
    
    void checkForCollisions(int dogIndex, float radius, vec3 position) {
        
        // check collisions against all the dogs
        for (int i = dogIndex + 1; i < dogs.size(); i++) {
            if (distance(position, dogs[i].position) <= radius + dogs[i].modelRadius) {
                if (dogIndex == -1 && !dogs[i].isCollected) {
                    collectDog(i);
                    dogs[i].isCollected = true;
                    
                } else {
                    dogs[dogIndex].orientation = dogs[dogIndex].orientation * vec3(-1, 1, -1);
                }
                return;
            }
        }
        
        float gridWidthMin = -gridWidth/2;
        float gridWidthMax = gridWidth/2;
        float gridLengthMin = -gridLength/2;
        float gridLengthMax = gridLength/2;
        
        // check collisions against all the walls
        if (dogIndex != -1) {
            if (dogs[dogIndex].position.x >= gridWidthMax - radius || dogs[dogIndex].position.x <= gridWidthMin + radius) {
                dogs[dogIndex].orientation.x *= -1;
            }
            if (dogs[dogIndex].position.z >= gridLengthMax - radius || dogs[dogIndex].position.z <= gridLengthMin + radius) {
                dogs[dogIndex].orientation.z *= -1;
                
            }
        }
    }
    
    
    void collectDog(int dogIndex){
        dogs[dogIndex].speed = 0;
        dogsCollected += 1;
        cout << "Dogs Collected: " << dogsCollected << " / " << dogs.size() << " total dogs" << endl;

        
    }
    
	void render()
	{
		// Get current frame buffer size.
		int width, height;
        
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        mat4 look = glm::lookAt(eye, lookAtPoint, upVector);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float aspect = width / (float) height;
        
        auto Model = make_shared<MatrixStack>();
        auto View = make_shared<MatrixStack>();

        checkForCollisions(-1, 2, eye);
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
        //for (int i = 0; i < trees.size()/2; i++) {
            //drawTree(trees[i].x, .1, trees[i].z, trees[i].thetaOffset, trees[i].material, Model);
            //drawDummy(dummies[i].x, dummies[i].y, dummies[i].z, dummies[i].material, Model);
        //}
        
        SetMaterial(0);
        
        // Generate a dog at a random location if 7 seconds have
        // passed since the last dog was generated
        double curTime = glfwGetTime();
       if (curTime > dogSpawnIntervalLow && (dogs.size() + 1) <= maxNumDogs) {
           glfwSetTime(0.0);
           generateDogs(1);
       }
        
        for(int i = 0; i < dogs.size(); i+=1){
            drawMovableDog(Model, &(dogs[i]));
        }
        
        SetMaterial(2);
        Model->pushMatrix();
        Model->translate(vec3(0, -1.5, 0));
        Model->scale(vec3(gridWidth, .5, gridLength));
        setModel(prog2, Model);
        allShapes[1]->draw(prog2);
        Model->popMatrix();
        
       
        prog2->unbind();
		// Draw
		prog->bind();
		updateParticles();
		updateGeom();

		texture->bind(prog->getUniform("alphaTexture"));
		CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix())));
		CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, value_ptr(look)));
        if (isRaining) {
            CHECKED_GL_CALL(glEnableVertexAttribArray(0));
            CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer));
            CHECKED_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0));

            CHECKED_GL_CALL(glEnableVertexAttribArray(1));
            CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorbuffer));
            CHECKED_GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0));

            CHECKED_GL_CALL(glVertexAttribDivisor(0, 1));
            CHECKED_GL_CALL(glVertexAttribDivisor(1, 1));
            // Draw the points !
            CHECKED_GL_CALL(glDrawArraysInstanced(GL_POINTS, 0, 1, numP));

            CHECKED_GL_CALL(glVertexAttribDivisor(0, 0));
            CHECKED_GL_CALL(glVertexAttribDivisor(1, 0));
            CHECKED_GL_CALL(glDisableVertexAttribArray(0));
            CHECKED_GL_CALL(glDisableVertexAttribArray(1));
        }
		prog->unbind();
     
		// Pop matrix stacks.
		MV->popMatrix();
        dummyTheta -= .03;
        //dogTheta += .005;
        updateDogs();
        if (move == true) {
            //animation update example
            sTheta = sin(glfwGetTime());
        }
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
    Player p = Player();
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
    application->initParticles();

    application->generateDogs(1);
    
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
