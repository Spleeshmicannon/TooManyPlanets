// the OpenGL wrangler library for
// later versions of OpenGL
#include <GL/glew.h>

// GLFW3 graphics library
// exposing some native stuff to 
// make OpenCL OpenGL interop work
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// OpenCL 1.2 header
#include <CL/cl.hpp>

// standard libraries
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <cstdlib>
#include <fstream>

// width and height need to be saved in memory
// for OpenGL calls.
// -----------------------------------------
// I could make them macros
// and then assign local variables the values
// but that seems to complex for a 0.00000001ms
// performance gain.
// -----------------------------------------
static constexpr size_t width = 1904;
static constexpr size_t height = 1071;

// using __forceinline only with compatible compiler
#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE static inline
#endif

// some useful macros
#define WINDOW_TITLE "OpenCL Render"
#define MATRIX_HEIGHT 20000
#define MATRIX_WIDTH 5
#define RENDER_DIMENSIONS 2
#define POINTS_COUNT 6

// a convenient struct for storing the many OpenCL objects
// that get used
struct clObjects
{
	cl::Context context;
	cl::Program program;
	cl::Buffer inBuffer;
	cl::Buffer outBuffer;
	cl::BufferGL glBuffer;
	cl::Kernel physicsKernel;
	cl::CommandQueue queue;
};

// computing sizes at compile time
constexpr size_t planets_size = MATRIX_HEIGHT * MATRIX_WIDTH * sizeof(float);
constexpr size_t planets_size_points = MATRIX_HEIGHT * RENDER_DIMENSIONS * sizeof(float) * POINTS_COUNT;

// global variables
clObjects clObjs;
uint8_t oldFps = 0;
GLFWwindow* window;
GLuint planets_vbo;

INLINE std::string readFile(const std::string filename)
{
	// technically this would be a little dodgy if
	// you were reading in more variable files
	// but I'm reading the same one file every time
	// so it's always going to work and is never
	// going to be problem unless the file is changed

	// string for file data to be stored in
	std::string data;

	// reading until \0 null termination character
	std::getline(std::ifstream(filename), data, '\0');

	// if data.size is 0, then the file wasn't found
	if (data.size() == 0)
	{
		std::cerr << "No file found: " << filename << "\t\n";
	}

	// returning any found file data
	return data;
}

INLINE void initOpenGL()
{
	// wrangling OpenGL functions
	const GLint GlewInitResult = glewInit();

	// checking if wrangle was
	if (GLEW_OK != GlewInitResult)
	{
		// if this failed then the program can't run so print error and exit
		std::cerr << "GLEW ERROR: " << glewGetErrorString(GlewInitResult) << "\t\n";
		exit(EXIT_FAILURE);
	}
}

INLINE void setupOpenCL()
{
	// seeding future rand() calls
	srand(50);

	// allocating memory for initial planet setup
	float* planets = new float[MATRIX_HEIGHT * MATRIX_WIDTH];

	for (int i = 0; i < MATRIX_HEIGHT; ++i)
	{
		planets[(i * MATRIX_WIDTH) + 0 /*mass*/] = (float(rand() % 99) + 1.0f);
		planets[(i * MATRIX_WIDTH) + 1 /*x*/] = ((float(rand() % width / 4)) * 2) + float(width / 4);
		planets[(i * MATRIX_WIDTH) + 2 /*y*/] = ((float(rand() % height / 4)) * 2) + float(height / 4);
		planets[(i * MATRIX_WIDTH) + 3 /*dx*/] = 0;
		planets[(i * MATRIX_WIDTH) + 4 /*dy*/] = 0;
	}

	// will match cl::Device::GetDefault since its the first platform
	// for some reason the context properties still only works with the C version
	// this is only OpenCL 1.2 though
	cl_platform_id c_platform;
	clGetPlatformIDs(1, &c_platform, NULL);

	cl_context_properties contextProperties[] =
	{
		CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(window), // setting context as the window
		CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(window)), // setting HDC, getting value from window
		CL_CONTEXT_PLATFORM, (cl_context_properties)c_platform, // the C version of the platform object
		0 // breaks without 0 at the end
	};

	clObjs.context = cl::Context(cl::Device::getDefault(), contextProperties);

	// The kernel
	cl::Program::Sources sources;

	// raw kernel string
	const std::string deviceData = readFile("device.cl");

	// adding kernel string to the source data structure
	sources.push_back({ deviceData.c_str(), deviceData.length() });

	// setting up program for building runnable kernel objects
	clObjs.program = cl::Program(clObjs.context, sources);

	// compiling the OpenCL compute kernel/shader
	if (clObjs.program.build({ cl::Device::getDefault() }) != CL_SUCCESS)
	{
		std::cout << "error: " << clObjs.program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl::Device::getDefault()) << std::endl;
	}

	// setting up GPU memory objects
	clObjs.inBuffer = cl::Buffer(clObjs.context, CL_MEM_READ_WRITE, planets_size);
	clObjs.outBuffer = cl::Buffer(clObjs.context, CL_MEM_READ_WRITE, planets_size_points);

	// assigning vertex buffer to an OpenCL object for OpenCL OpenGL interop
	clObjs.glBuffer = cl::BufferGL(clObjs.context, CL_MEM_READ_WRITE, planets_vbo);

	// buffer initialisation
	clObjs.queue = cl::CommandQueue(clObjs.context, cl::Device::getDefault());
	clObjs.queue.enqueueWriteBuffer(clObjs.inBuffer, CL_TRUE, 0, planets_size, planets);

	// setting up compiled kernel for execution
	clObjs.physicsKernel = cl::Kernel(clObjs.program, "planetCalc");
	clObjs.physicsKernel.setArg(0, clObjs.inBuffer);
	clObjs.physicsKernel.setArg(1, clObjs.outBuffer);

	// deleting intial planets memory
	delete[] planets;
}

