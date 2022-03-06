
#ifndef Airplane_hpp
#define Airplane_hpp

#include <stdio.h>
#include "model.h"
#include "PhongShader.h"
#include "Terrain.h"

class Airplane : public BaseModel
{
public:
	Airplane();
    virtual ~Airplane();
	bool loadModels(const char* ChassisFile);
    void steer( float ForwardBackward, float LeftRight);
    void update(float dtime, Vector size, Terrain *terrain, Camera& cam);
    virtual void draw(const BaseCamera& Cam);

	Model *chassis;
protected:
    
    
    Vector fb;

};

#endif /* Airplane_hpp */
