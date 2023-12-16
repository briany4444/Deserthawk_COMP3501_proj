#version 130

// Attributes passed from the vertex shader
in vec3 vertex_position;
in vec2 vertex_uv;
in mat3 TBN_mat;
in vec3 light_pos;

// Uniform (global) buffer
uniform sampler2D texture_map; // Texture Map
uniform sampler2D normal_map; // Normal map

// Material attributes (constants)
uniform vec4 object_color = vec4(0.0, 1.0, 0.0, 1.0);
uniform vec4 light_color;
uniform int spec_power;


void main() 
{
    vec4 pixel = texture(texture_map, vertex_uv);

// Incomplete demo -- does matrix multiplication in fragment shader.
// Left as exercise: move matrix multiplication to vertex shader,
// use interpolated L, V in fragment shader

    // Blinn-Phong shading

    vec3 N, // Interpolated normal for fragment
         L, // Light-source direction
         V, // View direction
         H; // Half-way vector

    // Get substitute normal in tangent space from the normal map
    vec2 coord = vertex_uv;
    coord.y = 1.0 - coord.y;
    N = normalize(texture2D(normal_map, coord).rgb*2.0 - 1.0);

    // Work in tangent space by multiplying our vectors by TBN_mat    
    // Get light direction
    L = TBN_mat * (vertex_position - light_pos);
    L = normalize(L);
    
    // Compute diffuse lighting intensity
    float lambertian = max(dot(N, L), 0.0);

    // Get view direction
    //V = TBN_mat * (eye_position - vertex_position);
    V = TBN_mat * (- vertex_position); // We already applied the view matrix, so the camera is at the origin
    V = normalize(V);
    
    // Phong specular component
    //H = 0.5*(V + L);
    H = (V + L);
    H = normalize(H);
    
    float spec_angle = max(0.0,dot(V,(-L + 2*(dot(L,N))*N)));
    float specular = pow(spec_angle, spec_power);
        
    // Assume all components have the same color but with different weights
    float ambient = 0.4;
    if (gl_FrontFacing){
        gl_FragColor = (0.25*ambient + 0.7*lambertian + 1.0*specular)*pixel;
    } else {
        gl_FragColor = pixel;
    }
}
