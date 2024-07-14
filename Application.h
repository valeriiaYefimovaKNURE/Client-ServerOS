#pragma once
#include "Lab3.h"
#include "Mathematics.h"
#include "Registry.h"
#include "Colors.h"

LRESULT CALLBACK Hook(int code, WPARAM wParam, LPARAM lParam) {
    LPKBDLLHOOKSTRUCT l = (LPKBDLLHOOKSTRUCT)lParam;
    if (wParam == WM_KEYDOWN) {
        if (l->vkCode == 'G' && GetKeyState(VK_CONTROL)) {
            ReadFromRegistry();
            fullMessage();
        }
        if (l->vkCode == 'R' && GetKeyState(VK_CONTROL)) {
            SaveDefaultToRegistry();
            MessageBox(NULL, L"Збережені значення по замовчюванню", title, MB_OK | MB_ICONQUESTION);
        }
        if (l->vkCode == 'C' && GetKeyState(VK_CONTROL)) {
            FillCosXAndSinX(cosX, sinX, start,end);
            SaveUserSettingsToRegistry(cosX, sinX, color.GetValue(), pen->GetWidth());
            MessageBox(NULL, L"Збережені користувацькі значення", title, MB_OK | MB_ICONQUESTION);
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}
void InitHooks() {
    hook = SetWindowsHookEx(WH_KEYBOARD_LL, Hook, GetModuleHandle(NULL), 0);
    if (!hook) {
        MessageBox(NULL, _T("Помилка при ініціалізації хука"), title, NULL);
    }
}
void InitTrayIcon() {
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = window;
    nid.uID = 1;
    nid.uFlags = NIF_TIP | NIF_ICON;
    _tcscpy_s(nid.szTip, _countof(nid.szTip), title);
    nid.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
        MessageBox(NULL, _T("Помилка при створенні іконки в панелі повідомлень"), title, MB_ICONERROR);
        ExitProcess(GetLastError());
    }
}
void DestroyTrayIcon() {
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = window;
    nid.uID = 1;
    if (!Shell_NotifyIcon(NIM_DELETE, &nid)) {
        MessageBox(NULL, _T("Помилка при видаленні іконки"), title, MB_ICONERROR);
    }
}

LRESULT CALLBACK EditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CHAR:
        if ((wParam >= '0' && wParam <= '9') || wParam == '-' || wParam == '.' || wParam == VK_BACK) {
            TCHAR buffer[256];
            GetWindowText(hwnd, buffer, sizeof(buffer) / sizeof(TCHAR));

            if (wParam == '.') {
                // Проверяем, что точка ещё не введена или введена, но не находится в начале строки
                if (_tcschr(buffer, '.') != NULL || lstrlen(buffer) == 0)
                    return 0;
            }
            if (wParam == '-') {
                if (_tcschr(buffer, '-') != NULL || lstrlen(buffer) != 0)
                    return 0;
            }
            return CallWindowProc((hwnd == hEdit) ? oldEditProc : oldEditProc2, hwnd, message, wParam, lParam);
        }
        else {
            MessageBeep(MB_ICONERROR);
            return 0;
        }
    default:
        return CallWindowProc((hwnd == hEdit) ? oldEditProc : oldEditProc2, hwnd, message, wParam, lParam);
    }
}
void CreateTextBox(HWND hwnd, const TCHAR* name, int x, int y, int width, int height) {
    CreateWindowEx(0, _T("STATIC"), name,
        WS_CHILD | WS_VISIBLE,
        x, y,width,height, hwnd, NULL, hInst, NULL);
}
void CreateButton(HWND hwnd, const TCHAR* text, int x, int y, int width, int height, UINT id) {
    CreateWindowEx(0, _T("BUTTON"), text,
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        x, y, width, height,
        hwnd, (HMENU)id, hInst, NULL);
}
HWND CreateInputBox(HWND hwnd, int x, int y, int width, int height, WNDPROC* editProc) {
    HWND hEditLocal = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        x, y, width, height, hwnd, NULL, hInst, NULL);

    if (!hEditLocal) {
        MessageBox(hwnd, _T("Не вдалось створити поле для введення"), _T("Помилка"), MB_OK | MB_ICONERROR);
    }
    else {
        *editProc = (WNDPROC)SetWindowLongPtr(hEditLocal, GWLP_WNDPROC, (LONG_PTR)EditProc);
        if (!*editProc) {
            MessageBox(hwnd, _T("Не вдалось встановити процедуру вікна для поля введення"), _T("Помилка"), MB_OK | MB_ICONERROR);
        }
    }

    return hEditLocal;
}
void fullMessage() {
    std::wstring message = L"Колір пера: ";
    message += GetColorName(color);
    message += L"\nТовщина пера: ";
    int roundedThickness = static_cast<int>(pen->GetWidth());
    message += std::to_wstring(roundedThickness) + L"\n";
    message += L"Значення CosX:\n";
    for (size_t i = 0; i < cosX.size(); ++i) {
        message += std::to_wstring(cosX[i]) + L", ";
    }
    message += L"\nЗначення SinX:\n";
    for (size_t i = 0; i < sinX.size(); ++i) {
        message += std::to_wstring(sinX[i]) + L", ";
    }
    MessageBox(window, message.c_str(), title, MB_OK | MB_ICONQUESTION);
}
void csMessage(double start, double end) {
    std::wstring message = L"Початкове значення: " + std::to_wstring(start)
        + L"\nКінцеве значення: " + std::to_wstring(end);
    message += L"\nЗначення CosX:\n";
    for (size_t i = 0; i < cosX.size(); ++i) {
        message += std::to_wstring(cosX[i]) + L", ";
    }
    message += L"\nЗначення SinX:\n";
    for (size_t i = 0; i < sinX.size(); ++i) {
        message += std::to_wstring(sinX[i]) + L", ";
    }
    MessageBox(window, message.c_str(), L"Кнопку натиснуто", MB_OK);
}
void penMessage() {
    std::wstring message = L"Колір пера: ";
    message += GetColorName(color);
    message += L"\nТовщина пера: ";
    int roundedThickness = static_cast<int>(pen->GetWidth());
    message += std::to_wstring(roundedThickness) + L"\n";
    MessageBox(window, message.c_str(), L"Кнопку натиснуто", MB_OK);
}
