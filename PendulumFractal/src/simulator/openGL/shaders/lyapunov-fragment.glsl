
#version 450 core

out vec4 fragColor;
in vec2 coordinate;

uniform float m1, m2;
uniform float l1, l2;
uniform float g;

const double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062lf;

double dsin(double x) {
	double res = 0, pow = x, fact = 1;
	for (int i = 0; i < 12; i++) {
		res += pow / fact;
		pow *= -1 * x * x;
		fact *= (2 * (i + 1)) * (2 * (i + 1) + 1);
	}
	return res;
}
double dcos(double x) {
	double res = 0, pow = 1, fact = 1;
	for (int i = 0; i < 12; i++) {
		res += pow / fact;
		pow *= -1 * x * x;
		fact *= (2 * (i + 1)) * (2 * (i + 1) - 1);
	}
	return res;
}

double A1() {
	return (double(m1) + double(m2)) * double(l1);
}
double B1(double a, double b) {
	return double(m2) * double(l2) * dcos(a - b);
}
double C1(double a, double b, double da, double db) {
	return -(db * db * double(l2) * double(m2) * dsin(a - b)) - (double(g) * (double(m1) + double(m2))) * dsin(a);
}

double A2(double a, double b) {
	return double(m2) * double(l1) * dcos(a - b);
}
double B2() {
	return double(m2) * double(l2);
}
double C2(double a, double b, double da, double db) {
	return (da * da * double(l1) * double(m2) * dsin(a - b)) - (double(g) * double(m2)) * dsin(b);
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

double dabs(dvec4 value) {
	return sqrt(value.x * value.x + value.y * value.y + value.z * value.z + value.w * value.w);
}
double lyapunov(dvec4 refStart, dvec4 deltaError, double D, int N, double dt) {
	dvec4 ref = refStart;
	dvec4 testStart = refStart + deltaError;
	double totalTime = 0.0;
	double sum = 0.0;

	for (int i = 0; i < N; i++) {
		dvec4 test = testStart;
		double delta = dabs(test - ref);

		double deltaTime = 0;
		while (delta < D && deltaTime < 20) {
			ref = iterateSnapshot(ref, dt);
			test = iterateSnapshot(test, dt);
			totalTime += dt;
			deltaTime += dt;

			delta = dabs(test - ref);
		}

		double a = delta / dabs(deltaError);
		testStart = ref + ((test - ref) / a);
		
		sum += log(float(a));	// TODO implement double precision!
	}

	return sum / totalTime;
}

void main() {
	dvec4 state = dvec4(coordinate.x * (2.0lf * PI) - PI, coordinate.y * (2.0lf * PI) - PI, 0, 0);
	double res = lyapunov(state, dvec4(0.0000001lf, 0.0lf, 0.0lf, 0.0lf), 0.0001lf, 100, 0.001lf);
	
	fragColor = vec4(float(res), 0.0, 0.0, 1.0);
}
