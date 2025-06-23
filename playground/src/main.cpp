#include <iostream>
#include <vector>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <battery/embed.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Mvp {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    glm::mat4 mvp;
    glm::ivec2 windowSize;
};

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Couldn't initialize SDL.\n";
        return 1;
    }

    glm::ivec2 windowSize = glm::ivec2(500, 500);
    SDL_Window *window = SDL_CreateWindow("calm", windowSize.x, windowSize.y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GLContextState *const context = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        std::cerr << "Couldn't initialize glad.\n";
    }

    float r = 1.0f;
    float sq = sqrtf(3.0f) / 2.0f;
    std::vector<Vertex> tri = {
            {{-r * sq, -r / 2.0f, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}},
            {{r * sq, -r / 2.0f, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0}},
            {{0.0, r, 0.0}, {0.0, 0.0, 0.0}, {0.5, 1.0}},
    };

    auto model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), 0.1f,
                                            100.0f);

    Mvp mvp = {
            model,
            view,
            projection,
            projection * view * model,
            windowSize,
    };

    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, sizeof(Vertex) * tri.size(), tri.data(), GL_DYNAMIC_STORAGE_BIT);

    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);

    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoords));

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribBinding(vao, 2, 0);

    glBindVertexArray(vao);

    GLuint ubo;
    glCreateBuffers(1, &ubo);
    glNamedBufferStorage(ubo, sizeof(Mvp), &mvp, GL_DYNAMIC_STORAGE_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

    GLuint shaderProgram = glCreateProgram();
    std::string vString, fString;
    bool vertNeedsUpdate = true, fragNeedsUpdate = true;

    b::embed<"shaders/triangle.vert">().get([&vString, &vertNeedsUpdate](const auto &content) {
        vString = content;
        vertNeedsUpdate = true;
    });

    b::embed<"shaders/triangle.frag">().get([&fString, &fragNeedsUpdate](const auto &content) {
        fString = content;
        fragNeedsUpdate = true;
    });

    SDL_Event event;
    bool quit = false;
    uint64_t last = SDL_GetPerformanceCounter();

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

    while (!quit) {
        uint64_t now = SDL_GetPerformanceCounter();
        double deltaTime = (double) (now - last) / (double) SDL_GetPerformanceFrequency();
        last = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }

            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                quit = true;
            }

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                mvp.windowSize.x = event.window.data1;
                mvp.windowSize.y = event.window.data2;

                mvp.projection = glm::perspective(glm::radians(45.0f),
                                                  static_cast<float>(mvp.windowSize.x) / static_cast<float>(mvp.
                                                      windowSize.y), 0.1f, 100.0f);
                mvp.mvp = mvp.projection * mvp.view * mvp.model;
                glNamedBufferSubData(ubo, 0, sizeof(Mvp), &mvp);

                glViewport(0, 0, mvp.windowSize.x, mvp.windowSize.y);
            }
        }

        if (vertNeedsUpdate) {
            auto vSrcStr = vString.data();
            auto vSrcSize = static_cast<GLint>(vString.size());

            glDetachShader(shaderProgram, vert);

            glShaderSource(vert, 1, &vSrcStr, &vSrcSize);
            glCompileShader(vert);

            // check errors
            GLint success;
            glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
            if (!success) {
                GLint logLength;
                glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &logLength);
                std::vector<GLchar> infoLog(logLength);
                glGetShaderInfoLog(vert, logLength, nullptr, infoLog.data());
                std::cerr << "Vertex Shader compilation failed:\n" << infoLog.data() << "\n";
            }

            glAttachShader(shaderProgram, vert);
            glLinkProgram(shaderProgram);

            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                GLint logLength;
                glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
                std::vector<GLchar> infoLog(logLength);
                glGetProgramInfoLog(shaderProgram, logLength, nullptr, infoLog.data());
                std::cerr << "Shader Program linking failed:\n" << infoLog.data() << "\n";
            }

            glUseProgram(shaderProgram);
            vertNeedsUpdate = false;
        }

        if (fragNeedsUpdate) {
            auto fSrcStr = fString.data();
            auto fSrcSize = static_cast<GLint>(fString.size());

            glDetachShader(shaderProgram, frag);

            glShaderSource(frag, 1, &fSrcStr, &fSrcSize);
            glCompileShader(frag);

            GLint success;
            glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
            if (!success) {
                GLint logLength;
                glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &logLength);
                std::vector<GLchar> infoLog(logLength);
                glGetShaderInfoLog(frag, logLength, nullptr, infoLog.data());
                std::cerr << "Fragment Shader compilation failed:\n" << infoLog.data() << "\n";
            }

            glAttachShader(shaderProgram, frag);
            glLinkProgram(shaderProgram);

            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                GLint logLength;
                glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
                std::vector<GLchar> infoLog(logLength);
                glGetProgramInfoLog(shaderProgram, logLength, nullptr, infoLog.data());
                std::cerr << "Shader Program linking failed:\n" << infoLog.data() << "\n";
            }

            glUseProgram(shaderProgram);
            fragNeedsUpdate = false;
        }

        mvp.model = glm::rotate(mvp.model, static_cast<float>(deltaTime / 4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mvp.mvp = mvp.projection * mvp.view * mvp.model;
        glNamedBufferSubData(ubo, 0, sizeof(Mvp), &mvp);

        glProgramUniform1f(shaderProgram, 0, static_cast<float>(SDL_GetTicks()) / 1000.0f);

        glClearColor(0.094f, 0.608f, 0.753f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
