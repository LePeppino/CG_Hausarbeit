#include "Terrain.h"
#include "rgbimage.h"
#include "Terrainshader.h"
#include "LinePlaneModel.h"
#include "Model.h"

Terrain::Terrain(const char* HeightMap, const char* DetailMap1, const char* DetailMap2,  const char* DetailMap3,  const char* DetailMap4, const char* MixTex) 
	: Size(24.6, 20, 24.6) //Size(14.3,6,14.3)
{
    if(HeightMap && DetailMap1 && DetailMap2)
    {
        bool loaded = load( HeightMap, DetailMap1, DetailMap2, DetailMap3, DetailMap4, MixTex);
        if(!loaded)
            throw std::exception();
    }
}

Terrain::~Terrain()
{
    
}

Vector Terrain::normal(const Vector& a , const Vector& b , const Vector& c ) {
    Vector triangleNormal = (b - a).cross(c - a);
    return triangleNormal.normalize();
}

bool Terrain::load( const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* DetailMap3, const char* DetailMap4, const char* MixTex)
{
    if( !HeightTex.load(HeightMap) )
        return false;
    if( !DetailTex[0].load(DetailMap1) )
        return false;
    if( !DetailTex[1].load(DetailMap2) )
        return false;
    if (!DetailTex[2].load(DetailMap3))
        return false;
    if (!DetailTex[3].load(DetailMap4))
        return false;
    if (!this->MixTex.load(MixTex))
        return false;


    //rgb Bild aus der Textur laden
    const RGBImage* tmp = HeightTex.getRGBImage();

    //Faktor berechnen um die reale Bildgröße in die gewünschte Terraingröße umrechnen zu können
    int imgWidth = tmp->width();
    int imgHeight = tmp->height();

	//Skalierungsverhältnis zwischen Maße der Ebene und Pixelmaße des Bildes
    float widthScale = this->Size.X / imgWidth;
    float heightScale = this->Size.Z / imgHeight;

    //Vertices vorläufig erstellen
    Vector** vertices = new Vector * [imgWidth + 2];
    for (int i = 0; i < imgWidth + 2; ++i) {
        vertices[i] = new Vector[imgHeight + 2];
    }

    for (int col = 0; col < imgHeight; col++) {
        for (int row = 0; row < imgWidth; row++) {
            //Grauwert für HeightMap bestimmen
            Color pixelColor = tmp->getPixelColor(row, col);
            float pixelY = (pixelColor.R + pixelColor.G + pixelColor.B) / 3;

            //x und z Wert berechnen
            float pixelX = (col * widthScale - (this->Size.X / 2));
            float pixelZ = (row * heightScale - (this->Size.Z / 2));

            //Vertex zum Buffer hinzufügen
            vertices[col + 1][row + 1] = Vector(pixelX, pixelY, pixelZ);
			
			//Höhenwerte für Kollisionen hinzufügen
			heights[col][row] = pixelY;

        }
    }

    //Vertexbuffer befüllen
    this->VB.begin();
    
    for (int col = 0; col < imgHeight; col++) {
        for (int row = 0; row < imgWidth; row++) {
            
            int x = col + 1;
            int y = row + 1;

            //Normalen der Vertizes berechnen, zusammen addieren und teilen
			// --> Normale der Dreiecke
            Vector normal;

            normal += this->normal(vertices[x][y], vertices[x - 1][y], vertices[x - 1][y - 1]);
			normal += this->normal(vertices[x][y], vertices[x - 1][y - 1], vertices[x][y - 1]);
            normal += this->normal(vertices[x][y], vertices[x][y - 1], vertices[x + 1][y]);
            normal += this->normal(vertices[x][y], vertices[x + 1][y], vertices[x + 1][y + 1]);
            normal += this->normal(vertices[x][y], vertices[x + 1][y + 1], vertices[x][y + 1]);
            normal += this->normal(vertices[x][y], vertices[x][y + 1], vertices[x - 1][y]);
            
            normal.normalize();

			//Normale invertieren, um die Belichtung der Vertizes zu korrigieren
            this->VB.addNormal(normal*-1);

			//Texturkoordinaten für Heightmap hinzufügen
            this->VB.addTexcoord0(row / (float)imgWidth - 1, col / (float)imgHeight - 1);

			//Faktor beschreibt die Wiederholung der Gras-Textur auf der Ebene
            this->VB.addTexcoord1(row / ((float)imgWidth - 1) * 1000, col / ((float)imgHeight - 1) * 1000);

            this->VB.addVertex(vertices[x][y]);
            
        }
    }
    this->VB.end();

	//Zum Speicher freigeben, aber hier nicht nötig
    /*for (int i = 0; i < imgWidth + 2; i++) {
        delete[](vertices[i]);
    }
    delete[](vertices);*/

	/*
	* Vierecke aus jeweils zwei Dreiecken beschreiben
	* Nicht die Quelle, aber eine gute Veranschaulichung: 
	* https://stackoverflow.com/questions/6656358/calculating-normals-in-a-triangle-mesh/6661242#6661242
	*/
    this->IB.begin();
    for (int col = 0; col < imgHeight - 1; col++) {
        for (int row = 0; row < imgWidth - 1; row++) {
            int index = row + col * imgWidth;
			//unteres linkes Dreieck
            this->IB.addIndex(index);
            this->IB.addIndex(index + 1);
            this->IB.addIndex(index + 1 + imgWidth);
			//oberes rechtes Dreieck
            this->IB.addIndex(index + 1 + imgWidth);
            this->IB.addIndex(index + imgWidth);
            this->IB.addIndex(index);
        }
    }
    this->IB.end();

    return true;
}

float Terrain::getHeightAtPoint(int x, int z) {
	if (heights != NULL) {
		return heights[x][z];
	}
	return 0;
}

void Terrain::shader( BaseShader* shader, bool deleteOnDestruction )
{
    BaseModel::shader(shader, deleteOnDestruction);
}

void Terrain::draw(const BaseCamera& Cam)
{
    applyShaderParameter();
    BaseModel::draw(Cam);
    VB.activate();
    IB.activate();

    glDrawElements(GL_TRIANGLES, IB.indexCount(), IB.indexFormat(), 0);

    IB.deactivate();
    VB.deactivate();
   
}

//Shaderwerte übergeben, aktualisieren und pro Frame neu anwenden
void Terrain::applyShaderParameter()
{
    TerrainShader* Shader = dynamic_cast<TerrainShader*>(BaseModel::shader());
    if(!Shader)
        return;
    
    Shader->mixTex(&MixTex);
    for(int i=0; i<4; i++)
        Shader->detailTex(i,&DetailTex[i]);
    Shader->scaling(Size);
    
    
    // TODO: add additional parameters if needed..
}
