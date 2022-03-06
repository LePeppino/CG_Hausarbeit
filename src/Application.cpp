//
//  Application.cpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Application.h"
#ifdef WIN32
#include <GL/glew.h>
#include <glfw/glfw3.h>
#define _USE_MATH_DEFINES
#include <math.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <glfw/glfw3.h>
#endif
#include "lineplanemodel.h"
#include "triangleplanemodel.h"
#include "trianglespheremodel.h"
#include "lineboxmodel.h"
#include "triangleboxmodel.h"
#include "model.h"
#include "terrainshader.h"
#include "WaterShader.h"
#include "rgbimage.h"


#ifdef WIN32
#define ASSET_DIRECTORY "../assets/"
#else
#define ASSET_DIRECTORY "assets/"
#endif


Application::Application(GLFWwindow* pWin) : pWindow(pWin), Cam(pWin)
{
	int windowWidth, windowHeight;
	glfwGetWindowSize(pWindow, &windowWidth, &windowHeight); //für PostFX
	createScene();
	//TODO PostFX
}

void Application::start()
{
    glEnable (GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::update(float dtime)
{
	keyPress(fb, lr, dtime);
	pAirplane->steer(fb, lr);
	pAirplane->update(dtime, Vector(0,0,0), pTerrain, Cam);
}

// Steuerung des Fliegers mit weicher Beschleunigung
// https://gamedev.stackexchange.com/questions/127924/smooth-character-movement
void Application::keyPress(float &fb, float &lr, float dtime)
{
	float minVel = -1;
	float maxVel = 1;
	float acc = 2;
	bool key_pressed = false;

	if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS) {
		key_pressed = true;
		if (fb > minVel) {
			fb -= acc * dtime;
			if (fb < minVel) {
				fb = minVel;
			}
		}
	}
	if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS) {
		key_pressed = true;
		if (fb < maxVel) {
			fb += acc * dtime;
			if (fb > maxVel) {
				fb = maxVel;
			}
		}
	}
	if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) {
		key_pressed = true;
		if (lr > minVel) {
			lr -= acc * dtime;
			if (lr < minVel) {
				lr = minVel;
			}
		}
	}
	if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) {
		key_pressed = true;
		if (lr < maxVel) {
			lr += acc * dtime;
			if (lr > maxVel) {
				lr = maxVel;
			}
		}
	}
	//Keine gedrückten Tasten ODER beide entgegengesetzte Tasten gedrückt
	if (!key_pressed || 
		glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS ||
		glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) 
	{
		//FB Beschleunigung abschwächen
		if (fb < 0) {
			fb += acc * dtime;
			if (fb > 0) {
				fb = 0;
			}
		}
		else if (fb > 0) {
			fb -= acc * dtime;
			if (fb < 0) {
				fb = 0;
			}
		}
		//LR Beschleunigung abschwächen
		if (lr < 0) {
			lr += acc * dtime;
			if (lr > 0) {
				lr = 0;
			}
		}
		else if (lr > 0) {
			lr -= acc * dtime;
			if (lr < 0) {
				lr = 0;
			}
		}
	}
}
*/
void Application::draw()
{
    // 1. clear screen
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. setup shaders and draw models
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
    {
        (*it)->draw(Cam);
    }
    
    // 3. check once per frame for opengl errors
    GLenum Error = glGetError();
    assert(Error==0);
}

void Application::end()
{
    for( ModelList::iterator it = Models.begin(); it != Models.end(); ++it )
        delete *it;
    
    Models.clear();
}

// Quelle: https://stackoverflow.com/questions/5289613/generate-random-float-between-two-floats
float Application::randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return  a + r;
}

