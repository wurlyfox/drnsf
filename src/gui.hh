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

#pragma once

/*
 * gui.hh
 *
 * FIXME explain
 */

#include <unordered_set>
#include <cairo.h>
#include "../imgui/imgui.h"
#include "gl.hh"

namespace drnsf {
namespace gui {

/*
 * gui::init
 *
 * This function should be called before any gui::* objects are created. The
 * `argc' and `argv' parameters to `main' should be given by-reference, as
 * some GUI targets require this.
 */
void init(int &argc, char **&argv);

/*
 * gui::run
 *
 * Runs the main UI event loop. This function returns after gui::end is called.
 */
void run();

/*
 * gui::end
 *
 * Terminates the current gui::run iteration. This function will return, but
 * the run() call will finish its current loop. On some systems, the loop will
 * continue running until all pending events are handled.
 */
void end();

/*
 * gui::sys_handle
 *
 * This type is used in place of a particular frontend's object handle types,
 * such as Window (XLIB), GtkWidget (GTK3), HWND (WINAPI), etc.
 */
#if USE_X11
using sys_handle = unsigned long;
#else
using sys_handle = void *;
#endif

/*
 * gui::keycode
 *
 * FIXME explain
 */
enum class keycode : int {
#if USE_X11
    backspace = 0xFF08,
    tab       = 0xFF09,
    enter     = 0xFF0D,
    escape    = 0xFF1B,

    left_arrow  = 0xFF51,
    right_arrow = 0xFF53,
    up_arrow    = 0xFF52,
    down_arrow  = 0xFF54,

    home      = 0xFF50,
    end       = 0xFF57,
    page_up   = 0xFF55,
    page_down = 0xFF56,
    insert    = 0xFF63,
    del       = 0xFFFF,

    l_shift = 0xFFE1,
    r_shift = 0xFFE2,
    l_ctrl  = 0xFFE3,
    r_ctrl  = 0xFFE4,
    l_meta  = 0xFFE7,
    r_meta  = 0xFFE8,
    l_alt   = 0xFFE9,
    r_alt   = 0xFFEA,
    l_super = 0xFFEB,
    r_super = 0xFFEC,

    // Only adding letters needed by ImGui for now.
    A = 'A',
    C = 'C',
    V = 'V',
    X = 'X',
    Y = 'Y',
    Z = 'Z'
#endif
};

/*
 * gui::layout
 *
 * FIXME explain
 */
struct layout {
    /*
     * gui::layout::h_pin
     * gui::layout::v_pin
     *
     * These types define a horizontal or vertical position for one side of a
     * widget (left, right, top, or bottom) relative to its parent container. The
     * `factor' field defines the anchor point in the parent container; a value of
     * 0.0f corresponds to the top or left side of the container, while a value of
     * 1.0f corresponds to the bottom or right side of the container. The `offset'
     * field specifies an offset from that point, in pixels, for this location.
     * X values increase from left to right, and Y values increase from top to
     * bottom (NOT bottom to top).
     *
     * For example, some possible pin values:
     *
     * `h_pin( 0.0f, 10 )`  10 pixels from the left side of the container
     * `h_pin( 0.5f, 0 )`   the horizontal center of the container
     * `h_pin( 1.0f, -64 )` 64 pixels from the right side of the container
     * `v_pin( 1.0f, 0 )`   the bottom of the container
     */
    struct h_pin {
        float factor;
        int offset;
    };
    struct v_pin {
        float factor;
        int offset;
    };

    /*
     * gui::layout::h_bar
     * gui::layout::v_bar
     *
     * These types consist of h_pin or v_pin pairs, respectively. An h_bar defines
     * the left and right side positions of a widget relative to its container, and
     * a v_bar defines the top and bottom side positions.
     */
    struct h_bar {
        h_pin left;
        h_pin right;
    };
    struct v_bar {
        v_pin top;
        v_pin bottom;
    };

    h_bar h;
    v_bar v;

    // FIXME comment
    static constexpr layout fill()
    {
        return {
            { { 0.0f, 0 }, { 1.0f, 0 } },
            { { 0.0f, 0 }, { 1.0f, 0 } }
        };
    }

