#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct TextureMaterial {
    vec3 diffuse;
    vec3 specular;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;
uniform Material material;
uniform Light light;
uniform vec3 viewPos;

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 viewDir, TextureMaterial materialTexture) {
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * materialTexture.diffuse;
    vec3 diffuse = light.diffuse * diff * materialTexture.diffuse;
    vec3 specular = light.specular * spec * materialTexture.specular;

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, TextureMaterial materialTexture) {
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float lightDistance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));

    vec3 ambient = light.ambient * materialTexture.diffuse;
    vec3 diffuse = light.diffuse * diff * materialTexture.diffuse;
    vec3 specular = light.specular * spec * materialTexture.specular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main()
{
    TextureMaterial materialTexture = TextureMaterial(
            texture(material.diffuse, TexCoord).rgb,
            texture(material.specular, TexCoord).rgb
        );

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalculateDirLight(dirLight, norm, viewDir, materialTexture);
    result += CalculatePointLight(light, norm, FragPos, viewDir, materialTexture);

    FragColor = vec4(result, 1.0);
}
