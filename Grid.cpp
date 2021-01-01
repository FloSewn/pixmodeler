#include "ModelSpace.h"
#include "Grid.h"
#include "Vec2.h"


/***********************************************************
* Constructor
***********************************************************/
Grid::Grid(ModelSpace& space) : space_{space} {}

/***********************************************************
* Function to update the grid
***********************************************************/
void Grid::update()
{
  if (space_.scale() < scale_thresh_)
    spacing_ = 1.0f;
  else 
    spacing_ = 0.5f;
}

/***********************************************************
* Function to draw the grid
***********************************************************/
void Grid::draw()
{
  // Get coordinates of visible screen
  Vec2f top_left;
  Vec2f bottom_right;
  space_.screen_to_coord(0, 0, top_left);
  space_.screen_to_coord(space_.ScreenWidth(), 
                         space_.ScreenHeight(), 
                         bottom_right);

  // Enlarge the screen a bit to prevent clipping
  top_left[0]     = floor(top_left[0]);
  top_left[1]     = floor(top_left[1]);
  bottom_right[0] = ceil(bottom_right[0]);
  bottom_right[1] = ceil(bottom_right[1]);

  // Draw dots
  int sx, sy;
  int ex, ey;
  for (float x = top_left[0]; x < bottom_right[0]; x += spacing_)
  {
    for (float y = top_left[1]; y < bottom_right[1]; y += spacing_)
    {
      space_.coord_to_screen( {x, y}, sx, sy);
      space_.Draw(sx, sy, olc::WHITE);
    }
  }

  // Draw axis
  space_.coord_to_screen( { 0, top_left[1]}, sx, sy);
  space_.coord_to_screen( { 0, bottom_right[1]}, ex, ey);
  space_.DrawLine(sx, sy, ex, ey, olc::GREY, 0xF0F0F0F0);

  space_.coord_to_screen( { top_left[0], 0}, sx, sy);
  space_.coord_to_screen( { bottom_right[0], 0}, ex, ey);
  space_.DrawLine(sx, sy, ex, ey, olc::GREY, 0xF0F0F0F0);


}