void Application::createScene() 
{
	BaseModel* pModel;

	//Map generieren aus Heightmap
	const char* file = ASSET_DIRECTORY "heightmap_iceland.png";
	Texture t;
	t.load(file);
	const RGBImage* src = t.getRGBImage();

	//MixMap generieren
	RGBImage dst(src->width(), src->height());
	RGBImage::SobelFilter(dst, *src, 10.f);
	dst.saveToDisk(ASSET_DIRECTORY "mixmap_sobel.bmp");

	//Skybox generieren und ausrichten
	pModel = new Model(ASSET_DIRECTORY "skybox.obj", true);
	pModel->shader(new PhongShader(), true);
	Matrix skyboxMatrix, skyboxScale, skyboxTrans;
	pModel->transform();
	skyboxScale.scale(3);
	skyboxTrans.translation(0, 5, 0);
	skyboxMatrix = skyboxScale * skyboxTrans;
	pModel->transform(skyboxMatrix);
	Models.push_back(pModel);

	//Terrain generieren
	pTerrain = new Terrain();
	TerrainShader* pTerrainShader = new TerrainShader(ASSET_DIRECTORY);
	pTerrainShader->diffuseTexture(Texture::LoadShared(ASSET_DIRECTORY "grass2.bmp"));
	pTerrain->shader(pTerrainShader, true);
	pTerrain->load(ASSET_DIRECTORY "heightmap_iceland.png", ASSET_DIRECTORY"grass2.bmp", ASSET_DIRECTORY"rock2.jpg", ASSET_DIRECTORY "mixmap_sobel.bmp");
	//pTerrain->loadNormalTextures(ASSET_DIRECTORY"grass2_n.jpg", ASSET_DIRECTORY"rock2_n.jpg");
	Models.push_back(pTerrain);

	//Airplane
	//Model von https://free3d.com/3d-model/airplane-v2--659376.html
	pAirplane = new Airplane();
	pAirplane->shader(new PhongShader(), true);
	pAirplane->loadModels(ASSET_DIRECTORY "airplane_v2/11804_Airplane_v2_l2.obj");

	Models.push_back(pAirplane);

	//Meeresspiegel als PlaneModel
	//Nach dem Terrain und Objekten rendern, um Transparenz sichtbar zu machen!
	pModel = new TrianglePlaneModel(1001, 1001, 100, 100, true);
	WaterShader* pWaterShader = new WaterShader(ASSET_DIRECTORY);
	pWaterShader->diffuseTexture(Texture::LoadShared(ASSET_DIRECTORY "water2.jpg"));
	pModel->shader(pWaterShader, true);
	Matrix waterMatrix;
	waterMatrix.translation(0, 5, 0);
	pModel->transform(waterMatrix);
	Models.push_back(pModel);


	PhongShader* pPhongShader;
	pTank = new Tank();
	pPhongShader = new PhongShader();
	pTank->shader(pPhongShader, true);
	pTank->loadModels(ASSET_DIRECTORY "tank_bottom.dae", ASSET_DIRECTORY "tank_top.dae");
	Models.push_back(pTank);

	for (int i = 0; i < 500; i++) {
		pModel = new Model(ASSET_DIRECTORY "Lowpoly_tree_sample.obj");
		pModel->shader(new PhongShader, true);
		
		Matrix pos, scale, tree;
		pos = pModel->transform();
		pos.translation();
		float x, z, y;
		x = randomFloat(-300.0, 300.0);
		z = randomFloat(-300.0, 300.0);
		float convX, convZ;
		convX = (((x / 24.6) + 24.6 / 2.0) / 0.03075);
		convZ = (((z / 24.6) + 24.6 / 2.0) / 0.03075);
		y = pTerrain->heights[static_cast<int>(convX)][static_cast<int>(convZ)] * 20;
		if (y > 5) {
			scale.scale(0.2);
			pos.translation(x, y, z);
			tree = pos * scale;
			pModel->transform(tree);
			Models.push_back(pModel);
		}
	}

	for (int i = 0; i < 50; i++) {
		rings[i] = new Ring();
		pPhongShader = new PhongShader();
		rings[i]->shader(pPhongShader, true);
		rings[i]->loadModels(ASSET_DIRECTORY "ring.obj", pTerrain);
		Models.push_back(rings[i]);
	}
}