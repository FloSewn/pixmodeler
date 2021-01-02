#include "ModelSpace.h"
#include "Grid.h"
#include "Cursor.h"
#include "Vec2.h"

#include "Polygon.h"

#include "Menu.h"

/***********************************************************
* Constructor
***********************************************************/
ModelSpace::ModelSpace() 
: grid_{ Grid(*this) }, cursor_{ Cursor(*this) }, 
  menu_{ MenuObject(*this) }, menu_manager_{ MenuManager(*this) } 
{
  sAppName = "ModelSpace";
  init_main_menu();
}

/***********************************************************
* Function to transform from coordinate space to screen 
***********************************************************/
void ModelSpace::coord_to_screen(const Vec2f& v, int& sx, int& sy)
{
  sx = (int) ((v[0]-offset_[0]) * scale_);
  sy = (int) ((v[1]-offset_[1]) * scale_);
}

/***********************************************************
* Function to transform from screen to coordinate space 
***********************************************************/
void ModelSpace::screen_to_coord(int sx, int sy, Vec2f& v)
{
  v[0] = offset_[0] + (float)(sx) / scale_;
  v[1] = offset_[1] + (float)(sy) / scale_;
}

/***********************************************************
* Function to initialize the screen
***********************************************************/
bool ModelSpace::OnUserCreate()
{
  offset_[0] = (float)(-ScreenWidth() / 2) / scale_;
  offset_[1] = (float)(-ScreenHeight() / 2) / scale_;

  return true;
}

/***********************************************************
* Function to initialize the main menu
***********************************************************/
void ModelSpace::init_main_menu()
{
  // Main menu structure
  menu_["main"].dimension(1, 2);
  menu_["main"]["Insert"].dimension(1,2);
  menu_["main"]["Insert"]["Exterior"].dimension(1,3);
  menu_["main"]["Insert"]["Interior"].enabled(false).dimension(1,3);
  menu_["main"]["Modify"].enabled(false).dimension(1,4);

  // Exteriror Inserts
  MenuObject& menu_12 = menu_["main"]["Insert"]["Exterior"];
  menu_12["Polygon"].callback(insert_extr_polygon_mode_cb);
  menu_12["Square"].enabled(false);
  menu_12["Rectangle"].enabled(false);
  menu_12["Circle"].enabled(false);

  // Interior Inserts
  MenuObject& menu_11 = menu_["main"]["Insert"]["Interior"];
  menu_11["Polygon"];
  menu_11["Square"].enabled(false);
  menu_11["Rectangle"].enabled(false);
  menu_11["Circle"].enabled(false);

  // Modifications
  MenuObject& menu_2 = menu_["main"]["Modify"];
  menu_2["Add Node"];
  menu_2["Move Node"].callback(move_node_cb);
  menu_2["Move Shape"].callback(move_shape_cb);
  menu_2["Remove Node"].callback(remove_node_cb);
  menu_2["Remove Shape"].callback(remove_shape_cb);

  menu_.build();
}

/***********************************************************
* Function to initialize the main menu
***********************************************************/
void ModelSpace::update_main_menu()
{
  // Activate/deactivate menu options 
  if (extr_shapes_.size() > 0)
  {
    //menu_["main"]["Insert"]["Interior"].enabled(true);
    menu_["main"]["Modify"].enabled(true);
  }
  else
  {
    //menu_["main"]["Insert"]["Interior"].enabled(false);
    menu_["main"]["Modify"].enabled(false);
  }
}

