
#include "Lyapunov.h"
#include "Fractal.h"
#include "Vec4.h"
#include "Simulator.h"

PREDEC Lyapunov::generate(Vec4 refStart, Vec4 deltaError, PREDEC D, int N, PREDEC dt, FractalData::InitialCondition ic) {
	Simulator ref(0, refStart, &ic);
	Vec4 testStart = refStart.add(deltaError);
	PREDEC totalTime = 0;
	PREDEC sum = 0;

	for (int i = 0; i < N; i++) {
		Simulator test(0, testStart, &ic);
		PREDEC delta = test.getState().sub(ref.getState()).abs();

		while (delta < D) {
			ref.iterateSnapshot(dt);
			test.iterateSnapshot(dt);
			totalTime += dt;

			delta = test.getState().sub(ref.getState()).abs();
		}

		PREDEC a = delta / deltaError.abs();
		testStart = ref.getState().add(test.getState().sub(ref.getState()).div(a));
		
		sum += PRElog(a);
	}

	return sum / totalTime;
}
