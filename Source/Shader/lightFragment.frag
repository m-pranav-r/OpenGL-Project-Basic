#version 330 core
in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

out vec4 FragColor;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	FragColor = vec4((diffuse + ambient) * objectColor, 1.0);
}