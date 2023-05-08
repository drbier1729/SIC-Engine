/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Brian Chen
 * Additional Authors: -
 */
 

namespace sic
{
    class ShaderProgram
    {
    public:
        int programId;

        ShaderProgram();
        void AddShader(const char* fileName, const GLenum type);
        void LinkProgram();
        void Use();
        void Unuse();
    };
}