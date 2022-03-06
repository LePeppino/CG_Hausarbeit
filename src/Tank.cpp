//
//  Tank.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Tank.h"

#define TERRAIN_DIM 24.6
#define TERRAIN_HEIGHT 20

Tank::Tank():fb(0,0,0), angleCorrection(0.0f)
{
	
}

Tank::~Tank()
{
    delete chassis;
    delete cannon;
}

bool Tank::loadModels(const char* ChassisFile, const char* CannonFile)
{

    if (ChassisFile != nullptr && CannonFile != nullptr) {
        chassis = new Model(ChassisFile);
        chassis->shader(pShader);
        cannon = new Model(CannonFile);
        cannon->shader(pShader);
		Matrix chassisMatrix, chassisMov, chassisUrsprung;
		chassisUrsprung = chassis->transform();
		chassisMov.translation(0, 35, 0);
		chassisMatrix = chassisUrsprung * chassisMov;
		this->chassis->transform(chassisMatrix);
		this->cannon->transform(chassisMatrix);
        return true; 
    }

    return false;
    
}

void Tank::steer( float ForwardBackward, float LeftRight)
{
    fb.X = ForwardBackward;
    fb.Y = LeftRight;
}

void Tank::update(float dtime, Vector size, Terrain *terrain, Camera& cam)
{
	Matrix chassisMatrix, chassisMov, chassisTurn, chassisPitch, chassisRoll, chassisUrsprung;
	Matrix cannonMatrix;
	//chassis
	chassisUrsprung = chassis->transform();
	float x = chassisUrsprung.m03;
	float y = chassisUrsprung.m23;

	Vector planePos = chassis->transform().translation();

	// ((X Position / Breite des Terrains ) + ( Breite des Terrains / 2) / (Breite des Terains/Bildbreite)
	float tryoutX = (((planePos.X / TERRAIN_DIM) + TERRAIN_DIM / 2.0) / (TERRAIN_DIM / 800)); 
	float tryoutZ = (((planePos.Z / TERRAIN_DIM) + TERRAIN_DIM / 2.0) / (TERRAIN_DIM / 800));
	float heightForCords = terrain->heights[static_cast<int>(tryoutX)][static_cast<int>(tryoutZ)];
	try {

		// Während Spieler über Terrain fliegt
		if (planePos.Y > heightForCords * TERRAIN_HEIGHT)
		{

			//https://www.youtube.com/watch?v=RpNPW89Y-3A

			//TODO: Leichte Bewegung nach links/rechts beim Neigen
			chassisPitch.rotationZ(-1 * fb.X * dtime);

			chassisRoll.rotationX(-1 * fb.Y * dtime);
			
			chassisTurn.rotationY(0.4 * fb.Y * dtime);

			chassisMov.translation(0.15, 0, 0);

			chassisMatrix = chassisUrsprung * chassisPitch * chassisRoll * chassisTurn * chassisMov;
			this->chassis->transform(chassisMatrix);

			cannonMatrix = chassisUrsprung;
			this->cannon->transform(cannonMatrix);

			// Third Person Camera
			Matrix thirdPerson;
			Vector target = chassisMatrix.translation();
			thirdPerson.lookAt(target, chassisMatrix.up(), chassisMatrix.translation() - chassisMatrix.right() * 7 + chassisMatrix.up() * 2);
			cam.setViewMatrix(thirdPerson);
			cam.setPosition(target); //for fog shaders
		}
		else 
		{
			//Kollision --> Feedback & neu starten

		}
	}
	catch (...) {

	}
}

void Tank::draw(const BaseCamera& Cam)
{
    chassis->draw(Cam);
    cannon->draw(Cam);
}
