#pragma once
#include "Triangle.hpp"
#include "cmath"
#include <algorithm>
#include <vector>


class PlayerTriangle : public Triangle
{
    using Verts = std::array<glm::vec2, 3>;
public:
    enum class CollisionType : uint8_t { noCollision, collision, finishCollision };

    PlayerTriangle(GLFWwindow* _win, const int _N)
        : Triangle()
        , win(_win)
        , N(_N)
    {
        this->setShaders();
    }

    void setShaders() override { compileShadersFromFile("PlayerTriangle.vertexshader", "Triangle.fragmentshader"); }
    
    void draw() override
    {
        calcVertices();
        bindProgram();
        bindBuffers();

        glUniform1f(1, angle);
        glUniform2f(2, x, y);
        glUniform1f(3, scale);

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    CollisionType catchKeyDown(Verts* triCoords)
    {
        if (glfwGetKey(win, GLFW_KEY_LEFT ) == GLFW_PRESS)
        {
            auto newAngle = angle + 0.1;
            if (auto c = doesMoveCreateCollision(x, y, newAngle, triCoords);
                    c != CollisionType::noCollision) return c;
            angle = newAngle;
        } 
        else if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            auto newAngle = angle - 0.1;
            if (auto c = doesMoveCreateCollision(x, y, newAngle, triCoords);
                    c != CollisionType::noCollision) return c;
            angle = newAngle;
        }
        else if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
        {
            auto newx = x + cos((M_PI / 2) + angle) / 100;
            auto newy = y + sin((M_PI / 2) + angle) / 100;
            if (auto c = doesMoveCreateCollision(newx, newy, angle, triCoords);
                    c != CollisionType::noCollision) return c;
            x = newx;
            y = newy;
        }
        else if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            auto newx = x - cos((M_PI / 2) + angle) / 100;
            auto newy = y - sin((M_PI / 2) + angle) / 100;
            if (auto c = doesMoveCreateCollision(newx, newy, angle, triCoords);
                    c != CollisionType::noCollision) return c;
            x = newx;
            y = newy;
        }
        return CollisionType::noCollision;
    }

    CollisionType doesMoveCreateCollision(float newX, float newY, float newAngle, Verts* triCoords)
    {
        auto trans = glm::mat4(1.0f);
        trans = glm::scale(trans, glm::vec3(scale, scale, 1.0));
        trans = glm::rotate(trans, newAngle, glm::vec3(0.0, 0.0, 1.0));
        for (auto i{0u}; i < N*N; i++)
        {
            if (i == (N-1) * N) continue;
            Verts verts = triCoords[i];
            std::array<glm::vec2, 3> playerNewVertices;
            auto j{0u};
            for (glm::vec2 vert : defaultVertices)
            {
                auto newVert = trans * glm::vec4(vert, 0.0, 1.0);
                newVert += glm::vec4(newX, newY, 0.0, 0.0);
                playerNewVertices[j] = {newVert[0], newVert[1]};
                if (Triangle::isPointIn({newVert[0], newVert[1]}, verts))
                {
                    return i == N - 1 ? CollisionType::finishCollision : CollisionType::collision;
                }
                j++;
            }
            for (auto vert : verts)
            {
                if (Triangle::isPointIn(vert, playerNewVertices))
                {
                    return i == N - 1 ? CollisionType::finishCollision : CollisionType::collision;
                }
            }
        }
        return CollisionType::noCollision;
        AGLErrors("collision-detection");
    }

private:
    GLFWwindow* win;
    const int N;
};