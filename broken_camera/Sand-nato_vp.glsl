#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;
in int phaseShift;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

// Attributes forwarded to the geometry shader
out vec3 vertex_color;
out float timestep;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float grav = 0.1; // Gravity
float speed = 2.0; // Controls the speed of the explosion
float upward = 0.0; // additional y velocity for all particles

void main()
{
    // Let time cycle every four seconds
    float circtime = timer  - 8.0 * floor(timer / 8);
    float t = circtime; // Our time parameter

    float period = 4.0;  // Time period in seconds for position.x to repeat
    float frequency = 1.0 / period;  // Frequency to ensure a full cycle within the period

    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 0.0);

	
	// add "upward" to y speed to launch the particles vertically -- can easily omit
    position.y +=  mod(((timer) + float(gl_VertexID)*abs(norm.y)),4.0) * 4;
    //position.y =  int(gl_VertexID);

    // Move point along normal and down with t*t (acceleration under gravity)
    position.x +=  cos((mod((norm.x+float(gl_VertexID)),5.0) * 1.0 / period * timer * 2 * 2.14159265)) * (position.y * 0.35) + (norm.y * 5.0) + abs(cos(timer * 1.0 / period * 2 * 2.14159265 * norm.y))*(position.y);

    position.z +=  sin((mod((norm.y+float(gl_VertexID)),5.0) * 1.0 / period * timer * 2 * 2.14159265)) * (position.y * 0.35) + (norm.y * 5.0);
    
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    // Define color of vertex
    //vertex_color = color.rgb; // Color defined during the construction of the particles
    //vertex_color = object_color; // Uniform color 
    //vertex_color = vec3(t, 0.0, 1-t); // red-purple dynamic color
    vertex_color = vec3(1.0, 1-t, 0.0); // red-yellow dynamic color

    // Forward time step to geometry shader
    timestep = t;
}