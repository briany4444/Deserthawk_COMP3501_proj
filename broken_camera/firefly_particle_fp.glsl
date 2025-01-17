#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;
in vec2 tex_coord;

// Uniform (global) buffer
uniform sampler2D texture_map;
uniform vec3 object_color = vec3(1.0, 1.0, 0.0);     // yellow

void main (void)
{
    // Get pixel from texture
    vec4 outval = texture(texture_map, tex_coord);
    // Adjust specified object color according to the grayscale texture value
    outval = vec4(outval.r*object_color.r, outval.g*object_color.g, outval.b*object_color.b, 1);
    gl_FragColor = outval;
}
