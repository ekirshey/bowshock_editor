#version 330 core
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
    sampler2D specular;
    float shininess;

	vec3 ambient_color;
    vec3 diffuse_color;
}; 

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

	float constant;
	float linear;
	float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
	vec3 direction;
	float cutoff;
	float outer_cutoff;

	float constant;
	float linear;
	float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 vNormal;  
in vec3 vPosition;  
in vec2 vTexCoords;

uniform vec3 viewPos;
uniform Material material;

// Lights
uniform DirectionalLight dir_light;

#define NUM_MAX_LIGHTS 4
uniform PointLight point_lights[NUM_MAX_LIGHTS];

uniform uint point_light_count;

uniform SpotLight spot_light;

vec3 calc_dir_light(DirectionalLight light, vec3 normal, vec3 view_dir);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main()
{
	vec3 result = vec3(0.0);

	vec3 view_dir = normalize(viewPos - vPosition);
	vec3 normal = normalize(vNormal);

	result += calc_dir_light(dir_light, normal, view_dir);

	for ( uint i = 0u; i < point_light_count; i++)
		result += calc_point_light( point_lights[i], normal, vPosition, view_dir);

	// Probably won't include a flashlight by default.
	result += calc_spot_light(spot_light, normal, vPosition, view_dir);

    FragColor = vec4(result, 1.0);
} 

vec3 calc_dir_light(DirectionalLight light, vec3 normal, vec3 view_dir)
{
	vec3 light_dir = normalize(-light.direction);
	
	// Diffuse shading
	float diff = max(dot(normal, light_dir), 0.0);

	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

	// Combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, vTexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vTexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, vTexCoords));

	return( ambient + diffuse + specular);

}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir) 
{
	vec3 light_dir = normalize(light.position - frag_pos);

	// diffuse shading 
	float diff = max(dot(normal, light_dir), 0.0);

	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0) , material.shininess);

	// attenuation
	float dist = length(light.position - frag_pos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, vTexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vTexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, vTexCoords));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
	vec3 light_dir = normalize(light.position - frag_pos);

	// diffuse
	float diff = max(dot(normal, light_dir), 0.0);
	
	// specular
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

	//attenuation
	float dist = length(light.position - frag_pos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	float theta = dot(light_dir, normalize(-light.direction));
	float epsilon = light.cutoff - light.outer_cutoff;
	float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

	// ambient
	vec3 ambient = light.ambient * texture(material.diffuse, vTexCoords).rgb;
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, vTexCoords).rgb;
	vec3 specular = light.specular * spec * vec3(texture(material.specular, vTexCoords));

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}