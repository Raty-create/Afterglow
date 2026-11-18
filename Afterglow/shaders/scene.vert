#version 330 core

// 씬 지오메트리(벽/바닥/괴물/소품)용 버텍스 셰이더.
// 위치와 노멀을 월드 공간으로 넘겨 프래그먼트에서 랜턴 조명을 계산한다.

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

out vec3 vWorldPos;
out vec3 vColor;
out vec3 vNormal;

uniform mat4 model, view, projection;
uniform vec3 uObjectColor;

void main() {
	vWorldPos = vec3(model * vec4(aPos, 1.0f));
	vNormal = mat3(transpose(inverse(model))) * aNormal;
	vColor = aColor * uObjectColor;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
