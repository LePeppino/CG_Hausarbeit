#include "vector.h"
#include <assert.h>
#include <math.h>

#define EPSILON 1e-6

Vector::Vector(float x, float y, float z) : X(x), Y(y), Z(z) {

}

Vector::Vector():X(0),Y(0),Z(0) {
}

//Skalarprodukt ( gibt winkel )
float Vector::dot(const Vector &v) const {
    return ((this->X * v.X) + (this->Y * v.Y) + (this->Z * v.Z));
}
//Kreuzprodukt ( gibt ein vektor der rechtswinkig zu beide vektoren steht )
Vector Vector::cross(const Vector &v) const {
    return Vector(((this->Y * v.Z) - (v.Y * this->Z)), ((v.X * this->Z) - (this->X * v.Z)),
                  ((this->X * v.Y) - (v.X * this->Y)));;
}


Vector Vector::operator+(const Vector &v) const {
    return Vector(this->X + v.X, this->Y + v.Y, this->Z + v.Z);
}

Vector Vector::operator-(const Vector &v) const {
    return Vector(this->X - v.X, this->Y - v.Y, this->Z - v.Z);
}

Vector Vector::operator*(float c) const {
    return Vector(this->X * c, this->Y * c, this->Z * c);
}

Vector Vector::operator-() const {
    return Vector(this->X * (-1), this->Y * (-1), this->Z * (-1));
}

Vector &Vector::operator+=(const Vector &v) {
    this->X += v.X;
    this->Y += v.Y;
    this->Z += v.Z;
    return *this; // dummy (remove)
}


Vector &Vector::normalize() {
    float length = this->length();
    this->X /= length;
    this->Y /= length;
    this->Z /= length;
    return *this;
}

//lenght greift auf squared length zu um erstens code wiederholungen zu vermeiden.
// und zweitens komplexere funktionen die nicht ein genaues ergebnis liefern aufgrund abrundungen zu vermeiden.

float Vector::length() const {
    return sqrt(lengthSquared());
}

float Vector::lengthSquared() const {
    return ((this->X * this->X) + (this->Y * this->Y) + (this->Z * this->Z));
}

Vector Vector::reflection(const Vector &normal) const {
    float dn = 2 * this->dot(normal);
    return *this - (normal * dn);
}

bool Vector::triangleIntersection(const Vector &d, const Vector &a, const Vector &b, const Vector &c, float &s) const {

    //vektor n
    Vector n = ((b - a).cross(c - a)).normalize();

    //->d parallel zu ebne ?
    if (fabs(d.dot(n)) < EPSILON) {
        return false;
    }

    //distantz von ursprung bis ebene
    float dist = n.dot(a);

    //Skalierung
    s = (dist - (n.dot(*this))) / n.dot(d);

    //->d zeigt weg von ebne ?
    if (s < 0) {
        return false;
    }

    //Vektor p
    Vector p = (*this) + (d * s);


    float abc = (((b - a).cross(c - a)).length()) / 2;
    float abp = (((b - a).cross(p - a)).length()) / 2;
    float acp = (((c - a).cross(p - a).length()) / 2);
    float bcp = (((c - b).cross(p - b).length()) / 2);

    if (abc + EPSILON >= abp + acp + bcp) {
        return true;
    }

    return false;
}
