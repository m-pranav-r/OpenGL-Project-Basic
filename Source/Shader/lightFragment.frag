#version 330 core
in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float specular = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	FragColor = vec4((diffuse + ambient + specular) * objectColor, 1.0);
}