/***********************************************************
* Function to update the screen
***********************************************************/
bool ModelSpace::OnUserUpdate(float fElapsedTime)
{
  // Update view of model space
  pan_and_zoom();

  // Update objects
  grid_.update();
  cursor_.update();
  menu_manager_.update(&menu_["main"]);

  switch (state_)
  {
  case UserState::InsertExtrPolygon:
    insert_extr_polygon();
    break;
  case UserState::MoveNode:
    move_node();
    break;
  case UserState::MoveShape:
    move_shape();
    break;
  case UserState::RemoveNode:
    remove_node();
    break;
  case UserState::RemoveShape:
    remove_shape();
    break;

  default:
    break;
  }


  // Return to view mode
  if (GetKey(olc::Key::ESCAPE).bPressed)
  {
    if (state_ == UserState::InsertExtrPolygon)
      delete temp_shape_;

      if (temp_shape_)
        temp_shape_->color(olc::WHITE);

      state_ = UserState::View;
      temp_shape_    = nullptr;
      selected_node_ = nullptr;
      last_action_   = "View";
  }

  // Draw
  draw_background();
  grid_.draw(); 
  cursor_.draw(); 
  draw_shapes();

  // Draw and updatemenus
  menu_manager_.draw({30, 30});
  update_main_menu();

  // Draw selected node
  if (selected_node_)
  {
    int sx, sy;
    coord_to_screen(selected_node_->coords(), sx, sy);
    FillCircle(sx, sy, 2, olc::GREEN);
  }

  // Draw status bar
  DrawString(10, 10, 
    "X="+std::to_string(cursor_.coords()[0])
    +", Y="+std::to_string(cursor_.coords()[1]),
    olc::YELLOW, 1);
  DrawString(10, ScreenHeight() - 20, 
             last_action_, olc::YELLOW, 1);

  return true;
}

/***********************************************************
* Set selected node for existing shapes
***********************************************************/
void ModelSpace::set_selected_node()
{
  if (GetMouse(1).bReleased)
  {
    for (auto s : extr_shapes_)
    {
      selected_node_ = s->get_node(cursor_.coords());
      if (selected_node_) 
      {
        temp_shape_ = s;
        temp_shape_->color(olc::GREEN);
        break;
      }
    }
    for (auto s : intr_shapes_)
    {
      selected_node_ = s->get_node(cursor_.coords());
      if (selected_node_)
      {
        temp_shape_ = s;
        temp_shape_->color(olc::GREEN);
        break;
      }
    }
  }
}


/***********************************************************
* Function to insert exterior polygonal elements
***********************************************************/
void ModelSpace::insert_extr_polygon()
{
  if ( temp_shape_ == nullptr)
  {
    temp_shape_ = new Polygon(*this, extr_shapes_.size(), true);
  }
  else
  {
    if (GetMouse(1).bReleased)
      selected_node_ = temp_shape_->add_node(cursor_.coords());

    if (temp_shape_->complete())
    {
      temp_shape_->color(olc::WHITE);
      extr_shapes_.push_back(temp_shape_);
      temp_shape_ = nullptr;
    }
  }
}

/***********************************************************
* Function to move existing nodes
***********************************************************/
void ModelSpace::move_node()
{
  
  if (selected_node_ == nullptr)
  {
    set_selected_node();
  }
  else 
  {
    Vec2f tmp_coords = selected_node_->coords();
    selected_node_->coords(cursor_.coords());

    // Check validity of new node
    if (!temp_shape_->valid())
      selected_node_->coords(tmp_coords);

    // Release
    if (GetMouse(1).bReleased)
    {
      temp_shape_->color(olc::WHITE);
      selected_node_ = nullptr;
      temp_shape_ = nullptr;
    }
  }
}


/***********************************************************
* Function to move existing shapes
***********************************************************/
void ModelSpace::move_shape()
{
  if (selected_node_ == nullptr)
  {
    set_selected_node();
  }
  else
  {
    Vec2f node_coords = selected_node_->coords();
    Vec2f delta = cursor_.coords() - node_coords;
    
    if (temp_shape_)
      temp_shape_->move(delta);

    // Release
    if (GetMouse(1).bReleased)
    {
      temp_shape_->color(olc::WHITE);
      selected_node_ = nullptr;
      temp_shape_ = nullptr;
    }
  }
}

/***********************************************************
* Function to remove existing shapes
***********************************************************/
void ModelSpace::remove_shape()
{
  if (selected_node_ == nullptr)
  {
    set_selected_node();
  }
  else
  {
    // Remove shape and update following shape indices
    if (GetMouse(1).bReleased &&
        cursor_.coords() == selected_node_->coords())
    {
      int index = temp_shape_->index();
      extr_shapes_.erase(extr_shapes_.begin() + index);

      for (int i = index; i < extr_shapes_.size(); i++)
        extr_shapes_[i]->index(i);

      selected_node_ = nullptr;
      temp_shape_ = nullptr;
    }
    else if (GetMouse(1).bReleased)
    {
      selected_node_ = nullptr;
      temp_shape_->color(olc::WHITE);
      temp_shape_ = nullptr;
    }
    
  }
}