    static constexpr layout grid(
        int h_num_offset,
        int h_num_width,
        int h_denom,
        int v_num_offset,
        int v_num_width,
        int v_denom)
    {
        float h_denom_f = h_denom;
        float v_denom_f = v_denom;
        return {
            {
                { h_num_offset / h_denom_f, 0 },
                { (h_num_offset + h_num_width) / h_denom_f, 0 }
            },
            {
                { v_num_offset / v_denom_f, 0 },
                { (v_num_offset + v_num_width) / v_denom_f, 0 }
            }
        };
    }
};

// defined later in this file
class container;
class window;

/*
 * gui::widget
 *
 * Common base class for all widget types.
 */
class widget : public util::nocopy {
    friend class container;
    friend class window;
    friend void run();

private:
    // (s-var) s_all_widgets
    // The set of all existing widget objects, whether visible or not.
    static std::unordered_set<widget *> s_all_widgets;

    // (s-var) s_hover_widget
    // The widget currently under the mouse cursor. This is null if there is no
    // such widget.
    static widget *s_hover_widget;

    // (var) m_real_width, m_real_height
    // The actual height of the widget, in pixels.
    int m_real_width;
    int m_real_height;

    // (func) apply_layout
    // Applies the widget's layout to its size and position based on the area
    // of the container given as parameters.
    void apply_layout(int ctn_x, int ctn_y, int ctn_w, int ctn_h);

protected:
    // (var) m_handle
    // The internal system handle for this widget. The base widget class will
    // release this handle or destroy the associated system object.
    sys_handle m_handle;

    // (var) m_parent
    // A reference to the container this widget exists inside of. All widgets
    // must exist within a container.
    container &m_parent;

    // (var) m_layout
    // The layout area which defines the location and size of this widget
    // relative to its container.
    layout m_layout{};

    // (func) mousemove
    // Called when the mouse moves into or while inside of the widget.
    //
    // The default implementation performs no operation.
    virtual void mousemove(int x, int y) {}

    // (func) mouseleave
    // Called whenever the mouse moves out of the widget. This function is not
    // called if the reason for the mouse leaving the widget is because the
    // widget is being destroyed.
    //
    // The default implementation performs no operation.
    virtual void mouseleave() {}

    // (func) mousewheel
    // Called when the mouse wheel is scrolled vertically. Whether this happens
    // to the focused widget or to the widget currently under the mouse could
    // vary by operating system.
    //
    // The default implementation performs no operation.
    virtual void mousewheel(int delta_y) {}

    // (func) mousebutton
    // Called when a mouse button is pressed or released while this widget has
    // focus. The button press could potentially be one which causes the widget
    // to gain focus.
    //
    // The default implementation performs no operation.
    //
    // X11: When a mouse button is pressed on a widget, that widget gains mouse
    // grab status. As part of this happening, the widget will first receive a
    // `mouseleave' call, followed by a `mousemove' call, then finally a call to
    // this function. When the final mouse button is released, the widget loses
    // mouse grab status. If the mouse is still over the widget at this time,
    // it will receive both a `mouseleave' call followed by a `mousemove' call.
    virtual void mousebutton(int number, bool down) {}

    // (func) key
    // Called when a key is pressed or released while this widget has focus.
    //
    // The default implementation performs no operation.
    virtual void key(keycode code, bool down) {}

    // (func) text
    // Called when text is entered into the widget. This is different from key
    // input, as multiple keys in series could input one character, or another
    // sequence of different characters. For more information, read about input
    // methods such as IME and methods used for inputting characters often used
    // in southeast asian languages.
    //
    // The default implementation performs no operation.
    virtual void text(const char *str) {}

    // (func) on_resize
    // Called when the widget is resized. This may be caused by the parent
    // container being resized, causing the child widgets to re-apply their
    // defined layouts.
    //
    // The default implementation performs no operation.
    virtual void on_resize(int width, int height) {}

#if USE_X11
    // (var) m_dirty
    // True if the widget needs to be redrawn. See on_draw below for more
    // details. Generally, you should not set this variable false; this is
    // handled for you by gui::run.
    bool m_dirty = true;

    // (func) on_draw
    // Called when the widget's contents should be redrawn. This function is
    // specific to X11. When this function returns, the widget is assumed to
    // have been redrawn successfully. Do not set m_dirty to true during the
    // execution of on_draw; this will have no effect.
    //
    // A widget can flag itself as needing redrawing by setting m_dirty. This
    // flag may also be set externally, for example in response to ExposeEvent.
    virtual void on_draw() = 0;
#endif

