__kernel void test(int size, __global float3* accelerations, __global float3* centers, __global float3* velocities, __global float* masses){
	__local float distances[1000];
	accelerations[get_global_id(0)].x = 0.0f;
	accelerations[get_global_id(0)].y = 0.0f;
	accelerations[get_global_id(0)].z = 0.0f;
	for (int i = 0; i < size; ++i) {
		if (i != get_global_id(0)) {
			distances[get_global_id(0)] = sqrt(pow(centers[get_global_id(0)].x - centers[i].x, 2) + pow(centers[get_global_id(0)].y - centers[i].y,2));
			if (distances[get_global_id(0)] > 0.05f) accelerations[get_global_id(0)] += masses[i] * (centers[i] - centers[get_global_id(0)])/pow(fabs(distances[get_global_id(0)]),3);
		}
	}
	accelerations[get_global_id(0)] *= 6.67f;
	__local float t;
	t = 0.00001f;
	__local float dt;
	dt = 0.000001f;
	__local int steps;
	steps = 100;
	//accelerations[0].x * dt;
	//if (get_global_id(0) == 0) printf("acceleration in kernel:%f\n", accelerations[0].x);
	for (int i = 0; i < steps; ++i){
		velocities[get_global_id(0)] += accelerations[get_global_id(0)] * dt;
		centers[get_global_id(0)] += velocities[get_global_id(0)] * dt;
	}
	if (get_global_id(0) == 0) printf(" ");
}