INLINE void configureOpenGL()
{
	// create vbo and use variable id to store vbo id
	glGenBuffers(1, &planets_vbo);

	// make the new vbo active
	glBindBuffer(GL_ARRAY_BUFFER, planets_vbo);

	// setting matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

INLINE void manageTitle(std::chrono::steady_clock::time_point start)
{
	// calculating frame rate
	const uint8_t fps = std::round(1 / (
		std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start)
		).count());

	// if framerate change is small enough don't bother updating title
	if (((fps >= oldFps - 2) && (fps <= oldFps + 2)) || !oldFps)
	{
		oldFps = fps;
		glfwSetWindowTitle(window, (WINDOW_TITLE + std::string(" FPS: ") + std::to_string(fps)).c_str());
	}
}

INLINE void updatePlanetVBO()
{
	// setting the 
	glVertexPointer(RENDER_DIMENSIONS, GL_FLOAT, 0, NULL);

	// binding the buffer
	glBindBuffer(GL_ARRAY_BUFFER, planets_vbo);
}

INLINE void runOpenCL()
{
	// executing the kernel
	clObjs.queue.enqueueNDRangeKernel(clObjs.physicsKernel, cl::NullRange, cl::NDRange(MATRIX_HEIGHT), cl::NullRange);
	clObjs.queue.finish();

	// making sure OpenGL is finished with its vertex buffer
	glFinish();

	// acquiring the OpenGL object (vertex buffer) for OpenCL use
	const std::vector<cl::Memory> glObjs = { clObjs.glBuffer };
	clObjs.queue.enqueueAcquireGLObjects(&glObjs);

	// copying the OpenCL buffer to the BufferGL
	clObjs.queue.enqueueCopyBuffer(clObjs.outBuffer, clObjs.glBuffer, 0, 0, planets_size_points);

	// releasing the OpenGL object
	clObjs.queue.enqueueReleaseGLObjects(&glObjs);
}

INLINE void render()
{
	// clear screen to avoid onioning
	glClear(GL_COLOR_BUFFER_BIT);

	// draw vertex buffer
	glDrawArrays(GL_TRIANGLES, 0, MATRIX_HEIGHT);

	// flush changes
	glFlush();
}

void setup()
{
	// initialising glfw (wrapper for platform code)
	glfwInit();

	// creating the window
	window = glfwCreateWindow(width, height, WINDOW_TITLE, NULL, NULL);

	if (!window)
	{
		// if window couldn't be created - terminate the program
		glfwTerminate();
		exit(-1);
	}

	// set the current OpenGL context to be the window
	glfwMakeContextCurrent(window);

	// initialising function pointers (glew.h)
	initOpenGL();

	// configuring openGL settings
	configureOpenGL();

	// setting vertex buffer data
	// the data var is NULL since I'll get it from OpenCL
	glBufferData(GL_ARRAY_BUFFER, planets_size_points, NULL, GL_STATIC_DRAW);

	// initial data binding to complete setup
	updatePlanetVBO();

	glEnableClientState(GL_VERTEX_ARRAY);

	// compiling/preparing the OpenCL compute shaders and setting up the queue
	setupOpenCL();
}


int main(int argc, char** argv)
{
	// sets up the program (OpenCL and OpenGL)
	setup();

	// initial running of OpenCL do get data
	runOpenCL();

	// getting first from from OpenCL
	updatePlanetVBO();

	// main program loop
	while (!glfwWindowShouldClose(window))
	{
		auto start = std::chrono::high_resolution_clock::now();

		// processing data on gpu with opencl
		runOpenCL();

		// OpenGL render code
		render();

		// swaping front and back buffers
		glfwSwapBuffers(window);

		// set title to fps
		manageTitle(start);

		// checking for events
		glfwPollEvents();
	}

	// terminate on close
	glfwTerminate();
	return 0;
}