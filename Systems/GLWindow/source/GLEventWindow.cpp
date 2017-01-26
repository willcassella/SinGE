// GLEventWindow.cpp

#include <Engine/UpdatePipeline.h>
#include <Engine/SystemFrame.h>
#include <Engine/ProcessingFrame.h>
#include <Engine/Components/Logic/CInput.h>
#include "../include/GLWindow/GLEventWindow.h"

namespace sge
{
    GLFWwindow* create_sge_opengl_window(const char* title, int width, int height)
    {
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
        return glfwCreateWindow(width, height, title, nullptr, nullptr);
    }

    GLEventWindow::GLEventWindow()
        : _has_focus(true),
        _window(nullptr)
    {
    }

    GLEventWindow::GLEventWindow(GLFWwindow* window)
        : GLEventWindow()
    {
        set_window(window);
    }

    GLEventWindow::~GLEventWindow()
    {
        unset_window();
    }

    void GLEventWindow::set_window(GLFWwindow* window)
    {
        // Remove the currently active window (if any)
        unset_window();
        if (!window)
        {
            return;
        }

        // Set up the given window on this window
        _window = window;
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, &glfw_window_resize_callback);
        glfwSetWindowFocusCallback(window, &glfw_window_focus_callback);
    }

    void GLEventWindow::unset_window()
    {
        if (!_window)
        {
            return;
        }

        glfwSetWindowFocusCallback(_window, nullptr);
        glfwSetFramebufferSizeCallback(_window, nullptr);
        glfwSetWindowUserPointer(_window, nullptr);
        _window = nullptr;
    }

    void GLEventWindow::set_resize_callback(std::function<ResizeCallbackFn> resize_callback)
    {
        _resize_callback = std::move(resize_callback);
    }

    void GLEventWindow::register_pipeline(UpdatePipeline& pipeline)
    {
        pipeline.register_system_fn("gl_window_input", this, &GLEventWindow::process_input);
    }

    void GLEventWindow::set_bindings(InputBindings bindings)
    {
        _bindings = std::move(bindings);
    }

    void GLEventWindow::glfw_window_resize_callback(GLFWwindow* window, int width, int height)
    {
        auto* gl_window = static_cast<GLEventWindow*>(glfwGetWindowUserPointer(window));

        if (gl_window->_resize_callback)
        {
            gl_window->_resize_callback(*gl_window, width, height);
        }
    }

    void GLEventWindow::glfw_window_focus_callback(GLFWwindow* window, int has_focus)
    {
        auto* event_window = static_cast<GLEventWindow*>(glfwGetWindowUserPointer(window));
        event_window->_has_focus = (has_focus == GLFW_TRUE);
    }

    void GLEventWindow::process_input(SystemFrame& frame, float /*time*/, float /*dt*/)
    {
        if (!_has_focus)
        {
            return;
        }

        frame.process_entities([this](
            ProcessingFrame& pframe,
            EntityId /*entity*/,
            const CInput& input)
        {
            // For each keyboard binding
            for (const auto& key : this->_bindings.bindings)
            {
                if (glfwGetKey(this->_window, key.first) == GLFW_PRESS)
                {
                    pframe.create_tag(input.id(), CInput::FActionEvent{ key.second });
                }
            }

            // For each mouse key binding
            for (const auto& mouse_key : this->_bindings.mouse_bindings)
            {
                if (glfwGetMouseButton(this->_window, mouse_key.first) == GLFW_PRESS)
                {
                    input.create_tag(CInput::FActionEvent{ mouse_key.second });
                }
            }

            // Get the cursor position
            double x, y;
            int max_x, max_y;
            glfwGetCursorPos(this->_window, &x, &y);
            glfwGetWindowSize(this->_window, &max_x, &max_y);

            // For each mouse x-axis binding
            for (const auto& mouse_x : this->_bindings.mouse_x_bindings)
            {
                input.create_tag(CInput::FAxisEvent{ mouse_x, static_cast<float>(x), static_cast<float>(max_x) });
            }

            // For each mouse y-axis binding
            for (const auto& mouse_y : this->_bindings.mouse_y_bindings)
            {
                input.create_tag(CInput::FAxisEvent{ mouse_y, static_cast<float>(y), static_cast<float>(max_y) });
            }
        });
    }
}
