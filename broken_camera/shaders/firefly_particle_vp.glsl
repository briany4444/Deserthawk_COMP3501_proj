#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

// Attributes forwarded to the geometry shader
out vec4 particle_color;
out float particle_id;
out float timestep;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float speed = 0.5; // Controls the speed of the explosion
float upward = 0.0; // additional y velocity for all particles

// Define some useful constants
const float pi = 3.1415926536;
const float pi_over_two = 1.5707963268;
const float two_pi = 2.0*pi;

void main()
{
    // Define particle id
    particle_id = color.r; // Derived from the particle color. We use the id to keep track of particles

    // Define time in a cyclic manner
    float phase = two_pi*particle_id; // Start the sin wave later depending on the particle_id
    float t = phase + timer; // Our time parameter

    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 0.0);

    // movement
    position.x += sin(speed* norm.x * t);
    position.y += sin((upward+norm.y)*t*speed);
    position.z += sin((norm.z*t*speed));
    
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    // Define color of vertex
    float alpha = sin(t);    // this changes the brightness of the firefly depending on its current phase
    particle_color = vec4(0.6, 0.4, 1.0, max(0.5, alpha));

    // Forward time step to geometry shader
    timestep = t;
}
