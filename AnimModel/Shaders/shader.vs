#version 330 core
const int MAX_BONES = 100;

layout (location = 0) in vec3 position;
// layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4 weights;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 bones[MAX_BONES];

void main()
{
	mat4 boneTransform = bones[boneIDs[0]] * weights[0];
	boneTransform += bones[boneIDs[1]] * weights[1];
	boneTransform += bones[boneIDs[2]] * weights[2];
	boneTransform += bones[boneIDs[3]] * weights[3];
	
    gl_Position = projection * view * model * boneTransform * vec4(position, 1.0f);
    TexCoords = texCoords;
}