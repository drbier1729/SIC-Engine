/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Brian Chen
 * Additional Authors: -
 */
 

#pragma once

#include <vector>
#include <string>
#include "SICTypes.h"

namespace sic
{
    class Shape
    {
    public:
        Shape() {}
        virtual ~Shape() {}

        unsigned int vaoID;
        unsigned int count;

        // Draw
        virtual void DrawVAO();
    };

    class Box2D : public Shape
    {
    public:
        Box2D();
    };

    class Arrow2D : public Shape
    {
    public:
        Arrow2D();
        void DrawVAO();
    };

    class Font : public Shape
    {
    public:
        Font();
        void Settings(const char* _str, float _x, float _y, float s, vec3 c) { str = _str; x = _x; y = _y; scale = s; color = c; }
        void DrawVAO();
        vec3 getColor() { return color; }

    private:
        const char* str;
        float x;
        float y;
        float scale;
        vec3 color;
        unsigned int vboID;
    };
}