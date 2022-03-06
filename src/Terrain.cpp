#include "Terrain.h"
#include "rgbimage.h"
#include "Terrainshader.h"
#include "LinePlaneModel.h"
#include "Model.h"

#define TERRAIN_DIM 24.6
#define TERRAIN_HEIGHT 20

Terrain::Terrain(const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixTex) 
	: Size(TERRAIN_DIM, TERRAIN_HEIGHT, TERRAIN_DIM)
{
    if(HeightMap && DetailMap1 && DetailMap2)
    {
        bool loaded = load( HeightMap, DetailMap1, DetailMap2, MixTex);
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

bool Terrain::loadNormalTextures(const char* NormalMap1, const char* NormalMap2) {
	if (!NormalTex[0].load(NormalMap1))
		return false;
	if (!NormalTex[1].load(NormalMap2))
		return false;
}

bool Terrain::load( const char* HeightMap, const char* DetailMap1, const char* DetailMap2, const char* MixTex)
{
    if( !HeightTex.load(HeightMap) )
        return false;
    if( !DetailTex[0].load(DetailMap1) )
        return false;
    if( !DetailTex[1].load(DetailMap2) )
        return false;
    if (!this->MixTex.load(MixTex))
        return false;


    //rgb Bild aus der Textur laden
    const RGBImage* tmp = HeightTex.getRGBImage();

    //Faktor berechnen um die reale Bildgröße in die gewünschte Terraingröße umrechnen zu können
	imageSize = tmp->width();

	//Skalierungsverhältnis zwischen Maße der Ebene und Pixelmaße des Bildes
    float widthScale = this->Size.X / imageSize;
    float heightScale = this->Size.Z / imageSize;

    //Vertices vorläufig erstellen
    Vector** vertices = new Vector * [imageSize + 2];
    for (int i = 0; i < imageSize + 2; ++i) {
        vertices[i] = new Vector[imageSize + 2];
    }
    for (int col = 0; col < imageSize; col++) {
        for (int row = 0; row < imageSize; row++) {
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
    
    for (int col = 0; col < imageSize; col++) {
        for (int row = 0; row < imageSize; row++) {
            
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
            this->VB.addTexcoord0(row / (float)imageSize - 1, col / (float)imageSize - 1);

			//Faktor beschreibt die Wiederholung der Gras-Textur auf der Ebene
            this->VB.addTexcoord1(row / ((float)imageSize - 1) * 500, col / ((float)imageSize - 1) * 500);

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
    for (int col = 0; col < imageSize - 1; col++) {
        for (int row = 0; row < imageSize - 1; row++) {
            int index = row + col * imageSize;
			//unteres linkes Dreieck
            this->IB.addIndex(index);
            this->IB.addIndex(index + 1);
            this->IB.addIndex(index + 1 + imageSize);
			//oberes rechtes Dreieck
            this->IB.addIndex(index + 1 + imageSize);
            this->IB.addIndex(index + imageSize);
            this->IB.addIndex(index);
        }
    }
    this->IB.end();

    return true;
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
	if (!Shader) {
		return;
	}
    
    Shader->mixTex(&MixTex);

	for (int i = 0; i < 2; i++) {
		Shader->detailTex(i, &DetailTex[i]);
		Shader->normalTex(i, &NormalTex[i]);
	}
        
    Shader->scaling(Size);
    
    // TODO: add additional parameters if needed..
}
