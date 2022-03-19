
#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace our {

    class ShaderProgram {

    private:
        //Shader Program Handle
        GLuint program;

    public:
        void create();
        void destroy();

        ShaderProgram(){ program = 0; }
        ~ShaderProgram(){ destroy(); }

        bool attach(const std::string &filename, GLenum type) const;

        bool link() const;

        void use() { 
            //TODO: call opengl to use the program identified by this->program
            glUseProgram(program);
        }

        GLuint getUniformLocation(const std::string &name) {
            //TODO: call opengl to get the uniform location for the uniform defined by name from this->program
            
            const GLchar *cstr = name.c_str();
            GLint Loc = glGetUniformLocation(program,cstr);
            return Loc;
        }

        void set(const std::string &uniform, GLfloat value) {
            //TODO: call opengl to set the value to the uniform defined by name
            glUniform1f(getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, glm::vec2 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            glUniform2f(getUniformLocation(uniform), value[0],value[1]);
        }

        void set(const std::string &uniform, glm::vec3 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            glUniform3f(getUniformLocation(uniform), value[0],value[1],value[2]);
        }

        void set(const std::string &uniform, glm::vec4 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            glUniform4f(getUniformLocation(uniform), value[0],value[1],value[2],value[3]);
        }


        //TODO: Delete the copy constructor and assignment operator
        ShaderProgram (const ShaderProgram&) = delete;
        ShaderProgram& operator= (const ShaderProgram&) = delete;
        //Question: Why do we do this? Hint: Look at the deconstructor
        // Answer: 
    };

}

#endif