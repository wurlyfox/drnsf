//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017  DRNSF contributors
//
// See the AUTHORS.md file for more details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "common.hh"
#include "render.hh"

namespace drnsf {

namespace embed {
	namespace reticle_vert {
		extern const unsigned char data[];
		extern const std::size_t size;
	}
	namespace reticle_frag {
		extern const unsigned char data[];
		extern const std::size_t size;
	}
}

namespace render {

// (internal struct) reticle_vert
// The vertex attribute type for the reticle model.
struct reticle_vert {
	float pos[3];
	enum : signed char {
		AXIS_X,
		AXIS_Y,
		AXIS_Z,
		AXIS_CUBE = -1
	} axis;
};

// (s-var) reticle_model
// The model used by the reticle. It consists of three lines, each on a
// particular axis (x, y, z; listed as 0, 1, 2). These will be colored by the
// shader as (red, green, blue).
reticle_vert reticle_model[] = {
	{ { -1.0f, 0.0f, 0.0f }, reticle_vert::AXIS_X },
	{ { +1.0f, 0.0f, 0.0f }, reticle_vert::AXIS_X },
	{ { 0.0f, -1.0f, 0.0f }, reticle_vert::AXIS_Y },
	{ { 0.0f, +1.0f, 0.0f }, reticle_vert::AXIS_Y },
	{ { 0.0f, 0.0f, -1.0f }, reticle_vert::AXIS_Z },
	{ { 0.0f, 0.0f, +1.0f }, reticle_vert::AXIS_Z },
	{ { -1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { +1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
	{ { -1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE }
};

// (s-var) s_vao
// The VAO for the reticle model.
static gl::vert_array s_vao;

// (s-var) s_vbo
// The VBO for the reticle model.
static gl::buffer s_vbo;

// (s-var) s_prog
// The GL shader program to use for the reticle model.
static gl::program s_prog;

// (s-var) s_matrix_uni
// The location of the "u_Matrix" shader uniform variable.
static int s_matrix_uni;

// (s-func) init
// Initializes the GL resources for this file if not already initialized.
// Otherwise, no operation occurs.
static void init()
{
	// Exit now if this function has been run previously.
	static bool s_is_init = false;
	if (s_is_init) return;
	s_is_init = true;

	glBindVertexArray(s_vao);
	glBindBuffer(GL_ARRAY_BUFFER,s_vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(reticle_model),
		reticle_model,
		GL_STATIC_DRAW
	);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		false,
		sizeof(reticle_vert),
		reinterpret_cast<void *>(offsetof(reticle_vert,pos))
	);
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(
		1,
		1,
		GL_BYTE,
		sizeof(reticle_vert),
		reinterpret_cast<void *>(offsetof(reticle_vert,axis))
	);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);

	gl::vert_shader vs;
	vs.compile({
		reinterpret_cast<const char *>(embed::reticle_vert::data),
		embed::reticle_vert::size
	});
	glAttachShader(s_prog,vs);

	gl::frag_shader fs;
	fs.compile({
		reinterpret_cast<const char *>(embed::reticle_frag::data),
		embed::reticle_frag::size
	});
	glAttachShader(s_prog,fs);

	glBindAttribLocation(s_prog,0,"a_Position");
	glBindAttribLocation(s_prog,1,"a_Axis");

	glLinkProgram(s_prog);

	s_matrix_uni = glGetUniformLocation(s_prog,"u_Matrix");
}

// declared in render.hh
void reticle_fig::draw(const env &e)
{
	init();

	glUseProgram(s_prog);
	glUniformMatrix4fv(s_matrix_uni,1,false,&e.matrix[0][0]);
	glBindVertexArray(s_vao);
	glDrawArrays(GL_LINES,0,sizeof(reticle_model) / sizeof(reticle_vert));
	glBindVertexArray(0);
	glUseProgram(0);
}

}
}