#pragma once

#include "Vec2.h"

class ModelSpace;

/***********************************************************
* This class defines the visible cursor for the user
***********************************************************/
class Cursor
{
public:
  Cursor(ModelSpace& space) : space_{space} {}
  ~Cursor() {}

  void update();
  void draw();

  Vec2f& coords() { return coords_; }

private:

  ModelSpace&   space_;
  Vec2f         coords_     = {0.0f, 0.0f}; 
  int           size_       = 3;

};