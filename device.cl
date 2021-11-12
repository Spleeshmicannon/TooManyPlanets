#define MATRIX_HEIGHT 20000
#define MATRIX_WIDTH 5
#define RENDER_DIMENSIONS 2
#define width 1904
#define height 1071
#define POINTS_COUNT 6

#define MASS 0
#define X 1
#define Y 2
#define DX 3
#define DY 4
#define G 0.000006743f

__kernel void planetCalc(__global float planet[MATRIX_HEIGHT][MATRIX_WIDTH], __global float outPlanet[MATRIX_HEIGHT][RENDER_DIMENSIONS * POINTS_COUNT])
{
	// Get the index of the current element to be processed
	__private int i = get_global_id(0);
	__private float fx = 0, fy = 0;


	// calcualte force
	for (int j = 0; j < MATRIX_HEIGHT; ++j) // B
	{
		if (j == i) continue;
		__private float F, rx, ry;

		rx = planet[i][X] - planet[j][X]; // A - B
		ry = planet[i][Y] - planet[j][Y]; // A - B

		F = (planet[i][MASS] * planet[j][MASS] * G) /
			(((rx * rx) + (ry * ry)) + 0.00000000000001f);

		fx += (-F * rx) + 0.00000000000001f; // A
		fy += (-F * ry) + 0.00000000000001f; // A
	}

	// calculate direction momentum over multiple iterations
	planet[i][DX] += (fx / planet[i][MASS]);
	planet[i][DY] += (fy / planet[i][MASS]);

	// calculate position
	planet[i][X] += planet[i][DX];
	planet[i][Y] += planet[i][DY];

	// calculate what is drawn
	const float x = (planet[i][X] / width) - 0.5;
	const float y = (planet[i][Y] / height) - 0.5;
	const float mass = (0.5 - (planet[i][MASS] + 0.01 / planet[i][MASS])) / 5000;

	outPlanet[i][0] = x;
	outPlanet[i][1] = y;

	outPlanet[i][2] = x + 1 * mass;
	outPlanet[i][3] = y;

	outPlanet[i][4] = x + 0.5 * mass;
	outPlanet[i][5] = y + 1 * mass;

	outPlanet[i][6] = x;
	outPlanet[i][7] = y + 0.7 * mass;

	outPlanet[i][8] = x + 1 * mass;
	outPlanet[i][9] = y + 0.7 * mass;

	outPlanet[i][10] = x + 0.5 * mass;
	outPlanet[i][11] = y - 0.5 * mass;
}