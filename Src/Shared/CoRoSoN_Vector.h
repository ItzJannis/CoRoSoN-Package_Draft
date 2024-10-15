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
* *    Project:     https://github.com/CoRoSoN-Kit/
* *    Competition: https://www.robocup.org/
*
* *    Created by BohleBots:
* *       https://www.bohlebots.de
* *       info@bohlebots.de
* 
*********************************************************************/
// -------------------------------------------------------------------
// File:    CoRoSoN_Vector.h
// Purpose: Providing a vector class for easy use
// -------------------------------------------------------------------
#ifndef COROSON_VECTOR_H
#define COROSON_VECTOR_H
/*********************************************************************
* 
*  Dependencies
*
*********************************************************************/
#include "CoRoSoN_Util.h"
#include <Math.h>
/*********************************************************************
* 
*  Types
*
*********************************************************************/
//
// Vector class
//
class Vector {
  public:
    /************************************************************
    *
    * ? Vector()
    *   
    * * Description:
    *     Constructor
    *
    ************************************************************/
    Vector(double x, double y);
    //
    // Getter
    //
    /************************************************************
    *
    * ? getX()
    *   
    * * Description:
    *     Returns the X value
    *
    ************************************************************/
    double getX();
    /************************************************************
    *
    * ? getY()
    *   
    * * Description:
    *     Returns the Y value
    *
    ************************************************************/
    double getY();
    /************************************************************
    *
    * ? getAngle()
    *   
    * * Description:
    *     Returns the angle in radians
    *
    ************************************************************/
    double getAngle();
    /************************************************************
    *
    * ? getRadius()
    *   
    * * Description:
    *     Returns the radius
    *
    ************************************************************/
    double getRadius();
    //
    // Cloning
    //
    /************************************************************
    *
    * ? clone()
    *   
    * * Description:
    *     Returns a deep copy
    *
    ************************************************************/
    Vector clone();
    /************************************************************
    *
    * ? clone()
    *   
    * * Description:
    *     Returns a deep copy of the passed vector
    *
    ************************************************************/
    static Vector clone(Vector v);
    //
    // Calculations
    //
    /************************************************************
    *
    * ? add()
    *   
    * * Description:
    *     Adds the passed vector
    *
    ************************************************************/
    void add(Vector v);
    void operator +=(Vector v);
    static Vector add(Vector v1, Vector v2);
    Vector operator +(Vector v);
    /************************************************************
    *
    * ? subtract()
    *   
    * * Description:
    *     Subtracts the passed vector
    *
    ************************************************************/
    void subtract(Vector v);
    void operator -=(Vector v);
    static Vector subtract(Vector v1, Vector v2);
    Vector operator -(Vector v);
    /************************************************************
    *
    * ? multiply()
    *   
    * * Description:
    *     Multiplies with the passed scalar
    *
    ************************************************************/
    void multiply(double d);
    void operator *=(double d);
    static Vector multiply(Vector v,double d);
    Vector operator *(double d);
    /************************************************************
    *
    * ? dotProduct()
    *   
    * * Description:
    *     Return the dotProduct of the passed vectors
    *
    ************************************************************/
    static double dotProduct(Vector v1, Vector v2);
    double operator *(Vector v);
    /************************************************************
    *
    * ? scale_XY()
    *   
    * * Description:
    *     Scales X and Y value by the passed scalars
    *
    ************************************************************/
    void scaleXY(double a, double b);
    static Vector scaleXY(Vector v, double a, double b);
    /************************************************************
    *
    * ? rotate()
    *   
    * * Description:
    *     Rotates the vector by passed value
    *
    ************************************************************/
    void rotate(double angle_rad);
    static Vector rotate(Vector z, double angle_rad);
    /************************************************************
    *
    * ? angleOffsetTo()
    *   
    * * Description:
    *     Returns the angle to the passed vector
    *     (smaller of the two, in radians)
    *
    ************************************************************/
    double angleOffsetTo(Vector v);
    /************************************************************
    *
    * ? angleOffsetBetween()
    *   
    * * Description:
    *     Returns the angle between the passed vectors
    *     (smaller of the two, in radians)
    *
    ************************************************************/
    static double angleOffsetBetween(Vector v1, Vector v2);
    /************************************************************
    *
    * ? isBetween()
    *   
    * * Description:
    *     Returns if the vector is in between the passed two
    *
    ************************************************************/
    bool isBetween(Vector v1, Vector v2);
    /************************************************************
    *
    * ? vectorIsInSector()
    *   
    * * Description:
    *     Returns if the 1st vector is between the 2nd and 3rd
    *
    ************************************************************/
    static bool vectorIsInSector(Vector v, Vector a, Vector b);
    
  private: 
    struct {
      double x;
      double y;
      double angle;
      double radius;
    } mPriv;
    void updateXY();
    void updatePhiRad();
};
#endif // COROSON_VECTOR_H
