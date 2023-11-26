#include <stdlib.h>
#include <stdio.h>
#include "MyWin.hpp"

int main(int argc, char *argv[])
{
   MyWin win;
   win.Init(1600, 900, "Maze", 0, 33);
   glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glEnable(GL_LINE_SMOOTH);
   glLineWidth(2.0);
   
   if (argc > 1)
   {
      if (argc > 2)
         win.MainLoop(atoi(argv[1]), atoi(argv[2]));
      else
         win.MainLoop(atoi(argv[1]));
   }
   else win.MainLoop(0);
   return 0;
}
