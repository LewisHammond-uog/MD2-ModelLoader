#version 330

in vec4 vNormal;
in vec4 vLightDir;
in vec4 vColour;
in vec2 vUV;

out vec4 FragColor;


uniform sampler2D DiffuseTexture;

void main()
{
	vec4 albedo = texture2D(DiffuseTexture, vUV);
	
	//calculate lambertian term
	float lambertFactor  = max( dot(vNormal, vLightDir), 0.0 ) ;

	//put components together
	vec4 ambientComponent  = vec4(0.15f, 0.15f, 0.15f, 1.f);
	vec4 diffuseComponent  = albedo *  lambertFactor * vColour;
	
	//get final colour
	FragColor = ambientComponent + diffuseComponent;
	FragColor.a = albedo.a;

}
