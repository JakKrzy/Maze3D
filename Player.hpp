#pragma once
#include <glm/glm.hpp>
#include "AGL3Window.hpp"
#include "AGL3Drawable.hpp"

float lastMouseX, lastMouseY, cameraVerticalAngle{0.0f}, cameraHorizontalAngle{0.0f};
bool mouseFirstMove{true};

class Player : AGLDrawable
{
    GLFWwindow* win;
    float deltaTime{0.0f}, lastFrameTime{0.0f};
    static float lastY, lastX;
    static constexpr float mouseSensitivity{0.1f};
public:
    Player(GLFWwindow* _win) : AGLDrawable(0), win(_win)
    {
        setShaders();
        setBuffers();
    }
    glm::vec3 cameraPos{-1.0f, -1.0f, -1.0f};
    glm::vec3 cameraTarget{0.0f, 0.0f, -1.0f};
    const float cameraTargetRadius{3.0f};
    const glm::vec3 up{0.0f, 1.0f, 0.0f};

    inline glm::vec3 getCenter() const { return cameraPos; } 

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

    void setShaders() { compileShadersFromFile("Player.vesh", "Player.frsh"); }
    void setBuffers()
    {
        bindBuffers();
        GLfloat vert[6][3]{
            {-1, -1, -1},
            {-1, 1, 1},
            { 1, 1, -1},
            {1, -1, 1},
            {-1, -1, -1},
            {-1, 1, 1}};

        glBufferData(GL_ARRAY_BUFFER, 6*3*sizeof(float), vert, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,                  // attribute 0, must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,//24,             // stride
            (void*)0);          // array buffer offset
    }

    void draw(glm::mat4 view)
    {
        bindProgram();
        bindBuffers();

        constexpr float scale{1.0f/18};

        auto model = glm::mat4(1.0f);
        model = glm::translate(model, cameraPos);
        model = glm::scale(model, {scale, scale, scale});

        auto projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
        glm::vec3 currColor{1.0, 1.0, 1.0};

        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(4, currColor.x, currColor.y, currColor.z);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
        AGLErrors("Player-draw");
    }
};