    // (func) update
    // FIXME explain
    virtual int update(int delta_ms)
    {
        return INT_MAX;
    }

    // (explicit ctor)
    // Constructs the base widget data for use in the given parent.
    //
    // The derived class's constructor should:
    //
    // - Create the actual underlying system object.
    // - Set m_handle.
    // - Call set_layout.
    explicit widget(container &parent);

public:
    // (dtor)
    // Destroys the widget (including the internal system handle), removing it
    // from its parent. A visible widget will vanish if destroyed.
    //
    // If a derived type wishes to release the underlying system object itself,
    // it should set m_handle null before this destructor runs.
    ~widget();

    // (func) show
    // Shows the widget, if it is hidden. By default, every widget is
    // hidden when first constructed.
    void show();

    // (func) hide
    // Hides the widget, if it is visible. By default, every widget is already
    // hidden when constructed.
    void hide();

    // (func) get_layout, set_layout
    // Gets or sets the widget's layout. The layout determines how the widget
    // is positioned and sized relative to its parent widget. When the layout
    // is changed, the widget's location will be reconfigured according to the
    // new layout.
    //
    // See the definition for the gui::layout type for more details.
    const layout &get_layout() const;
    void set_layout(layout layout);

    // (func) get_real_size
    // Gets the real size of the widget in pixels, as determined by its layout
    // and its parent container's real size.
    void get_real_size(int &width, int &height);

    // (func) get_screen_pos
    // Gets the coordinates of this widget relative to the entire screen. These
    // coordinates could be used for `popup::show_at(x, y)', for example.
    void get_screen_pos(int &x, int &y);
};

/*
 * gui::container
 *
 * FIXME explain
 */
class container : private util::nocopy {
    friend class widget;

private:
    // (func) m_widgets
    // The set of widgets contained inside this container.
    std::unordered_set<widget *> m_widgets;

protected:
    // (func) apply_layouts
    // Reapplies the child widgets' layouts according to the container's child
    // area (see get_child_area below). This should be called by the derived
    // type whenever the child area changes so that this change can be
    // propagated to the child widgets.
    void apply_layouts();

public:
    // (pure func) get_container_handle
    // FIXME explain
    virtual sys_handle get_container_handle() = 0;

    // (pure func) get_child_area
    // Gets the area of the container which the child widgets should use to size
    // and position themselves according to their respective layouts.
    //
    // Generally, this should return (0, 0) and the container's width and
    // height, but this may be different if the area for the child widgets is a
    // subsection of the container's full area. For instance, this may be the
    // case for a window with a non-native menubar.
    virtual void get_child_area(
        int &ctn_x,
        int &ctn_y,
        int &ctn_w,
        int &ctn_h) = 0;
};

/*
 * gui::composite
 *
 * FIXME explain
 *
 * Composite widgets do not receive any user input events such as mouse clicks,
 * mouse movement, or key presses.
 */
class composite : private widget, public container {
private:
    // input events not available for this type of widget
    void mousemove(int x, int y) final override {}
    void mouseleave() final override {}
    void mousewheel(int delta_y) final override {}
    void mousebutton(int number, bool down) final override {}
    void key(keycode code, bool down) final override {}
    void text(const char *str) final override {}

    // (func) on_resize
    // Implements on_resize to apply the change in container size to the child
    // widgets.
    //
    // Derived types which override this function MUST call up to this base
    // implementation to ensure child widgets are moved resized appropriately.
    void on_resize(int width, int height) override;

#if USE_X11
    // (func) on_draw
    // Implements on_draw to do nothing. Derived types may not override this
    // to perform any drawing of their own.
    void on_draw() final override {}
#endif

public:
    // (ctor)
    // FIXME explain
    composite(container &parent, layout layout);

    // (func) get_container_handle
    // FIXME explain
    sys_handle get_container_handle() override;

    // (func) get_child_area
    // Implements gui::container::get_child_area.
    void get_child_area(
        int &ctn_x,
        int &ctn_y,
        int &ctn_w,
        int &ctn_h) override;

    using widget::show;
    using widget::hide;
    using widget::get_layout;
    using widget::set_layout;
    using widget::get_real_size;
    using widget::get_screen_pos;
};

// defined later in this file
class menubar;

/*
 * gui::window
 *
 * FIXME explain
 */
class window : public container {
    friend class menubar;
    friend void run();

private:
    // (s-var) s_all_windows
    // The set of all existing window objects, whether visible or not.
    static std::unordered_set<window *> s_all_windows;

