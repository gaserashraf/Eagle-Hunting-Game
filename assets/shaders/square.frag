#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the side-length
  
uniform vec2 center;         // unifrom variable to store the slope of the line
uniform float side_length;   // unifrom variable to store the side length of the shape on it

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);   // the color of the shape
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);  // the color of the surrounded pixels of the shape

void main(){
    //TODO: Write code that will draw the square

    // the pixel considered as inside the shape if its distance on x axis or in y axis (the max of them) is less than
    // or equal to the side length over 2 if one of them (the max) is bigger than side length over 2 it's outside the shape 
    if(max(abs(gl_FragCoord.x - center[0]), abs(gl_FragCoord.y - center[1])) <= side_length/2){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}