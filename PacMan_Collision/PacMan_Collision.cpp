// PacMan_Collision.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdio.h"
#include "framework.h"
#include "PacMan_Collision.h"
#include <gl/gl.h>
#include <gl/glu.h>

#define IDT_TIMER 1
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

////////////////////// sk
HDC hDeviceContext;								// current device context
HGLRC hRenderingContext;						// current rendering context

bool bSetupPixelFormat(HDC hdc);
void Resize(int width, int height);
void DrawScene(HDC MyDC);

void DrawRect(float rgb[3], float left, float bottom, float right, float top, bool line);

float Back_RGB[3] = { 0.0f, 1.0f, 1.0f };
float Back_BLOCK[4] = { -1.0f, -1.0f, 1.0f, 1.0f };

float Rect1RGB[3] = { 1.0f, 0.0f, 0.0f };
float Rect1BLOCK[4] = { -0.1f, -0.1f, 0.1f, 0.1f };
float Rect1POS[2] = { -0.9f, 0.9f };
float Rect1MOVE[2] = { 0.05f, -0.05f };
float Rect1BP[4];

float Rect2RGB[3] = { 0.0f, 0.0f, 1.0f };
float Rect2BLOCK[4] = { -0.1f, -0.1f, 0.1f, 0.1f };
float Rect2POS[2] = { 0.9f, -0.9f };
float Rect2MOVE[2] = { -0.05f, 0.05f };
float Rect2BP[4];