    // (var) m_handle
    // Internal handle to the underlying system window.
    sys_handle m_handle;

    // (var) m_menubar
    // A pointer to the menubar associated with this window, or null if there
    // is none.
    gui::menubar *m_menubar = nullptr;

    // (var) m_width, m_height
    // The size of the popup window, in pixels.
    int m_width;
    int m_height;

public:
    // (explicit ctor)
    // FIXME explain
    explicit window(const std::string &title, int width, int height);

    // (dtor)
    // FIXME explain
    ~window();

    // (func) show
    // FIXME explain
    void show();

    // (func) show_dialog
    // FIXME explain
    void show_dialog();

    // (func) get_container_handle
    // FIXME explain
    sys_handle get_container_handle() override;

    // (func) get_child_area
    // Implements gui::container::get_child_area.
    void get_child_area(
        int &ctn_x,
        int &ctn_y,
        int &ctn_w,
        int &ctn_h) override;
};

/*
 * gui::popup
 *
 * This class implements a "popup window". This is similar to a normal window,
 * however it cannot have a menubar associated with it, and usually does not
 * have any usual window stylings such as a title bar, close button, etc.
 *
 * This kind of window is useful for implementing dropdowns, tooltips, menus,
 * etc.
 */
class popup : public container {
    friend void run();

private:
    // (s-var) s_all_popups
    // The set of all existing popup objects, whether visible or not.
    static std::unordered_set<popup *> s_all_popups;

    // (var) m_handle
    // A handle to the underlying system window.
    sys_handle m_handle;

    // (var) m_width, m_height
    // The size of the popup window, in pixels.
    int m_width;
    int m_height;

public:
    // (explicit ctor)
    // FIXME explain
    explicit popup(int width, int height);

    // (dtor)
    // Destroys the popup. If it is visible, it will vanish from the screen.
    ~popup();

    // (func) show_at
    // Makes the popup visible at the specified location in screen coordinates.
    // If the popup is already visible, it is moved to the location.
    //
    // The location specified matches the top-left of the popup.
    void show_at(int x, int y);

    // (func) hide
    // Makes the popup invisible. If it is already invisible, no change occurs.
    void hide();

    // (func) set_size
    // Sets the size of the popup, with the same behavior as the size passed to
    // the constructor.
    void set_size(int width, int height);

    // (func) get_container_handle
    // FIXME explain
    sys_handle get_container_handle() override;

    // (func) get_child_area
    // Implements gui::container::get_child_area.
    void get_child_area(
        int &ctn_x,
        int &ctn_y,
        int &ctn_w,
        int &ctn_h) override;
};

/*
 * gui::widget_gl
 *
 * FIXME explain
 */
class widget_gl : private widget {
private:
#if USE_X11
    // (func) on_draw
    // Implements on_draw to call draw_gl.
    //
    // Derived types may not override on_draw, but should instead override
    // draw_gl.
    void on_draw() final override;
#endif

protected:
    // (pure func) draw_gl
    // Draws the contents of this widget to the specified renderbuffer. Derived
    // classes must implement this method. This function will not be called on
    // every frame, only when the widget is resized or after invalidate is
    // called (see below).
    //
    // All global GL state will be default when this function is called. When
    // overriding it, you must ensure you return in such a state as well, or
    // this guarantee will not be met for widgets drawn afterwards.
    virtual void draw_gl(int width, int height, gl::renderbuffer &rbo) = 0;

    // (func) invalidate
    // Marks the current rendered output of this widget as out-of-date or stale
    // so that draw_gl will be called again. If the widget is not currently
    // visible to the user (due to being hidden, obscured, off-screen, etc)
    // then the call to draw_gl may be deferred until this is not the case, or
    // it may never be called if the widget is destroyed before such a case
    // occurs.
    void invalidate();

public:
    // (ctor)
    // FIXME explain
    widget_gl(container &parent, layout layout);

    using widget::show;
    using widget::hide;
    using widget::get_layout;
    using widget::set_layout;
    using widget::get_real_size;
    using widget::get_screen_pos;
};

/*
 * gui::widget_2d
 *
 * FIXME explain
 */
class widget_2d : private widget {
    friend class window;

private:
#if USE_X11
    // (func) on_draw
    // Implements on_draw to call draw_2d.
    //
    // Derived types may not override on_draw, but should instead override
    // draw_2d.
    void on_draw() final override;
#endif

protected:
    // (pure func) draw_2d
    // Draws the contents of this widget. This function will not be called on
    // every frame, only when the widget is resized or after invalidate is
    // called (see below).
    virtual void draw_2d(int width, int height, cairo_t *cr) = 0;

