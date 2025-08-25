#include <GL/glew.h>
#include <GLFW/glfw3.h>

const char* vs = "#version 330 core\nlayout(location=0) in vec3 a;void main(){gl_Position=vec4(a,1.0);}";
const char* fs = "#version 330 core\nout vec4 c;void main(){c=vec4(1.0,0.5,0.2,1.0);}";

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* w = glfwCreateWindow(800, 600, "", 0, 0);
    glfwMakeContextCurrent(w);
    glewExperimental = GL_TRUE;
    glewInit();

    float v[] = { 0.0f, 0.5f, 0.0f,  -0.5f, -0.5f, 0.0f,  0.5f, -0.5f, 0.0f };
    GLuint VAO, VBO, vsID, fsID, prog;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    vsID = glCreateShader(GL_VERTEX_SHADER);
    fsID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vsID, 1, &vs, 0);
    glShaderSource(fsID, 1, &fs, 0);
    glCompileShader(vsID);
    glCompileShader(fsID);
    prog = glCreateProgram();
    glAttachShader(prog, vsID);
    glAttachShader(prog, fsID);
    glLinkProgram(prog);
    glUseProgram(prog);

    while (!glfwWindowShouldClose(w)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(w);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
