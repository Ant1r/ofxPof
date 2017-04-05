//
precision highp float;

varying vec2 texCoordVarying;

uniform sampler2D tex0;
uniform vec2 distance;

void main (void) 
{ 
	vec4 sample = 0.5 * texture2D(tex0, texCoordVarying - distance);
	sample += texture2D(tex0, texCoordVarying);
	sample += 0.5 * texture2D(tex0, texCoordVarying + distance);

	sample /= 2.;

	gl_FragColor =  sample;
} 


