#pragma once
#include <glm/glm.hpp>
#include "AGL3Window.hpp"

float lastMouseX, lastMouseY, cameraVerticalAngle, cameraHorizontalAngle;
bool mouseFirstMove;

class Player
{
    GLFWwindow* win;
    float deltaTime{0.0f}, lastFrameTime{0.0f};
    static float lastY, lastX;
    static constexpr float mouseSensitivity{0.1f};
public:
    Player(GLFWwindow* _win) : win(_win) {}
    glm::vec3 cameraPos{0.0f, 0.0f, 3.0f};
    glm::vec3 cameraTarget{0.0f, 0.0f, -1.0f};
    const float cameraTargetRadius{3.0f};
    const glm::vec3 up{0.0f, 1.0f, 0.0f};

    glm::mat4 getViewMatrix() 
    {
        cameraTarget.x = glm::cos(glm::radians(cameraHorizontalAngle)) * glm::cos(glm::radians(cameraVerticalAngle));
        cameraTarget.y = glm::sin(glm::radians(cameraVerticalAngle));
        cameraTarget.z = glm::sin(glm::radians(cameraHorizontalAngle)) * glm::cos(glm::radians(cameraVerticalAngle));
        cameraTarget = glm::normalize(cameraTarget);
        return glm::lookAt(cameraPos, cameraPos + cameraTarget, up);
    }

    void catchCamKey()
    {
        if (glfwGetKey(win, GLFW_KEY_LEFT ) == GLFW_PRESS)
        {
            cameraHorizontalAngle-=1.5f;
        } 
        else if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            cameraHorizontalAngle+=1.5f;
        }
        if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
        {
            cameraVerticalAngle+=1.5f;
            if (cameraVerticalAngle > 89.0f) cameraVerticalAngle = 89.0f;   
        }
        else if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            cameraVerticalAngle-=1.5f;
            if (cameraVerticalAngle < -89.0f) cameraVerticalAngle = -89.0f;
        }
    }

    void catchMoveKey()
    {
        float currentFrame{glfwGetTime()};
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;
        const float cameraSpeed = 0.8f * deltaTime;
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraTarget;
        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraTarget;
    }

    static void mouseCallback(GLFWwindow* win, double xpos, double ypos)
    {
        constexpr float mouseSensitivity{0.1f};
        if (mouseFirstMove)
        {
            lastMouseX = xpos;
            lastMouseY = ypos;
            mouseFirstMove = false;
            return;
        }
        float xOffset = (xpos - lastMouseX) * mouseSensitivity;
        float yOffset = (lastMouseY - ypos) * mouseSensitivity;
        lastMouseX = xpos;
        lastMouseY = ypos;        

        cameraVerticalAngle += yOffset;
        cameraHorizontalAngle += xOffset;

        if (cameraVerticalAngle > 89.0f) cameraVerticalAngle = 89.0f;
        if (cameraVerticalAngle < -89.0f) cameraVerticalAngle = -89.0f;
    }

};