bool DoClide(float block[4], float left, float bottom, float right, float top);
bool BlockCollision(float blockA[4], float blockB[4]);

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PACMANCOLLISION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PACMANCOLLISION));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PACMANCOLLISION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = 0; // MAKEINTRESOURCEW(IDC_PACMANCOLLISION);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;

    switch (message)
    {
    case WM_CREATE:
        // Initialize for the OpenGL rendering
        hDeviceContext = GetDC(hWnd);
        if (!bSetupPixelFormat(hDeviceContext)) {
            MessageBox(hWnd, "Error in setting up pixel format for OpenGL", "Error", MB_OK | MB_ICONERROR);
            DestroyWindow(hWnd);
        }
        hRenderingContext = wglCreateContext(hDeviceContext);
        wglMakeCurrent(hDeviceContext, hRenderingContext);

        SetTimer(hWnd, IDT_TIMER, 100, NULL);
       
        break;

    case WM_PAINT:
        {
            DrawScene(hDeviceContext);
            ValidateRect(hWnd, NULL);
        }
        break;

    case WM_SIZE:
        GetClientRect(hWnd, &rect);
        Resize(rect.right, rect.bottom);

        InvalidateRect(hWnd, NULL, false);

        break;

    case WM_TIMER:
        if (wParam == IDT_TIMER) {
            float Rect1XY[2] = { Rect1POS[0] + Rect1MOVE[0], Rect1POS[1] + Rect1MOVE[1] };
            if (!DoClide(Back_BLOCK, Rect1XY[0] - 0.1f, Rect1XY[1] - 0.1f, Rect1XY[0] + 0.1f, Rect1XY[1] + 0.1f)) {
                Rect1POS[0] += Rect1MOVE[0];
                Rect1POS[1] += Rect1MOVE[1];
            }
            else {
                Rect1MOVE[0] = -Rect1MOVE[0];
                Rect1MOVE[1] = -Rect1MOVE[1];
            }

            float Rect2XY[2] = { Rect2POS[0] + Rect2MOVE[0], Rect2POS[1] + Rect2MOVE[1] };
            if (!DoClide(Back_BLOCK, Rect2XY[0] - 0.1f, Rect2XY[1] - 0.1f, Rect2XY[0] + 0.1f, Rect2XY[1] + 0.1f)) {
                Rect2POS[0] += Rect2MOVE[0];
                Rect2POS[1] += Rect2MOVE[1];
            }
            else {
                Rect2MOVE[0] = -Rect2MOVE[0];
                Rect2MOVE[1] = -Rect2MOVE[1];
            }

            float Rect1BP[4] = {
                Rect1BLOCK[0] + Rect1POS[0], Rect1BLOCK[1] + Rect1POS[1],
                Rect1BLOCK[2] + Rect1POS[0], Rect1BLOCK[3] + Rect1POS[1]
            };

            float Rect2BP[4] = {
                Rect2BLOCK[0] + Rect2POS[0], Rect2BLOCK[1] + Rect2POS[1],
                Rect2BLOCK[2] + Rect2POS[0], Rect2BLOCK[3] + Rect2POS[1]
            };

            if (BlockCollision(Rect1BP, Rect2BP)) {
                Rect1MOVE[0] = -Rect1MOVE[0];
                // Rect1MOVE[1] = -Rect1MOVE[1];

                Rect2MOVE[0] = -Rect2MOVE[0];
                // Rect2MOVE[1] = -Rect2MOVE[1];
            }

            InvalidateRect(hWnd, NULL, false);
        }
        break;

    case WM_DESTROY:
        if (hRenderingContext)
            wglDeleteContext(hRenderingContext);
        if (hDeviceContext)
            ReleaseDC(hWnd, hDeviceContext);

        KillTimer(hWnd, IDT_TIMER);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

bool bSetupPixelFormat(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd;
    int pixelformat;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.dwLayerMask = PFD_MAIN_PLANE;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.cAccumBits = 0;
    pfd.cStencilBits = 0;

    if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == 0) {
        MessageBox(NULL, "ChoosePixelFormat() failed!!!", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    if (SetPixelFormat(hdc, pixelformat, &pfd) == false) {
        MessageBox(NULL, "SetPixelFormat() failed!!!", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

void Resize(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    int cx = width;
    int cy = height;
    glViewport(0, 0, cx, cy);

    // 3D orthographic viewing
    if (cx <= cy) {
        float ratio = (float)cy / (float)cx;
        glOrtho(-1.0, 1.0, -ratio, ratio, -1.0, 1.0);
    }
    else {
        float ratio = (float)cx / (float)cy;
        glOrtho(-ratio, ratio, -1.0, 1.0, -1.0, 1.0);
    }

    return;

}

void DrawScene(HDC MyDC)
{
    glEnable(GL_DEPTH_TEST);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float Rect1BP[4] = {
        Rect1BLOCK[0] + Rect1POS[0], Rect1BLOCK[1] + Rect1POS[1],
        Rect1BLOCK[2] + Rect1POS[0], Rect1BLOCK[3] + Rect1POS[1]
    };

    float Rect2BP[4] = {
        Rect2BLOCK[0] + Rect2POS[0], Rect2BLOCK[1] + Rect2POS[1],
        Rect2BLOCK[2] + Rect2POS[0], Rect2BLOCK[3] + Rect2POS[1]
    };

    // Rect1 Draw
    DrawRect(Rect1RGB, Rect1BP[0], Rect1BP[1], Rect1BP[2], Rect1BP[3], true);

    // Rect2 Draw
    DrawRect(Rect2RGB, Rect2BP[0], Rect2BP[1], Rect2BP[2], Rect2BP[3], true);

    // Background Draw
    DrawRect(Back_RGB, Back_BLOCK[0], Back_BLOCK[1], Back_BLOCK[2], Back_BLOCK[3], false);

    SwapBuffers(MyDC);

    return;
}

void DrawRect(float rgb[3], float left, float bottom, float right, float top, bool line) {
    if (line) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(0.0f, 0.0f, 0.0f);
        glRectf(left, bottom, right, top);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(rgb[0], rgb[1], rgb[2]);
    glRectf(left, bottom, right, top);
}

bool DoClide(float block[4], float left, float bottom, float right, float top) {
    float bound[4] = { block[0] - 0.00001f, block[1] - 0.00001f, block[2] + 0.00001f, block[3] + 0.00001f };
    if (left < bound[0] || bottom < bound[1] || right > bound[2] || top > bound[3])
        return true;
    return false;
}

bool BlockCollision(float blockA[4], float blockB[4]) {
    // 0 : left, 1 : bottom, 2 : right, 3 : top
    if (blockA[0] < blockB[2] && blockA[2] > blockB[0] && blockA[1] < blockB[3] && blockA[3] > blockB[1])
        return true;
    return false;
}