#include <cstdio>
#include <cstdlib>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "./shader.hpp"

using namespace std;
using namespace glm;

constexpr auto WIDTH = 850;
constexpr auto HEIGHT = 600;
constexpr auto SCALE = 0.75;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(WIDTH, HEIGHT, "openGL 2D", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	glewInit();

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	auto vertex_array = 0u, vertex_buffer = 0u;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	auto program = LoadShaders("simple_shader.v", "simple_shader.f");

	auto vertices = vector<float>
	{
		 /*
		 1, -1,  0,
		-1, -1,  0,
		 0,  1,  0
		 */
	};

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
								  vertices.data(), GL_STATIC_DRAW);

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
			&& !glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

		glDisableVertexAttribArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &vertex_buffer);
	glDeleteVertexArrays(1, &vertex_array);
	glDeleteProgram(program);
	glfwDestroyWindow(window);
	glfwTerminate();
}

