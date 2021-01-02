#pragma once

#include <vector>
#include <list>

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
  Node(Shape& s, int index) 
  : parent_{s}, index_{index} {}
  Node(Shape& s, int index, const Vec2f& v) 
  : parent_{s}, index_{index}, coords_{v} {}
  ~Node() {}

  Shape& parent() { return parent_;}

  Vec2f coords() const { return coords_;}
  void coords(const Vec2f& v) { coords_ = v; }

  void index(int i) { index_ = i; }
  int index() const { return index_; }

private:
  Vec2f     coords_;
  Shape&    parent_;
  int       index_;

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
  Shape(ModelSpace& space, int index, bool extr) 
  : space_{space}, index_{index}, exteriror_{extr} {}
  virtual ~Shape() {}

  /*********************************************************
  * Drawing  
  *********************************************************/
  virtual void draw() = 0;  
  virtual void draw_nodes();

  /*********************************************************
  * Update / Check for validity
  *********************************************************/
  virtual void update() {};
  virtual bool valid() {};
  virtual void move(const Vec2f& d);

  /*********************************************************
  * Node handling
  *********************************************************/
  virtual Node* add_node(const Vec2f& n);
  virtual Node* get_node(const Vec2f& p);
  virtual void  rem_node(int index);

  /*********************************************************
  * Setters / Getters
  *********************************************************/
  void color(olc::Pixel col) { color_ = col; }

  void complete(bool c) { complete_ = c; }
  bool complete() const { return complete_; }

  int index(int i) { index_ = i; }
  int index() const { return index_; }

  int number_of_nodes() const { return nodes_.size(); }

protected:
  ModelSpace&       space_;
  int               index_;         
  bool              exteriror_;
  unsigned int      max_nodes_  = 0;
  std::vector<Node*> nodes_;
  olc::Pixel        color_      = olc::GREEN; 
  bool              complete_   = false;

};
