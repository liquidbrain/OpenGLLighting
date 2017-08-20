#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // Calculate ambient lighting (indirect light source(s)).
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Calulate diffuse lighting (direct light source).
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Calculate specular lighting (the spot of light that appears on a shiny object; usually the
    // color of the light rather than the color of the object).
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Calculate the Phong reflection model.
    vec3 result = (ambient + diffuse + specular) * objectColor;

    // Set the fragment's color.
    FragColor = vec4(result, 1.0);
}
