#version 140

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform mat4 normal_mat;

// Attributes forwarded to the fragment shader
out vec3 position_interp;
out vec3 normal_interp;
out vec4 color_interp;
out vec2 uv_interp;
out vec3 light_pos;
out vec3 obj_col;
out vec3 light_col;
out float spec_pwr;

// Material attributes (constants)
uniform vec3 light_position;
uniform vec3 obj_color;
uniform float spec_power;
uniform vec3 light_color;


void main()
{

	gl_Position = projection_mat * view_mat * (world_mat * vec4(vertex, 1.0));

    position_interp = vec3(view_mat * world_mat * vec4(vertex, 1.0));
    
    normal_interp = vec3(normal_mat * view_mat * vec4(normal, 0.0));

    color_interp = vec4(obj_color, 1.0);

    uv_interp = uv;

    light_pos = vec3(view_mat * vec4(light_position, 1.0));

    light_col = light_color;

    spec_pwr = spec_power;

}
