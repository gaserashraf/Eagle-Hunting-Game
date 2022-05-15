#pragma once
#include<iostream>
#include <glad/gl.h>
#include "vertex.hpp"

namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements 
        GLsizei elementCount;
    public:

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements)
        {
            //TODO: (Req 1) Write this function
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            elementCount= (int)elements.size();   // number of elements
            glGenVertexArrays(1, &VAO);         // create a VAO
            glBindVertexArray(VAO);           // bind the VAO

            glGenBuffers(1, &VBO);           // create a VBO
            glBindBuffer(GL_ARRAY_BUFFER, VBO);    // bind the VBO
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);   // copy the vertices data into the VBO

            glGenBuffers(1, &EBO);          // create a EBO
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // bind the EBO
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount*sizeof(unsigned int), elements.data(), GL_STATIC_DRAW); // copy the elements data into the EBO

            GLint positionLoc = ATTRIB_LOC_POSITION; // position attribute location                      
            glEnableVertexAttribArray(positionLoc);      // enable the attribute
            glVertexAttribPointer(positionLoc, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex,position)); // set the attribute pointer   

            GLint colorLoc = ATTRIB_LOC_COLOR; // color attribute location                
            glEnableVertexAttribArray(colorLoc);     // enable the attribute
            glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex, color)); // set the attribute pointer

            GLint texcoordLoc = ATTRIB_LOC_TEXCOORD; // texcoord attribute location                      
            glEnableVertexAttribArray(texcoordLoc);  // enable the attribute    
            glVertexAttribPointer(texcoordLoc, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord)); // set the attribute pointer   

            GLint normalLoc = ATTRIB_LOC_NORMAL;  // normal attribute location               
            glEnableVertexAttribArray(normalLoc);  // enable the attribute
            glVertexAttribPointer(normalLoc, 3, GL_FLOAT, false , sizeof(Vertex), (void*)offsetof(Vertex, normal)); // set the attribute pointer
        }

        // this function should render the mesh
        void draw() 
        {
            //TODO: (Req 1) Write this function
            glBindVertexArray(VAO); // bind the VAO
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, (void*)0);    // draw the mesh

        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh(){
            //TODO: (Req 1) Write this function
            glDeleteVertexArrays(1, &VAO); // delete the VAO
            glDeleteBuffers(1, &VBO); // delete the VBO
            glDeleteBuffers(1, &EBO); // delete the EBO
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}