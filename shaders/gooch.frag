#version 330 core
out vec4 FragColor;

in vec3 vNormal;  
in vec3 vPosition;  
  
uniform vec3 lightPosition; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
    //diffuse
    float kd = 1;
    float a = 0.2;
    float b = 0.6;
	float shine = 0.1;

	vec3 lightDir = normalize(lightPosition - vPosition);
	float NL = dot(vNormal, lightDir);

	float it = ((1 + NL) / 2);
    vec3 color = (1-it) * (vec3(0, 0, 0.4) + a*objectColor.xyz) 
               +  it * (vec3(0.4, 0.4, 0) + b*objectColor.xyz);

    //Highlights
    vec3 R = reflect( -lightDir, 
                      vNormal );

	vec3 viewDir = normalize(viewPos - vPosition);
    float ER = clamp( dot( viewDir, 
                           normalize(R)),
                     0, 1);
    
    vec4 spec = vec4(1) * pow(ER, shine);

	FragColor = vec4(color+spec.xyz, 1.0);
} 