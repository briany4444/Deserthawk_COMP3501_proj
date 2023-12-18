#version 140

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
in vec3 light_pos;

// Uniform (global) buffer
uniform sampler2D texture_map;
uniform float timer; 


void main (void)
{

    float rad = mod(timer, 5) / 5;
    float shell_len = 0.05;
    vec4 pixel = vec4(0, 0, 1, 1);
    
    if ((distance(uv_interp, vec2(0.5, 0.5)) > rad) && (distance(uv_interp, vec2(0.5, 0.5)) < (rad + shell_len)) ) {
        pixel = vec4(1, 1, 1, 1);
    }

    gl_FragColor = pixel;

}

