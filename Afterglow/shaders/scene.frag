#version 330 core

// 랜턴은 스포트라이트다. 밝기 단계가 그대로 광량과 도달 거리(uLightRange)가 된다.

in vec3 vWorldPos;
in vec3 vColor;
in vec3 vNormal;

out vec4 fragColor;

uniform int uUnlit;
uniform int uLightOn;
uniform vec3 uLightPos, uLightDir, uLightColor, uEyeDir;
uniform float uLightRange;       // 빛이 닿는 거리(m)
uniform float uLightIntensity;   // 0.0 ~ 1.0 (랜턴 밝기)

const float /*KA = 0.2f,*/ KS = 0.5f, SHININESS = 64.0f;

// Blinn-Phong
void main() {
	if (uUnlit == 1) {
		fragColor = vec4(vColor, 1.0f);
		return;
	}
	if (uLightOn == 0) {
		fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		return;
	}

	vec3 N = normalize(vNormal);
	vec3 L = normalize(uLightPos - vWorldPos);
	vec3 V = normalize(uEyeDir);
	vec3 H = normalize(L + V);

	vec3  toLight  = uLightPos - vWorldPos;
    float distance = length(toLight);
    vec3  lightDir = toLight / max(distance, 0.0001);

	// 거리 감쇠: light_range 밖은 완전히 어둡다. (괴물 감지 거리와 같은 값)
    float attenuation = clamp(1.0 - distance / max(uLightRange, 0.0001), 0.0, 1.0);
    attenuation *= attenuation;

	// 원뿔 마스크: 랜턴이 비추는 방향에서 벗어나면 어두워진다.
    float cosAngle = dot(normalize(-lightDir), normalize(uLightDir));
    float cone = smoothstep(0.55, 0.94, cosAngle);

	float diff = max(dot(N, L), 0.0f) * attenuation * cone * uLightIntensity;
	float spec = pow(max(dot(N, H), 0.0f), SHININESS) * KS * diff;

	vec3 result = (vColor * diff + vec3(spec)) * uLightColor;
	fragColor = vec4(result, 1.0);
}