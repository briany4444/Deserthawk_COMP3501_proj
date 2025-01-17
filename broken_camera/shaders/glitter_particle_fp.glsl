#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;
in vec2 tex_coord;

// Uniform (global) buffer
uniform sampler2D tex_samp;
uniform vec3 object_color = vec3(0.78, 0.615, 0.847);  // light purple

void main (void)
{
    // Get pixel from texture
    vec4 outval = texture(tex_samp, tex_coord);
    // Adjust specified object color according to the grayscale texture value
    outval = vec4(outval.r*object_color.r, outval.g*object_color.g, outval.b*object_color.b, sqrt(sqrt(outval.r))*frag_color.a);
    gl_FragColor = outval;
}
