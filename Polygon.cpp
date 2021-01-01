#include "Polygon.h"
#include "Shape.h"
#include "ModelSpace.h"


/***********************************************************
* Function to draw the polygon
***********************************************************/
void Polygon::draw()
{
  int sx, sy; 
  int ex, ey;

  if (nodes_.size() > 1)
  {
    for (int i = 1; i < nodes_.size(); ++i)
    {
      space_.coord_to_screen(nodes_[i-1].coords(), sx, sy);
      space_.coord_to_screen(nodes_[i].coords(), ex, ey);
      space_.DrawLine(sx, sy, ex, ey, color_);
    }

    if (complete_)
    {
      space_.coord_to_screen(nodes_[nodes_.size()-1].coords(), sx, sy);
      space_.coord_to_screen(nodes_[0].coords(), ex, ey);
      space_.DrawLine(sx, sy, ex, ey, color_);
    }
  }

}

/***********************************************************
* Function to add a new node to the shape 
***********************************************************/
Node* Polygon::add_node(const Vec2f& n)
{
  // Complete polygon if new node is first node
  if (nodes_.size() > 0)
  {
    Node* new_node = get_node(n);

    if (new_node)
    {
      if ( new_node->coords() == nodes_[0].coords() )
        complete_ = true;
      else
        return nullptr;
    }
  }

  // Check if new segment intersects with polygon
  if (nodes_.size() > 1)
  {
    Vec2f m = nodes_[nodes_.size()-1].coords();

    for (int i = 1; i < nodes_.size(); ++i)
    {
      Vec2f p = nodes_[i-1].coords();
      Vec2f q = nodes_[i].coords();

      if ( line_intersection(p,q,m,n) )
        return nullptr;
    }
  }

  // Ignore query if shape is already complete
  if (complete_)
    return nullptr;

  // Else create new node and add to shape
  nodes_.push_back( Node {*this, n} );
  return &nodes_[nodes_.size()-1];
}
