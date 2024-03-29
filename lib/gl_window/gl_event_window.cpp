#include "lib/gl_window/gl_event_window.h"
#include "lib/engine/components/gameplay/input.h"
#include "lib/engine/scene.h"
#include "lib/engine/system_frame.h"
#include "lib/engine/update_pipeline.h"

namespace sge {
GLFWwindow* create_sge_opengl_window(const char* title, int width, int height, bool fullscreen) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
  return glfwCreateWindow(width, height, title, monitor, nullptr);
}

GLEventWindow::GLEventWindow()
    : _quit_requested(false),
      _has_focus(true),
      _window(nullptr),
      _window_x(0),
      _window_y(0),
      _window_width(0),
      _window_height(0),
      _mouse_x(0.0),
      _mouse_y(0.0),
      _mouse_delta_x(0.0),
      _mouse_delta_y(0.0) {}

GLEventWindow::GLEventWindow(GLFWwindow* window) : GLEventWindow() {
  set_window(window);
}

GLEventWindow::~GLEventWindow() {
  unset_window();
}

void GLEventWindow::set_window(GLFWwindow* window) {
  // Remove the currently active window (if any)
  unset_window();
  if (!window) {
    return;
  }

  // Set up the given window on this window
  _window = window;
  glfwSetWindowUserPointer(window, this);
  glfwSetWindowPosCallback(window, &glfw_window_pos_callback);
  glfwSetFramebufferSizeCallback(window, &glfw_window_resize_callback);
  glfwSetWindowFocusCallback(window, &glfw_window_focus_callback);
  glfwSetKeyCallback(window, &glfw_key_callback);
  glfwSetMouseButtonCallback(window, &glfw_mouse_button_callback);
  glfwSetCursorPosCallback(window, &glfw_cursor_callback);

  // Get window initial properties
  glfwGetWindowPos(window, &_window_x, &_window_y);
  glfwGetWindowSize(window, &_window_width, &_window_height);
  glfwGetCursorPos(window, &_mouse_x, &_mouse_y);
  _mouse_delta_x = 0;
  _mouse_delta_y = 0;
}

void GLEventWindow::unset_window() {
  if (!_window) {
    return;
  }

  glfwSetCursorPosCallback(_window, nullptr);
  glfwSetMouseButtonCallback(_window, nullptr);
  glfwSetKeyCallback(_window, nullptr);
  glfwSetWindowFocusCallback(_window, nullptr);
  glfwSetFramebufferSizeCallback(_window, nullptr);
  glfwSetWindowPosCallback(_window, nullptr);
  glfwSetWindowUserPointer(_window, nullptr);
  _window = nullptr;
}

void GLEventWindow::set_resize_callback(std::function<ResizeCallbackFn> resize_callback) {
  _resize_callback = std::move(resize_callback);
}

void GLEventWindow::register_pipeline(UpdatePipeline& pipeline) {
  pipeline.register_system_fn("gl_window_input", this, &GLEventWindow::process_input);
}

void GLEventWindow::set_bindings(InputBindings bindings) {
  _bindings = std::move(bindings);
}

