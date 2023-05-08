/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: -
 * Additional Authors: -
 */
 

#include "Shape.h"
#include <utility>              // for pair<Object*,glm::mat4>

namespace sic
{

    class ShaderProgram;
    class Object;

    typedef std::pair<Object*, glm::mat4> INSTANCE;

    // Object:: A shape, and its transformations, colors, and textures and sub-objects.
    class Object
    {
    public:
        Shape* shape;               // Polygons 
        glm::mat4 animTr;                // This model's animation transformation
        //int objectId;               // Object id to be sent to the shader
        //bool drawMe;                // Toggle specifies if this object (and children) are drawn.
        //bool refelective;
        //Texture* texture, * normalMap;

        glm::vec3 diffuseColor;          // Diffuse color of object
        glm::vec3 specularColor;         // Specular color of object
        float shininess;            // Surface roughness value

        std::vector<INSTANCE> instances; // Pairs of sub-objects and transformations 

        Object(Shape* _shape,
            const glm::vec3 _d = glm::vec3(), const glm::vec3 _s = glm::vec3(), const float _n = 1);

        // If this object is to be drawn with a texture, this is a good
        // place to store the texture id (a small positive integer).  The
        // texture id should be set in Scene::InitializeScene and used in
        // Object::Draw.

        void Draw(ShaderProgram* program, glm::mat4& objectTr);

        void add(Object* m, glm::mat4 tr = glm::mat4()) { instances.push_back(std::make_pair(m, tr)); }
    };

}