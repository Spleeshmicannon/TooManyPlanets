#include "clBuilder.h"

#include <GL/freeglut.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include <cstdlib>

static const size_t width = 1904;
static const size_t height = 1071;

#define WINDOW_TITLE "OpenCL Render"
#define MATRIX_HEIGHT 500000
#define MATRIX_WIDTH 5

#define X 1
#define Y 2

float planets[MATRIX_HEIGHT][MATRIX_WIDTH];
clProgram prog;
uint8_t oldFps = 0;

inline void runOpenCL()
{
	prog.queue.enqueueNDRangeKernel(prog.physicsKernel, cl::NullRange, cl::NDRange(MATRIX_HEIGHT), cl::NullRange);
	
	/*prog.queue.enqueueNDRangeKernel(prog.blackKernel, cl::NullRange, cl::NDRange(width, height), cl::NullRange);
	prog.queue.enqueueNDRangeKernel(prog.whiteKernel, cl::NullRange, cl::NDRange(MATRIX_HEIGHT), cl::NullRange);*/
	
	prog.queue.finish();

	prog.queue.enqueueReadBuffer(prog.inBuffer, CL_TRUE, 0, (sizeof(float) * MATRIX_HEIGHT * MATRIX_WIDTH), planets);

	//cl::size_t<3> origin;
	//origin[0] = 0;
	//origin[0] = 0;
	//origin[0] = 0;

	//cl::size_t<3> region;
	//region[0] = width;
	//region[1] = height;
	//region[2] = 1;

	//screenData = new cl_uint(width * height * sizeof(cl_uchar) * 4);

	//prog.queue.enqueueReadImage(prog.image, CL_TRUE, origin, region, 0, 0, screenData);

}

inline void setupOpenCL()
{
	srand(500);

	for (int i = 0; i < MATRIX_HEIGHT; ++i)
	{
		planets[i][0] = (float(rand() % 999) + 1.0f); // mass
		planets[i][1] = ((float(rand() % width / 4)) * 2) + float(width / 4); // x
		planets[i][2] = ((float(rand() % height / 4)) * 2) + float(height / 4); // y
		planets[i][3] = 0; // dx
		planets[i][4] = 0; // dy
	}

	prog.context = cl::Context({ cl::Device::getDefault() });

	// The kernel
	cl::Program::Sources sources;

	// raw kernel string
	const std::string deviceData = getFileData("device.cl");

	// adding kernel string to the source data structure
	sources.push_back({ deviceData.c_str(), deviceData.length() });

	// setting up program for building runnable kernel objects
	prog.program = cl::Program(prog.context, sources);

	if (prog.program.build({ cl::Device::getDefault() }) != CL_SUCCESS)
	{
		std::cout << "error: " << prog.program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl::Device::getDefault()) << std::endl;
	}

	prog.inBuffer = cl::Buffer(prog.context, CL_MEM_READ_WRITE, (sizeof(float) * MATRIX_HEIGHT * MATRIX_WIDTH));
	prog.outBuffer = cl::Buffer(prog.context, CL_MEM_READ_WRITE, (sizeof(float) * MATRIX_HEIGHT * 2));
	//prog.image = cl::Image2D(prog.context, CL_MEM_READ_WRITE, cl::ImageFormat(CL_SIGNED_INT8, CL_RGB), width, height, 0, NULL);


	// buffer initialisation
	prog.queue = cl::CommandQueue(prog.context, cl::Device::getDefault());
	prog.queue.enqueueWriteBuffer(prog.inBuffer, CL_TRUE, 0, (sizeof(float) * MATRIX_HEIGHT * MATRIX_WIDTH), planets);

	prog.physicsKernel = cl::Kernel(prog.program, "planetCalc");
	prog.physicsKernel.setArg(0, prog.inBuffer);
	prog.physicsKernel.setArg(1, prog.outBuffer);

	/*prog.blackKernel = cl::Kernel(prog.program, "image_make_black");
	prog.blackKernel.setArg(0, prog.image);

	prog.whiteKernel = cl::Kernel(prog.program, "image_draw_planets");
	prog.whiteKernel.setArg(0, prog.image);
	prog.whiteKernel.setArg(1, prog.outBuffer);*/
}

inline void manageTitle(std::chrono::steady_clock::time_point start)
{
	const uint8_t fps = std::round( 1 / (
			std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start)
		).count());

	if (((fps >= oldFps - 2) && (fps <= oldFps + 2)) || !oldFps)
	{
		oldFps = fps;
		glutSetWindowTitle((WINDOW_TITLE + std::string(" FPS: ") + std::to_string(fps)).c_str());
	}
}

void renderer()
{
	auto start = std::chrono::high_resolution_clock::now();

	std::thread clThread = std::thread(runOpenCL);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, width, height, 0.0);

	clThread.join();

	glBegin(GL_LINES);
	for (int i = 0; i < MATRIX_HEIGHT; ++i) {
		glVertex2f(planets[i][X], planets[i][Y]);
		glVertex2f(planets[i][X] + 1, planets[i][Y] + 1);
	}
	glEnd();
	glFlush();

	manageTitle(start);

	glutPostRedisplay();
	return;
}

inline void glutSetup()
{
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(1, 4);
	glutInitWindowSize(width, height);
	glutCreateWindow(WINDOW_TITLE);
	glutDisplayFunc(renderer);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutSetup();

	setupOpenCL();

	glutMainLoop();

	return 1;
}