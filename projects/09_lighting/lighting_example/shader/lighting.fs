#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos;

struct Light {
    vec3 position;
    vec3 direction;
    float cutoff;
    vec3 attenuation; // 감쇠계수 (Kc, Kl, Kq)
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;  
};
uniform Light light;
 
struct Material {
    sampler2D diffuse; 
    sampler2D specular;
    float shininess;
};
uniform Material material;

void main() {
  vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
  vec3 ambient = texColor * light.ambient;

  float dist = length(light.position - position);
  vec3 distPoly = vec3(1.0, dist, dist*dist);
  float attenuation = 1.0 / dot(distPoly, light.attenuation); // attenuation = 1 / (Kc + Kl*dist + Kq*dist*dist)
  vec3 lightDir = (light.position - position) / dist; 

  float theta = dot(lightDir, normalize(-light.direction));
  vec3 result = ambient;

  if (theta > light.cutoff) { // theta는 cos이므로 theta > light.cutoff는 각이 cutoff 범위 안이라는 뜻.
    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * texColor * light.diffuse;

    vec3 specColor = texture2D(material.specular, texCoord).xyz;
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * specColor * light.specular;

    result += diffuse + specular;
  }

  result *= attenuation;

  fragColor = vec4(result, 1.0);
}