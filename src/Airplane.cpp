
#include "Airplane.h"

#define TERRAIN_DIM 24.6
#define TERRAIN_HEIGHT 20

Airplane::Airplane():fb(0,0,0)
{
	
}

Airplane::~Airplane()
{
    delete chassis;
}

bool Airplane::loadModels(const char* ChassisFile)
{

    if (ChassisFile != nullptr) {
		Matrix chassisMatrix, chassisMov, chassisUrsprung;

		chassis = new Model(ChassisFile);
        chassis->shader(pShader);
		
		chassisUrsprung = chassis->transform();
		
		chassisMov.translation(0, 35, 0);
		chassisMatrix = chassisUrsprung * chassisMov;
		chassis->transform(chassisMatrix);
        return true; 
    }

    return false;
    
}

void Airplane::steer( float ForwardBackward, float LeftRight)
{
    fb.X = ForwardBackward;
    fb.Y = LeftRight;
}

int Airplane::update(float dtime, Vector size, Terrain *terrain, Camera& cam)
{
	Matrix chassisMatrix, chassisMov, chassisTurn, chassisPitch, chassisRoll, chassisUrsprung;

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
		if (planePos.Y > heightForCords * TERRAIN_HEIGHT && tryoutX < 750.0 && tryoutZ < 750.0)
		{
			//https://www.youtube.com/watch?v=RpNPW89Y-3A

			chassisPitch.rotationZ(-1 * fb.X * dtime);

			chassisRoll.rotationX(-1 * fb.Y * dtime);
			
			chassisTurn.rotationY(0.4 * fb.Y * dtime);

			chassisMov.translation(0.15, 0, 0);

			chassisMatrix = chassisUrsprung * chassisPitch * chassisRoll * chassisTurn * chassisMov;
			this->chassis->transform(chassisMatrix);

			// Third Person Camera
			Matrix thirdPerson;
			Vector target = chassisMatrix.translation();
			thirdPerson.lookAt(target, chassisMatrix.up(), chassisMatrix.translation() - chassisMatrix.right() * 3 + chassisMatrix.up() * 0.5);
			cam.setViewMatrix(thirdPerson);
			cam.setPosition(target); //für fog shader
		}
		else 
		{
			//Kollision --> TODO: Feedback & neu starten
			std::cout << "Kollision mit Terrain!" << std::endl;
			Matrix chassisReset;
			chassisReset.translation(0, 35, 0);
			this->chassis->transform(chassisReset);
			return 1;
		}
	}
	catch (...) {

	}
	return 0;
}

void Airplane::draw(const BaseCamera& Cam)
{
    chassis->draw(Cam);
}
