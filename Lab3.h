#pragma once
#include "framework.h"
#include "resource.h"
#include <ShellAPI.h>
#include<vector>
#include <string>
#include <objidl.h>
#include <gdiplus.h>
#include <windowsx.h>
#include <commctrl.h>
#include <WinSock2.h> 
#include <WS2tcpip.h>
#include "string.h"

#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib, "Ws2_32.lib")

#define M_PI 3.14159265358979323846
#define WINVER 0x0501 
#define _WIN32_WINNT 0x0501 
#define MAX_LOADSTRING 100
#define MAX_NET 3000

HINSTANCE hInst;

typedef void (*tfunc)(double, double, int, std::vector<double>&);
HMODULE hLib;
tfunc calculateSinArray;
tfunc calculateCosArray;

const TCHAR* title = _T("Клієнт");

Gdiplus::Color color(255, 0, 0);
Gdiplus::Pen* pen = NULL;
std::vector<double> cosX;
std::vector<double> sinX;

WNDPROC oldEditProc;
WNDPROC oldEditProc2;
HWND hEdit;
HWND hEdit2;
static HWND hColorComboBox;
static HWND hThicknessComboBox;

wchar_t textEdit1[10];
wchar_t textEdit2[10];
double start;
double end;

char server_ip[80];
int* masSend;

HHOOK hook;
HWND window = 0;
const int width = 1200;
const int height = 760;

DWORD WINAPI DrawCos(LPVOID);
DWORD WINAPI DrawSin(LPVOID);
bool isDrawed = false;
CRITICAL_SECTION cs;
HANDLE threat1;
HANDLE threat2;

void CreateTextBox(HWND hwnd, const TCHAR* name, int x, int y, int width, int height);
void CreateButton(HWND hwnd, const TCHAR* text, int x, int y, int width, int height, UINT id);
HWND CreateInputBox(HWND hwnd, int x, int y, int width, int height, WNDPROC* editProc);
HWND CreateColorMenu(HWND hWnd, HINSTANCE hInst);
HWND CreateThicknessMenu(HWND hWnd, HINSTANCE hInst);
const TCHAR* GetColorName(const Gdiplus::Color& color);
void fullMessage();