    // (func) invalidate
    // Marks the current rendered output of this widget as out-of-date or stale
    // so that draw_2d will be called again. If the widget is not currently
    // visible to the user (due to being hidden, obscured, off-screen, etc)
    // then the call to draw_2d may be deferred until this is not the case, or
    // it may never be called if the widget is destroyed before such a case
    // occurs.
    void invalidate();

public:
    // (ctor)
    // FIXME explain
    widget_2d(container &parent, layout layout);

    using widget::show;
    using widget::hide;
    using widget::get_layout;
    using widget::set_layout;
    using widget::get_real_size;
    using widget::get_screen_pos;
};

/*
 * gui::widget_im
 *
 * FIXME explain
 */
class widget_im : private widget_gl {
private:
    // (var) m_im
    // The ImGui context for this widget. Each `widget_im' instance has its own
    // context so that they do not interfere with eachother.
    ImGuiContext *m_im;

    // (var) m_io
    // A pointer to a structure for the ImGui context which is used to provide
    // input into ImGui, such as mouse position or keystroke inputs.
    ImGuiIO *m_io;

    // (var) m_pending_time
    // The number of milliseconds passed since the last ImGui render. This
    // accumulates from successive calls to `update' (see below) and is then
    // consumed when `ImGui::NewFrame' is run.
    int m_pending_time = 0;

    // (var) m_remaining_time
    // The number of milliseconds remaining until the next update should be
    // performed. Usually this should be (time per update - m_pending_time) but
    // this could be reduced to zero if an earlier update is needed, such as
    // in response to user input.
    int m_remaining_time = 0;

    // (var) m_render_ready
    // True if ImGui::Render() has been called and is ready for drawing to the
    // widget, false otherwise. This is necessary because drawing occurs in
    // draw_gl(), but ImGui::NewFrame()...ImGui::Render() occurs in update().
    bool m_render_ready = false;

    // (func) draw_gl
    // FIXME explain
    void draw_gl(int width, int height, gl::renderbuffer &rbo) final override;

    // (func) mousemove
    // FIXME explain
    void mousemove(int x, int y) final override;

    // (func) mouseleave
    // FIXME explain
    void mouseleave() final override;

    // (func) mousewheel
    // FIXME explain
    void mousewheel(int delta_y) final override;

    // (func) mousebutton
    // FIXME explain
    void mousebutton(int number, bool down) final override;

    // (func) key
    // FIXME explain
    void key(keycode code, bool down) final override;

    // (func) text
    // FIXME explain
    void text(const char *str) final override;

    // (func) on_resize
    // Clears out the m_render_ready status and reduces the remaining update
    // time to zero so that ImGui can be updated for the new size.
    void on_resize(int width, int height) final override;

    // (func) update
    // Implements `update' to handle updating ImGui on a regular interval.
    // ImGui is designed for debugging games, and its design necessitates
    // calling imgui every "frame", which is not a concept present in this
    // gui code.
    int update(int delta_ms) final override;

protected:
    // (pure func) frame
    // FIXME explain
    virtual void frame() = 0;

public:
    // (ctor)
    // FIXME explain
    widget_im(container &parent, layout layout);

    // (dtor)
    // FIXME explain
    ~widget_im();

    using widget_gl::show;
    using widget_gl::hide;
    using widget_gl::get_layout;
    using widget_gl::set_layout;
    using widget_gl::get_real_size;
    using widget_gl::get_screen_pos;
};

/*
 * gui::label
 *
 * FIXME explain
 */
class label : private widget_2d {
private:
    // (var) m_text
    // The text displayed by the label.
    std::string m_text;

    // (func) draw_2d
    // Implements draw_2d to render the label text.
    void draw_2d(int width, int height, cairo_t *cr) override;

public:
    // (ctor)
    // FIXME explain
    label(container &parent, layout layout, const std::string &text = "");

    // (func) set_text
    // Sets the text displayed by the label.
    void set_text(const std::string &text);

