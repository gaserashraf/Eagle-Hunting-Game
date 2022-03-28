#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the radius
uniform vec2 center;  // unifrom variable to store the center of the shape on it 
uniform float radius; // unifrom variable to store the radius of the shape on it
uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);  // the color of the shape
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0); // the color of the surrounded pixels of the shape

void main(){
    //TODO: Write code that will draw the circle

    // diff stores the euclidean distance between the current pixel and the center sqrt((x1-c1)^2+(y1+c2)^2) 
    float diff = (gl_FragCoord.x - center[0])*(gl_FragCoord.x - center[0]) + (gl_FragCoord.y - center[1])*(gl_FragCoord.y - center[1]);
    diff = sqrt(diff);
    if( diff <= radius){          // if the distance less than or equal to the radius then obviously the pixel is inside the circle
                                  // and should be colored with the inside color
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}