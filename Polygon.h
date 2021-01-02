#pragma once

#include "Shape.h"
#include "Vec2.h"

/***********************************************************
* This class defines a polygonal element 
***********************************************************/
class Polygon : public Shape
{
public:
  Polygon(ModelSpace& space, int index, bool extr) 
  : Shape(space, index, extr) {}

  void draw() override;

  bool valid() override;
  
  Node* add_node(const Vec2f& n) override;

};