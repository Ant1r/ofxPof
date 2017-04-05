/* 
 * Color engine : RGB, alpha, chromaKey, saturation/contrast/brightness
 * 
 * fragment program
 * 
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */

precision highp float;

varying vec2 texCoordVarying;

uniform sampler2D texture0;

// color
uniform vec3 color;
uniform bool colorOn;
uniform float alpha;

// chromaKey
uniform vec3 chromaKey;
uniform float chromaThreshold;
uniform float chromaFade;
uniform bool chromaKeyOn;

// contrast
uniform float brightness, contrast, saturation;
uniform bool bcsOn;


void main()
{
	vec4 col = texture2D(texture0, texCoordVarying);
	
	// chromaKey
	if (chromaKeyOn) col.a *= clamp((distance(chromaKey, col.rgb) - chromaThreshold) / chromaFade, 0.0, 1.0);	
//	if (chromaKeyOn) col.a *= clamp((length(chromaKey - col.rgb) - chromaThreshold) / chromaFade, 0, 1);	
		
	// contrast
	if (bcsOn){
		const vec3 lumCoeff = vec3(0.2125,  0.7154, 0.0721);
		float l = min(1.0, 5.0 * length(col.rgb));

		col.rgb /= col.a;
		col.rgb += brightness;

		col.rgb = (col.rgb - 0.5) * contrast + 0.5 ;

		vec3 intens = vec3(dot(col.rgb, lumCoeff));
		col.rgb = mix(intens, col.rgb, saturation);

		col.rgb *= col.a;
		col.rgb *= pow(l,0.1);
	}
	
	// color
	if (colorOn) col.rgb *= color;
	
	// alpha
	 gl_FragColor = vec4(col.rgb, col.a*alpha);
}

