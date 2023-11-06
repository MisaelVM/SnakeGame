#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "SnakeGame.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

SnakeGame snakeGame;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE)
#endif

	GLFWwindow* window = glfwCreateWindow(800, 800, "Snake Game", nullptr, nullptr);
	if (!window) {
		std::cout << "Failed to create GLFW window!\n";
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD!\n";
		glfwTerminate();
		return 1;
	}

	float quadVertices[] = {
		-1.0f, -1.0f,	0.0f, 0.0f,
		-1.0f,  1.0f,	0.0f, 1.0f,
		 1.0f,  1.0f,	1.0f, 1.0f,

		 1.0f,  1.0f,	1.0f, 1.0f,
		 1.0f, -1.0f,	1.0f, 0.0f,
		-1.0f, -1.0f,	0.0f, 0.0f
	};

	const char* vertexShaderSource =
		"#version 430 core\n"
		"layout(location = 0) in vec4 position;\n"
		"layout(location = 1) in vec2 texCoord;\n"
		"out vec2 v_TexCoord;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = mat4(1.0f) * position;\n"
		"	v_TexCoord = texCoord;\n"
		"}\n\0";

	const char* fragmentShaderSource =
		"#version 430 core\n"
		"out vec4 color;\n"
		"in vec2 v_TexCoord;\n"
		"uniform sampler2D u_Sampler;\n"
		"void main()\n"
		"{\n"
		"	color = texture(u_Sampler, v_TexCoord);\n"
		"}\n\0";

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	{
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
			std::cout << "Vertex Shader Compilation Failed!\n" << infoLog << "\n";
		}
	}
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	{
		int success;
		char infoLog[512];
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
			std::cout << "Vertex Shader Compilation Failed!\n" << infoLog << "\n";
		}
	}
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)snakeGame.GetWidth(), (GLsizei)snakeGame.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, snakeGame.GetPixelData());
	glGenerateMipmap(GL_TEXTURE_2D);

	float frameStartTime = 0.0f, frameEndTime = 0.0f;
	float deltaTime = 0.0f;

	glBindVertexArray(0);
	glUseProgram(0);

	while (!glfwWindowShouldClose(window) && !snakeGame.IsGameOver()) {
		glfwSwapBuffers(window);
		glfwPollEvents();

		snakeGame.Update(deltaTime);
		glBindVertexArray(VAO);
		glUseProgram(shaderProgram);

		glDrawArrays(GL_TRIANGLES, 0, 12);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)snakeGame.GetWidth(), (GLsizei)snakeGame.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, snakeGame.GetPixelData());

		frameEndTime = (float)glfwGetTime();
		deltaTime = frameEndTime - frameStartTime;
		frameStartTime = frameEndTime;
	}

	glDeleteTextures(1, &texture);
	glDeleteProgram(shaderProgram);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS)
		return;

	if (key == GLFW_KEY_W || key == GLFW_KEY_UP)
		snakeGame.PushMovement(SnakeGame::Movement::Up);

	else if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
		snakeGame.PushMovement(SnakeGame::Movement::Left);

	else if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)
		snakeGame.PushMovement(SnakeGame::Movement::Down);

	else if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
		snakeGame.PushMovement(SnakeGame::Movement::Right);

	else if (key == GLFW_KEY_ESCAPE)
		snakeGame.Shutdown();
}
