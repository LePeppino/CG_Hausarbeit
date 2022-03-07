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

	void keyPress(float &fb, float &lr, float dtime);
	float randomFloat(float a, float b);

	int score = 0;
	int crashes = 0;

protected:
    Camera Cam;
    ModelList Models;
    GLFWwindow* pWindow;
    Terrain* pTerrain;
	Airplane* pAirplane;

	float fb = 0;
	float lr = 0;
	
	Ring* rings[50];
	double mx, my;
    float oldXPosition,oldYPosition;
};

#endif /* Application_hpp */
