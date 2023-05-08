/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Brian Chen
 * Additional Authors: -
 */
 

#include <glew.h>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <gl/GLU.h>

#include "SICTypes.h"
#include "SICEngine.h"

#include "Shape.h"
#include "Transform.h"
#include <ext.hpp>
#include <unordered_map>

namespace sic
{
    const float PI = 3.14159f;
    const float rad = PI / 180.0f;

    void Shape::DrawVAO()
    {
        CHECK_GL_ERROR;
        glBindVertexArray(vaoID);
        CHECK_GL_ERROR;
        glDrawElements(GL_TRIANGLES, 3 * count, GL_UNSIGNED_INT, 0);
        CHECK_GL_ERROR;
        glBindVertexArray(0);
    }

    float Box2D_vertices[] = {
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f,  // top right
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f   // top left 
    };

    unsigned int Box2D_indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    float Arrow2D_vertices[] = {
     0.0f, -0.5f,  0.0f,  // arrow bot
     0.0f,  0.5f,  0.0f,  // arrow top
    -0.3f, 0.25f,  0.0f,   // arrow middle left
     0.0f,  0.5f,  0.0f,   // arrow top
     0.3f, 0.25f,  0.0f,   // arrow middle right
     0.0f,  0.5f,  0.0f    // arrow top
    };

    Box2D::Box2D()
    {
        unsigned int VAO;
        GLuint VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // 2. copy our vertices array in a vertex buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Box2D_vertices), Box2D_vertices, GL_STATIC_DRAW);
        // 3. copy our index array in a element buffer for OpenGL to use
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Box2D_indices), Box2D_indices, GL_STATIC_DRAW);
        // 4. then set the vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        count = 2;
        vaoID = VAO;
    }

    Arrow2D::Arrow2D()
    {
        unsigned int VAO;
        GLuint VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Arrow2D_vertices), Arrow2D_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        count = 3;
        vaoID = VAO;
    }

    void Arrow2D::DrawVAO()
    {
        CHECK_GL_ERROR;
        glBindVertexArray(vaoID);
        CHECK_GL_ERROR;
        glDrawArrays(GL_LINES, 0, 2 * count);
        CHECK_GL_ERROR;
        glBindVertexArray(0);
    }

    Font::Font() : str("test"), x(0.0f), y(0.0f), scale(1.0f), color(vec3(0.0f))
    {
        // gen
        glGenVertexArrays(1, &vaoID);
        glGenBuffers(1, &vboID);

        // bind
        glBindVertexArray(vaoID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        // unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }

    void Font::DrawVAO()
    {             
        glBindVertexArray(vaoID);

        auto const *Characters = System::Resources().GetCharMap();

        for (int i = 0; i < std::strlen(str); ++i)
        {
            ResourceManager::Character ch = Characters->at(str[i]);

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, vboID);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}