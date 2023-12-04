// Material with no illumination simulation

#version 130

// Attributes passed from the vertex shader
in vec4 color_in;
in vec2 uv_coord;

//uniforms
uniform int num_collected;

void main() 
{
	vec4 color = color_in;
	color.a = 0.0f;

	vec4 background_gray = vec4(.7, .7, .7, 1);
	vec4 objective_yellow = vec4(.98, 1, .02, 1);

	// botton left corner represent num collected as dots

	//95% down to 98% down
	if( uv_coord.y > 0.95 && uv_coord.y < .99) {
		// 4 dots 4% wide each
		//right to left
		if(uv_coord.x > .94 && uv_coord.x < .98) {
			color = background_gray;
		} else if(uv_coord.x > 0.81 && uv_coord.x < .93) {
			color = background_gray;
		} else if(uv_coord.x > 0.76 && uv_coord.x < .80) {
			color = background_gray;
		} else if(uv_coord.x > 0.71 && uv_coord.x < .75) {
			color = background_gray;
		}
	}

	// Check for transparency
    if(color.a < 1.0)
    {
            discard;
    }

	gl_FragColor = color;
	


}