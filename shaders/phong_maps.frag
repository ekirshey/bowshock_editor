#version 330 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
    sampler2D specular;
    float shininess;

	vec3 ambient_color;
    vec3 diffuse_color;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 vNormal;  
in vec3 vPosition;  
in vec2 vTexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light; 

void main()
{
	// ambient
    vec3 ambient = light.ambient * material.ambient_color * texture(material.diffuse, vTexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(light.position - vPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse_color * texture(material.diffuse, vTexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - vPosition);
	// lightDir expected to point TO the fragment, so flip
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, vTexCoords));
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 