// GLEventWindow.h
#pragma once

#include <functional>
#include <GLFW/glfw3.h>
#include "InputBindings.h"

namespace sge
{
    struct SystemFrame;
    struct UpdatePipeline;

    /**
     * \brief Creates a GLFW opengl window with the usual SGE OpenGL window settings.
     * \param title The title for the window.
     * \param width The width of the window.
     * \param height The height of the window.
     * \return A pointer to the created window.
     */
    GLFWwindow* create_sge_opengl_window(const char* title, int width, int height);

    /**
     * \brief A wrapper around a
     */
    struct GLEventWindow
    {
        using ResizeCallbackFn = void(GLEventWindow& window, int width, int height);

        ////////////////////////
        ///   Constructors   ///
    public:

        GLEventWindow();
        explicit GLEventWindow(GLFWwindow* window);
        GLEventWindow(const GLEventWindow& copy) = delete;
        GLEventWindow& operator=(const GLEventWindow& copy) = delete;
        GLEventWindow(GLEventWindow&& move) = delete;
        GLEventWindow& operator=(GLEventWindow&& move) = delete;
        ~GLEventWindow();

        ///////////////////
        ///   Methods   ///
    public:

        void set_window(GLFWwindow* window);

        void unset_window();

        void set_resize_callback(std::function<ResizeCallbackFn> resize_callback);

        void register_pipeline(UpdatePipeline& pipeline);

        void set_bindings(InputBindings bindings);

    private:

        static void glfw_window_resize_callback(GLFWwindow* window, int width, int height);

        void process_input(SystemFrame& frame, float current_time, float dt);

        //////////////////
        ///   Fields   ///
    private:

        GLFWwindow* _window;
        std::function<ResizeCallbackFn> _resize_callback;
        InputBindings _bindings;
    };
}
