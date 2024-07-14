#pragma once
#include "Lab3.h"
#include "Application.h"

#define IDC_COLOR_COMBO 2000
#define IDC_THICKNESS_COMBO 2001
#define CCLRPREDEF  16              /* 16 predefined colors */

const COLORREF c_rgclrPredef[CCLRPREDEF] = {
    RGB(0x00, 0x00, 0x00),          /* 0 = black    */
    RGB(0x80, 0x00, 0x00),          /* 1 = maroon  */
    RGB(0x00, 0x80, 0x00),          /* 2 = green    */
    RGB(0x80, 0x80, 0x00),          /* 3 = olive    */
    RGB(0x00, 0x00, 0x80),          /* 4 = navy    */
    RGB(0x80, 0x00, 0x80),          /* 5 = purple  */
    RGB(0x00, 0x80, 0x80),          /* 6 = teal    */
    RGB(0x80, 0x80, 0x80),          /* 7 = gray    */
    RGB(0xC0, 0xC0, 0xC0),          /* 8 = silver  */
    RGB(0xFF, 0x00, 0x00),          /* 9 = red      */
    RGB(0x00, 0xFF, 0x00),          /* A = lime    */
    RGB(0xFF, 0xFF, 0x00),          /* B = yelow    */
    RGB(0x00, 0x00, 0xFF),          /* C = blue    */
    RGB(0xFF, 0x00, 0xFF),          /* D = fuchsia  */
    RGB(0x00, 0xFF, 0xFF),          /* E = cyan    */
    RGB(0xFF, 0xFF, 0xFF),          /* F = white    */
};

#define Thickness_Count 4
const int thicknessValues[Thickness_Count] = { 1, 2, 3, 4 };

HWND CreateColorMenu(HWND hWnd, HINSTANCE hInst)
{
    CreateTextBox(hWnd, L"Колір лінії:", 980, 55, 150, 25);

    HWND hColorComboBox = CreateWindowEx(0, WC_COMBOBOX, _T(""),
        CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        980, 80, 150, 200, hWnd, (HMENU)IDC_COLOR_COMBO, hInst, NULL);

    if (!hColorComboBox) {
        MessageBox(hWnd, _T("Не вдалось створити поле для кольорів пера"), _T("Помилка"), MB_OK | MB_ICONERROR);
    }
    else {
        for (int i = 0; i < CCLRPREDEF; ++i) {
            TCHAR szColorName[32];
            switch (i)
            {
            case 0:
                lstrcpy(szColorName, _T("Black"));
                break;
            case 1:
                lstrcpy(szColorName, _T("Maroon"));
                break;
            case 2:
                lstrcpy(szColorName, _T("Green"));
                break;
            case 3:
                lstrcpy(szColorName, _T("Olive"));
                break;
            case 4:
                lstrcpy(szColorName, _T("Navy"));
                break;
            case 5:
                lstrcpy(szColorName, _T("Purple"));
                break;
            case 6:
                lstrcpy(szColorName, _T("Teal"));
                break;
            case 7:
                lstrcpy(szColorName, _T("Gray"));
                break;
            case 8:
                lstrcpy(szColorName, _T("Silver"));
                break;
            case 9:
                lstrcpy(szColorName, _T("Red"));
                break;
            case 10:
                lstrcpy(szColorName, _T("Lime"));
                break;
            case 11:
                lstrcpy(szColorName, _T("Yellow"));
                break;
            case 12:
                lstrcpy(szColorName, _T("Blue"));
                break;
            case 13:
                lstrcpy(szColorName, _T("Fuchsia"));
                break;
            case 14:
                lstrcpy(szColorName, _T("Cyan"));
                break;
            case 15:
                lstrcpy(szColorName, _T("White"));
                break;
            }

            SendMessage(hColorComboBox, CB_ADDSTRING, 0, (LPARAM)szColorName);
        }

        SendMessage(hColorComboBox, CB_SETCURSEL, (WPARAM)0, 0);
    }
    return hColorComboBox;
}

HWND CreateThicknessMenu(HWND hWnd, HINSTANCE hInst)
{
    CreateTextBox(hWnd, L"Товщина лінії:", 980, 125, 150, 25);

    HWND hThicknessComboBox = CreateWindowEx(0, WC_COMBOBOX, _T(""),
        CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        980, 150, 150, 200, hWnd, (HMENU)IDC_THICKNESS_COMBO, hInst, NULL);

    if (!hThicknessComboBox) {
        MessageBox(hWnd, _T("Не вдалось створити випадаючий список для товщини пера"), _T("Помилка"), MB_OK | MB_ICONERROR);
    }
    else {
        for (int i = 0; i < Thickness_Count; ++i) {
            TCHAR szThickness[32];
            swprintf_s(szThickness, _T("%d"), thicknessValues[i]);

            SendMessage(hThicknessComboBox, CB_ADDSTRING, 0, (LPARAM)szThickness);
        }

        SendMessage(hThicknessComboBox, CB_SETCURSEL, (WPARAM)0, 0);
    }
    return hThicknessComboBox;
}

const TCHAR* GetColorName(const Gdiplus::Color& color) {
    COLORREF colorRef = RGB(color.GetR(), color.GetG(), color.GetB());

    for (int i = 0; i < CCLRPREDEF; ++i) {
        if (c_rgclrPredef[i] == colorRef) {
            switch (i)
            {
            case 0: return _T("Black");
            case 1: return _T("Maroon");
            case 2: return _T("Green");
            case 3: return _T("Olive");
            case 4: return _T("Navy");
            case 5: return _T("Purple");
            case 6: return _T("Teal");
            case 7: return _T("Gray");
            case 8: return _T("Silver");
            case 9: return _T("Red");
            case 10: return _T("Lime");
            case 11: return _T("Yellow");
            case 12: return _T("Blue");
            case 13: return _T("Fuchsia");
            case 14: return _T("Cyan");
            case 15: return _T("White");
            }
        }
    }
    return _T("Невідомо");
}