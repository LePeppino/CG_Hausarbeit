#include "Terrain.h"
#include "rgbimage.h"
#include "Terrainshader.h"
#include "LinePlaneModel.h"
#include "Model.h"

static int imageSize = NULL;

Terrain::Terrain(const char* HeightMap, const char* DetailMap1, const char* DetailMap2,  const char* DetailMap3,  const char* DetailMap4, const char* MixTex) : Size(10,7,10)
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

    //Faktor berechnen um die reale Bildgr��e in die gew�nschte Terraingr��e umrechnen zu k�nnen
	imageSize = tmp->width();

	//Skalierungsverh�ltnis zwischen Ma�e der Ebene und Pixelma�e des Bildes
    float widthScale = this->Size.X / imageSize;
    float heightScale = this->Size.Z / imageSize;

    //Vertices vorl�ufig erstellen
    Vector** vertices = new Vector * [imageSize + 2];
    for (int i = 0; i < imageSize + 2; ++i) {
        vertices[i] = new Vector[imageSize + 2];
    }
    for (int col = 0; col < imageSize; col++) {
        for (int row = 0; row < imageSize; row++) {
            //Grauwert f�r HeightMap bestimmen
            Color pixelColor = tmp->getPixelColor(row, col);
            float pixelY = (pixelColor.R + pixelColor.G + pixelColor.B) / 3;

            //x und z Wert berechnen
            float pixelX = (col * widthScale - (this->Size.X / 2));
            float pixelZ = (row * heightScale - (this->Size.Z / 2));

            //Vertex zum Buffer hinzuf�gen
            vertices[col + 1][row + 1] = Vector(pixelX, pixelY, pixelZ);
			heights[col][row] = pixelY;

        }
    }

    //Vertexbuffer bef�llen
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

			//Texturkoordinaten f�r Heightmap hinzuf�gen
            this->VB.addTexcoord0(row / (float)imageSize - 1, col / (float)imageSize - 1);

			//Faktor 150 beschreibt die Wiederholung der Gras-Textur auf der Ebene
            this->VB.addTexcoord1(row / ((float)imageSize - 1) * 150, col / ((float)imageSize - 1) * 150);

            this->VB.addVertex(vertices[x][y]);
            
        }
    }
    this->VB.end();

	//Zum Speicher freigeben, aber hier nicht n�tig
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

//Shaderwerte �bergeben, aktualisieren und pro Frame neu anwenden
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
