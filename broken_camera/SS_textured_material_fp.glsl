#version 400

// Attributes passed from the vertex shader
in vec4 color_;
in vec2 uv_;


// Uniform (global) buffer
uniform sampler2D texture_map;


void main() 
{
    

    // Retrieve texture value
    vec4 pixel = texture(texture_map, uv_);

    // Use texture in determining fragment colour
    gl_FragColor = pixel;
}
