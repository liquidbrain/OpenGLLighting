#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Calculate the normal's position.
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Calculate the normal vector. Apply a normal matrix to avoid distortion caused by non-uniform
    // scaling. See "One last thing" at https://learnopengl.com/#!Lighting/Basic-Lighting
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Set the position of the current vertex using gl_Position, a GLSL built-in variable.
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
