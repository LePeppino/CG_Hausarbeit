//
//  LineBoxModel.cpp
//  CGXcode
//
//  Created by Philipp Lensing on 10.10.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "LineBoxModel.h"

LineBoxModel::LineBoxModel( float Width, float Height, float Depth )
{

    float x = Width / 2.0f;
    float y = Height / 2.0f;
    float z = Depth / 2.0f;
    
    VB.begin();

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {

            // In X Richtung
            VB.addVertex(x, y - j * Height, z - i * Depth);
            VB.addVertex(-x, y - j * Height, z - i * Depth);

            // In Y Richtung
            VB.addVertex(x - j * Width, y, z - i * Depth);
            VB.addVertex(x - j * Width, -y, z - i * Depth);

            // In Z Richtung
            VB.addVertex(x - i * Width, y - j * Height, z);
            VB.addVertex(x - i * Width, y - j * Height, -z);
        }

    }

    VB.end();

}

void LineBoxModel::draw(const BaseCamera& Cam)
{
    BaseModel::draw(Cam);
  
    VB.activate();

    glDrawArrays(GL_LINES, 0, VB.vertexCount());

    VB.deactivate();
}
