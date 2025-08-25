#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <gl/GL.h>
#include <iostream>

// --- WGL Extensions ---
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);
typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC, const int*, const FLOAT*, UINT, int*, UINT*);

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;

// --- OpenGL Function Pointers ---
#define LOAD_GL_FUNC(type, name) type name = (type)wglGetProcAddress(#name)
LOAD_GL_FUNC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
LOAD_GL_FUNC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
LOAD_GL_FUNC(PFNGLGENBUFFERSPROC, glGenBuffers);
LOAD_GL_FUNC(PFNGLBINDBUFFERPROC, glBindBuffer);
LOAD_GL_FUNC(PFNGLBUFFERDATAPROC, glBufferData);
LOAD_GL_FUNC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
LOAD_GL_FUNC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
LOAD_GL_FUNC(PFNGLCREATESHADERPROC, glCreateShader);
LOAD_GL_FUNC(PFNGLSHADERSOURCEPROC, glShaderSource);
LOAD_GL_FUNC(PFNGLCOMPILESHADERPROC, glCompileShader);
LOAD_GL_FUNC(PFNGLGETSHADERIVPROC, glGetShaderiv);
LOAD_GL_FUNC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
LOAD_GL_FUNC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
LOAD_GL_FUNC(PFNGLATTACHSHADERPROC, glAttachShader);
LOAD_GL_FUNC(PFNGLLINKPROGRAMPROC, glLinkProgram);
LOAD_GL_FUNC(PFNGLUSEPROGRAMPROC, glUseProgram);
LOAD_GL_FUNC(PFNGLDELETESHADERPROC, glDeleteShader);

// Vertex and Fragment Shader
const char* vertexShaderSource = R"(#version 330 core
layout(location = 0) in vec3 aPos;
void main() {
   gl_Position = vec4(aPos, 1.0);
})";

const char* fragmentShaderSource = R"(#version 330 core
out vec4 FragColor;
void main() {
   FragColor = vec4(1.0, 0.5, 0.2, 1.0);
})";

// Globals
HDC hDC;
HGLRC hRC;
GLuint VAO, VBO, shaderProgram;

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CLOSE || msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Dummy context to load WGL extensions
void CreateDummyContext(HWND hwnd) {
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32,
        0,0,0,0,0,0,0,0,0,0,0,0,0,
        24, 8, 0, PFD_MAIN_PLANE, 0,0,0,0
    };

    hDC = GetDC(hwnd);
    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);

    HGLRC tempRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, tempRC);

    // Load WGL extensions
    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(tempRC);
}

// Create real OpenGL 3.3 context
void CreateGLContext(HWND hwnd) {
    const int attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0
    };

    int pixelFormat;
    UINT numFormats;
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
    wglChoosePixelFormatARB(hDC, attribs, nullptr, 1, &pixelFormat, &numFormats);

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(hDC, pixelFormat, sizeof(pfd), &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);

    const int contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    hRC = wglCreateContextAttribsARB(hDC, 0, contextAttribs);
    wglMakeCurrent(hDC, hRC);
}

// Compile and link shaders
GLuint CompileShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, log);
        std::cerr << "Vertex shader error: " << log << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, log);
        std::cerr << "Fragment shader error: " << log << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// Initialize triangle
void InitTriangle() {
    float vertices[] = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    shaderProgram = CompileShaders();
    glUseProgram(shaderProgram);
}

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    const wchar_t CLASS_NAME[] = L"OpenGLWinClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"OpenGL Triangle (No GLFW)", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, SW_SHOW);

    CreateDummyContext(hwnd);
    CreateGLContext(hwnd);

    InitTriangle();

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SwapBuffers(hDC);
    }

    return 0;
}
