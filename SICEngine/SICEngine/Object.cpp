/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#include "math.h"
#include <fstream>
#include <stdlib.h>

#include <glew.h>
#include <glm.hpp>

#include "Shape.h"
#include "transform.h"
#include "Shader.h"
#include "Object.h"

namespace sic
{
    Object::Object(Shape* _shape, const glm::vec3 _diffuseColor, const glm::vec3 _specularColor, const float _shininess)
        : shape(_shape), diffuseColor(_diffuseColor), specularColor(_specularColor), shininess(_shininess)
    {}

    void Object::Draw(ShaderProgram* program, glm::mat4& objectTr)
    {
        int loc = glGetUniformLocation(program->programId, "diffuse");
        glUniform3fv(loc, 1, &diffuseColor[0]);

        loc = glGetUniformLocation(program->programId, "specular");
        glUniform3fv(loc, 1, &specularColor[0]);

        loc = glGetUniformLocation(program->programId, "shininess");
        glUniform1f(loc, shininess);

        // Inform the shader of which object is being drawn so it can make
        // object specific decisions.
        //loc = glGetUniformLocation(program->programId, "objectId");
        //glUniform1i(loc, objectId);

        // Inform the shader of this object's model transformation.  The
        // inverse of the model transformation, needed for transforming
        // normals, is calculated and passed to the shader here.
        loc = glGetUniformLocation(program->programId, "ModelTr");
        glUniformMatrix4fv(loc, 1, GL_FALSE, Pntr(objectTr));

        glm::mat4 inv = glm::inverse(objectTr);
        loc = glGetUniformLocation(program->programId, "NormalTr");
        glUniformMatrix4fv(loc, 1, GL_FALSE, Pntr(inv));

        //loc = glGetUniformLocation(program->programId, "reflective");
        //glUniform1i(loc, refelective);

        // Draw this object
        if (shape)
            shape->DrawVAO();

        // Recursivelyy draw each sub-objects, each with its own transformation.
        for (int i = 0; i < instances.size(); i++) {
            //printf("transform: %f, %f, %f, %f\n", instances[i].second[0][0], instances[i].second[1][0], instances[i].second[2][0], instances[i].second[3][0]);
            //printf("transform: %f, %f, %f, %f\n", instances[i].second[0][1], instances[i].second[1][1], instances[i].second[2][1], instances[i].second[3][1]);
            //printf("transform: %f, %f, %f, %f\n", instances[i].second[0][2], instances[i].second[1][2], instances[i].second[2][2], instances[i].second[3][2]);
            //printf("transform: %f, %f, %f, %f\n", instances[i].second[0][3], instances[i].second[1][3], instances[i].second[2][3], instances[i].second[3][3]);
            glm::mat4 itr = instances[i].second;
            instances[i].first->Draw(program, itr);
        }
    }
}