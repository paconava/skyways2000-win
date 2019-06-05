#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
struct Light {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;
in vec2 ex_texCoord;

out vec4 color;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform int selector;
uniform float changer;

uniform vec3 viewPos;
uniform Light light;  
uniform Material material;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));

    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);   

    // ambient lighting
    float ambientStrength = 1.0f;
    vec3 ambientLighting = light.ambient * ambientStrength;

    // Ambient
    //vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = light.diffuse * diff;

    // specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specular * spec;  

    diffuse *= intensity;
    specular *= intensity;

    vec3 result = ambientLighting + diffuse + specular;
    color = vec4(result, 1.0f);

    

    switch(selector){
    case 0:
    color = vec4(result, 1.0f) * texture(texture0, ex_texCoord);
    break;
    case 1:
    color = vec4(result, 1.0f) * texture(texture1, vec2(ex_texCoord.x - changer * 2, ex_texCoord.y + changer)) * (0.95f + (0.05f * sin(changer * 100)));
    break;
    }
    if(color.a < 0.1)
        discard;
}