//
#version 120
#extension GL_ARB_texture_rectangle : enable

#ifdef GL_ES
// define default precision for float, vec, mat.
precision highp float;
#endif

uniform sampler2DRect tex0;
uniform vec2 distance = vec2(0,0);
//uniform vec2 TX;

void main (void) 
{ 

	vec2 texcoord = (gl_TextureMatrix[0] * gl_TexCoord[0]).st;
	vec4 sample = 0.5 * texture2DRect(tex0, texcoord - distance);
	sample += texture2DRect(tex0, texcoord);
	sample += 0.5 * texture2DRect(tex0, texcoord + distance);

	sample /= 2.;

	gl_FragColor =  sample;
} 