    using widget_2d::show;
    using widget_2d::hide;
    using widget_2d::get_layout;
    using widget_2d::set_layout;
    using widget_2d::get_real_size;
    using widget_2d::get_screen_pos;
};

/*
 * gui::treeview
 *
 * FIXME explain
 */
class treeview : private widget_im {
public:
    // inner class defined later in this file
    class node;

private:
    // (var) m_nodes
    // The list of pointers to the top-level nodes in the treeview.
    std::list<node *> m_nodes;

    // (var) m_selected_node
    // FIXME explain
    node *m_selected_node = nullptr;

    // (func) frame
    // FIXME explain
    void frame() final override;

public:
    // (ctor)
    // FIXME explain
    treeview(container &parent, layout layout);

    using widget_im::show;
    using widget_im::hide;
    using widget_im::get_layout;
    using widget_im::set_layout;
    using widget_im::get_real_size;
    using widget_im::get_screen_pos;
};

/*
 * gui::treeview::node
 *
 * FIXME explain
 */
class treeview::node : private util::nocopy {
    friend class treeview;

private:
    // (var) m_view
    // The treeview this node is contained within.
    treeview &m_view;

    // (var) m_parent
    // A pointer to the parent node of this node, or null if this is a
    // top-level node.
    node *m_parent;

    // (var) m_subnodes
    // A list of pointers to the child nodes of this node.
    std::list<node *> m_subnodes;

    // (var) m_iter
    // An iterator pointing to the pointer to this subnode in the parent's
    // node list, or the treeview's node list if this is a top-level node.
    std::list<node *>::iterator m_iter;

    // (var) m_expanded
    // True if this node is expanded (its child nodes are visible) or false
    // otherwise.
    bool m_expanded = false;

    // (var) m_text
    // The text displayed for this tree node.
    std::string m_text;

    // (func) run
    // Handles and displays this treenode (for ImGui).
    void run();

public:
    // (explicit ctor)
    // FIXME explain
    explicit node(treeview &parent);

    // (explicit ctor)
    // FIXME explain
    explicit node(node &parent);

    // (dtor)
    // FIXME explain
    ~node();

    // (func) set_text
    // FIXME explain
    void set_text(const std::string &text);

    // (event) on_select
    // FIXME explain
    util::event<> on_select;

    // (event) on_deselect
    // FIXME explain
    util::event<> on_deselect;
};

/*
 * gui::menu
 *
 * FIXME explain
 */
class menu : private popup, private widget_2d {
    friend class menubar;

public:
    // inner class defined later in this file
    class item;

private:
    // (var) m_items
    // The list of pointers to all of the contained menu items.
    std::vector<item *> m_items;

    // (var) m_active_item
    // A pointer to the currently active item in the menu. This is null if
    // there is no such item. The active item is whatever is currently under
    // the mouse cursor, or potentially an item whose submenu is currently
    // open and active.
    item *m_active_item = nullptr;

    // (func) draw_2d
    // Implements draw_2d to draw the menu items.
    void draw_2d(int width, int height, cairo_t *cr) final override;

    // (func) mousemove
    // Implements mousemove for hovering over menu items.
    void mousemove(int x, int y) final override;

    // (func) mouseleave
    // Implements mouseleave for when the mouse leaves the menu entirely.
    void mouseleave() final override;

    // (func) mousebutton
    // Implements mousebutton for clicking to activate menu items.
    void mousebutton(int button, bool down) final override;

    // Solve ambiguity of popup::hide vs widget_2d::hide. The widget base
    // should never be hidden.
    using popup::hide;

public:
    // (ctor)
    // Initializes an empty menu.
    menu();
};

/*
 * gui::menu::item
 *
 * FIXME explain
 */
class menu::item : private util::nocopy {
    friend class menu;

private:
    // (var) m_menu
    // The menu this item exists in.
    menu &m_menu;

    // (var) m_text
    // The text displayed on this menu item.
    std::string m_text;

    // (var) m_enabled
    // If false, this menu item does not respond to clicks and may be displayed
    // in a disabled style, such as with gray text.
    bool m_enabled = true;

    // (func) on_activate
    // This function is called when the menu item is clicked by the mouse or
    // activated by a key press.
    //
    // The default implementation performs no action.
    virtual void on_activate() {}

public:
    // (explicit ctor)
    // Initializes a menu item with the given text, and adds it to the given
    // menu.
    explicit item(menu &menu, std::string text);

