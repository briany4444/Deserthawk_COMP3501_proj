#version 400

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
in vec3 light_pos;

// Uniform (global) buffer
uniform sampler2D texture_map;


void main() 
{
    vec2 new_uv = vec2(mod(uv_interp[0], 1.0), mod(uv_interp[1], 1.0));

    // Retrieve texture value
    vec4 pixel = texture(texture_map, new_uv);

    // Use texture in determining fragment colour
    gl_FragColor = pixel;
}
