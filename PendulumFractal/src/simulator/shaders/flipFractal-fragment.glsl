
#version 450 core

out vec4 fragColor;
in vec2 coordinate;

uniform float m1, m2;
uniform float l1, l2;
uniform float g;

const double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062lf;

// TODO: IMPLEMENT DOUBLE PRECISION TRIGONOMETRIC OPERATIONS!!!

double A1() {
	return (double(m1) + double(m2)) * double(l1);
}
double B1(double a, double b) {
	return double(m2) * double(l2) * cos(float(a - b));
}
double C1(double a, double b, double da, double db) {
	return -(db * db * double(l2) * double(m2) * sin(float(a - b))) - (double(g) * (double(m1) + double(m2))) * sin(float(a));
}

double A2(double a, double b) {
	return double(m2) * double(l1) * cos(float(a - b));
}
double B2() {
	return double(m2) * double(l2);
}
double C2(double a, double b, double da, double db) {
	return (da * da * double(l1) * double(m2) * sin(float(a - b))) - (double(g) * double(m2)) * sin(float(b));
}

dvec4 getStateChange(dvec4 state) {
	double a1 = A1();
	double b1 = B1(state.x, state.y);
	double c1 = C1(state.x, state.y, state.z, state.w);

	double a2 = A2(state.x, state.y);
	double b2 = B2();
	double c2 = C2(state.x, state.y, state.z, state.w);

	double ddb = (a2 * c1 - a1 * c2) / (a2 * b1 - a1 * b2);
	double dda = (c1 - b1 * ddb) / a1;

	return dvec4(state.z, state.w, dda, ddb);
}

dvec4 iterateSnapshot(dvec4 state, double dt) {
	dvec4 k1 = getStateChange(state);
	dvec4 k2 = getStateChange(state + (k1 * (dt / 2)));
	dvec4 k3 = getStateChange(state + (k2 * (dt / 2)));
	dvec4 k4 = getStateChange(state + (k3 * (dt)));

	dvec4 stateChange = (k1 + (k2 * 2) + (k3 * 2) + (k4)) * (dt / 6);

	double a = mod(mod(state.x + stateChange.x, 2.0lf * PI) + 2.0lf * PI, 2.0lf * PI);
	double b = mod(mod(state.y + stateChange.y, 2.0lf * PI) + 2.0lf * PI, 2.0lf * PI);
	double da = state.z + stateChange.z;
	double db = state.w + stateChange.w;

	return dvec4(a, b, da, db);
}

void main() {
	dvec4 state = dvec4(coordinate.x * (2.0lf * PI) - PI, coordinate.y * (2.0lf * PI) - PI, 0, 0);

	state = iterateSnapshot(state, 0.01lf);

	vec3 color = vec3(0, 0, 0);

	for(int i = 0; i < 1000; i++) {
		double prevB = state.y;
		state = iterateSnapshot(state, 0.01lf);
		double currentB = state.y;

		if ((currentB < PI && prevB > PI && currentB < 5 && prevB < 5) || (currentB > PI && prevB < PI && currentB < 5 && prevB < 5)) {
			color = vec3(1, 1, 1);
			break;
		}
	}

	fragColor = vec4(color, 1.0);
}