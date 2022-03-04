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
		chassisMov.translation(0, 10, 0);
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
    Matrix chassisMatrix, chassisMov , chassisRot, chassisUrsprung;
    Matrix cannonMatrix;
	float i = terrain->getHeight((int)chassisUrsprung.m03, (int)chassisUrsprung.m23);
    //chassis
    chassisUrsprung = chassis->transform(); 
	if (chassisUrsprung.m13 * 7 > terrain->getHeight((int) chassisUrsprung.m03 / 10 , (int) chassisUrsprung.m23 /10 )) {
		
		chassisRot.rotationY(fb.Y * dtime);
		//chassisRot.rotationZ(fb.X *dtime);
		chassisMov.translation(0.05, fb.X*dtime * 1, 0);

		chassisMatrix = chassisUrsprung * chassisMov * chassisRot;
		this->chassis->transform(chassisMatrix);
	}
    


    cannonMatrix = chassisUrsprung;
    this->cannon->transform(cannonMatrix);
	Matrix thirdPerson;
	Vector target = chassisMatrix.translation();
	thirdPerson.lookAt(target, chassisMatrix.up(), chassisMatrix.translation() - chassisMatrix.right() * 7 + chassisMatrix.up());
	cam.setViewMatrix(thirdPerson);
}

void Tank::draw(const BaseCamera& Cam)
{
    chassis->draw(Cam);
    cannon->draw(Cam);
}
