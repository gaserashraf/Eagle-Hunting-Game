#pragma once

#include <glad/gl.h>

namespace our {

    // This class defined an OpenGL texture which will be used as a GL_TEXTURE_2D
    class Texture2D {
        // The OpenGL object name of this texture 
        GLuint name = 0;
    public:
        // This constructor creates an OpenGL texture and saves its object name in the member variable "name" 
        Texture2D() {
            //TODO: (Req 4) Complete this function
            glGenTextures(1,&name);
            // Generating the texture in constructor ; 
            
        };

        // This deconstructor deletes the underlying OpenGL texture
        ~Texture2D() { 
            //TODO: (Req 4) Complete this function
            glDeleteTextures(1,&name);
            // delete the texture in the destructor 

        }

        // Get the internal OpenGL name of the texture which is useful for use with framebuffers
        GLuint getOpenGLName() {
            return name;
        }

        // This method binds this texture to GL_TEXTURE_2D
        void bind() const {
            //TODO: (Req 4) Complete this function
            glBindTexture(GL_TEXTURE_2D,name);
            // Bind the texture ; 

        }

        // This static method ensures that no texture is bound to GL_TEXTURE_2D
        static void unbind(){
            //TODO: (Req 4) Complete this function
            glBindTexture(GL_TEXTURE_2D,0);
            // unbind it by passing 0 to the function 
        }

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;
    };
    
}