//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2018  DRNSF contributors
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

DRNSF_DECLARE_EMBED(frameonly_vert);
DRNSF_DECLARE_EMBED(frameonly_frag);

namespace drnsf {
namespace render {

const float cube_vb_data[] = {
    -1, -1, -1,
    +1, -1, -1,
    +1, +1, -1,
    -1, +1, -1,
    -1, -1, +1,
    +1, -1, +1,
    +1, +1, +1,
    -1, +1, +1
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

// declared in render.hh
void frameonly_fig::draw(const env &e)
{
    if (!s_vao.ok) {
        glBindVertexArray(s_vao);
        glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        s_vao.ok = true;
    }

    if (!s_vbo.ok) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, s_vbo);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            sizeof(cube_vb_data),
            cube_vb_data,
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        s_vbo.ok = true;
    }

    if (!s_prog.ok) {
        gl::vert_shader vs;
        compile_shader(
            vs,
            embed::frameonly_vert::data,
            embed::frameonly_vert::size
        );

        gl::frag_shader fs;
        compile_shader(
            fs,
            embed::frameonly_frag::data,
            embed::frameonly_frag::size
        );

        glAttachShader(s_prog, vs);
        glAttachShader(s_prog, fs);
        glBindAttribLocation(s_prog, 0, "a_Position");
        glBindFragDataLocation(s_prog, 0, "f_Color");
        glLinkProgram(s_prog);
        s_matrix_uni = glGetUniformLocation(s_prog, "u_Matrix");

        s_prog.ok = true;
    }

    glPointSize(4);
    DRNSF_ON_EXIT { glPointSize(1); };

    glUseProgram(s_prog);
    auto matrix = e.projection * e.view * m_matrix;
    glUniformMatrix4fv(s_matrix_uni, 1, false, &matrix[0][0]);
    glBindVertexArray(s_vao);
    glDrawArrays(GL_POINTS, 0, 8);
    glBindVertexArray(0);
    glUseProgram(0);
}

const gfx::frame::ref &frameonly_fig::get_frame() const
{
    return m_frame;
}

void frameonly_fig::set_frame(gfx::frame::ref frame)
{
    if (m_frame != frame)
    {
        m_frame = std::move(frame);
        invalidate();
    }
}

const glm::mat4 &frameonly_fig::get_matrix() const
{
    return m_matrix;
}

void frameonly_fig::set_matrix(glm::mat4 matrix)
{
    if (m_matrix != matrix) {
        m_matrix = matrix;
        invalidate();
    }
}

}
}