/***********************************************************
* Function to remove existing nodes
***********************************************************/
void ModelSpace::remove_node()
{
  if (selected_node_ == nullptr)
  {
    set_selected_node();
  }
  else
  {
    // Remove node and update following nodes indices
    if (GetMouse(1).bReleased && 
        cursor_.coords() == selected_node_->coords())
    {
      int n_nodes = temp_shape_->number_of_nodes();

      if ( n_nodes > 3)
      {
        int index = selected_node_->index();
        temp_shape_->rem_node(index);
        selected_node_ = nullptr;
        temp_shape_->color(olc::WHITE);
        temp_shape_ = nullptr;
      }
      else
        remove_shape();
    }
    else if (GetMouse(1).bReleased)
    {
      selected_node_ = nullptr;
      temp_shape_->color(olc::WHITE);
      temp_shape_ = nullptr;
    }
    
  }
}

/***********************************************************
* Function to pan and zoom the space coordinats 
***********************************************************/
void ModelSpace::pan_and_zoom()
{
  Vec2f mouse = { (float)GetMouseX(), (float)GetMouseY() };

  // Mouse panning
  if (GetMouse(0).bPressed)
  {
    start_pan_ = mouse;
  }
  if (GetMouse(0).bHeld)
  {
    offset_   -= (mouse - start_pan_) / scale_;
    start_pan_ = mouse;
  }

  // Mouse zooming
  Vec2f mouse_z_old, mouse_z_new;
  screen_to_coord((int)mouse[0], (int)mouse[1], mouse_z_old);

  if (GetKey(olc::Key::Q).bHeld || GetMouseWheel() > 0)
    if (1.1f*scale_ < max_scale_)
      scale_ *= 1.1f;

  if (GetKey(olc::Key::A).bHeld || GetMouseWheel() < 0)
    if (0.9f*scale_ > min_scale_)
      scale_ *= 0.9f;

  screen_to_coord((int)mouse[0], (int)mouse[1], mouse_z_new);
  offset_ += (mouse_z_old - mouse_z_new);
  mouse_coords_ = mouse_z_new;
}

/***********************************************************
* Function to draw the background
***********************************************************/
void ModelSpace::draw_background()
{
  // Clear screen
  Clear(olc::VERY_DARK_BLUE);
}

/***********************************************************
* Function to draw the shapes  
***********************************************************/
void ModelSpace::draw_shapes()
{
  // Draw current shape 
  if (temp_shape_ != nullptr)
  {
    temp_shape_->draw();
    temp_shape_->draw_nodes();
  }

  // Draw exterior shapes
  for (auto s : extr_shapes_)
  {
    s->draw();
    s->draw_nodes();
  }
}

/***********************************************************
* Callback function for creation of an exterior polygon
***********************************************************/
void insert_extr_polygon_mode_cb(ModelSpace& sp, MenuObject& mo)
{
  sp.state( UserState::InsertExtrPolygon );
  sp.last_action( "Insert exterior polygon" );
}

/***********************************************************
* Callback function for moving nodes
***********************************************************/
void move_node_cb(ModelSpace& sp, MenuObject& mo)
{
  sp.state( UserState::MoveNode );
  sp.last_action( "Move node" );
}

/***********************************************************
* Callback function for moving shapes
***********************************************************/
void move_shape_cb(ModelSpace& sp, MenuObject& mo)
{
  sp.state( UserState::MoveShape );
  sp.last_action( "Move shape" );
}

/***********************************************************
* Callback function for removing shapes
***********************************************************/
void remove_shape_cb(ModelSpace& sp, MenuObject& mo)
{
  sp.state( UserState::RemoveShape );
  sp.last_action( "Remove shape" );
}

/***********************************************************
* Callback function for removing nodes
***********************************************************/
void remove_node_cb(ModelSpace& sp, MenuObject& mo)
{
  sp.state( UserState::RemoveNode );
  sp.last_action( "Remove node" );
}