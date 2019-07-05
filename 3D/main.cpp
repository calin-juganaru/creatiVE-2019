#include <cstdio>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "include/shader.hpp"
#include "include/objloader.hpp"
#include "include/2D_text.hpp"

using glm::radians;
using glm::perspective;

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 700;
constexpr auto RATIO = 1.0f * WIDTH / HEIGHT;
constexpr auto FoV = 45.0f;
constexpr auto NEAR = 0.01f;
constexpr auto FAR = 1000.0f;

constexpr auto PI = 3.1415926535897932384626433f;

constexpr auto KEY_SPEED = 8.0f;
constexpr auto MOUSE_SPEED = 0.005f;

constexpr auto ROTATE = false;
constexpr auto rotation_axis = vec3(0, 1, 0);

template <typename T>
size_t bytes(const vector<T>& v)
{
	return v.size() * sizeof(T);
}

int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(WIDTH, HEIGHT, "creatiVE 2019 Result", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	glewInit();

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwPollEvents();
    glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

	glClearColor(0.0f, 0.2f, 0.4f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	auto vertex_array = 0u;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	auto program = LoadShaders("./shaders/shader.v", "./shaders/shader.f");
	auto MatrixID = glGetUniformLocation(program, "MVP");

	auto [vertices, uvs, normals] = load_obj(argv[1]);

	auto vertex_buffer = 0u, color_buffer = 0u, uv_buffer = 0u, normal_buffer = 0u;

	auto vertex_colors = normals;
	auto min_val = 0.0f, max_val = 1.0f;

	for (auto&& color: vertex_colors)
	{
		min_val = min(min_val, color.x);
		min_val = min(min_val, color.y);
		min_val = min(min_val, color.z);

		max_val = max(max_val, color.x);
		max_val = max(max_val, color.y);
		max_val = max(max_val, color.z);
	}
	max_val = max(max_val, -min_val);

	for (auto&& color: vertex_colors)
	{
		color -= min_val;
		color /= max_val;
		color /= 10.0f;
	}

	for (auto&& vertex: vertices)
		if (argc > 1)
			 vertex /= stoi(argv[2]);
		//else vertex /= 4;

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, bytes(vertices), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
	glBufferData(GL_ARRAY_BUFFER, bytes(vertex_colors), vertex_colors.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, bytes(uvs), uvs.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, bytes(uvs), uvs.data(), GL_STATIC_DRAW);

	initText2D("font.dds");

	auto Model = translate(mat4(1), vec3(-3.0, -5, -25.0));
	auto View = lookAt(vec3(), vec3(), vec3());

	auto Projection = perspective(FoV, RATIO, NEAR, FAR);
	auto Animation = mat4(1);

	auto camera = vec3(0, 0, 0);
	auto last_moment = glfwGetTime();
	auto theta = PI, phi = 0.0f;

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
		   && !glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);

		auto present = glfwGetTime();
		auto time = static_cast<float>(present - last_moment);

		auto mouse_x = 0.0, mouse_y = 0.0;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

		theta += MOUSE_SPEED * (WIDTH / 2 - mouse_x);
		phi   += MOUSE_SPEED * (HEIGHT / 2 - mouse_y);

		auto normal = vec3
		(
			-sin(theta) * sin(phi),
			 cos(phi),
			-cos(theta) * sin(phi)
		);

		auto x_offset = vec3(-cos(theta), 0, sin(theta));

		auto z_offset = vec3
		(
			sin(theta) * cos(phi),
			sin(phi),
			cos(theta) * cos(phi)
		);

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera += 0.025f * KEY_SPEED * normal;

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera -= 0.025f * KEY_SPEED * normal;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera += time * KEY_SPEED * z_offset;

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera -= time * KEY_SPEED * z_offset;

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera += time * KEY_SPEED * x_offset;

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera -= time * KEY_SPEED * x_offset;

		if (ROTATE)
			Animation = rotate(Animation, radians(time * 10), rotation_axis);

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			Animation = rotate(Animation, radians(1.0f), vec3(0, 1, 0));

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			Animation = rotate(Animation, radians(1.0f), vec3(0, -1, 0));

		auto MVP = Projection * View * Model * Animation;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

 		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

 	 	glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		printText2D("creatiVE Summer School", 10, HEIGHT * 0.8, 25);
		printText2D("8th Edition, 2019", WIDTH * 0.45, 10, 20);

		glDisableVertexAttribArray(0);
 		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

		View = lookAt(camera, camera + z_offset, normal);
		last_moment = present;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanupText2D();
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &color_buffer);
	glDeleteBuffers(1, &uv_buffer);
	glDeleteBuffers(1, &normal_buffer);

	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vertex_array);
	glfwDestroyWindow(window);
	glfwTerminate();
}

