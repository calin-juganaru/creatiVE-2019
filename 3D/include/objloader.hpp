#pragma once

#include <tuple>
#include <vector>
#include <cstdio>
#include <string>

#include <glm/glm.hpp>

#include "objloader.hpp"

using namespace std;

using glm::vec2;
using glm::vec3;
using glm::mat4;

inline tuple<vector<vec3>, vector<vec2>, vector<vec3>> load_obj(string path)
{
	vector<unsigned> vertices, uvs, normals;
	vector<vec3> temp_vertices, out_vertices;
	vector<vec2> temp_uvs, out_uvs;
	vector<vec3> temp_normals, out_normals;

	FILE* file = fopen(path.c_str(), "r");
	if (file == nullptr) return {};

	while (true)
	{
		char buffer[128];
		if (fscanf(file, "%s", buffer) == EOF) break;

		if (!strcmp(buffer, "v"))
		{
			auto vertex = vec3();
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}

		else if (!strcmp(buffer, "vt"))
		{
			auto uv = vec2();
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y;
			temp_uvs.push_back(uv);
		}

		else if (!strcmp(buffer, "vn"))
		{
			auto normal = vec3();
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}

		else if (!strcmp(buffer, "f"))
		{
			auto vertex_index = 0u, uv_index = 0u, normal_index = 0u;

			for (auto i: {0, 1, 2})
			{
				fscanf(file, "%d/%d/%d ", &vertex_index, &uv_index, &normal_index);
				vertices.push_back(vertex_index);
				uvs.push_back(uv_index);
				normals.push_back(normal_index);
			}
		}

		else
		{
			char garbage[100];
			fgets(garbage, 100, file);
		}
	}

	for (auto i = 0; i < vertices.size(); ++i)
	{
		auto vertex_index = vertices[i];
		auto uv_index = uvs[i];
		auto normal_index = normals[i];

		auto vertex = temp_vertices[vertex_index - 1];
		auto uv = temp_uvs[uv_index - 1];
		auto normal = temp_normals[normal_index - 1];

		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}

	fclose(file);
	return {out_vertices, out_uvs, out_normals};
}