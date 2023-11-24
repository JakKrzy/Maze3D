#pragma once
#include <chrono>
#include <AGL3Window.hpp>
#include <stdlib.h>
#include <vector>
#include "Triangle.hpp"
#include "PlayerTriangle.hpp"


class MyWin : public AGLWindow {
    using Verts = std::array<glm::vec2, 3>;
public:
    MyWin() {};
    MyWin(int _wd, int _ht, const char *name, int vers, int fullscr=0)
       : AGLWindow(_wd, _ht, name, vers, fullscr) {};
    
    void MainLoop(int seed, const int N = 10)
    {
        srand(seed);
        ViewportOne(0, 0, wd, ht);

        startTime = std::chrono::system_clock::now();

        do {
            glClear( GL_COLOR_BUFFER_BIT );
            AGLErrors("main-loopbegin");


            AGLErrors("main-afterdraw");
            glfwSwapBuffers(win());
            glfwPollEvents();
        } while (glfwGetKey(win(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
                 glfwWindowShouldClose(win()) == 0 );
    }

    void gameFinish()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        auto endTime{std::chrono::system_clock::now()};
        auto timeMinutes{std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime)};
        auto timeSeconds{
            std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime) -
            std::chrono::seconds(timeMinutes.count()*60)};
        auto timeMilliseconds{
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime) -
            std::chrono::milliseconds(timeMinutes.count()*60000) -
            std::chrono::milliseconds(timeSeconds.count()*1000)};
        printf("Your time (m:s:ms): %ld:%ld:%ld\n", timeMinutes.count(), timeSeconds.count(), timeMilliseconds.count());
    }

private:
   std::chrono::time_point<std::chrono::system_clock> startTime;
};