#version 330 core
#define NR_POINT_LINES 4
struct Material
{
    float shininess;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
    vec3 direction;

    vec3 ambient;
	vec3 diffuse;
	vec3 specular;

    float cutOff;
    float outerCutOff;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
//uniform SpotLight spotLight;
uniform PointLight pointLights[NR_POINT_LINES];
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

out vec4 FragColor;

float near = 0.1; 
float far  = 100.0;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

void main()
{   
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    for(int i = 0; i<NR_POINT_LINES; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    FragColor = vec4(result, 1.0);
    //float ndc = gl_FragCoord.z * 2.0 - 1.0;
    //float linearDepth = (2.0 * near * far) / (far + near - ndc * (far - near));
    //FragColor = vec4(vec3(linearDepth), 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
//{
//    vec3 lightDir = normalize(viewPos - FragPos);
//    float theta = dot(lightDir, normalize(-light.direction));
//    float epsilon = light.cutOff - light.outerCutOff;
//    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//    float diff = max(dot(normal, lightDir), 0.0);
//    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;
//    vec3 reflectDir = reflect(-lightDir, normal);  
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;
//    return   (diffuse * intensity) + (specular * intensity);
//}