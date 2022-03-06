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
	//TODO PostFX?
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
    
	keyPress(fb, lr);
	pTank->steer(fb, lr);
	fb = 0;
	lr = 0;
	pTank->update(dtime, Vector(0,0,0), pTerrain, Cam);
	//Cam.update();

	for (int i = 0; i < 50; i++) {
			if (rings[i]->calcCollision(pTank) && !(rings[i]->getActivated())) {
				rings[i]->activate();
				std::cout << "HIT" << std::endl;
			}
	}
	
}

void Application::keyPress(float &fb, float &lr) {


	if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS) {
		fb = -1;
	}
	if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS) {
		fb = 1;
	}


	if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) {
		lr = -1;
	}

	if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) {
		lr = 1;
	}


}

float Application::toRadian(float degrees) {

	return (float)degrees * M_PI / 180;

}
/*s
Vector Application::calc3DRay(float x, float y, Vector& Pos)
{
	//normalisierte Koordinaten berechnen
	int windowWidthHalf = 0;
	int windowHeightHalf = 0;
	glfwGetWindowSize(this->pWindow, &windowWidthHalf, &windowHeightHalf);
	windowHeightHalf /= 2;
	windowWidthHalf /= 2;

	//Feld auf -1 bis 1 normalisieren
	float xNormal, yNormal;
	xNormal = (x - windowWidthHalf) / windowWidthHalf;
	yNormal = -(y - windowHeightHalf) / windowHeightHalf;

	//Richtungsvector(Kamararaum) erzeugen mit inverser Projektionsmatrix
	Matrix projectionCam = this->Cam.getProjectionMatrix();
	Vector normalCursor(xNormal, yNormal, 0);
	Vector direction = projectionCam.invert() * normalCursor;

	//Umwandlung des Richtungsvectors in den Weltraum
	Matrix viewMatrix = this->Cam.getViewMatrix();
	Vector directionInWeltraum = viewMatrix.invert().transformVec3x3(direction);

	//Schnittpunkt mit der Ebene y=0 bestimmen
	Vector camPos = this->Cam.position();
	directionInWeltraum.normalize();
	float s;
	camPos.triangleIntersection(directionInWeltraum, Vector(0, 0, 1), Vector(0, 0, 0), Vector(1, 0, 0), s);

	//falls directionWolrld von der Ebene weg zeigt (0,0,0) zurückgeben
	if (s < 0) {
		return Vector(0, 0, 0);
	}

	//Vektor zum Punkt auf der Ebene y=0 berechnen
	Vector positionOnGround = camPos + directionInWeltraum * s;

	//float Ungenauigkeiten umgehen indem der Vektor erneut auf y = 0 gesetzt wird
	return Vector(positionOnGround.X, 0, positionOnGround.Z);


	// Pos:Ray Origin
	// return:Ray Direction

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
	//TODO: DetailTexes aussortieren!
	pTerrain->load(ASSET_DIRECTORY "heightmap_iceland.png", ASSET_DIRECTORY"grass2.bmp", ASSET_DIRECTORY"rock2.jpg", ASSET_DIRECTORY"sand2.jpg", ASSET_DIRECTORY"snow2.jpg", ASSET_DIRECTORY "mixmap_sobel.bmp");
	Models.push_back(pTerrain);

	//Meeresspiegel als PlaneModel
	//Nach dem Terrain rendern, um Transparenz sichtbar zu machen!
	pModel = new TrianglePlaneModel(601, 601, 100, 100, true);
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