/////////////////////////////////////////////////////////
//                                                     //
//    Cubpack++                                        //
//                                                     //
//        A Package For Automatic Cubature             //
//                                                     //
//        Authors : Ronald Cools                       //
//                  Dirk Laurie                        //
//                  Luc Pluym                          //
//                                                     //
/////////////////////////////////////////////////////////
//////////////////////////////////////////////////
//File geometry.c
// History:
//   (date)          (version)
//   19 Aug 1994     V0.1 (first limited distribution)
//    1 Aug 1996     V1.0 (paper accepted by ACM TOMS)
///////////////////////////////////////////////////
#include "geometry.h"
///////////////////////////////////////////////////
Geometry::Geometry(unsigned int Dim)
  :ReferenceCounting(),TheDimension(Dim)
  {
  }
///////////////////////////////////////////////////
unsigned int
Geometry::Dimension()
const
  {
  return TheDimension;
  }
//////////////////////////////////////////////////
