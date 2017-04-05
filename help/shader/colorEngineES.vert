/* 
 * Color engine : RGB, alpha, chromaKey, saturation/contrast/brightness
 * 
 * vertex program
 * 
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */

precision highp float;
uniform mat4 modelViewProjectionMatrix;
attribute vec2 texcoord;
attribute vec4 position;

varying vec2 texCoordVarying;

void main()
{
	texCoordVarying = texcoord;
	gl_Position = modelViewProjectionMatrix * position;
}

