#define _CRT_SECURE_NO_WARNINGS

#include "Lab3.h"
#include "Registry.h"
#include "Application.h"
#include "Mathematics.h"
#include "Colors.h"

WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    EditProc(HWND, UINT, WPARAM, LPARAM);

ULONG_PTR gdi_token;

void sendData();
void CreateDrawingThreads();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    Gdiplus::GdiplusStartupInput gdi_input;
    if (GdiplusStartup(&gdi_token, &gdi_input, NULL) != Gdiplus::Ok)
    {
        MessageBox(NULL,
            _T("Помилка ініціалізації GDI+."),
            title,
            MB_ICONERROR | MB_OK);
        return 1;
    }
    if (!InitializeCriticalSectionAndSpinCount(&cs, 0x00000400)) {
        MessageBox(NULL, _T("Помилка ініціалізації критичної секції для потоків."), title, MB_ICONERROR | MB_OK);
        return 1;
    }
    pen = new Gdiplus::Pen(color, 1);

    hLib = LoadLibrary(L"Dll1.dll");
    if (hLib == NULL) {
        MessageBox(NULL, _T("hLib::Помилка при роботі з DLL."), title,
            MB_ICONERROR | MB_OK);
        return 1;
    }
    calculateSinArray = (tfunc)GetProcAddress((HMODULE)hLib, "calculateSinArray");
    if (calculateSinArray == NULL) {
        MessageBox(NULL, _T("SinX::Помилка при роботі з DLL."), title,
            MB_ICONERROR | MB_OK);
        FreeLibrary(hLib);
        return 1;
    }
    calculateCosArray = (tfunc)GetProcAddress((HMODULE)hLib, "calculateCosArray");
    if (calculateCosArray == NULL) {
        MessageBox(NULL, _T("CosX::Помилка при роботі з DLL."), title,
            MB_ICONERROR | MB_OK);
        FreeLibrary(hLib);
        return 1;
    }

    InitHooks();
    
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB3));

    InitTrayIcon();

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    delete pen;
    DestroyTrayIcon();
    DeleteCriticalSection(&cs);
    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB3);
    wcex.lpszClassName  = _T("Lab3");
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(_T("Lab3"), title, WS_OVERLAPPEDWINDOW|WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, width,height, NULL, nullptr, hInstance, NULL);

   if (!hWnd)
   {
      MessageBox(NULL, _T("Помилка при створенні вікна"), title, MB_ICONERROR | MB_OK);
      return FALSE;
   }

   window = hWnd;


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        
        CreateTextBox(hWnd, L"Початкове значення:", 10, 10, 200, 25);
        hEdit=CreateInputBox(hWnd, 160, 10, 200, 25,&oldEditProc);

        CreateTextBox(hWnd, L"Кінцеве значення:", 450, 10, 200, 25);
        hEdit2=CreateInputBox(hWnd, 600, 10, 200, 25, &oldEditProc2);

        CreateButton(hWnd, L"ОК", 850, 10, 100, 25, BTN_OK);

        hColorComboBox = CreateColorMenu(hWnd, hInst);

        hThicknessComboBox = CreateThicknessMenu(hWnd, hInst);

        CreateButton(hWnd, L"Примінити", 980, 190, 150, 25, BTN_APPLY);

        CreateButton(hWnd, L"Відправити", 980, 600, 150, 25, BTN_SEND);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_Register:
        {
            ReadFromRegistry();
            fullMessage();
        }
        break;
        case IDM_DefaultRegister:
        {
            SaveDefaultToRegistry();
            MessageBox(NULL, L"Збережені значення по замовчюванню", title, MB_OK | MB_ICONQUESTION);
        }
        break;
        case IDM_UserRegister:
        {
            FillCosXAndSinX(cosX, sinX, start, end);
            SaveUserSettingsToRegistry(cosX, sinX, color.GetValue(), pen->GetWidth());
            MessageBox(NULL, L"Збережені користувацькі значення", title, MB_OK | MB_ICONQUESTION);
        }
        break;
        case BTN_OK: {
            isDrawed = false;
            int length1 = GetWindowTextLength(hEdit);
            int length2 = GetWindowTextLength(hEdit2);

            if (length1 > 0 && length2 > 0) {
                GetWindowText(hEdit, textEdit1, 10);
                GetWindowText(hEdit2, textEdit2, 10);

                start = std::stod(textEdit1);
                end = std::stod(textEdit2);

                FillCosXAndSinX(cosX, sinX, start, end);

                csMessage(start, end);
                CreateDrawingThreads();
            }
                EndDialog(hWnd, BTN_OK);
            }
            break;
        case BTN_APPLY: {
            TCHAR colorName[20];
            GetWindowText(hColorComboBox, colorName, 20);
            int colorIndex = SendMessage(hColorComboBox, CB_GETCURSEL, 0, 0);
            if (colorIndex >= 0 && colorIndex < CCLRPREDEF) {
                COLORREF selectedColor = c_rgclrPredef[colorIndex];
                color = Gdiplus::Color(GetRValue(selectedColor), GetGValue(selectedColor), GetBValue(selectedColor));
                pen->SetColor(color);
            }
            int thicknessIndex = SendMessage(hThicknessComboBox, CB_GETCURSEL, 0, 0);
            if (thicknessIndex >= 0 && thicknessIndex < Thickness_Count) {
                pen->SetWidth(thicknessValues[thicknessIndex]);
            }
            penMessage();

            EndDialog(hWnd, BTN_APPLY);
        }
        break;
        case BTN_SEND: {
            sendData();
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (isDrawed == true) {
            CreateDrawingThreads();
        }
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
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
void CreateDrawingThreads() {
    threat1 = CreateThread(NULL, 0, DrawSin, NULL, 0, NULL);
    if (threat1 == NULL) {
        MessageBox(window, L"Неможливо створити потік для Sin", L"Thread", MB_OK);
        return;
    }

    threat2 = CreateThread(NULL, 0, DrawCos, NULL, 0, NULL);
    if (threat2 == NULL) {
        MessageBox(window, L"Неможливо створити потік для Cos", L"Thread", MB_OK);
        return;
    }

    WaitForSingleObject(threat1, INFINITE);
    WaitForSingleObject(threat2, INFINITE);

    CloseHandle(threat1);
    CloseHandle(threat2);
}
DWORD WINAPI DrawCos(LPVOID) {
    HDC hdcGraph = GetDC(window);
    if (hdcGraph != NULL) {
        Gdiplus::Graphics graphics(hdcGraph);
        EnterCriticalSection(&cs);
        std::vector<Gdiplus::PointF> cosPoints;
        Gdiplus::Color color2(5, 159, 255);
        Gdiplus::Color color3(0, 0, 0);
        for (size_t i = 0; i < cosX.size(); ++i) {
            cosPoints.emplace_back(Gdiplus::PointF((i * 5) + 50, cosX[i] * 50 + 200));
        }

        Gdiplus::Pen penCos(Gdiplus::Color(5, 159, 255), pen->GetWidth());
        graphics.DrawCurve(&penCos, cosPoints.data(), cosPoints.size());

        Gdiplus::Pen penAxis(Gdiplus::Color(0, 0, 0), pen->GetWidth());
        graphics.DrawLine(&penAxis, 45, 200, 185 * 4, 200);

        TextOut(hdcGraph, 185 * 4 + 10, 190, L"Х", 1);
        LeaveCriticalSection(&cs);
        ReleaseDC(window, hdcGraph);
        isDrawed = true;
    }
    return 0;
}
DWORD WINAPI DrawSin(LPVOID) {
    HDC hdcGraph = GetDC(window);
    if (hdcGraph != NULL) {
        Gdiplus::Graphics graphics(hdcGraph);
        EnterCriticalSection(&cs);
        std::vector<Gdiplus::PointF> sinPoints;
        for (size_t i = 0; i < sinX.size(); ++i) {
            sinPoints.emplace_back(Gdiplus::PointF((i * 5) + 50, sinX[i] * 50 + 200));
        }
        graphics.DrawCurve(pen, sinPoints.data(), sinPoints.size());

        LeaveCriticalSection(&cs);
        ReleaseDC(window, hdcGraph);
        isDrawed = true;
    }
    return 0;
}
void sendData() {
    strcpy(server_ip, "127.0.0.1");
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        MessageBox(NULL, _T("Помилка ініціалізації WinSock."), _T("Client"), MB_ICONERROR | MB_OK);
        return;
    }
    struct addrinfo* res = NULL, * ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    result = getaddrinfo(server_ip, "9999", &hints, &res);
    if (result != 0) {
        MessageBox(NULL, _T("Помилка перетворення порта."), _T("Client"), MB_ICONERROR | MB_OK);
        WSACleanup(); 
        return;
    }

    SOCKET client_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (client_socket == INVALID_SOCKET) {
        MessageBox(NULL, _T("Помилка сокета."), _T("Client"), MB_OK);
        freeaddrinfo(res);
        WSACleanup();
        return;
    }

    result = connect(client_socket, res->ai_addr, (int)res->ai_addrlen);
    if (result == SOCKET_ERROR || client_socket == INVALID_SOCKET) {
        MessageBox(NULL, _T("Неможливо підключитись до сервера."), _T("Client"), MB_ICONERROR | MB_OK);
        freeaddrinfo(res);
        WSACleanup();
        return;
    }
    res = NULL;

    std::string cosXStr, sinXStr;
    for (size_t i = 0; i < cosX.size(); ++i) {
        cosXStr += std::to_string(cosX[i]) + " ";
    }
    for (size_t i = 0; i < sinX.size(); ++i) {
        sinXStr += std::to_string(sinX[i]) + " ";
    }
    std::string colorAndThickness =
        std::to_string(color.GetR()) + " " +
        std::to_string(color.GetG()) + " " +
        std::to_string(color.GetB()) + " " +
        std::to_string(pen->GetWidth()) + "|";

    std::string masNet = colorAndThickness + cosXStr + "|" + sinXStr;

    int len = masNet.size() + 1;
    if (send(client_socket, masNet.c_str(), len, 0) == SOCKET_ERROR) {
        MessageBox(NULL, _T("Помилка при передачі даних."), _T("Client"), MB_ICONERROR | MB_OK);
        freeaddrinfo(res);
        closesocket(client_socket);
        WSACleanup();
        return;
    }
    result = shutdown(client_socket, SD_RECEIVE);
    if (result == SOCKET_ERROR) {
        MessageBox(NULL, _T("Помилка відключення клієнтського сокета."), _T("Client"), MB_ICONERROR | MB_OK);
        closesocket(client_socket); WSACleanup();
        return;
    }
    result = closesocket(client_socket);
    if (result == SOCKET_ERROR) {
        MessageBox(NULL, _T("Помилка зачинення клієнтського сокета."), _T("Client"), MB_ICONERROR | MB_OK);
        WSACleanup();
        return;
    }
    WSACleanup();
    return;
}
