#include "ModelSpace.h"
#include "Menu.h"
#include "Vec2.h"
#include "olc_pixel_game_engine.h"

#include <string>
#include <unordered_map>


/***********************************************************
* Function to build the menu object structure recursively
***********************************************************/
void MenuObject::build()  
{
  for (auto& m : items_)
  {
    if (m.has_children())
      m.build();
    
    cell_size_[0] = std::max(m.size()[0], cell_size_[0]);
    cell_size_[1] = std::max(m.size()[1], cell_size_[1]);
  }

  table_size_[0] = 2 + table_dimension_[0] * cell_size_[0]
                  + (table_dimension_[0]-1) * cell_padding_[0];
  table_size_[1] = 2 + table_dimension_[1] * cell_size_[1]
                  + (table_dimension_[1]-1) * cell_padding_[1];
  total_rows_ = (items_.size() / table_dimension_[0])
              + (((items_.size() % table_dimension_[0]) > 0) ? 1 : 0);
}

/***********************************************************
* Function to clamp the cursor position within the visible
* menu space 
***********************************************************/
void MenuObject::clamp_cursor()
{
  cursor_item_ = cell_cursor_[1] * table_dimension_[0]
                + cell_cursor_[0];
  
  if (cursor_item_ >= int(items_.size()))
  {
    cell_cursor_[0] = (items_.size() % table_dimension_[0]) - 1;
    cell_cursor_[1] = (items_.size() / table_dimension_[0]);
    cursor_item_ = items_.size() - 1;
  }
}

/***********************************************************
* Function that is applied to the menu as the up button is 
* pushed
***********************************************************/
void MenuObject::on_up()
{
  cell_cursor_[1]--;
  
  if (cell_cursor_[1] < 0)
    cell_cursor_[1] = 0;

  if (cell_cursor_[1] < top_visible_row_)
  {
    top_visible_row_--;
    if (top_visible_row_ < 0)
      top_visible_row_ = 0;
  }

  clamp_cursor();
}

/***********************************************************
* Function that is applied to the menu as the down button is 
* pushed
***********************************************************/
void MenuObject::on_down()
{
  cell_cursor_[1]++;
  
  if (cell_cursor_[1] == total_rows_)
    cell_cursor_[1] = total_rows_ - 1;

  int lower = top_visible_row_ + table_dimension_[1] - 1;

  if (cell_cursor_[1] > lower)
  {
    top_visible_row_++;
    int limit = total_rows_-table_dimension_[1];
    if (top_visible_row_ > limit)
      top_visible_row_ = limit;
  }

  clamp_cursor();
}

/***********************************************************
* Function that is applied to the menu as the right button  
* is pushed
***********************************************************/
void MenuObject::on_right()
{
  cell_cursor_[0]++;
  if (cell_cursor_[0] == table_dimension_[0])
    cell_cursor_[0] = table_dimension_[0] - 1;

  clamp_cursor();
}

/***********************************************************
* Function that is applied to the menu as the left button  
* is pushed
***********************************************************/
void MenuObject::on_left()
{
  cell_cursor_[0]--;
  if (cell_cursor_[0] < 0)
    cell_cursor_[0] = 0;

  clamp_cursor();
}

/***********************************************************
* Function that is applied to the menu as the confirm button  
* is pushed
***********************************************************/
MenuObject* MenuObject::on_confirm()
{
  if(items_[cursor_item_].has_children())
    return &items_[cursor_item_];
  
  return this;
}

