//
//  Tank.hpp
//  CGXcode
//
//  Created by Philipp Lensing on 16.11.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Tank_hpp
#define Tank_hpp

#include <stdio.h>
#include "model.h"
#include "PhongShader.h"
#include "Terrain.h"

class Tank : public BaseModel
{
public:
    Tank();
    virtual ~Tank();
    bool loadModels(const char* ChassisFile, const char* CannonFile);
    void steer( float ForwardBackward, float LeftRight);
    void update(float dtime, Vector size, Terrain *terrain, Camera& cam);
    virtual void draw(const BaseCamera& Cam);
protected:
    Model *chassis, *cannon;
    float angleCannon;
    float angleCorrection;
    
    Vector fb;

};

#endif /* Tank_hpp */
