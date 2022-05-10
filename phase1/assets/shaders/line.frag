#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the slope and the intercept
uniform float slope;       // unifrom variable to store the slope of the line
uniform float intercept;   // unifrom variable to store the intercept (when x=0 -> y=inter)

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);   // the color of the shape
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);  // the color of the surrounded pixels of the shape

void main(){
    //TODO: Write code that will draw the square

    // the condition of inside color is that the pixel location is below the line or on it
    // so it should be satisfy the condition slope*x + intercept less than or equal to y
    if(gl_FragCoord.y <= slope*gl_FragCoord.x + intercept){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}