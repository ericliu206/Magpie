#pragma once

namespace Magpie {
    class Display {
        public:
            ~Display();
            void Initialize();
            void Render();
            void SwitchToColorTexture();
        private:
            const GLchar* vertexSource = R"glsl(
            #version 330 core
            layout (location = 0) in vec2 aPos;
            layout (location = 1) in vec2 aTexCoords;

            out vec2 TexCoords;

            void main()
            {
                gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
                TexCoords = aTexCoords;
            }
            )glsl";

            const GLchar* fragmentSource = R"glsl(
            #version 330 core
            out vec4 FragColor;
            
            in vec2 TexCoords;

            uniform sampler2D screenTexture;

            void main()
            { 
                FragColor = texture(screenTexture, TexCoords);
            }
            )glsl";

            GLuint shaderProgram, VAO, VBO, framebuffer, textureColorbuffer;
    };
}