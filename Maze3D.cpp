#include <stdlib.h>
#include <stdio.h>
#include "MyWin.hpp"

void windowSizeCallback(GLFWwindow* win, int width, int height)
{
   int minSize{std::min(width, height)};
   int xOffset{(width - minSize)/2};
   int yOffset{(height - minSize)/2};
   glViewport(xOffset, yOffset, minSize, minSize);
}

int main(int argc, char *argv[]) {
   MyWin win;
   win.Init(1024, 768, "Maze", 0, 33);
   glfwSetWindowSizeCallback(win.win(), windowSizeCallback);
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
