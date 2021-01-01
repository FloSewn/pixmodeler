#include "Shape.h"
#include "ModelSpace.h"


/***********************************************************
* Function to draw the nodes of a shape
***********************************************************/
void Shape::draw_nodes()
{
  for (auto &n : nodes_)
  {
    int sx, sy;
    space_.coord_to_screen(n.coords(), sx, sy);
    space_.FillCircle(sx, sy, 2, olc::RED);
  }
}

/***********************************************************
* Function to add a new node to the shape 
***********************************************************/
Node* Shape::add_node(const Vec2f& n)
{
  // Ignore query if shape is already complete
  if (nodes_.size() == max_nodes_)
    return nullptr;
  
  // Else create new node and add to shape
  nodes_.push_back( Node {*this, n} );
  return &nodes_[nodes_.size()-1];
}

/***********************************************************
* Function returns the first node located at a position p
***********************************************************/
Node* Shape::get_node(const Vec2f& p)
{
  for (auto &n : nodes_)
    if ( (p-n.coords()).length_squared() < 0.01f )
      return &n;
  return nullptr;
}