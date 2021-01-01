#include "ModelSpace.h"
#include "Cursor.h"
#include "Vec2.h"

/***********************************************************
* Function to update the cursor
***********************************************************/
void Cursor::update()
{
  Vec2f& mouse_coords = space_.mouse_coords(); 
  float spacing = space_.grid().spacing();
  coords_[0] = spacing * round(mouse_coords[0]/spacing);
  coords_[1] = spacing * round(mouse_coords[1]/spacing);
}


/***********************************************************
* Function to draw the cursor
***********************************************************/
void Cursor::draw()
{
  int sx, sy;
  space_.coord_to_screen(coords_, sx, sy);
  space_.DrawCircle(sx, sy, size_, olc::YELLOW);
}