void GLEventWindow::capture_mouse(bool value) {
  if (value) {
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  } else {
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

bool GLEventWindow::quit_requested() const {
  return _quit_requested;
}

void GLEventWindow::glfw_window_resize_callback(GLFWwindow* window, int width, int height) {
  auto* event_window = static_cast<GLEventWindow*>(glfwGetWindowUserPointer(window));
  event_window->_window_width = width;
  event_window->_window_height = height;

  if (event_window->_resize_callback) {
    event_window->_resize_callback(*event_window, width, height);
  }
}

void GLEventWindow::glfw_window_pos_callback(GLFWwindow* window, int x, int y) {
  auto* event_window = static_cast<GLEventWindow*>(glfwGetWindowUserPointer(window));
  event_window->_window_x = x;
  event_window->_window_y = y;
}

void GLEventWindow::glfw_window_focus_callback(GLFWwindow* window, int has_focus) {
  auto* event_window = static_cast<GLEventWindow*>(glfwGetWindowUserPointer(window));
  event_window->_has_focus = (has_focus == GLFW_TRUE);
}

void GLEventWindow::
    glfw_key_callback(GLFWwindow* window, int key, int /*scancode*/, int state, int /*mods*/) {
  auto* event_window = static_cast<GLEventWindow*>(glfwGetWindowUserPointer(window));

  // Search for the key in the set of bindings
  auto iter = event_window->_bindings.key_bindings.find(key);
  if (iter == event_window->_bindings.key_bindings.end()) {
    return;
  }

  if (iter->second == "QUIT") {
    event_window->_quit_requested = true;
    return;
  }

  // If the key has been pressed
  switch (state) {
    case GLFW_PRESS:
      event_window->_active_action_bindings.insert(iter->second.c_str());
      break;

    case GLFW_RELEASE:
      event_window->_active_action_bindings.erase(iter->second.c_str());
      break;
  }
}

void GLEventWindow::glfw_mouse_button_callback(GLFWwindow* window, int key, int state, int /*mods*/) {
  auto* event_window = static_cast<GLEventWindow*>(glfwGetWindowUserPointer(window));

  // Search for the mouse button in the set of bindings
  auto iter = event_window->_bindings.mouse_button_bindings.find(key);
  if (iter == event_window->_bindings.mouse_button_bindings.end()) {
    return;
  }

  // If the button has been pressed
  switch (state) {
    case GLFW_PRESS:
      event_window->_active_action_bindings.insert(iter->second.c_str());
      break;

    case GLFW_RELEASE:
      event_window->_active_action_bindings.erase(iter->second.c_str());
      break;
  }
}

void GLEventWindow::glfw_cursor_callback(GLFWwindow* window, double x, double y) {
  auto event_window = static_cast<GLEventWindow*>(glfwGetWindowUserPointer(window));

  // Update delta
  event_window->_mouse_delta_x = -(x - event_window->_mouse_x);
  event_window->_mouse_delta_y = -(y - event_window->_mouse_y);

  // Update mouse X and Y
  event_window->_mouse_x = x;
  event_window->_mouse_y = y;
}

void GLEventWindow::process_input(Scene& scene, SystemFrame& /*frame*/) {
  if (!_has_focus) {
    return;
  }

  auto* input_component = scene.get_component_container(CInput::type_info);

  // Get all input nodes
  NodeId input_node_ids[32];
  CInput* input_instances[32];
  size_t num_instances;
  size_t start_index = 0;

  // Get input nodes
  while (input_component->get_instance_nodes(start_index, 32, &num_instances, input_node_ids)) {
    start_index += 32;

    // Get input component instances
    input_component->get_instances(input_node_ids, num_instances, input_instances);

    // Dispatch events
    for (size_t i = 0; i < num_instances; ++i) {
      auto& input = *input_instances[i];

      // For each keyboard binding
      for (auto binding : this->_active_action_bindings) {
        input.add_action_event(binding);
      }

      // For each mouse x-axis binding
      for (const auto& binding : this->_bindings.mouse_x_bindings) {
        input.add_axis_event(
            binding.c_str(),
            static_cast<float>(this->_mouse_x),       // value
            static_cast<float>(this->_window_width),  // min
            static_cast<float>(0)                     // max
        );
      }

      // For each mouse y-axis binding
      for (const auto& binding : this->_bindings.mouse_y_bindings) {
        input.add_axis_event(
            binding.c_str(),
            static_cast<float>(this->_mouse_y),        // value
            static_cast<float>(this->_window_height),  // min
            static_cast<float>(0)                      // max
        );
      }

      // For each mouse delta-x binding
      for (const auto& binding : this->_bindings.mouse_delta_x_bindings) {
        input.add_axis_event(
            binding.c_str(),
            static_cast<float>(this->_mouse_delta_x),  // value
            0.f,
            0.f
        );
      }
      this->_mouse_delta_x = 0;

      // For each mouse delta-y binding
      for (const auto& binding : this->_bindings.mouse_delta_y_bindings) {
        input.add_axis_event(
            binding.c_str(),
            static_cast<float>(this->_mouse_delta_y),  // value
            0.f,
            0.f
        );
      }
      this->_mouse_delta_y = 0;
    }
  }
}
}  // namespace sge
