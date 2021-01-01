#include <iostream>
#include <string>
#include <algorithm>
#define OLC_PGE_APPLICATION
#include "ModelSpace.h"


int main()
{
  ModelSpace app;
  if (app.Construct(384, 240, 4, 4))
    app.Start();
  return 0;
}