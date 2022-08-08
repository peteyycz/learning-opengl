#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

static unsigned int CompileShader(const char* source, GLenum type) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        printf("Failed to compile %s shader\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment");
        printf("%s\n", message);
        glDeleteShader(id);
        return 0;
    }

    return id;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static unsigned int CreateShader(const char* vertexShaderSource, const char* fragmentShaderSource) {
    unsigned int vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
    unsigned int fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    int success;
    // check for linking errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        int length;
        glGetShaderiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, 512, NULL, message);
        printf("Shader program linking failed, reason: %s\n", message);
    }
    glValidateProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

unsigned int getVertexArrayObject(float vertices[], unsigned int verticesSize, unsigned int indices[], unsigned int indiciesSize) {
    unsigned int VBO, VAO, EBO;

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiciesSize, indices, GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    return VAO;
}

int main(void)
{
    /* Initialize the library */
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Failed to open GLFW window.");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    printf("OpenGL version: %s", glGetString(GL_VERSION));

    unsigned int shader = CreateShader(vertexShaderSource, fragmentShaderSource);

    float leftTriangleVertices[] = {
        // First Rectangle
        -0.25f,  0.75f, 0.0f,  // top right
        -0.25f, -0.25f, 0.0f,  // bottom right
        -0.75f, -0.25f, 0.0f,  // bottom left
        -0.75f,  0.75f, 0.0f,   // top left 
    };
    unsigned int leftTriangleIndices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3,    // second triangle
    };
    unsigned int leftTriangleVAO = getVertexArrayObject(leftTriangleVertices, sizeof(leftTriangleVertices), leftTriangleIndices, sizeof(leftTriangleIndices));

    float rightTriangleVertices[] = {
        // Second Rectangle
        0.25f,  0.75f, 0.0f,  // top right
        0.25f, -0.25f, 0.0f,  // bottom right
        0.75f, -0.25f, 0.0f,  // bottom right
        0.75f,  0.75f, 0.0f   // top right 
    };
    unsigned int rightTriangleIndices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3,    // second triangle
    };
    unsigned int rightTriangleVAO = getVertexArrayObject(rightTriangleVertices, sizeof(rightTriangleVertices), rightTriangleIndices, sizeof(rightTriangleIndices));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        /* Draw our stuff */
        glUseProgram(shader);
        glBindVertexArray(leftTriangleVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(rightTriangleVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            printf("error %d", error);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
