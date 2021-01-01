#pragma once 

#include "Vec2.h"
#include "ModelSpace.h"
#include "olc_pixel_game_engine.h"

#include <string>
#include <unordered_map>

// Forward declaration
class MenuObject;
class MenuManager;

// Function pointer for menu functions
typedef void(*MenuCallback)(ModelSpace&,MenuObject&);

/***********************************************************
* The menu objects that stores and draws the data
***********************************************************/
class MenuObject
{
public:

  /*--------------------------------------------------------
  | Constructors
  --------------------------------------------------------*/
  MenuObject(ModelSpace& space)
  : space_{space} { name_ = "root"; }
  MenuObject(ModelSpace& space, const std::string& name) 
  : space_{space}, name_{name} {}

  /*--------------------------------------------------------
  | Setters
  --------------------------------------------------------*/
  MenuObject& dimension(int col, int row)
  {
    table_dimension_[0] = col;
    table_dimension_[1] = row;
    return *this;
  }
  MenuObject& id(int id)
  { id_ = id; return *this; }
  MenuObject& enabled(bool e)
  { enabled_ = e; return *this; } 
  MenuObject& callback(MenuCallback cb)
  { cb_ = cb; return *this; }

  /*--------------------------------------------------------
  | Getters
  --------------------------------------------------------*/
  bool enabled() const { return enabled_; }
  int id() const { return id_; }
  std::string& name() { return name_; }
  Vec2i cursor_position() { return cursor_position_; }
  bool has_children() { return !items_.empty(); }
  Vec2i size() { return { int(name_.size()), 1}; }
  int patch_unit() const { return patch_unit_; }
  MenuObject* get_selected_item() { return &items_[cursor_item_]; }
  MenuObject* callback() 
  { 
    if (cb_) { cb_(space_, *this); return this; } 
    return nullptr; 
  }

  /*--------------------------------------------------------
  | Access / definition operator
  --------------------------------------------------------*/
  MenuObject& operator[](const std::string& name)
  {
    if (item_table_.count(name) == 0)
    {
      item_table_[name] = items_.size();
      items_.push_back( MenuObject{space_, name} );
    }
    return items_[item_table_[name]];
  }

  /*--------------------------------------------------------
  | Buid the menu structure recursively
  --------------------------------------------------------*/
  void build();

  /*--------------------------------------------------------
  | Clamp the cursor
  --------------------------------------------------------*/
  void clamp_cursor();

  /*--------------------------------------------------------
  | Button functions
  --------------------------------------------------------*/
  void on_up();
  void on_down();
  void on_right();
  void on_left();
  MenuObject* on_confirm();

  /*--------------------------------------------------------
  | Draw the menu 
  --------------------------------------------------------*/
  void draw(olc::Sprite* sprGFX,
            Vec2i screen_offset);

protected:
  ModelSpace&     space_;

  std::string     name_;
  bool            enabled_          = true;
  int             id_               = -1;
  MenuCallback    cb_               = nullptr;


  int             total_rows_       = 0;
  int             top_visible_row_  = 0;

  // Dimension of the entire menu object in [patch_size]
  // <patch_len_> is defined by the sprites used
  const int       patch_unit_       = 8;
  olc::vi2d       patch_size_       = { patch_unit_, 
                                        patch_unit_ };

  Vec2i           table_dimension_  = { 1, 0 };
  Vec2i           table_size_       = { 0, 0 };
  Vec2i           cell_size_        = { 0, 0 };
  Vec2i           cell_padding_     = { 2, 1 };

  // Data 
  std::unordered_map<std::string, size_t> item_table_;
  std::vector<MenuObject> items_;

  // Cursor in table space
  Vec2i           cell_cursor_      = { 0, 0 };
  int             cursor_item_      = 0;
  Vec2i           cursor_position_  = { 0, 0 };


};



/***********************************************************
* The menu manager 
***********************************************************/
class MenuManager
{
public:
  MenuManager(ModelSpace& space) 
  : space_{space} 
  { 
    sprGFX_ = new olc::Sprite("../data/RetroMenu.png");
  }

  /*--------------------------------------------------------
  | Handle open / closing of menus
  --------------------------------------------------------*/
  void Open(MenuObject* mo)
  { 
    Close(); 
    menu_panels_.push_back(mo);
  }
  void Close()
  {
    menu_panels_.clear();
  }

  /*--------------------------------------------------------
  | Handle key pressing
  --------------------------------------------------------*/
  void on_up()
  { if(!menu_panels_.empty()) menu_panels_.back()->on_up(); }
  void on_down()
  { if(!menu_panels_.empty()) menu_panels_.back()->on_down(); }
  void on_left()
  { if(!menu_panels_.empty()) menu_panels_.back()->on_left(); }
  void on_right()
  { if(!menu_panels_.empty()) menu_panels_.back()->on_right(); }
  void on_back()
  { if(!menu_panels_.empty()) menu_panels_.pop_back(); }

  MenuObject* on_confirm()
  {
    if (menu_panels_.empty()) 
      return nullptr;

    MenuObject *next = menu_panels_.back()->on_confirm();

    if ( next == menu_panels_.back())
    {
      if (menu_panels_.back()->get_selected_item()->enabled())
        return menu_panels_.back()->get_selected_item();
    }
    else
    {
      if (next->enabled())
        menu_panels_.push_back(next);
    }
    
    return nullptr;
  }

  /*--------------------------------------------------------
  | Draw the menus
  --------------------------------------------------------*/
  void draw(Vec2i screen_offset);

  /*--------------------------------------------------------
  | Update the manager
  --------------------------------------------------------*/
  void update(MenuObject *mo);

private:
  ModelSpace& space_;

  olc::Sprite* sprGFX_ = nullptr;

  std::list<MenuObject*> menu_panels_;
};