#version 430 core

in vec2 coordinate;

out vec4 fragColor;

float m1 = 1;
float m2 = 1;
float l1 = 1;
float l2 = 1;
float g = 9.82;

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

	float a = mod(mod(state.x + stateChange.x, 6.28318530718) + 6.28318530718, 6.28318530718);
	float b = mod(mod(state.y + stateChange.y, 6.28318530718) + 6.28318530718, 6.28318530718);
	float da = state.z + stateChange.z;
	float db = state.w + stateChange.w;

	return vec4(a, b, da, db);
}

void main() {
	vec4 state = vec4(((coordinate.x + 1) / 2) * 6.28318530718 - 3.14159265, ((-coordinate.y + 1) / 2) * 6.28318530718 - 3.14159265, 0, 0);
	
	state = iterateSnapshot(state, 0.01);
	
	vec3 color = vec3(0, 0, 0);
	float count = 0;
	
	for(int i = 0; i < 1000; i++) {
		//float prevB = state.y;
		state = iterateSnapshot(state, 0.01);
		/*float currentB = state.y;
		
		if ((currentB < 3.14159265 && prevB > 3.14159265 && currentB < 5 && prevB < 5) || (currentB > 3.14159265 && prevB < 3.14159265 && currentB < 5 && prevB < 5)) {
			//count++;
			color = vec3(1, 1, 1);
		}*/
	}
	
	/*if(count >= 7) {
		color = vec3(1, 1, 1);
	}
	else if(count >= 6) {
		color = vec3(0, 1, 1);
	}
	else if(count >= 5) {
		color = vec3(1, 0, 1);
	}
	else if(count >= 4) {
		color = vec3(1, 1, 0);
	}
	else if(count >= 3) {
		color = vec3(0, 0, 1);
	}
	else if(count >= 2) {
		color = vec3(0, 1, 0);
	}
	else if(count >= 1) {
		color = vec3(1, 0, 0);
	}
	else if(count >= 0) {
		color = vec3(0, 0, 0);
	}*/
	
	float xxx = -(l1 * sin(state.x)) - (l2 * sin(state.y));
	if(xxx < 0)
		color = vec3(0, 0, 0);
	if(xxx >= 0)
		color = vec3(1, 1, 1);
	
	fragColor = vec4(color, 1.0);
}
