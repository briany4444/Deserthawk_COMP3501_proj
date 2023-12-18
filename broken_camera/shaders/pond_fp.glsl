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

    vec3 N, // Interpolated normal for fragment
         L, // Light-source direction
         V, // View direction
         H; // Half-way vector

    // Get substitute normal in tangent space from the normal map
    N = normalize(vec3(0.0,1.0,0.0));

    // Work in tangent space by multiplying our vectors by TBN_mat    
    // Get light direction
    L = (position_interp - light_pos);
    L = normalize(L);
    
    // Compute diffuse lighting intensity
    float lambertian = max(dot(N, L), 0.0);

    // Get view direction
    //V = TBN_mat * (eye_position - vertex_position);
    V = normalize(V);
    
    // Phong specular component
    //H = 0.5*(V + L);
    H = (V + L);
    H = normalize(H);
    
    float spec_angle = max(0.0,dot(V,(-L + 2*(dot(L,N))*N)));
    float specular = 0;
        
    // Assume all components have the same color but with different weights
    float ambient = 0.4;
    gl_FragColor = (ambient + 1*lambertian + 1*specular)*pixel;


}

