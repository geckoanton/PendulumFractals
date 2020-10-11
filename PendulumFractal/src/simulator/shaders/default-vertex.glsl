#version 430 core

layout(location = 0) in vec2 vPos;

out vec2 coordinate;

uniform vec2 end;
uniform vec4 section;

void main() {
	gl_Position = vec4(vPos, 0.0, 1.0);
	vec2 posRatio = vec2((vPos.x + 1) / (end.x + 1), (vPos.y + 1) / (end.y + 1));
	coordinate = vec2(posRatio.x * section.z + section.x, posRatio.y * section.w + section.y);
}