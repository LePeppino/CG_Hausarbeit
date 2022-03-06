//
//  Tank.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Tank.h"


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
		chassisMov.translation(0, 25, 0);
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

void Tank::aim(const Vector& Target )
{   
    //Abfangen ob der übergeneme Vektor 0 0 0 ist, da es hier keine Richtung gibt in die das Rohr zeigen kann
    if (Target.X == 0 && Target.Y == 0 && Target.Z == 0) {
        return;
    }


    //aktzuelle Position der Kanone holen
    Matrix cannonPos = this->cannon->transform();

    //delta Werte zwischen Kanone und Target Berechnen
    float deltaX =  -Target.X + cannonPos.m03;
    float deltaZ =  -Target.Z + cannonPos.m23;

    //mit arctan den Winkel der Kanone berechnen
    angleCannon = atan2f(deltaZ, -deltaX);
    
}

void Tank::update(float dtime, Vector size, Terrain *terrain, Camera& cam)
{
    Matrix chassisMatrix, chassisMov , chassisRot, chassisPitch, chassisRoll, chassisUrsprung;
    Matrix cannonMatrix;
    //chassis
    chassisUrsprung = chassis->transform(); 
	float x = chassisUrsprung.m03;
	float y = chassisUrsprung.m23;

	
	Vector planePos = chassis->transform().translation();
	/*
	float xWert = planePos.X * 0.005 - 5.0;
	float yWert = planePos.Y * 0.005 - 5.0;
	float zWert = planePos.Z * 0.005 - 5.0;

	Vector boundingBoxMin = chassis->boundingBox().Min;
	float planeMin = boundingBoxMin.length();
	Vector boundingBoxMax = chassis->boundingBox().Max;
	float planeMax = boundingBoxMax.length();

	float planeModel;

	if (planeMin < planeMax)
		planeModel = planeMax;
	else
		planeModel = planeMin;
		*/
	float tryoutX = (((planePos.X / 24.6) + 24.6/2.0) / 0.03075); //  ((X Position / Breite des Terrains ) + ( Breite des Terrains / 2) / (Breite des Terains/Bildbreite)
	float tryoutZ = (((planePos.Z / 24.6)+ 24.6/2.0) / 0.03075);
	float heightForCords = terrain->heights[static_cast<int>(tryoutX)][static_cast<int>(tryoutZ)];
	try {

		// 7 weil Terrain höhe
		if (planePos.Y > heightForCords * 20.0) { // Höhe des Terrains 

			//https://www.youtube.com/watch?v=RpNPW89Y-3A

			chassisPitch.rotationZ(-1 * fb.X *dtime);

			chassisRoll.rotationX(-1 * fb.Y * dtime);

			chassisMov.translation(0.1, 0, 0);

			chassisMatrix = chassisUrsprung * chassisPitch * chassisRoll * chassisMov;
			this->chassis->transform(chassisMatrix);

			boundingBox = this->chassis->boundingBox();

			cannonMatrix = chassisUrsprung;
			this->cannon->transform(cannonMatrix);
			// Camera
			Matrix thirdPerson;
			Vector target = chassisMatrix.translation();
			thirdPerson.lookAt(target, chassisMatrix.up(), chassisMatrix.translation() - chassisMatrix.right() * 7 + chassisMatrix.up() * 2);
			cam.setViewMatrix(thirdPerson);
			cam.setPosition(target);

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
