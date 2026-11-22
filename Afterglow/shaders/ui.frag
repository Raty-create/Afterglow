#version 330 core

// 단색/텍스처 사각형. uUseTexture 가 1이면 폰트 아틀라스를 샘플링한다.

in vec2 vUV;

uniform vec4      uColor;
uniform sampler2D uTexture;
uniform int       uUseTexture;
uniform float     uFillRightPx;

out vec4 fragColor;

void main()
{
    vec4 base = uColor;
    if (uUseTexture == 1)
        base.a *= texture(uTexture, vUV).r;
    if (uFillRightPx > 0.0)
        base.a *= clamp(uFillRightPx - (gl_FragCoord.x - 0.5), 0.0, 1.0);

    fragColor = base;
}