/***********************************************************
* Function to draw a menu object
***********************************************************/
void MenuObject::draw(olc::Sprite* sprGFX,
                      Vec2i screen_offset)
{
  // Record current pixel mode
  olc::Pixel::Mode cur_pixel_mode = space_.GetPixelMode();
  space_.SetPixelMode(olc::Pixel::MASK);

  // Draw menu boundary
  Vec2i patch_pos = { 0, 0};
  for (patch_pos[0] = 0; patch_pos[0] < table_size_[0]; patch_pos[0]++)
  {
    for (patch_pos[1] = 0; patch_pos[1] < table_size_[1]; patch_pos[1]++)
    {
      // Position in screenspace
      Vec2i screen_loc = patch_pos * patch_unit_ + screen_offset;

      // Choose correct patch
      olc::vi2d source_patch = { 0, 0 };
      if (patch_pos[0] > 0 )                   source_patch.x = 1;
      if (patch_pos[0] == table_size_[0] - 1 ) source_patch.x = 2;
      if (patch_pos[1] > 0 )                   source_patch.y = 1;
      if (patch_pos[1] == table_size_[1] - 1 ) source_patch.y = 2;

      // Draw actual patch
      space_.DrawPartialSprite({screen_loc[0], screen_loc[1]},
                            sprGFX, source_patch * patch_unit_, 
                            patch_size_);
    }
  }

  // Draw Panel contents
  int top_left = top_visible_row_ * table_dimension_[0];
  int bottom_right = top_left 
                    + table_dimension_[0] * table_dimension_[1];

  // Clamp to size of child item vector
  bottom_right = std::min(int(items_.size()), bottom_right);
  int visible_items = bottom_right - top_left;


  // Draw scroll markers
  if (top_visible_row_ > 0)
  {
    patch_pos[0] = table_size_[0] - 2;
    patch_pos[1] = 0;
    olc::vi2d screen_loc = { patch_pos[0] * patch_unit_ + screen_offset[0], 
                              patch_pos[1] * patch_unit_ + screen_offset[1] };
    olc::vi2d source_patch = { 3, 0 };
    space_.DrawPartialSprite(screen_loc, sprGFX, 
                          source_patch * patch_unit_, patch_size_);
  }
  if ((total_rows_-top_visible_row_) > table_dimension_[1])
  {
    patch_pos[0] = table_size_[0] - 2;
    patch_pos[1] = table_size_[1] - 1;
    olc::vi2d screen_loc = { patch_pos[0] * patch_unit_ + screen_offset[0], 
                              patch_pos[1] * patch_unit_ + screen_offset[1] };
    olc::vi2d source_patch = { 3, 2 };
    space_.DrawPartialSprite(screen_loc, sprGFX, 
                          source_patch * patch_unit_, patch_size_);
  }

  // Draw visible items
  Vec2i index     = { 0, 0 };
  for (int i = 0; i < visible_items; i++)
  {
    index[0] = i % table_dimension_[0];
    index[1] = i / table_dimension_[0];

    patch_pos[0] =  index[0] * (cell_size_[0] + cell_padding_[0]) + 1; 
    patch_pos[1] =  index[1] * (cell_size_[1] + cell_padding_[1]) + 1; 

    // Actual location in pixels
    olc::vi2d screen_loc = { patch_pos[0] * patch_unit_ + screen_offset[0], 
                              patch_pos[1] * patch_unit_ + screen_offset[1] };

    // Display header 
    space_.DrawString(screen_loc, items_[top_left+i].name(),
                    items_[top_left+i].enabled() ? olc::WHITE : olc::DARK_GREY);

    // Display indicator that panel has sub panel
    if (items_[top_left+i].has_children())
    {
      patch_pos[0] =  index[0] * (cell_size_[0] + cell_padding_[0]) + 1 + cell_size_[0];
      patch_pos[1] =  index[1] * (cell_size_[1] + cell_padding_[1]) + 1; 

      screen_loc = { patch_pos[0] * patch_unit_ + screen_offset[0], 
                      patch_pos[1] * patch_unit_ + screen_offset[1] };

      space_.DrawPartialSprite(screen_loc, sprGFX, 
                            {3 * patch_unit_, 1 * patch_unit_}, 
                            patch_size_);
    }
  }

  // Calculate cursor position in screen space in case system draws it
  cursor_position_[0] = (cell_cursor_[0] * (cell_size_[0]+cell_padding_[0])) * patch_unit_
                      + screen_offset[0] - patch_unit_;
  cursor_position_[1] = ((cell_cursor_[1]-top_visible_row_) * (cell_size_[1]+cell_padding_[1])) * patch_unit_
                      + screen_offset[1] + patch_unit_;

  // Reset pixel mode
  space_.SetPixelMode(cur_pixel_mode);
}



/***********************************************************
* Function to handle drawing of all menu objects through
* the menu manager
***********************************************************/
void MenuManager::draw(Vec2i screen_offset)
{
  if (menu_panels_.empty()) return;
  
  // Draw visible menu system
  for (auto &p : menu_panels_)
  {
    p->draw(sprGFX_, screen_offset);
    screen_offset += 10;
  }

  // Draw cursor
  MenuObject* top_menu = menu_panels_.back();
  int patch_unit = top_menu->patch_unit();
  olc::vi2d cursor_pos = { top_menu->cursor_position()[0],
                            top_menu->cursor_position()[1] };

  olc::Pixel::Mode cur_pixel_mode = space_.GetPixelMode();
  space_.SetPixelMode(olc::Pixel::ALPHA);
  space_.DrawPartialSprite(cursor_pos, sprGFX_, 
                        olc::vi2d(4,0 ) * patch_unit,
                        {2*patch_unit, 2*patch_unit});
  space_.SetPixelMode(cur_pixel_mode);
}

/***********************************************************
* Function to update the menu manager
***********************************************************/
void MenuManager::update(MenuObject *mo)
{
  MenuObject *command = nullptr;

  if (space_.GetKey(olc::Key::M).bPressed) 
    Open(mo);
  if (space_.GetKey(olc::Key::UP).bPressed) 
    on_up();
  if (space_.GetKey(olc::Key::DOWN).bPressed) 
    on_down();
  if (space_.GetKey(olc::Key::LEFT).bPressed) 
    on_left();
  if (space_.GetKey(olc::Key::RIGHT).bPressed) 
    on_right();
  if (space_.GetKey(olc::Key::BACK).bPressed) 
    on_back();
  if (space_.GetKey(olc::Key::ENTER).bPressed) 
    command = on_confirm();

  if (command != nullptr)
  {
    command->callback();
    Close();
  }

}