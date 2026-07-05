#include <windows.h>
#include <GL/gl.h>
#include "audio.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_ASPECT (WINDOW_WIDTH / WINDOW_HEIGHT)

void main() {
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    HWND hWnd;
    HDC hDC;
    int pf;

    SetProcessDPIAware();

    hWnd = CreateWindow("static", 0, WS_POPUP | WS_VISIBLE, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, 0, 0);
    hDC = GetDC(hWnd);

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    pf = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pf, &pfd);

    wglMakeCurrent(hDC, wglCreateContext(hDC));

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-WINDOW_ASPECT, WINDOW_ASPECT, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    ShowCursor(FALSE);

    DWORD startTime = timeGetTime();

    //PlayMusic();

    while (!GetAsyncKeyState(VK_ESCAPE)) {
        float angle = (float)((timeGetTime() - startTime)) * 0.1f;

        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(angle, 0.0f, 0.0f, 1.0f);
        
        glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(0.0f, 0.5f);

            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(-0.5f, -0.5f);

            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex2f(0.5f, -0.5f);
        glEnd();

        SwapBuffers(hDC);
    }

    ExitProcess(0);
}