/* 
 * Color engine : RGB, alpha, chromaKey, saturation/contrast/brightness
 * 
 * vertex program
 * 
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */

#version 120

//varying vec2 texCoordVarying;


void main()
{
	//texCoordVarying = /*gl_TexCoord[0] = */gl_MultiTexCoord0.xy;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

