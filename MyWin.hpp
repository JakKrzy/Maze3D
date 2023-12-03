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
       : AGLWindow(_wd, _ht, name, vers, fullscr) {}
    
    void MainLoop(int seed, const int N = 10)
    {
        srand(seed);
        ViewportOne(0, 0, wd, ht);
        glfwSetWindowSizeCallback(win(), MyWin::windowSizeCallback);
        glfwSetInputMode(win(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        float scale = 1.0f / (2 * (float)N);

        bool gameFinishFlag{false};

        Player player(win(), aspect, gameFinishFlag);
        glfwSetCursorPosCallback(win(), Player::mouseCallback);

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
                    if (not (x_index == 0 and y_index == 0 and z_index == 0))
                    {
                        auto isFinish = x_index == N-1 and y_index == N-1 and z_index == N-1;
                        auto isTrap = (rand() % 100) < 15;
                        auto o = std::make_shared<Obstacle>(
                            glm::vec3(curr_x, curr_y, curr_z),
                            glm::vec3(rand() % 360, rand() % 360, rand() % 360),
                            scale,
                            aspect,
                            player.cameraPos,
                            isTrap,
                            isFinish);
                        obstacles.push_back(o);
                    }
                    curr_z += diff;
                }
                curr_y += diff;
            }
            curr_x += diff;
        }

        startTime = std::chrono::system_clock::now();
        glm::vec3 minimapCameraPos{-3.0, -3.0, -3.0};

        auto getClosestObstacles = [&obstacles](){
            const int numOfObstacles{obstacles.size() < 9 ? obstacles.size() : 9};
            std::sort(
                obstacles.begin(),
                obstacles.end(),
                [](std::shared_ptr<Obstacle> o1, std::shared_ptr<Obstacle> o2){
                    return o1->getDistanceFromPlayer() < o2->getDistanceFromPlayer();
                });
            std::vector<std::shared_ptr<Obstacle>> res;
            for (auto i{0u}; i < numOfObstacles; i++)
                res.push_back(obstacles[i]);
            return res;
        };

        do {
            glEnable(GL_DEPTH_TEST);
            glLineWidth(2.0f);
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, wd, ht);
            
            player.catchCamKey();
            player.catchMoveKey(getClosestObstacles());

            // Draw main view
            auto view{player.getViewMatrix()};
            for (auto& o : obstacles) 
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                o->draw(view);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                o->draw(view, true);
            }

            // Draw minimap
            auto p = player.getCenter();
            minimapCameraPos = {
                p.x > 0.0f ? 3.0f : -3.0f,
                // p.y > 0.0f ? 3.0f : -3.0f,
                3.0f,
                p.z > 0.0f ? 3.0f : -3.0f};

            auto minimapView = glm::lookAt(
                minimapCameraPos,
                glm::vec3{0.0f, 0.0f, 0.0f},
                glm::vec3{0.0f, 1.0f, 0.0f});
            glViewport(2*(wd/3), 0, wd/3, ht/3);
            glClear(GL_DEPTH_BUFFER_BIT);
            glLineWidth(0.7f);
            for (auto& o : obstacles)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                o->draw(minimapView);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                player.draw(minimapView);
            }

            if (gameFinishFlag)
            {
                gameFinish();
                break;
            }

            glfwSwapBuffers(win());
            glfwPollEvents();
        } while (glfwGetKey(win(), GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
                 glfwWindowShouldClose(win()) == 0 );
    }

    void gameFinish()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto endTime{std::chrono::system_clock::now()};
        auto timeMinutes{std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime)};
        auto timeSeconds{
            std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime) -
            std::chrono::seconds(timeMinutes.count()*60)};
        auto timeMilliseconds{
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime) -
            std::chrono::milliseconds(timeMinutes.count()*60000) -
            std::chrono::milliseconds(timeSeconds.count()*1000)};
        printf("You win!\n");
        printf("Your time (m:s:ms): %ld:%ld:%ld\n", timeMinutes.count(), timeSeconds.count(), timeMilliseconds.count());
    }

    static void windowSizeCallback(GLFWwindow* win, int width, int height)
    {
        void* ptr = glfwGetWindowUserPointer(win);
        if (AGLWindow* window = static_cast<AGLWindow*>(ptr))
        {
            window->wd = width;
            window->ht = height;
            window->aspect = (float)width / (float)height;
        }
        glViewport(0, 0, width, height);
    }

private:
    std::chrono::time_point<std::chrono::system_clock> startTime;
};