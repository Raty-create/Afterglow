#version 330 core

// 화면 좌표(NDC) 기준 2D 쿼드. HUD/오버레이에 사용한다.

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

out vec2 vUV;

void main()
{
    vUV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
