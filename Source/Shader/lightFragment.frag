#version 330 core
struct Material
{
	sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

in vec2 TexCoords;

struct Light
{
	vec3 position;

	vec3 diffuse;
	vec3 ambient;
	vec3 specular;
};


in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
//uniform sampler2D emissiveTexture;
uniform Light light;

out vec4 FragColor;

void main()
{   
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  	
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    //vec3 emission = vec3(0.0);
    //if(texture(material.specular, TexCoords).r == 0)
    //{
    //    emission = texture(emissiveTexture, TexCoords).rgb;
    //    emission *= 2;
    //}
        
    vec3 result = ambient + diffuse + specular;// + emission;
    FragColor = vec4(result, 1.0);
}