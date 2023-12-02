#pragma once
#include <glm/glm.hpp>
#include "AGL3Window.hpp"
#include "AGL3Drawable.hpp"
#include "Obstacle.hpp"
#include "ClosestPointTriangle.hpp"

float lastMouseX, lastMouseY, cameraVerticalAngle{0.0f}, cameraHorizontalAngle{0.0f};
bool mouseFirstMove{true};

class Player : AGLDrawable
{
    GLFWwindow* win;
    float deltaTime{0.0f}, lastFrameTime{0.0f};
    static constexpr float mouseSensitivity{0.1f};
    std::vector<GLfloat> vertices;
    float& aspect;
    const float scale{1.0f/18};
    const float radius{1.0f};

public:
    Player(GLFWwindow* _win, float& _aspect) : AGLDrawable(0), win(_win), aspect(_aspect)
    {
        setShaders();
        setSphereBuffers();
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

    void catchMoveKey(std::vector<std::shared_ptr<Obstacle>> obstacles)
    {
        float currentFrame{glfwGetTime()};
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;
        const float cameraSpeed = 0.8f * deltaTime;
        if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        {
            cameraPos += cameraSpeed * cameraTarget;
            if (isCollision(obstacles))
                cameraPos -= cameraSpeed * cameraTarget;
        }
        if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        {
            cameraPos -= cameraSpeed * cameraTarget;
            if (isCollision(obstacles))
                cameraPos += cameraSpeed * cameraTarget;
        }
        checkOutOfBounds();
    }

    bool isCollision(std::vector<std::shared_ptr<Obstacle>> obstacles)
    {
        auto scaledRadius = scale * radius;
        for (auto& obstacle : obstacles)
        {
            for (auto& triangle : obstacle->triangles)
            {
                auto closestPoint = closestPointTriangle(cameraPos, triangle[0], triangle[1], triangle[2]);
                auto distance = glm::length(cameraPos - closestPoint);
                if (distance <= scaledRadius)
                {
                    return true;
                }
            }
        }
        return false;
    }

    void checkOutOfBounds()
    {
        if (cameraPos.x > 1.0f)
            cameraPos.x = 1.0f;
        if (cameraPos.x < -1.0f)
            cameraPos.x = -1.0f;
        
        if (cameraPos.y > 1.0f)
            cameraPos.y = 1.0f;
        if (cameraPos.y < -1.0f)
            cameraPos.y = -1.0f;
        
        if (cameraPos.z > 1.0f)
            cameraPos.z = 1.0f;
        if (cameraPos.z < -1.0f)
            cameraPos.z = -1.0f;
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


        auto model = glm::mat4(1.0f);
        model = glm::translate(model, cameraPos);
        model = glm::scale(model, {scale, scale, scale});

        auto projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        glm::vec3 currColor{1.0, 1.0, 1.0};

        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(4, currColor.x, currColor.y, currColor.z);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        AGLErrors("Player-draw");
    }

    // adapted from http://www.songho.ca/opengl/gl_sphere.html
    void setSphereBuffers()
    {
        bindBuffers();
        const int sectorCount{24}, stackCount{16};
        std::vector<GLfloat> sphereVerts;
        
        float x, y, z, xy;                              // vertex position
        
        float sectorStep = 2 * M_PI / sectorCount;
        float stackStep = M_PI / stackCount;
        float sectorAngle, stackAngle;  
        
        for(int i = 0; i <= stackCount; ++i)
        {
            stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = radius * cosf(stackAngle);             // r * cos(u)
            z = radius * sinf(stackAngle);              // r * sin(u)
        
            // add (sectorCount+1) vertices per stack
            // first and last vertices have same position and normal, but different tex coords
            for(int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi
        
                // vertex position (x, y, z)
                x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                sphereVerts.push_back(x);
                sphereVerts.push_back(y);
                sphereVerts.push_back(z);
            }
        }


        auto getVertAt = [&sphereVerts](int i){
            auto index = 3*i;
            return glm::vec3(sphereVerts[index], sphereVerts[index+1], sphereVerts[index+2]);
        };

        // generate CCW index list of sphere triangles
        // k1--k1+1
        // |  / |
        // | /  |
        // k2--k2+1
        int k1, k2;
        for(int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if(i != 0)
                {
                    auto vert = getVertAt(k1);
                    vertices.push_back(vert.x);
                    vertices.push_back(vert.y);
                    vertices.push_back(vert.z);

                    vert = getVertAt(k2);
                    vertices.push_back(vert.x);
                    vertices.push_back(vert.y);
                    vertices.push_back(vert.z);

                    vert = getVertAt(k1 + 1);
                    vertices.push_back(vert.x);
                    vertices.push_back(vert.y);
                    vertices.push_back(vert.z);
                }

                // k1+1 => k2 => k2+1
                if(i != (stackCount-1))
                {
                    auto vert = getVertAt(k1 + 1);
                    vertices.push_back(vert.x);
                    vertices.push_back(vert.y);
                    vertices.push_back(vert.z);

                    vert = getVertAt(k2);
                    vertices.push_back(vert.x);
                    vertices.push_back(vert.y);
                    vertices.push_back(vert.z);

                    vert = getVertAt(k2 + 1);
                    vertices.push_back(vert.x);
                    vertices.push_back(vert.y);
                    vertices.push_back(vert.z);
                }
            }
        }

        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,                  // attribute 0, must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,//24,             // stride
            (void*)0);          // array buffer offset

    }
};