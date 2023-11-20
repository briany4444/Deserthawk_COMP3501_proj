// Material with no illumination simulation

#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_coords;

//uniforms
uniform int num_collected;

void main() 
{
	vec4 color = color_interp;
	color.a = 0.0f;

	vec4 background_gray = vec4(.7, .7, .7, 1);
	vec4 objective_yellow = vec4(.98, 1, .02, 1);

	// botton left corner represent num collected as dots

	//95% down to 98% down
	if( uv_coords.y > 0.95 && uv_coords.y < .99) {
		// 4 dots 4% wide each
		//right to left
		if(uv_coords.x > .94 && uv_coords.x < .98) {
			color = background_gray;
		} else if(uv_coords.x > 0.81 && uv_coords.x < .93) {
			color = background_gray;
		} else if(uv_coords.x > 0.76 && uv_coords.x < .80) {
			color = background_gray;
		} else if(uv_coords.x > . 71 && uv_coords.x < .75) {
			color = background_gray;
		}
	}

	gl_FragColor = color;
	//gl_FragColor = vec4(0.6, 0.6, 0.6, 1.0);


}