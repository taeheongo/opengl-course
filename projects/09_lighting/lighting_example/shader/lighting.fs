#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;  
};
uniform Light light;
 
struct Material {
    sampler2D diffuse; 
    vec3 specular;
    float shininess;
};
uniform Material material;

void main() {
  vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
  vec3 ambient = texColor * light.ambient; // 보통 이렇게 diffuse color를 이용해서 ambient를 만든다.

  vec3 lightDir = normalize(light.position - position);
  vec3 pixelNorm = normalize(normal);
  float diff = max(dot(pixelNorm, lightDir), 0.0);
  vec3 diffuse = diff * texColor * light.diffuse;
 
  vec3 viewDir = normalize(viewPos - position);
  vec3 reflectDir = reflect(-lightDir, pixelNorm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = spec * material.specular * light.specular;
 
  vec3 result = ambient + diffuse + specular;
  fragColor = vec4(result, 1.0);
}