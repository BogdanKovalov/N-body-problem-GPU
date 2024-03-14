#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 transform;
uniform float colorTransform;

out vec3 myColor;
void main() {
	gl_Position = transform * vec4(position, 1.0);
	if (colorTransform >= 1.0f) myColor = vec3(1.0f, 0.0f, 0.0f);
	else myColor = vec3(colorTransform, 1 - colorTransform, 0.0f);
//myColor = vec3(0.5f, 0.3f, 0.7f);
}