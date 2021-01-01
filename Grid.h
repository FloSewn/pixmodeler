#pragma once

#include "Vec2.h"

class ModelSpace;

/***********************************************************
* This class defines the visible grid for the user
***********************************************************/
class Grid
{
public:
  Grid(ModelSpace& space); 
  ~Grid() {}

  float spacing() const { return spacing_; }
  void update();
  void draw();

private:

  ModelSpace& space_;
  float  spacing_      = 0.5f;
  float  scale_thresh_ = 50.0f;
  
};