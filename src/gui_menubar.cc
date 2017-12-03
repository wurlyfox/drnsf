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
#include <algorithm>
#include "gui.hh"

namespace drnsf {
namespace gui {

const int item_width = 64;
const int item_font_size = 12;

// declared in gui.hh
void menubar::draw_2d(int width, int height, cairo_t *cr)
{
    int x = 0;
    cairo_select_font_face(
        cr,
        "Monospace",
        CAIRO_FONT_SLANT_NORMAL,
        CAIRO_FONT_WEIGHT_NORMAL
    );
    cairo_set_font_size(cr, item_font_size);
    for (auto item : m_items) {
        /*if (!item->m_enabled) {
            cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
        } else */if (item == m_hover_item) {
            cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
            cairo_rectangle(cr, x, 0, item_width, height);
            cairo_fill(cr);
            cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        } else {
            cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
        }
        cairo_text_extents_t area;
        cairo_text_extents(cr, item->m_text.c_str(), &area);
        cairo_move_to(
            cr,
            x + item_width / 2 - area.width / 2,
            height - area.height / 2
        );
        cairo_show_text(cr, item->m_text.c_str());
        x += item_width;
    }
}

// declared in gui.hh
void menubar::mousemove(int x, int y)
{
    item *selection;
    if (x < 0) {
        selection = nullptr;
    } else {
        int i = x / item_width;
        if (i < 0 || i >= static_cast<int>(m_items.size())) {
            selection = nullptr;
        } else {
            selection = m_items[i];
        }
    }
    if (selection != m_hover_item) {
        m_hover_item = selection;
        if (selection && m_open_item) {
            m_open_item->hide();
            m_open_item = selection;
            m_open_item->show_at_mouse();
        }
        invalidate();
    }
}

// declared in gui.hh
void menubar::mousebutton(int button, bool down)
{
    if (button != 1 || !down) return;
    if (m_hover_item && !m_open_item) {
        m_open_item = m_hover_item;
        m_open_item->show_at_mouse();
    } else if (m_open_item) {
        m_open_item->hide();
        m_open_item = nullptr;
    }
}

// declared in gui.hh
menubar::menubar(window &wnd) :
    widget_2d(wnd, layout::fill()),
    m_wnd(wnd)
{
    if (m_wnd.m_menubar) {
        throw 0; //FIXME
    }
    m_wnd.m_menubar = this;
    widget_2d::show();
}

// declared in gui.hh
menubar::~menubar()
{
    m_wnd.m_menubar = nullptr;
}

// declared in gui.hh
menubar::item::item(menubar &menubar, std::string text) :
    m_menubar(menubar),
    m_text(std::move(text))
{
    m_menubar.m_items.push_back(this);
    m_menubar.invalidate();
}

// declared in gui.hh
menubar::item::~item()
{
    m_menubar.m_items.erase(
        std::find(m_menubar.m_items.begin(), m_menubar.m_items.end(), this)
    );
    if (m_menubar.m_hover_item == this) {
        m_menubar.m_hover_item = nullptr;
    }
    if (m_menubar.m_open_item == this) {
        m_menubar.m_open_item = nullptr;
    }
    m_menubar.invalidate();
}

}
}