// Material with no illumination simulation

#version 130

// Vertex buffer
in vec3 vertex;
in vec3 color;
in vec2 uv;

// Attributes forwarded to the fragment shader
out vec4 color_;
out vec2 uv_;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

void main()
{
    gl_Position = projection_mat * view_mat* world_mat * vec4(vertex, 1.0);
    uv_ = uv;
    color_ = vec4(color, 1.0);
}