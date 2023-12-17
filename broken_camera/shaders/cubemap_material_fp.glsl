#version 400

// Attributes passed from the vertex shader
in vec4 color_;
in vec2 uv_;


// Uniform (global) buffer
uniform sampler2D texture_map;


void main() 
{
    
    vec2 uv = uv_;

    

    //if(color_.r == -1.0f){ // tree point of teh triangle or on the bottom
    //    uv.y += 0.333f * color_.g;
        
    //}
    //else if(color_.r == 1.0f){ // tree point of teh triangle or on the top
    //    uv.y -= 0.333f * color_.g;
        
    //}
    // Retrieve texture value
    vec4 pixel = texture(texture_map, uv);

    // Use texture in determining fragment colour
    gl_FragColor = pixel;
}
