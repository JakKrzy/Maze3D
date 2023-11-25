#pragma once
#include <glm/glm.hpp>
#include "AGL3Window.hpp"

class Player
{
    GLFWwindow* win;
public:
    Player(GLFWwindow* _win) : win(_win) {}
    glm::vec3 cameraPos{0.0f, 0.0f, 3.0f};
    glm::vec3 cameraTarget{0.0f, 0.0f, -1.0f};
    const float cameraTargetRadius{3.0f};
    float verticalAngle{0.0f}, horizontalAngle{0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

    inline glm::mat4 getViewMatrix() const { return glm::lookAt(cameraPos, cameraPos + cameraTarget, up); }

    void catchCamKey()
    {
        if (glfwGetKey(win, GLFW_KEY_LEFT ) == GLFW_PRESS)
        {
            horizontalAngle-=1.0f;
        } 
        else if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            horizontalAngle+=1.0f;
        }
        if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
        {
            verticalAngle+=1.0f;
            if (verticalAngle > 89.0f) verticalAngle = 89.0f;      
        }
        else if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            verticalAngle-=1.0f;
            if (verticalAngle < -89.0f) verticalAngle = -89.0f;
        }
        cameraTarget.x = glm::cos(glm::radians(horizontalAngle)) * glm::cos(glm::radians(verticalAngle));
        cameraTarget.y = glm::sin(glm::radians(verticalAngle));
        cameraTarget.z = glm::sin(glm::radians(horizontalAngle)) * glm::cos(glm::radians(verticalAngle));
        cameraTarget = glm::normalize(cameraTarget);
    }

    void catchMoveKey()
    {
        const float cameraSpeed = 0.01f;
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraTarget;
        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraTarget;
    }

};