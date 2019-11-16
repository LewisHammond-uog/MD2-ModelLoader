#version 330

in vec4 Position;
in vec4 Normal;
in vec4 Colour;
in vec2 Tex1;


out vec4 vNormal;
out vec4 vColour;
out vec4 vLightDir;
out vec2 vUV;


uniform vec4 cameraPosition;
uniform vec4 lightDirection;
uniform mat4 ProjectionView; 
uniform mat4 Model;


void main() 
{ 
	//send outputs from vertex shader to fragment shader
	vNormal = Normal;
	vColour = Colour;
	vUV = Tex1;

	vLightDir = lightDirection;

	gl_Position = ProjectionView * Model * Position;

}