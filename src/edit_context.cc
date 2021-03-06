//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
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
#include "edit.hh"

namespace drnsf {
namespace edit {

// declared in edit.hh
const std::shared_ptr<res::project> &context::get_proj() const
{
    return m_proj;
}

// declared in edit.hh
void context::set_proj(std::shared_ptr<res::project> proj)
{
    if (m_proj != proj) {
        std::swap(m_proj, proj);
        on_project_change(m_proj);
    }
}

// declared in edit.hh
context::~context()
{
    for (size_t i = 0; i < m_windows.size(); i++) {
        auto window = m_windows[i];
        if (window->m_owned_by_context) {
            delete window;
            i--;
        }
    }
}

}
}
