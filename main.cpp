#include "main.h"
// Example program:
// Using SDL2 to create an application window

#include "SDL.h"

static const GLuint WIDTH = 640;
static const GLuint HEIGHT = 480;
static const GLchar* vertex_shader_path = "./vertex.cpp";
static const GLchar* fragment_shader_path = "./fragment.cpp";
static GLfloat vertices[] = {
	-0.5,  0.0,
	-0.25, -0.5,
	-0.75, -0.5,
};


std::string readFile(const char* filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

GLuint common_get_shader_program( const char* vertex_shader_path, const char* fragment_shader_path)
{
	GLchar* log = NULL;
	GLint log_length, success;
	GLuint fragment_shader, program, vertex_shader;
	std::string vertShaderStr = readFile(vertex_shader_path);
	const char* vertex_shader_source = vertShaderStr.c_str();

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);

	log = static_cast<GLchar*>(malloc(log_length));
    if (log_length > 0) {
        glGetShaderInfoLog(vertex_shader, log_length, NULL, log);
        printf("vertex shader log:\n\n%s\n", log);
    }
    if (!success) {
        printf("vertex shader compile error\n");
        exit(EXIT_FAILURE);
    }

	std::string fragShaderStr = readFile(fragment_shader_path);
	const char* fragment_shader_source = fragShaderStr.c_str();
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);

    if (log_length > 0) {
        log = static_cast<GLchar*>(realloc(log, log_length));
        glGetShaderInfoLog(fragment_shader, log_length, NULL, log);
        printf("fragment shader log:\n\n%s\n", log);
    }
    if (!success) {
        printf("fragment shader compile error\n");
        exit(EXIT_FAILURE);
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

    if (log_length > 0) {
        log = static_cast<GLchar*>(realloc(log, log_length));
        glGetProgramInfoLog(program, log_length, NULL, log);
        printf("shader link log:\n\n%s\n", log);
    }
    if (!success) {
        printf("shader link error");
        exit(EXIT_FAILURE);
    }

    free(log);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

int main(int argc, char* argv[])
{
	GLint attribute_coord2d, attribute_in_color;
    GLuint program, vbo, vertexs[1];
    SDL_GLContext Context;
	SDL_Event e;
	SDL_Window* window;
	int timeValue;
	float greenValue = 0.5f;
	int vertexColorLocation;       
	const Uint8* state = SDL_GetKeyboardState(NULL);
	bool x = false;

	SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

	// Create an application window with the following settings:
	window = SDL_CreateWindow(
		"An SDL2 window",                  // window title
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		WIDTH,                               // width, in pixels
		HEIGHT,                               // height, in pixels
		SDL_WINDOW_OPENGL            // flags - see below
	);

	Context = SDL_GL_CreateContext(window);
	// Check that the window was successfully created
	if (window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}
	glewInit();


	// The window is open: could enter program loop here (see SDL_PollEvent())
	program = common_get_shader_program(vertex_shader_path, fragment_shader_path);
    attribute_coord2d = glGetAttribLocation(program, "aPos");

    
    glGenVertexArrays(1, &vertexs[0]);

	glBindVertexArray(vertexs[0]);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attribute_coord2d);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
	
	while (1)
	{
		glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
    	glUseProgram(program);

		timeValue = SDL_GetTicks();
			greenValue = sin(timeValue/300) / 5.0f + 0.8f;

		vertexColorLocation = glGetUniformLocation(program, "ourColor");
    	glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		glBindVertexArray(vertexs[0]);	
	    glDrawArrays(GL_TRIANGLES, 0, 3);
	    //glDisableVertexAttribArray(attribute_coord2d);
	    //glDisableVertexAttribArray(attribute_in_color);


		SDL_GL_SwapWindow(window);
		if (SDL_PollEvent(&e) && (state[SDL_SCANCODE_ESCAPE] || e.type == SDL_QUIT))
			break;
		if (state[SDL_SCANCODE_F])
		{
			if (x)
			{
				SDL_SetWindowFullscreen(window, 0);
				x = false;
			}
			else
			{
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
				x = true;
			}
		}
	}

	glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    SDL_GL_DeleteContext(Context);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}