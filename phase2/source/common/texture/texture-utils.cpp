#include "texture-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

our::Texture2D* our::texture_utils::empty(GLenum format, glm::ivec2 size){
    our::Texture2D* texture = new our::Texture2D();
    //TODO: (Req 10) Finish this function to create an empty texture with the given size and format
    int RT_W = size[0];
    int RT_H = size[1];
    GLuint tex = texture->getOpenGLName();
    texture->bind();
    glTexStorage2D(GL_TEXTURE_2D, 1, format, RT_W, RT_H);
    return texture;
}

our::Texture2D* our::texture_utils::loadImage(const std::string& filename, bool generate_mipmap) {
    glm::ivec2 size;
    int channels;
    //Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    //We need to till stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    //Load image data and retrieve width, height and number of channels in the image
    //The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    //Note: channels (the 4th argument) always returns the original number of channels in the file
    unsigned char* pixels = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
    if(pixels == nullptr){
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    // Create a texture
    our::Texture2D* texture = new our::Texture2D();
    //Bind the texture such that we upload the image data to its storage
    //TODO: (Req 4) Finish this function to fill the texture with the data found in "pixels" and generate the mipmaps if requested
    // first thing to bind the texture  
    texture->bind();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // OpenGL assumes that each row (composing #width pixels) starts at an address divisible by 4.
    // This would be always true if we used RGBA (4 * #width) is divisible by 4.
    // Meanwhile in this case we have 9 * 3 = 27 bytes which isn't divisible by 4.
    // it will assume each row is at an integer multiple of 1 = not assuming anything (always true)


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);
    //parameters 
    // 1- targt is Texture_2D  
    // 2- level 0 
    // 3- format RGBA8 
    // 4- width  of the texture 
    // 5- height of the texture 
    // 6- border always zero backward compatibality 
    // 7- format 
    // 8- type 
    // 9- the pixels of the image 
    if(generate_mipmap) glGenerateMipmap(GL_TEXTURE_2D);
    // Must have different mipmaps for the texture (e.g. 8x8 -> 8x8, 4x4, 2x2, 1x1 by successive 2x2 averaging)
    // glGenerateMipMap starts with the base level (we passed above as 0: default 8x8) and uses it
    // to generate mipmaps that OpenGL will need.
    
    stbi_image_free(pixels); //Free image data after uploading to GPU
    return texture;
}