/****************************
*
* Bowshock editor.
* Interfaces with a react frontend to send commands
* to the rendering backend
*
* Author: ekirshey
* Created: Sat Aug 17 10:53:07 2019
****************************/

#include <iostream>
#include <queue>
#include <string>
#include <boost/beast/core.hpp>
#include <boost/asio/error.hpp>
#include <nlohmann/json.hpp>

#include "spdlog/spdlog.h"

#include "websocket_client.h"

// Create an all, like restinio?
#include "bowshock/bs_math.h"
#include "bowshock/bs_time.h"
#include "bowshock/engine.h"
#include "bowshock/camera.h"
#include "bowshock/viewport.h"
#include "bowshock/shader.h"
#include "bowshock/renderer.h"

enum class MessageType {
    ADD,
    DEL
};

struct Entity {
    std::string name;
    int model_id;

    glm::vec3 scale;
    glm::vec3 rotation_axis;
    glm::vec3 translation;

    float rotation;
};

struct Message {
    MessageType type;
    uint64_t entity_id;
    Entity entity;
};

std::queue< std::string > message_queue;


std::mutex proc_mutex;
int count = 0;
int entity_tracker = 0;

struct DirectionalLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

void timer_update( const boost::system::error_code&, 
                   boost::asio::steady_timer* t,
                   WebsocketClient* client)
{
    client->read( message_queue );

    t->expires_at(t->expiry() + std::chrono::milliseconds(250));
    t->async_wait(std::bind(timer_update,
        std::placeholders::_1, t, client));
}

// This needs to be better. I need an abstraction for lights in the engine 100%
void set_light(bowshock::Shader& shader, const DirectionalLight& light)
{
    auto ldir = shader.get_uniform_location("light.direction");
    glUniform3fv(ldir, 1, &light.direction[0]);

    auto la = shader.get_uniform_location("light.ambient");
    glUniform3fv(la, 1, &light.ambient[0]);

    auto ld = shader.get_uniform_location("light.diffuse");
    glUniform3fv(ld, 1, &light.diffuse[0]);

    auto ls = shader.get_uniform_location("light.specular");
    glUniform3fv(ls, 1, &light.specular[0]);
}

void mouse_callback(bowshock::Camera& c, bool& first_mouse, float& yaw, float& pitch, float& last_mouse_x, float& last_mouse_y, double xpos, double ypos)
{
    float f_xpos = (float)xpos;
    float f_ypos = (float)ypos;

    if (first_mouse) // this bool variable is initially set to true
    {
        last_mouse_x = f_xpos;
        last_mouse_y = f_ypos;
        first_mouse = false;
    }

    float xoffset = f_xpos - last_mouse_x;
    float yoffset = last_mouse_y - f_ypos; // reversed since y-coordinates range from bottom to top
    last_mouse_x = f_xpos;
    last_mouse_y = f_ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    c.rotate(yaw, pitch);
}

void processInput(bowshock::BSWindow& window, bowshock::Camera& c, float delta_time)
{

    if (window.is_key_pressed(GLFW_KEY_ESCAPE))
        window.close();

    if (window.is_key_pressed(GLFW_KEY_W))
        c.move(glm::vec3(0.0f, 0.0f, 1.0f), delta_time);
    if (window.is_key_pressed(GLFW_KEY_S))
        c.move(glm::vec3(0.0f, 0.0f, -1.0f), delta_time);
    if (window.is_key_pressed(GLFW_KEY_A))
        c.move(glm::vec3(-1.0f, 0.0f, 0.0f), delta_time);
    if (window.is_key_pressed(GLFW_KEY_D))
        c.move(glm::vec3(1.0f, 0.0, 0.0f), delta_time);
    if (window.is_key_pressed(GLFW_KEY_E))
        c.move(glm::vec3(0.0f, 1.0f, 0.0f), delta_time);
    if (window.is_key_pressed(GLFW_KEY_Q))
        c.move(glm::vec3(0.0f, -1.0, 0.0f), delta_time);

}

