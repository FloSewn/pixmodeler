#include "Shape.h"
#include "ModelSpace.h"
#include "Polygon.h"

#include <iostream>
#include <iomanip>
#include <cstdio>



/***********************************************************
* Weiler-Atherthon algorithm for the estimation of polygon
* intersections
* This function takes as input argument a top shape t and a
* bottom shape b. It estimates the intersections between 
* t and b and creates the respective point lists, and
* intersection links, which can be used to construct 
* the union or cuttings of both polygons.
* 
* References: 
* https://www.geeksforgeeks.org/weiler-atherton-polygon-clipping-algorithm/
***********************************************************/
IntersectData* prepare_poly_intersection(Shape* t, Shape *b)
{
  int Nt = t->number_of_nodes();
  int Nb = b->number_of_nodes();

  std::vector<Vec2f> intersecs;
  std::vector<int> t_intersec_index;
  std::vector<int> b_intersec_index;

  for (int i = 0; i < Nt; ++i)
  {
    Vec2f t_1 = t->get_node(i)->coords();
    Vec2f t_2 = t->get_node((i+1)%Nt)->coords();

    for (int j = 0; j < Nb; ++j)
    {
      Vec2f b_1 = b->get_node(j)->coords();
      Vec2f b_2 = b->get_node((j+1)%Nb)->coords();

      /*----------------------------------------------------
      | Check for intersections
      ----------------------------------------------------*/
      bool intersect = false;

      Orient o1 = orientation(t_1, t_2, b_1);
      Orient o2 = orientation(t_1, t_2, b_2);
      Orient o3 = orientation(b_1, b_2, t_1);
      Orient o4 = orientation(b_1, b_2, t_2);

      if (  ( (o1 == Orient::CCW && o2 == Orient::CW ) ||
              (o1 == Orient::CW  && o2 == Orient::CCW) ) 
        &&  ( (o3 == Orient::CCW && o4 == Orient::CW ) ||
              (o3 == Orient::CW  && o4 == Orient::CCW) ) )
      {
        intersect = true;
      }

      // (t_1,t_2) and b_1 are collinear 
      // and b_2 lies on segment (t_1,t_2)
      if ( (o1 == Orient::CL) && in_on_segment(t_1,t_2,b_1) )
        intersect = true;
      // (t_1,t_2) and b_2 are collinear 
      // and b_1 lies on segment (t_1,t_2)
      if ( (o2 == Orient::CL) && in_on_segment(t_1,t_2,b_2) )
        intersect = true;
      // (b_1,b_2) and t_1 are collinear 
      // and t_2 lies on segment (b_1,b_2)
      if ( (o3 == Orient::CL) && in_on_segment(b_1,b_2,t_1) )
        intersect = true;
      // (b_1,b_2) and t_1 are collinear 
      // and t_2 lies on segment (b_1,b_2)
      if ( (o4 == Orient::CL) && in_on_segment(b_1,b_2,t_2) )
        intersect = true;

      /*----------------------------------------------------
      | Intersection detected
      ----------------------------------------------------*/
      if ( intersect )
      {
        // Compute point of intersection
        Vec2f dt = t_2 - t_1;
        Vec2f db = b_2 - b_1;
        Vec2f tb = b_1 - t_1;
        float d = cross(dt, db);
        if (fabs(d) < geometry_small)
          continue;

        float t = cross( tb, db) / d;
        Vec2f m_ts = t_1 + dt * t;

        bool already_found = false;
        for (auto ints : intersecs)
          if (m_ts == ints)
            already_found = true;
        
        if (already_found)
          continue;

        intersecs.push_back(m_ts);

        // Push back indices of line start
        t_intersec_index.push_back( i ); 
        b_intersec_index.push_back( j ); 
      }
    }
  }

  // No intersections found
  if (intersecs.size() == 0)
    return nullptr;
  
  // Create point lists that contain the intersection points
  IntersectData* data = new IntersectData();

  // Init with original points and add intersections
  for (int i = 0; i < Nt; ++i)
  {
    data->t_list.push_back(t->get_node(i)->coords());
    data->t_intersec.push_back(false);

    for (int j = 0; j < intersecs.size(); ++j)
      if (t_intersec_index[j] == i)
      {
        data->t_list.push_back(intersecs[j]);
        data->t_intersec.push_back(true);
      }
  }

  // Remove duplicates
  for (int i = data->t_list.size()-1; i >= 0; --i)
    for (int j = i-1; j >= 0; --j)
      if (data->t_list[i] == data->t_list[j])
      {
        if (data->t_intersec[i])
          data->t_intersec[j] = data->t_intersec[i];
        data->t_list.erase(data->t_list.begin()+i);
        data->t_intersec.erase(data->t_intersec.begin()+i);
      }


  // Init with original points and add intersections
  for (int i = 0; i < Nb; ++i)
  {
    data->b_list.push_back(b->get_node(i)->coords());
    data->b_intersec.push_back(false);

    for (int j = 0; j < intersecs.size(); ++j)
      if (b_intersec_index[j] == i)
      {
        data->b_list.push_back(intersecs[j]);
        data->b_intersec.push_back(true);
      }
  }

  // Remove duplicates
  for (int i = data->b_list.size()-1; i >= 0; --i)
    for (int j = i-1; j >= 0; --j)
      if (data->b_list[i] == data->b_list[j])
      {
        if (data->b_intersec[i])
          data->b_intersec[j] = data->b_intersec[i];
        data->b_list.erase(data->b_list.begin()+i);
        data->b_intersec.erase(data->b_intersec.begin()+i);
      }

  // Create links between lists
  for (int i = 0; i < data->t_list.size(); ++i)
    data->t_link.push_back(-1);
  for (int i = 0; i < data->b_list.size(); ++i)
    data->b_link.push_back(-1);

  // Bad way: O(n^2) --> improve!
  for (int i = 0; i < data->t_list.size(); ++i)
    if (data->t_intersec[i])
      for (int j = 0; j < data->b_list.size(); ++j)
        if ( data->t_list[i] == data->b_list[j] )
        {
          data->t_link[i] = j;
          data->b_link[j] = i;
        }

  data->Nt = data->t_list.size();
  data->Nb = data->b_list.size();

  // Now start with algorithm and create new polygons
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "\n--- Top shape ---\n";
  for (int i = 0; i < data->Nt; i++)
    std::cout << "Index: " << i << " - " << data->t_list[i] 
      << " - intersection: " << data->t_intersec[i] 
      << " - link: " << data->t_link[i] << "\n";

  std::cout << "\n--- Bottom shape ---\n";
  for (int i = 0; i < data->Nb; i++)
    std::cout << "Index: " << i << " - " << data->b_list[i] 
      << " - intersection: " << data->b_intersec[i]  
      << " - link: " << data->b_link[i] << "\n";

 return data;
}




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
    space_.DrawString(sx+3, sy+3, std::to_string(n.index()), olc::WHITE);
  }
}

