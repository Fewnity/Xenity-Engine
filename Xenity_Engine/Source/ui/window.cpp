#include "window.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <stb_image.h>
#include <GLFW/glfw3.h>

SDL_Window* Window::window;

int Window::width = 1280;
int Window::height = 720;
const char * ENGINE_NAME = "Xenity Engine";

int Window::InitWindow()
{
	/* Initialize the library */
	if (!glfwInit() && SDL_Init(SDL_INIT_EVERYTHING) != 0)
		return -1;

	unsigned int center = SDL_WINDOWPOS_CENTERED;

	/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);*/

	window = SDL_CreateWindow(ENGINE_NAME, center, center, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	//Use depth management
	glEnable(GL_DEPTH_TEST);

	//0 is our origin, the higher the z, the farther the object
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND); //Enable blending.
	//glEnable(GL_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.

	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); //TODO Move to texture init?

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_GL_SetSwapInterval(1);
	stbi_set_flip_vertically_on_load(true);

	return 0;
}

void Window::Resize(int newWidth, int newHeight)
{
	printf("%d %d", newWidth, newHeight);
	glViewport(0, 0, newWidth, newHeight);
	width = newWidth;
	height = newHeight;
}

void Window::UpdateScreen() {

	SDL_GL_SwapWindow(window);
}

void Window::SetTitle(const std::string title) {
	SDL_SetWindowTitle(window, title.c_str());
}

void  Window::SetWidth(int width_)
{
	width = width_;
}
void  Window::SetHeight(int height_)
{
	height = height_;
}
int  Window::GetWidth()
{
	return width;
}
int  Window::GetHeight()
{
	return height;
}