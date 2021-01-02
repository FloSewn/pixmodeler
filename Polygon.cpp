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
      space_.coord_to_screen(nodes_[i-1]->coords(), sx, sy);
      space_.coord_to_screen(nodes_[i]->coords(), ex, ey);
      space_.DrawLine(sx, sy, ex, ey, color_);
    }

    if (complete_)
    {
      space_.coord_to_screen(nodes_[nodes_.size()-1]->coords(), sx, sy);
      space_.coord_to_screen(nodes_[0]->coords(), ex, ey);
      space_.DrawLine(sx, sy, ex, ey, color_);
    }
  }
}

/***********************************************************
* Function to check if the polygon is valid
***********************************************************/
bool Polygon::valid()
{
  int N = nodes_.size();

  // Check if segments intersects within polygon
  if (N > 1)
  {
    for (int i = 0; i < N-2; ++i)
    {
      Vec2f m = nodes_[i]->coords();
      Vec2f n = nodes_[i+1]->coords();

      for (int j = i+2; j < i+N-1; ++j)
      {
        if (j%N == 0)
          break;

        Vec2f p = nodes_[j%N]->coords();
        Vec2f q = nodes_[(j+1)%N]->coords();

        if ( line_intersection(p,q,m,n) )
          return false;
        
        if (m == p || m == q || n == p || n == q)
          return false;
      }
    }
  }

  return true;

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
      if ( new_node->coords() == nodes_[0]->coords() )
        complete_ = true;
      else
        return nullptr;
    }
  }

  // Check if new segment intersects with polygon
  if (nodes_.size() > 1)
  {
    Vec2f m = nodes_[nodes_.size()-1]->coords();

    for (int i = 1; i < nodes_.size(); ++i)
    {
      Vec2f p = nodes_[i-1]->coords();
      Vec2f q = nodes_[i]->coords();

      if ( line_intersection(p,q,m,n) )
        return nullptr;
    }
  }

  // Ignore query if shape is already complete
  if (complete_)
    return nullptr;

  // Else create new node and add to shape
  int index = nodes_.size();
  nodes_.push_back( new Node {*this, index, n} );
  return nodes_[nodes_.size()-1];
}
