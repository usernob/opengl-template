#version 330 core
out vec4 FragColor;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main()
{
    vec3 result = clamp(mix(light.diffuse, light.specular, dot(light.diffuse, light.specular)), 0.0, 1.0);
    FragColor = vec4(result, 1.0); // set all 4 vector values to 1.0
}