/***********************************************************
* Function to draw the shape
***********************************************************/
void Shape::draw()
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
* Function to check if the shape is valid 
* -> check if shape edges intersect
***********************************************************/
bool Shape::valid()
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
* Every shape is a list of nodes. The last node is 
* connected to the first one.
* The shape edges must not intersect.
***********************************************************/
Node* Shape::add_node(const Vec2f& n)
{
  // Complete shape if new node is first node
  // Adjust orientation of shape is needed
  if (nodes_.size() > 0)
  {
    Node* new_node = get_node(n);

    if (new_node)
    {
      if ( new_node->coords() == nodes_[0]->coords() )
      {
        complete_ = true;
        set_orientation(Orient::CCW);
      }
      else
      {
        return nullptr;
      }
    }
  }

  // Check if new segment intersects with polygon
  if (nodes_.size() > 1 && !complete_)
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

/***********************************************************
* Function to add a new node to the shape before the 
* element with index 
* Works only on complete shapes
***********************************************************/
Node* Shape::add_node(int index, const Vec2f& n)
{
  if (!complete_)
    return nullptr;

  if (index >= nodes_.size() || index < 0 )
    return nullptr;

  nodes_.insert(nodes_.begin()+index, 
                new Node {*this, index, n} );

  // Update indices of nodes 
  for (int i = index+1; i < nodes_.size(); i++)
    nodes_[i]->index(i);
  
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
* Function to check the shape for its orientation. If
* it is not oriented counter-clockwise, the nodes are 
* rearranged
***********************************************************/
void Shape::set_orientation(Orient orient)
{
  int N = nodes_.size();
  int cw_turns = 0;
  int ccw_turns = 0;

  for (int i = 0; i < N; ++i)
  {
    Vec2f p = nodes_[i]->coords();
    Vec2f q = nodes_[(i+1)%N]->coords();
    Vec2f r = nodes_[(i+2)%N]->coords();

    if (orientation(p, q, r) == Orient::CCW)
      ccw_turns++;
    else if (orientation(p, q, r) == Orient::CW)
      cw_turns++;
  }
  // Re-orientation 
  if ( (orient == Orient::CCW && cw_turns > ccw_turns) ||
       (orient == Orient::CW && ccw_turns > cw_turns) )
  {
    for (int i = 1; i < int(ceil(N/2.)); ++i)
    {
      Node* tmp = nodes_[i];
      nodes_[i]->index(N-i);
      nodes_[N-i]->index(i);
      nodes_[i] = nodes_[N-i];
      nodes_[N-i] = tmp;
    }
  }
}

/***********************************************************
* Function to check if another shape s is contained 
* inside this shape.
***********************************************************/
bool Shape::contains_shape(Shape* s)
{
  for (int i = 0; i < s->number_of_nodes(); ++i)
    if (!contains_node(s->get_node(i)->coords()))
      return false;
  return true;
}

/***********************************************************
* Function to clip this shape on another shape b.
* Returns pointer to a vector of all resulting new shapes
***********************************************************/
std::vector<Shape*> Shape::clip(Shape* s)
{
  std::vector<Shape*> new_shapes;

  if (!s || !complete_ || !s->complete() || s == this)
    return new_shapes;

  /*--------------------------------------------------------
  | Check if shapes contain each other
  --------------------------------------------------------*/
  if (contains_shape(s) || s->contains_shape(this))
    return new_shapes;

  /*--------------------------------------------------------
  | Find all intersection points and mark them as 
  | entering or exiting
  --------------------------------------------------------*/
  IntersectData* intersec = prepare_poly_intersection(this, s);
  
  return new_shapes;
}

/***********************************************************
* Function to merge this shape with another one.
* Returns pointer to a new shape
* --> Weiler-Atherton Algorithm
***********************************************************/
Shape* Shape::merge(Shape* s)
{
  if (!s || !complete_ || !s->complete())
    return nullptr;

  /*--------------------------------------------------------
  | Check if s is contained in this shape
  --------------------------------------------------------*/
  int n = 0;
  for (int i = 0; i < s->number_of_nodes(); ++i)
  {
    if (contains_node(s->get_node(i)->coords()))
      n++;
  }
  if (n == s->number_of_nodes())
    return nullptr;

  /*--------------------------------------------------------
  | Get a node in current shape that is not in s
  --------------------------------------------------------*/
  Node* start = nullptr;
  for (int i = 0; i < nodes_.size(); ++i)
  {
    if (!s->contains_node(nodes_[i]->coords()))
    {
      start = nodes_[i];
      break;
    }
  }
  if (start == nullptr)
    return nullptr;

  /*--------------------------------------------------------
  | Init a new shape
  --------------------------------------------------------*/
  int new_index = -1;
  if (exteriror_)
    new_index = space_.number_of_extr_shapes();
  else
    new_index = space_.number_of_intr_shapes();

  Polygon* new_poly = new Polygon(space_, new_index, exteriror_);
  new_poly->add_node(start->coords());

  /*--------------------------------------------------------
  | Start Weiler-Atherton algorithm
  --------------------------------------------------------*/
  Shape* a = this;
  Shape* b = s;

  int N_a = a->number_of_nodes();
  int N_b = b->number_of_nodes();

  int i = 0;
  int intersections = 0;

  // Init states of all edges
  for (int i = 0; i < N_a; ++i)
    a->get_node(i)->state(Node::State::Unvisited);
  for (int i = 0; i < N_b; ++i)
    b->get_node(i)->state(Node::State::Unvisited);

  // Traverse shape a
  do
  {
    Vec2f p_a = a->nodes_[i%N_a]->coords();
    Vec2f pr_a = a->nodes_[(i+1)%N_a]->coords();

    // Traverse shape b
    for (int j = 0; j < N_b; j++)
    {
      Vec2f p_b = b->nodes_[j]->coords();
      Vec2f pr_b = b->nodes_[(j+1)%N_b]->coords();

      // Check for intersection
      if ( line_intersection(p_a,pr_a, p_b,pr_b) )
      {
        // Ignore if intersecting lines are colinear
        if (orientation(p_a, pr_a, p_b) == Orient::CL &&
            orientation(p_a, pr_a, pr_b) == Orient::CL)
            break;

        // Compute point of intersection
        Vec2f r_a = pr_a - p_a;
        Vec2f r_b = pr_b - p_b;
        float t = cross( p_a-p_b, r_b / cross(r_a, r_b) );
        Vec2f m = p_a - r_a * t;

        // If edge was not visited yet -> outgoing
        if (b->nodes_[j]->state() == Node::State::Unvisited)
        {
          b->nodes_[j]->state(Node::State::Visited);
          new_poly->add_node(m);
          intersections++;

          if (m == pr_b)
          {
            pr_a = b->nodes_[(j+2)%N_b]->coords();
            i = (j+1)%N_b;
          }
          else
          {
            pr_a = pr_b; 
            i = j;
          }

          // Switch shapes
          Shape* tmp = b;
          b = a;
          a = tmp;
          
          int N_tmp = N_b;
          N_b = N_a;
          N_a = N_tmp;
          break;
        }
        else
        {
          // edge was visited -> unmark for next intersection
          b->nodes_[j]->state(Node::State::Unvisited);
        }
      }
    }

    // No intersection -> add new node
    new_poly->add_node(pr_a);
    i++;

  } while (a->nodes_[i%N_a] != start);

  // No intersection occured -> delete shape
  if (intersections == 0)
  {
    delete new_poly;
    new_poly = nullptr;
  }

  return new_poly;
}


/***********************************************************
* Function to check if a node is contained inside the shape
***********************************************************/
bool Shape::contains_node(const Vec2f& n)
{
  int N = nodes_.size();
  for (int i = 0; i < N; i++)
  {
    Vec2f p = nodes_[i]->coords();
    Vec2f q = nodes_[(i+1)%N]->coords();
    if (!is_left(p,q,n))
      return false;
  }
  return true;
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
* Function returns the node at index i
***********************************************************/
Node* Shape::get_node(int index)
{
  if (index >= nodes_.size() || index < 0)
    return nullptr;

  return nodes_[index];
}

/***********************************************************
* Function to move the entire shape
***********************************************************/
void Shape::move(const Vec2f& d)
{
  for (auto n : nodes_)
    n->coords(n->coords() +d );
}
