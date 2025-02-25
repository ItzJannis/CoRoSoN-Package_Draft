
// -------------------------------------------------------------------
/*********************************************************************
* 
* !    LICENSE
*         GNU GPL 3.0
*
*********************************************************************/
// -------------------------------------------------------------------
/*********************************************************************
* 
* *     CoRoSoN
* *       Co|mponents for 
* *       Ro|bocupJunior
* *       So|ccer
* *        N|ewcomers
*
* *    Project:     https://github.com/CoRoSoN-Kit/CoRoSoN-Kit
* *    Competition: https://www.robocup.org/
*
* *    Created by BohleBots:
* *       https://www.bohlebots.de
* *       info@bohlebots.de
* 
*********************************************************************/
// -------------------------------------------------------------------
// File:    CoRoSoNmPriv.Vector.cpp
// Purpose: Implementations for CoRoSoNmPriv.Vector.h
// -------------------------------------------------------------------
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include "CoRoSoN_Vector.h"
/*********************************************************************
* 
*  Implementations
*
*********************************************************************/
Vector::Vector(double a, double b) {
  ZEROMEM(this->mPriv);
  this->mPriv.x = a;
  this->mPriv.y = b;
  this->updatePhiRad(); 
}

double Vector::     getX() { return this->mPriv.x;      }
double Vector::     getY() { return this->mPriv.y;      }
double Vector:: getAngle() { return this->mPriv.angle;  }
double Vector::getRadius() { return this->mPriv.radius; }

Vector Vector::clone() { 
  return Vector(this->mPriv.x, this->mPriv.y); 
}
Vector Vector::clone(Vector v) {
  return Vector(v.mPriv.x, v.mPriv.y); 
}

void   Vector::operator +=(Vector v) { this->add(v);      }
void   Vector::operator -=(Vector v) { this->subtract(v); }
void   Vector::operator *=(double d) { this->multiply(d); }
Vector Vector::operator  +(Vector v) { return Vector::       add(*this, v); }
Vector Vector::operator  -(Vector v) { return Vector::  subtract(*this, v); }
Vector Vector::operator  *(double d) { return Vector::  multiply(*this, d); }
double Vector::operator  *(Vector v) { return Vector::dotProduct(*this, v); }

void Vector::add(Vector v) {
  this->mPriv.x += v.mPriv.x;
  this->mPriv.y += v.mPriv.y;
  this->updatePhiRad();
}

Vector Vector::add(Vector v1, Vector v2) { 
  return Vector(v1.mPriv.x + v2.mPriv.x, v1.mPriv.y + v2.mPriv.y);
}

void Vector::subtract(Vector v) {
  this->mPriv.x -= v.mPriv.x;
  this->mPriv.y -= v.mPriv.y;
  updatePhiRad();
}

Vector Vector::subtract(Vector v1, Vector v2) { 
  return Vector(v1.mPriv.x - v2.mPriv.x, v1.mPriv.y - v2.mPriv.y);
}

void Vector::multiply(double d) {
  this->mPriv.x *= d;
  this->mPriv.y *= d;
  this->updatePhiRad();
}

Vector Vector::multiply(Vector v,double d) { 
  return Vector(v.mPriv.x * d, v.mPriv.y * d);
}
double Vector::dotProduct(Vector v1, Vector v2) {
    return (v1.mPriv.x * v2.mPriv.x) + (v1.mPriv.y * v2.mPriv.y);
}

void Vector::scaleXY(double a, double b) {
  this->mPriv.x *= a;
  this->mPriv.y *= b;
  this->updatePhiRad();
}

Vector Vector::scaleXY(Vector v, double a, double b) {
  return Vector(v.mPriv.x * a, v.mPriv.y * b);
}  

void Vector::rotate(double angle_rad) {
  this->mPriv.angle += angle_rad;
  this->updateXY();
  this->updatePhiRad();
}

Vector Vector::rotate(Vector v, double angle_rad) {
  Vector _v = Vector::clone(v);
  _v.mPriv.angle += angle_rad;
  _v.updateXY();
  _v.updatePhiRad();
  return _v;
}

double Vector::angleOffsetTo(Vector v) {
  return acos( Vector::dotProduct(*this, v) / (this->mPriv.radius * v.mPriv.radius) );
}

double Vector::angleOffsetBetween(Vector v1, Vector v2) {
  return acos( Vector::dotProduct(v1, v2) / (v1.mPriv.radius * v2.mPriv.radius) );
}
bool Vector::isBetween(Vector v1, Vector v2) {
  int angleThisV1 = (int)(this->angleOffsetTo(v1) * 1000);
  int angleThisV2 = (int)(this->angleOffsetTo(v2) * 1000);
  int angleV1V2   = (int)(Vector::angleOffsetBetween(v1, v2) * 1000);
  return ((angleThisV1 + angleThisV2) == angleV1V2);
}

bool Vector::vectorIsInSector(Vector v, Vector a, Vector b) {
  int av = (int)(Vector::angleOffsetBetween(a, v) * 1000); 
  int vb = (int)(Vector::angleOffsetBetween(v, b) * 1000); 
  int ab = (int)(Vector::angleOffsetBetween(a, b) * 1000);
  return ((av + vb) == ab);
}

void Vector::updateXY() {
  this->mPriv.x = this->mPriv.radius * cos(this->mPriv.angle);
  this->mPriv.y = this->mPriv.radius * sin(this->mPriv.angle);
}

void Vector::updatePhiRad() {
  this->mPriv.angle  = atan2(this->mPriv.y, this->mPriv.x);
  this->mPriv.radius = sqrt((this->mPriv.x * this->mPriv.x) + (this->mPriv.y * this->mPriv.y));
}