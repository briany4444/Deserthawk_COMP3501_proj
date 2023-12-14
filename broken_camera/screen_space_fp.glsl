#version 130

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D texture_map;

void main() 
{
vec4 pixel;
vec2 pos = uv0;
pixel = texture(texture_map,uv0);
	
	float r = .5 - timer/10;
	vec2 dir = pos - vec2(.5,.5);
	float len = sqrt(dir.x*dir.x + dir.y*dir.y);
	if( (len > 0.5+r)) {
		pixel = vec4(0,0,0,1);
	}


  //gl_FragColor = (pixel+pixel2)/2;
  gl_FragColor = pixel;
}
