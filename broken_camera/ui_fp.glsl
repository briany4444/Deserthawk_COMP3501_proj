// Material with no illumination simulation

#version 130

// Attributes passed from the vertex shader
in vec4 color_in;
in vec2 uv_coord;

//uniforms
uniform sampler2D texture_map; // Texture Map
uniform int num_collected = 0;
uniform float timer;


void inerDotsHelper(inout vec4 color, in vec4 red,in vec4 objective_yellow) {

	if( uv_coord.y > 0.955 && uv_coord.y < .985){

		if(uv_coord.x > .945 && uv_coord.x < .975) { // one
			if(num_collected >= 1) {
				color = objective_yellow;
			} else{
				color = red;
			}
		}
		else if(uv_coord.x > 0.895 && uv_coord.x < .925) { // two
			if(num_collected >= 2) {
				color = objective_yellow;
			} else{
				color = red;
			}
		}
		else if(uv_coord.x > 0.845 && uv_coord.x < .875) { //tree
			if(num_collected >= 3) {
				color = objective_yellow;
			} else{
				color = red;
			}
		}
		else if(uv_coord.x > 0.795 && uv_coord.x < .825) {//four
			if(num_collected >= 4) {
				color = objective_yellow;
			} else{
				color = red;
			}
		}
		else if(uv_coord.x > 0.745 && uv_coord.x < .775) { //five
			if(num_collected >= 5) {
				color = objective_yellow;
			} else{
				color = red;
			}
		}
	}

}

void main() 
{
	vec4 background_gray = vec4(.6, .6, .6, 1);
	vec4 objective_yellow = vec4(.98, 1, .02, 1);
	vec4 red = vec4(1,0,0,1);
	vec4 dark_green = vec4( 0.32549, 0.77647, 0.32549, 1);
	vec4 light_gray = vec4(.7,.7,.7, 1);
	

	//flicker out of night vision for a couple seconds and then back in
	float phase = mod(timer-10, 20);
	int compA = abs(int( round(10*sin(.28 * phase)) ));
	int compB = abs(int( round(8*sin(.31 * phase)) ));
	int compC = abs(int( round(3*cos(.1 * phase)) ));
	int onOff = min(min(compA, min(compB, compC)), 1); // clamp at 1, already clamp at 0 implicitly

	vec4 overlay_green = vec4(0.2196 * onOff, .750 * onOff, 0.20196 * onOff, 0.2);


	//sample the noise texture as a function 
	//uv_coord.x = abs(sin(timer*timer)) + abs(sin(timer)) * uv_coord;
	vec4 color = texture2D(texture_map, uv_coord);
	//might need to invert 
	color = vec4(color.r * overlay_green.r, color.g * overlay_green.g, color.b * overlay_green.b, overlay_green.a);
	
	
	
	
	/*
	// texture
			float spacing = 0.0001;
			float thick = 0.00005;
			if ((mod(uv_coord.y, spacing) < thick) || (mod(uv_coord.x,spacing) < thick)) {
				color = light_gray;
			}
	*/

	
	// botton left corner represent num collected as dots

	//95% down to 98% down
	if( uv_coord.y > 0.95 && uv_coord.y < .99) {
		// 4 dots 4% wide each
		//right to left
		if       (uv_coord.x > .94 && uv_coord.x < .98) {
			color = background_gray;
			
			inerDotsHelper(color, red, objective_yellow);			

		} else if(uv_coord.x > 0.89 && uv_coord.x < .93) {
			color = background_gray;
			inerDotsHelper(color, red, objective_yellow);

		} else if(uv_coord.x > 0.84 && uv_coord.x < .88) {
			color = background_gray;
			inerDotsHelper(color, red, objective_yellow);

		} else if(uv_coord.x > 0.79 && uv_coord.x < .83) {
			color = background_gray;
			inerDotsHelper(color, red, objective_yellow);

		} else if(uv_coord.x > 0.74 && uv_coord.x < .78) {
			color = background_gray;
			inerDotsHelper(color, red, objective_yellow);

		}
	}

	// Check for transparency  --remove when blending is on--
    if(color.a < 0.2)
    {
            discard;
    }

	

	

	gl_FragColor = color;
	


}