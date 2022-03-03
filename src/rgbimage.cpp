#include "rgbimage.h"
#include "color.h"
#include "assert.h"

RGBImage::RGBImage(unsigned int Width, unsigned int Height) :m_Width(Width), m_Height(Height)
{
	m_Image = new Color[Width * Height];
}

RGBImage::~RGBImage()
{
	delete m_Image;
}

void RGBImage::setPixelColor(unsigned int x, unsigned int y, const Color& c)
{
	if (x >= m_Width|| y>=m_Height)
		return;

		m_Image[(y * m_Width) + x] = c;

}

const Color& RGBImage::getPixelColor(unsigned int x, unsigned int y) const
{
	if (x >= m_Width || y >= m_Height)
		return m_Image[0];

		return m_Image[y * m_Width + x];
}

unsigned int RGBImage::width() const
{
	return m_Width;
}
unsigned int RGBImage::height() const
{
	return m_Height;
}

unsigned char RGBImage::convertColorChannel(float v)
{
	char erg = NULL;
	if (v > 0.0f && v < 1.0f) {
		erg = v * 255;
	}
	else if (v <= 0.0f) {
		erg = 0;
	}
	else if (v >= 1.0f) {
		erg = 255;
	}
	return erg;
}

 RGBImage& RGBImage::SobelFilter(RGBImage& dst, const RGBImage& src, float factor) {
	assert(dst.m_Width == src.m_Width && dst.m_Height == src.m_Width);

	float  m[3][3] = { {1.0f, 0.0f, -1.0f},
					 {2.0f, 0.0f, -2.0f},
					 {1.0f, 0.0f, -1.0f} };

	for (int x = 0; x < src.m_Width; x++)
	{
		for (int y = 0; y < src.m_Height; y++) {

			Color tmpU;
			Color tmpV;
			for (int j = 0; j <= 2; j++) {
				for (int i = 0; i <= 2; i++) {
					tmpU += src.getPixelColor(x - i - 1, y - j - 1) * m[j][i];
					tmpV += src.getPixelColor(x - i - 1, y - j - 1) * m[i][j];
				}
			}
			Color tmp;
			tmp.R = sqrt(pow(tmpU.R, 2) + pow(tmpV.R, 2));
			tmp.G = sqrt(pow(tmpU.G, 2) + pow(tmpV.G, 2));
			tmp.B = sqrt(pow(tmpU.B, 2) + pow(tmpV.B, 2));

			dst.setPixelColor(x, y, tmp * factor);

		}

	}
	return dst;
}

bool RGBImage::saveToDisk(const char* Filename)
{

	FILE* file;
	errno_t err = fopen_s(&file, Filename, "wb");
	if (file == NULL) {
		return false;
	}

	int imgSize = 3 * m_Height * m_Width;
	int fileSize = 54 + imgSize;

	unsigned char bmpheader[54];
	//BMPFILEHEADER//
	//Magic
	bmpheader[0] = 'B';
	bmpheader[1] = 'M';
	//bfSize 4Bytes: gesamten file size, imagsize = 3* hohe *breite // filesize = 54 + imagsize
	bmpheader[2] = (unsigned char)(fileSize);
	bmpheader[3] = (unsigned char)(fileSize >> 8);
	bmpheader[4] = (unsigned char)(fileSize >> 16);
	bmpheader[5] = (unsigned char)(fileSize >> 24);
	//reserved 4Bytes
	bmpheader[6] = 0;
	bmpheader[7] = 0;
	bmpheader[8] = 0;
	bmpheader[9] = 0;
	//bfOffBits 4Bytes: kann auch NICHT 54 sein (40+14 / info+file headers)
	bmpheader[10] = 54;
	bmpheader[11] = 0;
	bmpheader[12] = 0;
	bmpheader[13] = 0;
	//BMPINFOHEADER//
	//biSize: info header size
	bmpheader[14] = 40;
	bmpheader[15] = 0;
	bmpheader[16] = 0;
	bmpheader[17] = 0;
	//biWidt: Breite der BMP
	bmpheader[18] = (unsigned char)(this->m_Width);
	bmpheader[19] = (unsigned char)(this->m_Width >> 8);
	bmpheader[20] = (unsigned char)(this->m_Width >> 16);
	bmpheader[21] = (unsigned char)(this->m_Width >> 24);
	//biHeight: Höhe der BMP
	//positiv - bottom up, negativ - top down.
	bmpheader[22] = (unsigned char)(this->m_Height*-1);
	bmpheader[23] = (unsigned char)(this->m_Height*-1 >> 8);
	bmpheader[24] = (unsigned char)(this->m_Height*-1 >> 16);
	bmpheader[25] = (unsigned char)(this->m_Height*-1 >> 24);
	//biPlanes: anzahl an farbe "planes", standard 1, (wird nicht bei BMP verwendet ?)
	bmpheader[26] = 1;
	bmpheader[27] = 0;
	//biBitCount: Color bit depth - 1,4,8,16,24,32
	bmpheader[28] = 24;
	bmpheader[29] = 0;
	//biCompression: 0-3 ( 0 (BI_RGB) kein komprimierung, 1 (BI_RLE8), 2 (BI_RLE4), 3 (BI_BITFIELDS))
	bmpheader[30] = 0;
	bmpheader[31] = 0;
	bmpheader[32] = 0;
	bmpheader[33] = 0;
	//biSizeImage: bei BI_RGB compression entweder 0 oder Größe der Bilddaten in Byte. 
	bmpheader[34] = (unsigned char)(imgSize);
	bmpheader[35] = (unsigned char)(imgSize >> 8);
	bmpheader[36] = (unsigned char)(imgSize >> 16);
	bmpheader[37] = (unsigned char)(imgSize >> 24);
	//biXPelsPerMeter: horizontal auflösung
	bmpheader[38] = 0;
	bmpheader[39] = 0;
	bmpheader[40] = 0;
	bmpheader[41] = 0;
	//biYPelsPerMeter: Vertikal auflösung
	bmpheader[42] = 0;
	bmpheader[43] = 0;
	bmpheader[44] = 0;
	bmpheader[45] = 0;
	//biClrUsed: colors in pallete
	bmpheader[46] = 0;
	bmpheader[47] = 0;
	bmpheader[48] = 0;
	bmpheader[49] = 0;
	//biClrimportant: important colors
	bmpheader[50] = 0;
	bmpheader[51] = 0;
	bmpheader[52] = 0;
	bmpheader[53] = 0;

	fwrite(bmpheader, 1, 54, file);

	for (int y = 0; y < m_Height; y++) {
		for (int x = 0; x < m_Width; x++) {
			Color c = m_Image[(y * m_Width) + x];
			unsigned char R = convertColorChannel(c.R);
			unsigned char G = convertColorChannel(c.G);
			unsigned char B = convertColorChannel(c.B);
			fwrite(&B, 1, 1, file);
			fwrite(&G, 1, 1, file);
			fwrite(&R, 1, 1, file);
		}
	}

	fclose(file);

	return true;
}
