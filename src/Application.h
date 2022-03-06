//
//  Application.hpp
//  ogl4
//
//  Created by Philipp Lensing on 16.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <list>
#include "camera.h"
#include "phongshader.h"
#include "constantshader.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "basemodel.h"
#include "terrain.h"
#include "airplane.h"
#include "Tank.h"
#include "Ring.h"

class Application
{
public:
    typedef std::list<BaseModel*> ModelList;
    Application(GLFWwindow* pWin);
    void start();
    void update(float dtime);
    void draw();
    void end();
	void createScene();

	void keyPress(float&, float&, float dtime);
	void keyPress(float&, float &);
	float toRadian(float degrees);
	float randomFloat(float a, float b);

protected:
    Camera Cam;
    ModelList Models;
    GLFWwindow* pWindow;
    Terrain* pTerrain;

	Airplane* pAirplane;
	//Model* pPlane;
	float fb = 0;
	float lr = 0;
	float moveVel = 0;
	float turnVel = 0;
	Vector calc3DRay(float x, float y, Vector& Pos);
	Tank* pTank;
	Model* pTankTop;
	Model* pTankBot;
	Ring* rings[50];
	float fb, lr;
	double mx, my;
    float oldXPosition,oldYPosition;
};

#endif /* Application_hpp */
