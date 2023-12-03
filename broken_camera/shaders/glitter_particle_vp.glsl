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
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float grav = 0.0; // Gravity
float upward = 0.0; // additional y velocity for all particles
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0); // Up direction
float accel = 1.2; // An acceleration applied to the particles coming from some attraction force
float speed = 3; // Control the speed of the motion

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
    float param = timer / 10.0 + phase; // The constant that divides "timer" also helps to adjust the "speed" of the fire
    float rem = mod(param, pi_over_two); // Use the remainder of dividing by pi/2 so that we are always in the range [0..pi/2] where sin() gives values in [0..1]
    float t = sin(rem); // Get time value in [0..1], according to a sinusoidal wave
    
    // Let's first work in model space (apply only world matrix)
    vec3 pos = vertex;
    vec4 norm = normal_mat * vec4(normal, 0.0);
    pos += speed*up_vec*accel*t*t; // Particle moves up
    vec4 position = world_mat * vec4(pos, 1.0);
    
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    float alpha = 1.0 - t * t;
    particle_color = vec4(0.6, 0.4, 1.0, alpha);

    // Forward time step to geometry shader
    timestep = t;
}
