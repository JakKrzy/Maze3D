#pragma once

#include <AGL3Drawable.hpp>
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <GLFW/glfw3.h> 
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

using Triangle = std::array<glm::vec3, 3>;

class Obstacle : public AGLDrawable {
public:
    Obstacle(glm::vec3 _center, glm::vec3 angles, float& _aspect, const glm::vec3& plPos)
        : AGLDrawable(0), aspect(_aspect), playerPos(plPos)
    {
        setShaders();
        setBuffers();

        setCenter(_center);
        setAngles(angles.x, angles.y, angles.z);

        calcModelMatrix();
        calcVertices();
    }

    void setAngles(const float x, const float y, const float z)
    {
        x_angle = x;
        y_angle = y;
        z_angle = z;
    }

    void setCenter(const glm::vec3 _center)
    {
        center = _center;
        color = {0.4*center.x+0.6, 0.01*center.y, 0.5*center.z+0.7};

    }

    void calcModelMatrix()
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, center);
        model = glm::rotate(model, glm::radians(x_angle), {1.0, 0.0, 0.0});
        model = glm::rotate(model, glm::radians(y_angle), {0.0, 1.0, 0.0});
        model = glm::rotate(model, glm::radians(z_angle), {0.0, 0.0, 1.0});
        model = glm::scale(model, {scale, scale, scale});
    }

    void calcVertices()
    {
        for (auto& v : vertices)
        {
            auto u = model * glm::vec4(v, 1.0);
            v.x = u.x;
            v.y = u.y;
            v.z = u.z;
        }
        for (auto i{0u}; i < 4; i++)
        {
            triangles[i][0] = vertices[i];
            triangles[i][1] = vertices[(i + 1) % 4];
            triangles[i][2] = vertices[(i + 2) % 4];
        }
    }
    // void setScale(const int gridSize) { scale = 1.0 / (1.2 * gridSize); }

    void setShaders() { compileShadersFromFile("Obstacle.vesh", "Obstacle.frsh"); }
    
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

    void draw(glm::mat4 view, bool border=false)
    {
        bindProgram();
        bindBuffers();

        auto projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), aspect, 0.01f, 100.0f);
        glm::vec3 currColor{1.0, 1.0, 1.0};

        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(projection));
        if (not border) { currColor = color; }
        glUniform3f(4, currColor.x, currColor.y, currColor.z);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
        AGLErrors("Obstacle-draw");
    }

    float getDistanceFromPlayer()
    {
        return glm::length(center - playerPos);
    }

// protected:

    float scale{1.0/18};
    float x_angle{0.0}, y_angle{0.0}, z_angle{0.0};
    glm::mat4 model;
    glm::vec3 color{1.0, 1.0, 1.0};
    glm::vec3 center{0.0, 0.0, 0.0};
    std::array<glm::vec3, 6> vertices{
        glm::vec3{-1.0f, -1.0f, -1.0f},
        glm::vec3{-1.0f,  1.0f,  1.0f},
        glm::vec3{ 1.0f,  1.0f, -1.0f},
        glm::vec3{ 1.0f, -1.0f,  1.0f}};
    std::array<Triangle, 4> triangles;
    bool isFirstDraw{true};
    float& aspect;
    const glm::vec3& playerPos;
};