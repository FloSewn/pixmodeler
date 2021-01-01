#pragma once

#include <vector>

#include "Vec2.h"
#include "olc_pixel_game_engine.h"

class Shape;
class ModelSpace;

/***********************************************************
* This class defines a node from which shapes are made of 
***********************************************************/
class Node
{

public:
  explicit Node(Shape& s) : parent_{s} {}
  Node(Shape& s, const Vec2f& v) : parent_{s}, coords_{v} {}

  Shape& parent() { return parent_;}

  Vec2f coords() const { return coords_;}
  void coords(const Vec2f& v) { coords_ = v; }

private:
  Vec2f     coords_;
  Shape&    parent_;

};

/***********************************************************
* This class defines a basic shape to draw on the screen
***********************************************************/
class Shape
{
public:

  /*********************************************************
  * Constructor
  *********************************************************/
  Shape(ModelSpace& space) : space_{space} {}
  virtual ~Shape() {}

  /*********************************************************
  * Drawing  
  *********************************************************/
  virtual void draw() = 0;  
  virtual void draw_nodes();

  /*********************************************************
  * Update
  *********************************************************/
  virtual void update() {};

  /*********************************************************
  * Node handling
  *********************************************************/
  virtual Node* add_node(const Vec2f& n);
  virtual Node* get_node(const Vec2f& p);

  /*********************************************************
  * Setters
  *********************************************************/
  void color(olc::Pixel col) { color_ = col; }

  void complete(bool c) { complete_ = c; }
  bool complete() const { return complete_; }

protected:
  ModelSpace&       space_;
  unsigned int      max_nodes_  = 0;
  std::vector<Node> nodes_;
  olc::Pixel        color_      = olc::GREEN; 
  bool              complete_   = false;

};
