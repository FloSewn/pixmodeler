#pragma once

#include <vector>
#include <list>

#include "Vec2.h"
#include "olc_pixel_game_engine.h"



class Shape;
class ModelSpace;


/***********************************************************
* Structure to handle the data for polygon intersection
***********************************************************/
struct IntersectData
{
  // t_list / b_list: contain point coordinates
  std::vector<Vec2f> t_list;
  std::vector<Vec2f> b_list;

  // t_intersec / b_intersec: true, if point coordinate is 
  //                          an intersection
  std::vector<bool>  t_intersec;
  std::vector<bool>  b_intersec;

  // t_link / b_link : link to point list of other polygon
  //                   at intersection points
  //                   = -1 if no intersection
  std::vector<int>   t_link;
  std::vector<int>   b_link;

  // Poly lengths
  int Nt;
  int Nb;

};

/***********************************************************
* Weiler-Atherthon algorithm for the estimation of polygon
* intersections
* This function takes as input argument a top shape t and a
* bottom shape b. It estimates the intersections between 
* t and b and creates the respective point lists, and
* intersection links, which can be used to construct 
* the union or cuttings of both polygons.
***********************************************************/
IntersectData* prepare_poly_intersection(Shape* t, Shape *b);


/***********************************************************
* This class defines a node from which shapes are made of 
***********************************************************/
class Node
{
public:
  enum class State {
    Visited,
    Unvisited
  };

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

  void state(State s) { state_ = s; }
  State state() { return state_; }

private:
  Vec2f     coords_;
  Shape&    parent_;
  int       index_;
  State     state_ = State::Unvisited;

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
  virtual void draw();  
  virtual void draw_nodes();

  /*********************************************************
  * Update / Check for validity
  *********************************************************/
  virtual bool valid();
  virtual void move(const Vec2f& d);

  /*********************************************************
  * Node handling
  *********************************************************/
  virtual Node* add_node(const Vec2f& n);
  virtual Node* add_node(int i, const Vec2f& n);
  virtual Node* get_node(const Vec2f& p);
  virtual Node* get_node(int i);
  virtual void  rem_node(int index);
  virtual void  set_orientation(Orient orient);
  virtual bool  contains_node(const Vec2f& n);

  /*********************************************************
  * Interaction with other shapes
  *********************************************************/
  virtual bool contains_shape(Shape *s);
  virtual Shape* merge(Shape* s);
  virtual std::vector<Shape*> clip(Shape* s);

  /*********************************************************
  * Setters / Getters
  *********************************************************/
  void color(olc::Pixel col) { color_ = col; }

  void complete(bool c) { complete_ = c; }
  bool complete() const { return complete_; }

  int index(int i) { index_ = i; }
  int index() const { return index_; }

  int number_of_nodes() const { return nodes_.size(); }

  bool exterior() const { return exteriror_; }

protected:
  ModelSpace&       space_;
  int               index_;         
  bool              exteriror_;
  unsigned int      max_nodes_  = 0;
  std::vector<Node*> nodes_;
  olc::Pixel        color_      = olc::GREEN; 
  bool              complete_   = false;

};
