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
  menu_["main"].dimension(1, 3);
  menu_["main"]["Insert"].dimension(1,2);
  menu_["main"]["Insert"]["Exterior"].dimension(1,3);
  menu_["main"]["Insert"]["Interior"].enabled(false).dimension(1,3);
  menu_["main"]["Modify"].enabled(false).dimension(1,4);
  menu_["main"]["Delete"].enabled(false);

  // Exteriror Inserts
  MenuObject& menu_12 = menu_["main"]["Insert"]["Exterior"];
  menu_12["Polygon"].callback(insert_extr_polygon_mode_cb);
  menu_12["Square"];
  menu_12["Rectangle"];
  menu_12["Circle"];

  // Interior Inserts
  MenuObject& menu_11 = menu_["main"]["Insert"]["Interior"];
  menu_11["Polygon"];
  menu_11["Square"];
  menu_11["Rectangle"];
  menu_11["Circle"];

  // Modifications
  MenuObject& menu_2 = menu_["main"]["Modify"];
  menu_2["Add Node"];
  menu_2["Remove Node"];
  menu_2["Move Node"];
  menu_2["Move Shape"];

  menu_.build();
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

  // Handle insertion of exterior polygon
  if ( state_ == UserState::InsertExtrPolygon )
    insert_extr_polygon();

  // Return to view mode
  if (GetKey(olc::Key::ESCAPE).bPressed)
  {
    state_ = UserState::View;
    delete temp_shape_;
    temp_shape_    = nullptr;
    selected_node_ = nullptr;
    last_action_   = "View";
  }

  // Draw
  draw_background();
  grid_.draw(); 
  cursor_.draw(); 
  draw_shapes();

  // Draw menus
  menu_manager_.draw({30, 30});

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
* Function to insert exterior polygonal elements
***********************************************************/
void ModelSpace::insert_extr_polygon()
{
  if ( temp_shape_ == nullptr)
  {
    temp_shape_ = new Polygon(*this);
  }
  else
  {
    if (GetMouse(1).bReleased)
      selected_node_ = temp_shape_->add_node(cursor_.coords());

    if (temp_shape_->complete())
    {
      temp_shape_->color(olc::WHITE);
      shapes_.push_back(temp_shape_);
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

  // Draw all shapes
  for (auto s : shapes_)
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
  sp.last_action( "Insert exterior polygon");
}