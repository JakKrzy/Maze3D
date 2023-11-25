#pragma once
#include <chrono>
#include <AGL3Window.hpp>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <memory>
#include "Obstacle.hpp"
#include "Player.hpp"


class MyWin : public AGLWindow {
public:
    MyWin() {};
    MyWin(int _wd, int _ht, const char *name, int vers, int fullscr=0)
       : AGLWindow(_wd, _ht, name, vers, fullscr) {};
    
    void MainLoop(int seed, const int N = 10)
    {
        srand(seed);
        ViewportOne(0, 0, wd, ht);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(2.0);


        std::vector<std::shared_ptr<Obstacle>> obstacles;
        obstacles.reserve(N*N*N);

        float diff{2.0f / N};
        float curr_x{-1.0f + diff / 2};
        for (int x_index{0}; x_index < N; x_index++)
        {
            float curr_y{-1.0f + diff / 2};
            for (int y_index{0}; y_index < N; y_index++)
            {
                float curr_z{-1.0f + diff / 2};
                for (int z_index{0}; z_index < N; z_index++)
                {
                    auto o = std::make_shared<Obstacle>(
                        glm::vec3(curr_x, curr_y, curr_z),
                        glm::vec3(rand() % 360, rand() % 360, rand() % 360));
                    obstacles.push_back(o);
                    curr_z += diff;
                }
                curr_y += diff;
            }
            curr_x += diff;
        }

        Player player(win());

        startTime = std::chrono::system_clock::now();

        auto radius = 3.0f;
        do {
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            AGLErrors("main-loopbegin");
            // player.cameraPos.x = sin(glfwGetTime()) * radius;
            // player.cameraPos.y = sin(glfwGetTime());
            // player.cameraPos.z = cos(glfwGetTime()) * radius;
            auto view{player.getViewMatrix()};
            for (auto& o : obstacles) 
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                o->draw(view);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                o->draw(view, true);
            }

            player.catchCamKey();
            player.catchMoveKey();

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