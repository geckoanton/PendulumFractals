
#version 450 core

out vec4 fragColor;
in vec2 coordinate;

uniform float m1, m2;
uniform float l1, l2;
uniform float g;

const float PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062;

float A1() {
	return (m1 + m2) * l1;
}
float B1(float a, float b) {
	return m2 * l2 * cos(a - b);
}
float C1(float a, float b, float da, float db) {
	return -(db * db * l2 * m2 * sin(a - b)) - (g * (m1 + m2)) * sin(a);
}

float A2(float a, float b) {
	return m2 * l1 * cos(a - b);
}
float B2() {
	return m2 * l2;
}
float C2(float a, float b, float da, float db) {
	return (da * da * l1 * m2 * sin(a - b)) - (g * m2) * sin(b);
}

vec4 getStateChange(vec4 state) {
	float a1 = A1();
	float b1 = B1(state.x, state.y);
	float c1 = C1(state.x, state.y, state.z, state.w);

	float a2 = A2(state.x, state.y);
	float b2 = B2();
	float c2 = C2(state.x, state.y, state.z, state.w);

	float ddb = (a2 * c1 - a1 * c2) / (a2 * b1 - a1 * b2);
	float dda = (c1 - b1 * ddb) / a1;

	return vec4(state.z, state.w, dda, ddb);
}

vec4 iterateSnapshot(vec4 state, float dt) {
	vec4 k1 = getStateChange(state);
	vec4 k2 = getStateChange(state + (k1 * (dt / 2)));
	vec4 k3 = getStateChange(state + (k2 * (dt / 2)));
	vec4 k4 = getStateChange(state + (k3 * (dt)));

	vec4 stateChange = (k1 + (k2 * 2) + (k3 * 2) + (k4)) * (dt / 6);

	float a = mod(mod(state.x + stateChange.x, 2.0f * PI) + 2.0f * PI, 2.0f * PI);
	float b = mod(mod(state.y + stateChange.y, 2.0f * PI) + 2.0f * PI, 2.0f * PI);
	float da = state.z + stateChange.z;
	float db = state.w + stateChange.w;

	return vec4(a, b, da, db);
}

float cabs(vec4 value) {
	return sqrt(value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w);
}
float lyapunov(vec4 refStart, vec4 deltaError, float D, int N, float dt) {
	vec4 ref = refStart;
	vec4 testStart = refStart + deltaError;
	float totalTime = 0.0;
	float sum = 0.0;

	for (int i = 0; i < N; i++) {
		vec4 test = testStart;
		float delta = cabs(test - ref);

		float deltaTime = 0;
		while (delta < D && deltaTime < 20) {
			ref = iterateSnapshot(ref, dt);
			test = iterateSnapshot(test, dt);
			totalTime += dt;
			deltaTime += dt;

			delta = cabs(test - ref);
		}

		float a = delta / cabs(deltaError);
		testStart = ref + ((test - ref) / a);
		
		sum += log(a);
	}

	return sum / totalTime;
}

void main() {
	vec4 state = vec4(coordinate.x * (2.0f * PI) - PI, coordinate.y * (2.0f * PI) - PI, 0, 0);
	float res = lyapunov(state, vec4(0.0000001f, 0.0lf, 0.0lf, 0.0f), 0.0001f, 100, 0.001f);
	
	fragColor = vec4(res, 0.0, 0.0, 1.0);
}
