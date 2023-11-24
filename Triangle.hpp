#pragma once

#include <AGL3Drawable.hpp>
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <GLFW/glfw3.h> 
#include <sstream>

class Triangle : public AGLDrawable {
public:
    Triangle() : AGLDrawable(0)
    {
        setShaders();
        setBuffers();
    }
    void setAngle(const float a) { angle = a; }
    void setCoords(const float _x, const float _y) { x = _x; y = _y; }
    void setScale(const int gridSize) { scale = 1.0 / (1.2 * gridSize); }

    virtual void setShaders() { compileShadersFromFile("Triangle.vertexshader", "Triangle.fragmentshader"); }
    
    void setBuffers()
    {
        bindBuffers();
        GLfloat vert[3][2]{
            {-0.2, -1},
            { 0.2, -1},
            {   0,  1}};

        glBufferData(GL_ARRAY_BUFFER, 3*2*sizeof(float), vert, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,                  // attribute 0, must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,//24,             // stride
            (void*)0);          // array buffer offset
    }

    virtual void draw()
    {
        if (isFirstDraw)
        {
            calcVertices();
            isFirstDraw = false;
        }

        bindProgram();
        bindBuffers();

        glUniform1f(1, angle);
        glUniform2f(2, x, y);
        glUniform1f(3, scale);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        AGLErrors("Triangle-draw");
    }

    // Copied from:
    // https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle 
    static bool isPointIn(glm::vec2 point, std::array<glm::vec2, 3> verts)
    {
        auto sign = [](glm::vec2 v1, glm::vec2 v2, glm::vec2 v3)
        {
            return (v1.x - v3.x) * (v2.y - v3.y) - (v2.x - v3.x) * (v1.y - v3.y);
        };

        auto d1 = sign(point, verts[0], verts[1]);
        auto d2 = sign(point, verts[1], verts[2]);
        auto d3 = sign(point, verts[2], verts[0]);

        bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    }

    glm::vec2 getCenter() const { return {x, y}; }

    std::string toString()
    {
        std::stringstream ss;
        ss << "{";
        for (auto vert : vertices)
            ss << "(" << vert[0] << ", " << vert[1] << ")";
        ss << "}";
        return ss.str();
    }

    std::array<glm::vec2, 3> getVerts() { calcVertices(); return vertices; }

protected:
    void calcVertices()
    {
        auto trans = glm::mat4(1.0f);
        trans = glm::scale(trans, glm::vec3(scale, scale, 1.0));
        trans = glm::rotate(trans, angle, glm::vec3(0.0, 0.0, 1.0));

        for (glm::vec2& vert : vertices)
        {
            auto resVert = trans * glm::vec4(vert, 0.0, 1.0);
            resVert += glm::vec4(x, y, 0.0, 0.0);
            vert[0] = resVert[0]; vert[1] = resVert[1];
        }
    }

    float scale{1.0/16};
    std::array<glm::vec2, 3> defaultVertices{glm::vec2(-0.2, -1), { 0.2, -1}, { 0.0,  1}};

    GLfloat circle_color[3] = { 0.0, 1.0, 0.0 };
    float angle{0.0}, x{0.0}, y{0.0};
    std::array<glm::vec2, 3> vertices = defaultVertices;
    bool isFirstDraw{true};
};