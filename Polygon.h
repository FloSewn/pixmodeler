#pragma once

#include "Shape.h"
#include "Vec2.h"

/***********************************************************
* This class defines a polygonal element 
* A polygon actually offers the same properties as a 
* general shape but is defined for consistency.
***********************************************************/
class Polygon : public Shape
{
public:
  Polygon(ModelSpace& space, int index, bool extr) 
  : Shape(space, index, extr) {}
};