#pragma once

#include "olc_pixel_game_engine.h"

#include "Vec2.h"
#include "Cursor.h"
#include "Grid.h"

#include "Menu.h"
#include "Shape.h"

/***********************************************************
* Program state
***********************************************************/
enum class UserState {
  View,
  InsertExtrPolygon,
  MoveNode,
  MoveShape,
  RemoveShape,
  RemoveNode,
  InsertNode,
  MergeShapes,
  ClipShape
};

/***********************************************************
* This class defines a two dimensional space which can 
* be accessed by a user 
***********************************************************/
class ModelSpace : public olc::PixelGameEngine
{


public:
  // Constructor
  ModelSpace();

  // Destructor
  ~ModelSpace()
  {
    for (auto s: extr_shapes_)
      delete s;

    for (auto s: intr_shapes_)
      delete s;
  }

  // coordinate transformations
  void coord_to_screen(const Vec2f& v, int& sx, int& sy);
  void screen_to_coord(int sx, int sy, Vec2f& v);

  Vec2f& mouse_coords() { return mouse_coords_; }
  Vec2f& offset() { return offset_; }

  Grid& grid() { return grid_; }
  Cursor& cursor() { return cursor_; }

  float scale() const { return scale_; }
  float min_scale() const { return min_scale_; }
  float max_scale() const { return max_scale_; }

  // Main menu
  void init_main_menu();
  void update_main_menu();

  // PixelEngine functions
  bool OnUserCreate() override;
  bool OnUserUpdate(float fElapsedTime) override; 

  // Getters / Setters
  UserState state() { return state_; }
  void state(UserState s) { state_ = s; }

  Shape* temp_shape() { return temp_shape_; }
  void temp_shape(Shape* shape) { temp_shape_ = shape; }

  Node* selected_node() { return selected_node_; }
  void selected_node(Node* node) { selected_node_ = node; }

  void last_action(std::string s) {last_action_ = s; }
  std::string& last_action() { return last_action_; }

  int number_of_extr_shapes() const 
  { return extr_shapes_.size(); }
  int number_of_intr_shapes() const 
  { return intr_shapes_.size(); }

  void reset();

  // Shape insertion functions
  void insert_extr_polygon(); 

  void move_node(); 
  void remove_node(); 
  void move_shape();
  void remove_shape();
  void insert_node();
  void merge_shapes();
  void clip_shape();

private:
  Grid        grid_;
  Cursor      cursor_;
  MenuObject  menu_;
  MenuManager menu_manager_;

  std::string  last_action_ = "View";

  Vec2f   offset_         = {0.0f, 0.0f};
  Vec2f   start_pan_      = {0.0f, 0.0f};
  Vec2f   mouse_coords_   = {0.0f, 0.0f};

  Node*   selected_node_  = nullptr;
  Shape*  temp_shape_     = nullptr;

  std::vector<Shape*> extr_shapes_;
  std::vector<Shape*> intr_shapes_;

  float   scale_          = 50.0f;
  float   max_scale_      = 100.0f;
  float   min_scale_      = 5.0f;

  UserState state_   = UserState::View;

  void pan_and_zoom();
  void draw_background();
  void draw_shapes();
  void set_selected_node();
  Shape* pick_shape(const Vec2f& c, bool extr_shape);

};


/***********************************************************
* Menu callback functions
***********************************************************/
void insert_extr_polygon_mode_cb(ModelSpace& sp, MenuObject& mo);
void move_node_cb(ModelSpace& sp, MenuObject& mo);
void move_shape_cb(ModelSpace& sp, MenuObject& mo);
void remove_shape_cb(ModelSpace& sp, MenuObject& mo);
void remove_node_cb(ModelSpace& sp, MenuObject& mo);
void insert_node_cb(ModelSpace& sp, MenuObject& mo);
void merge_shapes_cb(ModelSpace& sp, MenuObject& mo);
void clip_shape_cb(ModelSpace& sp, MenuObject& mo);