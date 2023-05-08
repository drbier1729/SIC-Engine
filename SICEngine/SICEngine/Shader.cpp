/*
 * © 2022 DigiPen (USA) Corporation
 * Course: GAM541, Spring 2022
 * Professor: Eric Fleegal
 * Primary Author: Dylan Bier
 * Additional Authors: -
 */
 

#include <fstream>
#include <glew.h>
#include <cassert>

#include "Shader.h"

namespace sic
{

    // Reads a specified file into a string and returns the string.  The
    // file is examined first to determine the needed string size.
    char* ReadFile(const char* name)
    {
        std::ifstream f;
        f.open(name, std::ios_base::binary); // Open
        assert(f && "File failed to open.");

        f.seekg(0, std::ios_base::end);      // Position at end
        size_t length = f.tellg();           //   to get the length

        char* content = new char[length + 1]; // Create buffer of needed length
        f.seekg(0, std::ios_base::beg);       // Position at beginning
        f.read(content, length);              //   to read complete file
        f.close();                            // Close

        content[length] = char(0);           // Finish with a NULL
        return content;
    }

    // Creates an empty shader program.
    ShaderProgram::ShaderProgram()
    {
        programId = glCreateProgram();
    }

    // Use a shader program
    void ShaderProgram::Use()
    {
        glUseProgram(programId);
    }

    // Done using a shader program
    void ShaderProgram::Unuse()
    {
        glUseProgram(0);
    }

    // Read, send to OpenGL, and compile a single file into a shader
    // program.  In case of an error, retrieve and print the error log
    // string.
    void ShaderProgram::AddShader(const char* fileName, GLenum type)
    {
        // Read the source from the named file
        char* src = ReadFile(fileName);
        const char* psrc[1] = { src };

        // Create a shader and attach, hand it the source, and compile it.
        int shader = glCreateShader(type);
        glAttachShader(programId, shader);
        glShaderSource(shader, 1, psrc, NULL);
        glCompileShader(shader);
        delete[] src;

        // Get the compilation status
        int status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        // If compilation status is not OK, get and print the log message.
        if (status != 1) {
            int length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            char* buffer = new char[length];
            glGetShaderInfoLog(shader, length, NULL, buffer);
            printf("Compile log for %s:\n%s\n", fileName, buffer);
            delete[] buffer;
        }
    }

    // Link a shader program after all the shader files have been added
    // with the AddShader method.  In case of an error, retrieve and print
    // the error log string.
    void ShaderProgram::LinkProgram()
    {
        // Link program and check the status
        glLinkProgram(programId);
        int status;
        glGetProgramiv(programId, GL_LINK_STATUS, &status);

        // If link failed, get and print log
        if (status != 1) {
            int length;
            glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
            char* buffer = new char[length];
            glGetProgramInfoLog(programId, length, NULL, buffer);
            printf("Link log:\n%s\n", buffer);
            delete[] buffer;
        }
    }
}