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
#include <random>

// glm
#include <glm/gtc/matrix_transform.hpp>

// other project files
#include "macros.h"
#include "file_io.h"
#include "ShaderManager.h"

// defining implementation here
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// width and height need to be saved in memory
// for OpenGL calls.
// -----------------------------------------
// I could make them macros
// and then assign local variables the values
// but that seems to complex for a 0.00000001ms
// performance gain.
// -----------------------------------------
static constexpr size_t width = 1928;
static constexpr size_t height = 1072;

// some useful macros
#define WINDOW_TITLE "OpenCL Render"
#define MATRIX_HEIGHT 50000
#define MATRIX_WIDTH 5
#define RENDER_DIMENSIONS 7 // adding extra dimensions for colour
#define POINTS_COUNT 4

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
constexpr size_t planets_size_points = (MATRIX_HEIGHT * RENDER_DIMENSIONS * sizeof(float) * POINTS_COUNT);

// global variables
clObjects clObjs;
uint8_t oldFps = 0;
GLFWwindow* window;
GLuint planets_vbo;
GLuint planets_vao;
GLuint texture;
ShaderManager * shader_manager;


static int frame_buffer[width * height];

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

	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(-(int)height*1.5, height*1.5);

	for (int i = 0; i < MATRIX_HEIGHT; ++i)
	{
		float x, y, sqrt_val;
		do
		{
			x = distribution(generator);
			y = distribution(generator);

			sqrt_val = sqrt((x * x) + (y * y));
		} while (!(sqrt_val < (height * 1.5f)));

		planets[(i * MATRIX_WIDTH) + 0 /*mass*/] = (float(rand() % 99) + 1.0f);
		planets[(i * MATRIX_WIDTH) + 1 /*x*/] = rand() % width;
		planets[(i * MATRIX_WIDTH) + 2 /*y*/] = rand() % height;
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
	// use shaders
	shader_manager->use_program();

	// create vao
	glGenVertexArrays(1, &planets_vao);

	// create vbo and use variable id to store vbo id
	glGenBuffers(1, &planets_vbo);

	// bind vao
	glBindVertexArray(planets_vao);

	// make the new vbo active
	glBindBuffer(GL_ARRAY_BUFFER, planets_vbo);

	// texture setup
	
	// saving texture ID
	glGenTextures(1, &texture);
	
	// making the texture active
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// get texture with stb_image from assets folder
	Image image = read_bitmap("./assets/circle.png");

	// setting the texture image and passing image metadata
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
	
	// freeing data since its on the GPU now
	stbi_image_free(image.data);

	// generating the mimpmap for the texture
	glGenerateMipmap(GL_TEXTURE_2D);

	// setting matrix
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glMatrixMode(GL_MODELVIEW);

	glm::mat4 Projection = glm::ortho<float>(0, 2, 0, 2);
	glm::mat4 mvp = Projection * glm::lookAt(
		glm::vec3(-1.5, -1, 2),
		glm::vec3(-0.75, -1, -1),
		glm::vec3(0, 1, 0)
	);

	shader_manager->add_uniform(UNIFORM_TYPE::MAT4, "MVP", &mvp[0][0]);
	shader_manager->add_uniform(UNIFORM_TYPE::TEXTURE, "texture_data", (void*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

INLINE void setVertexAttributePointers()
{
	// doing magic shit
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, RENDER_DIMENSIONS * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, RENDER_DIMENSIONS * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, RENDER_DIMENSIONS * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
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

	// bind buffer
	glBindBuffer(GL_ARRAY_BUFFER, planets_vbo);
	glBindVertexArray(planets_vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// use shaders
	shader_manager->use_program();

	glDrawArrays(GL_QUADS, 0, MATRIX_HEIGHT);

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

	// create shader manager (and shaders as a result)
	shader_manager = new ShaderManager();

	// configuring openGL settings
	configureOpenGL();

	// setting vertex buffer data
	// the data var is NULL since I'll get it from OpenCL
	glBufferData(GL_ARRAY_BUFFER, planets_size_points, NULL, GL_DYNAMIC_DRAW);

	// set vertex attribute pointers so data is compatible with shaders
	setVertexAttributePointers();

	// compiling/preparing the OpenCL compute shaders and setting up the queue
	setupOpenCL();
}


int main(int argc, char** argv)
{
	// start ffmpeg telling it to expect raw rgba 720p-60hz frames
	// -i - tells it to read frames from stdin
	const char* cmd = "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s 1928x1072 -i - "
		"-preset slow -pix_fmt yuv420p -y -crf 1 -vf vflip output.mp4";

	// open pipe to ffmpeg's stdin in binary write mode
	FILE* ffmpeg = _popen(cmd, "wb");

	// sets up the program (OpenCL and OpenGL)
	setup();

	// initial running of OpenCL do get data
	runOpenCL();

	// main program loop
	while (!glfwWindowShouldClose(window))
	{
		auto start = std::chrono::high_resolution_clock::now();

		// processing data on gpu with opencl
		runOpenCL();

		// OpenGL render code
		render();

		// writing pixels to video file
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, frame_buffer);
		fwrite(frame_buffer, sizeof(int) * width * height, 1, ffmpeg);

		// swaping front and back buffers
		glfwSwapBuffers(window);

		// set title to fps
		manageTitle(start);

		// checking for events
		glfwPollEvents();
	}

	_pclose(ffmpeg);

	// terminate on close
	glfwTerminate();

	// deleting shader manager that is allocated in setup()
	delete shader_manager;

	return 0;
}