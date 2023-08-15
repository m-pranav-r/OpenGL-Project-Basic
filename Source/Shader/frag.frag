#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;

void main()
{
    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);//texture(texture_diffuse1, TexCoords);
}