#include "color.h"
#include <assert.h>

Color::Color() : R(0), G(0), B(0) {

}

Color::Color(float r, float g, float b) : R(r), G(g), B(b) {

}

Color Color::operator*(const Color &c) const {
    float nR;
    float nG;
    float nB;

    if (this->R * c.R > 255.0f) {
        nR = 255.0f;
    } else {
        nR = this->R * c.R;
    }
    if (this->G * c.G > 255.0f) {
        nG = 255.0f;
    } else {
        nG = this->G * c.G;
    }
    if (this->B * c.B > 255.0f) {
        nB = 255.0f;
    } else {
        nB = this->B * c.B;
    }

    return Color(nR, nG, nB);
}

Color Color::operator*(const float Factor) const {
    float nR;
    float nG;
    float nB;

    if (this->R * Factor > 255.0f) {
        nR = 255.0f;
    } else {
        nR = this->R * Factor;
    }
    if (this->G * Factor > 255.0f) {
        nG = 255.0f;
    } else {
        nG = this->G * Factor;
    }
    if (this->B * Factor > 255.0f) {
        nB = 255.0f;
    } else {
        nB = this->B * Factor;
    }

    return Color(nR, nG, nB);
}

Color Color::operator+(const Color &c) const {
    float nR;
    float nG;
    float nB;

    if (this->R + c.R > 255.0f) {
        nR = 255.0f;
    } else {
        nR = this->R + c.R;
    }
    if (this->G + c.G > 255.0f) {
        nG = 255.0f;
    } else {
        nG = this->G + c.G;
    }
    if (this->B + c.B > 255.0f) {
        nB = 255.0f;
    } else {
        nB = this->B + c.B;
    }

    return Color(nR, nG, nB);
}

Color &Color::operator+=(const Color &c) {
    if (this->R + c.R > 255.0f) {
        this->R = 255.0f;
    } else {
        this->R = this->R + c.R;
    }
    if (this->G + c.G > 255.0f) {
        this->G = 255.0f;
    } else {
        this->G = this->G + c.G;
    }
    if (this->B + c.B > 255.0f) {
        this->B = 255.0f;
    } else {
        this->B = this->B + c.B;
    }

    return *this;
}