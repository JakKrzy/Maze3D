#pragma once

#include <AGL3Drawable.hpp>
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <GLFW/glfw3.h> 
#include <glm/gtc/type_ptr.hpp>

class Obstacle : public AGLDrawable {
public:
    Obstacle(glm::vec3 _center, glm::vec3 angles) : AGLDrawable(0)
    {
        setShaders();
        setBuffers();

        setCenter(_center);
        setAngles(angles.x, angles.y, angles.z);

        model = glm::mat4(1.0f);
        model = glm::translate(model, center);
        model = glm::rotate(model, glm::radians(x_angle), {1.0, 0.0, 0.0});
        model = glm::rotate(model, glm::radians(y_angle), {0.0, 1.0, 0.0});
        model = glm::rotate(model, glm::radians(z_angle), {0.0, 0.0, 1.0});
        model = glm::scale(model, {scale, scale, scale});
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
    // void setScale(const int gridSize) { scale = 1.0 / (1.2 * gridSize); }

    virtual void setShaders() { compileShadersFromFile("Obstacle.vesh", "Obstacle.frsh"); }
    
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
        projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
        glm::vec3 currColor{1.0, 1.0, 1.0};

        glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(projection));
        if (not border) { currColor = color; }
        glUniform3f(4, currColor.x, currColor.y, currColor.z);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
        AGLErrors("Obstacle-draw");
    }

// protected:

    float scale{1.0/18};
    float x_angle{0.0}, y_angle{0.0}, z_angle{0.0};
    glm::mat4 model;
    glm::vec3 color{1.0, 1.0, 1.0};
    glm::vec3 center{0.0, 0.0, 0.0};
    bool isFirstDraw{true};
};