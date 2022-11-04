#define MATRIX_HEIGHT 50000
#define MATRIX_WIDTH 5
#define RENDER_DIMENSIONS 7
#define width 2856
#define height 1608
#define POINTS_COUNT 4
#define COLOR_RGB 3

#define MASS 0
#define X 1
#define Y 2
#define DX 3
#define DY 4
#define G (0.000006743f)

__kernel void planetCalc(__global float planet[MATRIX_HEIGHT][MATRIX_WIDTH], 
	__global float outPlanet[MATRIX_HEIGHT][(RENDER_DIMENSIONS * POINTS_COUNT)])
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
	const float x = (planet[i][X] / (width)) - 0.35;
	const float y = (planet[i][Y] / (height)) - 0.35;
	const float mass = (0.5 - (planet[i][MASS] + 0.01 / planet[i][MASS])) / 5000;

	__private float velocity = planet[i][DX];

	if (velocity < 0)
	{
		velocity = -velocity;
	}

	if (planet[i][DY] < 0)
	{
		velocity += -planet[i][DY];
	}
	else
	{
		velocity += planet[i][DY];
	}
#define RAW_RED		0.05
#define RAW_GREEN	0.9
#define RAW_BLUE	1.0

	float RED	= 0.1 - (RAW_RED * (velocity * 6));
	float GREEN	= 1.0 - (RAW_GREEN * (velocity / 2));
	float BLUE	= 1.0 - (RAW_BLUE * (velocity / 3));
	
	if(RED < 0) 
	{
		RED = -RED;
	}

	// ------------- Point 1 -------------
	outPlanet[i][0] = x;
	outPlanet[i][1] = y;

	outPlanet[i][2] = RED;
	outPlanet[i][3] = GREEN;
	outPlanet[i][4] = BLUE;

	outPlanet[i][5] = 0;
	outPlanet[i][6] = 1;

	// ------------- Point 2 -------------
	outPlanet[i][7] = x;
	outPlanet[i][8] = y - mass;

	outPlanet[i][9] = RED;
	outPlanet[i][10] = GREEN;
	outPlanet[i][11] = BLUE;

	outPlanet[i][12] = 0;
	outPlanet[i][13] = 0;

	// ------------- Point 3 -------------
	outPlanet[i][14] = x + (mass / 2);
	outPlanet[i][15] = y - mass;

	outPlanet[i][16] = RED;
	outPlanet[i][17] = GREEN;
	outPlanet[i][18] = BLUE;

	outPlanet[i][19] = 1;
	outPlanet[i][20] = 0;

	// ------------- Point 4 -------------
	outPlanet[i][21] = x + (mass / 2);
	outPlanet[i][22] = y;

	outPlanet[i][23] = RED;
	outPlanet[i][24] = GREEN;
	outPlanet[i][25] = BLUE;

	outPlanet[i][26] = 1;
	outPlanet[i][27] = 1;
}