Message to_message(std::string& data) 
{
    Message msg;
    auto body = nlohmann::json::parse(data);
    msg.type = MessageType::ADD;
    msg.entity_id = entity_tracker++;
    msg.entity.name = body.at("name");
    msg.entity.model_id = body.at("model_id");

    msg.entity.translation = glm::vec3(body["translation"]["x"],
        body["translation"]["y"],
        body["translation"]["z"]);

    msg.entity.rotation_axis = glm::vec3(body["rotation_axis"]["x"],
        body["rotation_axis"]["y"],
        body["rotation_axis"]["z"]);

    msg.entity.scale = glm::vec3(body["scale"]["x"],
        body["scale"]["y"],
        body["scale"]["z"]);
    msg.entity.rotation = 0.0f;

    return msg;
}

int main(int argc, char* argv[]) {
    
    boost::asio::io_context io_context;

    WebsocketClient client(io_context, "127.0.0.1", "8080");

    boost::asio::steady_timer timer(io_context, std::chrono::milliseconds(250));

    timer.async_wait(std::bind(timer_update, std::placeholders::_1, &timer, &client));

    std::thread io_thread([&io_context]() {
        io_context.run();
    });

    using namespace bowshock;
    // Bowshock init
    spdlog::set_level(spdlog::level::debug);

    Engine engine("../../config.ini");
    auto& window = engine.window;
    auto& mc = engine.model_container;
    auto& tc = engine.texture_container;
    auto& config = engine.config;

    float yaw = -90.0f;
    float pitch = 0.0f;

    bool first_mouse = true;
    float last_mouse_x = config.width / 2.0f;
    float last_mouse_y = config.height / 2.0f;

    // Can probably do this type of config from a file
    Viewport viewport(config.width,
        config.height,
        0.1f,
        100.0f);

    Camera c(glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        45.0f,
        2.5f);

    window.register_mouse_cb([&](double xpos, double ypos) {
        mouse_callback(c, first_mouse, yaw, pitch, last_mouse_x, last_mouse_y, xpos, ypos);
    });

    Shader phong("F:/github/bowshock_editor/shaders/mat.vert", "F:/github/bowshock_editor/shaders/phong.frag");

    DirectionalLight dir_light;
    dir_light.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    dir_light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    dir_light.diffuse = glm::vec3(0.3f, 0.3f, 0.3f);
    dir_light.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    float delta_time = 0.0;	// Time between current frame and last frame
    float last_frame = 0.0; // Time of last frame
    std::queue< std::string > events;
    std::unordered_map< uint64_t, Entity > entities;

    while (!window.is_closing() ) {
        /////////////// Messaging ////////////////
        {
            std::lock_guard<std::mutex> lock(proc_mutex);
            events = std::move(message_queue);
        }

        while (!events.empty()) {
            Message temp = to_message(events.front());
            events.pop();
            entities[temp.entity_id] = temp.entity;
        }
        //////////////////////////////////////////

        float current_frame = bowshock::get_time();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        processInput(window, c, delta_time);

        Renderer::clear_screen(0.3f, 0.4f, 0.6f);

        // Can also group this boilder plate somewhere
        // view_proj = get_view_proj(Viewport, Camera);
        glm::mat4 v = c.lookat();

        glm::mat4 p;
        p = viewport.projection(c.fov());
        glm::mat4 view_proj = p * v;

        for (auto& [key, entity] : entities) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, entity.translation);
            model = glm::rotate(model, entity.rotation, entity.rotation_axis);
            model = glm::scale(model, entity.scale);

            phong.use();
            set_light(phong, dir_light);
            phong.set_mvp(model, view_proj);
            mc.draw(entity.model_id, phong);
        }

        window.update();
    }

    // Close up everything... gracefully?
    io_context.stop(); // Killing the timer??

    io_thread.join();

    return 0;
}
