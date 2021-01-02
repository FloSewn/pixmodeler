#include "Shape.h"
#include "ModelSpace.h"


/***********************************************************
* Function to draw the nodes of a shape
***********************************************************/
void Shape::draw_nodes()
{
  //for (auto &n : nodes_)
  for (int i = 0; i < nodes_.size(); ++i)
  {
    Node n = *nodes_[i];
    int sx, sy;
    space_.coord_to_screen(n.coords(), sx, sy);
    space_.FillCircle(sx, sy, 2, olc::RED);
    space_.DrawString(sx+3, sy+3, std::to_string(i), olc::WHITE);
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
  int index = nodes_.size();
  nodes_.push_back( new Node {*this, index, n} );
  return nodes_[nodes_.size()-1];
}

/***********************************************************
* Function removes a specified node
***********************************************************/
void Shape::rem_node(int index)
{
  if ( index >= nodes_.size() || index < 0)
    return;
  
  nodes_.erase( nodes_.begin()+index );

  for (int i = index; i < nodes_.size(); i++)
    nodes_[i]->index(i);
}

/***********************************************************
* Function returns the first node located at a position p
***********************************************************/
Node* Shape::get_node(const Vec2f& p)
{
  for (auto n : nodes_)
    if ( (p-n->coords()).length_squared() < 0.01f )
      return n;
  return nullptr;
}

/***********************************************************
* Function to move the entire shape
***********************************************************/
void Shape::move(const Vec2f& d)
{
  for (auto n : nodes_)
    n->coords(n->coords() +d );
}