    // (dtor)
    // Removes the menu item from its associated menu.
    ~item();

    // (func) set_text
    // Sets the text of the menu item.
    void set_text(std::string text);

    // (func) set_enabled
    // Sets whether or not the menu item is enabled. Disabled items cannot be
    // activated by the user, and may appear grayed out.
    void set_enabled(bool enabled);
};

/*
 * gui::menubar
 *
 * FIXME explain
 */
class menubar : private widget_2d {
    friend class window;

public:
    // inner class defined later in this file
    class item;

private:
    // (var) m_wnd
    // The window this menubar is attached to.
    window &m_wnd;

    // (var) m_items
    // The list of pointers to all of the contained menubar items.
    std::vector<item *> m_items;

    // (var) m_hover_item
    // A pointer to the currently selected menubar item, whether by mouse-hover
    // or keyboard input. If there is no such item, this pointer is null.
    item *m_hover_item = nullptr;

    // (var) m_open_item
    // A pointer to the menubar item which has its menu open, or null if there
    // is none.
    item *m_open_item = nullptr;

    // (func) draw_2d
    // Implements draw_2d to draw the menubar items.
    void draw_2d(int width, int height, cairo_t *cr) final override;

    // (func) mousemove
    // Implements mousemove for hovering over menubar items.
    void mousemove(int x, int y) final override;

    // (func) mouseleave
    // Implements mouseleave for when the mouse leaves the menubar entirely.
    void mouseleave() final override;

    // (func) mousebutton
    // Implements mouse click for clicking on menubar items.
    void mousebutton(int button, bool down) final override;

public:
    // (explicit ctor)
    // Initializes an empty menubar and associates it with the given window.
    // The window must not already have a menubar associated with it.
    explicit menubar(window &wnd);

    // (dtor)
    // Removes the menubar from the associated window.
    ~menubar();
};

/*
 * gui::menubar::item
 *
 * FIXME explain
 */
class menubar::item : public menu {
    friend class menubar;

private:
    // (var) m_menubar
    // The menubar this item exists in.
    menubar &m_menubar;

    // (var) m_text
    // The text displayed on this menubar item.
    std::string m_text;

public:
    // (explicit ctor)
    // Initializes a menubar item with the given text, and adds it to the end
    // of the given menubar.
    explicit item(menubar &menubar, std::string text);

    // (dtor)
    // Removes the menubar item from its associated menubar.
    ~item();
};

/*
 * gui::show_open_dialog
 *
 * FIXME explain
 */
bool show_open_dialog(std::string &path);

/*
 * gui::gl_canvas
 *
 * FIXME explain
 */
class gl_canvas : private widget_gl {
private:
    void draw_gl(int width, int height, gl::renderbuffer &rbo) final override;
    void mousemove(int x, int y) final override;
    void mousewheel(int delta_y) final override;
    void mousebutton(int number, bool down) final override;
    void key(keycode code, bool down) final override;
    void text(const char *str) final override;

    int update(int) final override
    {
        invalidate();
        return INT_MAX;
    }

public:
    // (explicit ctor)
    // FIXME explain
    explicit gl_canvas(container &parent, layout layout);

    // (func) invalidate
    // FIXME explain
    using widget_gl::invalidate;

    // (event) on_render
    // FIXME explain
    util::event<int, int> on_render;

    // (event) on_mousemove
    // FIXME explain
    util::event<int, int> on_mousemove;

    // (event) on_mousewheel
    // FIXME explain
    util::event<int> on_mousewheel;

    // (event) on_mousebutton
    // FIXME explain
    util::event<int, bool> on_mousebutton;

    // (event) on_key
    // FIXME explain
    util::event<int, bool> on_key;

    // (event) on_text
    // FIXME explain
    util::event<const char *> on_text;

    using widget_gl::show;
    using widget_gl::hide;
};

// FIXME obsolete
namespace im {

using namespace ImGui;

void label(
    const std::string &text);

void subwindow(
    const std::string &id,
    const std::string &title,
    const std::function<void()> &f);

void main_menu_bar(
    const std::function<void()> &f);

void menu(
    const std::string &text,
    const std::function<void()> &f = nullptr);

void menu_item(
    const std::string &text,
    const std::function<void()> &f = nullptr);

void menu_separator();

class scope : private util::nocopy {
public:
    explicit scope(int id);
    explicit scope(void *ptr);
    ~scope();
};

}

}
}
