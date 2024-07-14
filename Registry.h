#pragma once
#include "Lab3.h"

inline std::vector<double> ReadDoubleArrayFromRegistry(const std::wstring& valueName)
{
    std::vector<double> result;
    HKEY key;
    LONG openResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\KNURE_Labs\\Lab3"), 0, KEY_ALL_ACCESS, &key);
    if (openResult == ERROR_SUCCESS) {
        DWORD valueType;
        DWORD dataSize = 0;
        LONG queryResult = RegQueryValueEx(key, valueName.c_str(), nullptr, &valueType, nullptr, &dataSize);
        if (queryResult == ERROR_SUCCESS && valueType == REG_BINARY) {
            std::vector<BYTE> data(dataSize);
            LONG readResult = RegQueryValueEx(key, valueName.c_str(), nullptr, nullptr, data.data(), &dataSize);
            if (readResult == ERROR_SUCCESS) {
                size_t numDoubles = data.size() / sizeof(double);
                result.resize(numDoubles);
                memcpy(result.data(), data.data(), data.size());
            }
        }
        RegCloseKey(key);
    }
    return result;
}

inline bool SaveThicknessInRegistry() {
    Gdiplus::REAL width = pen->GetWidth();
    HKEY key;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\KNURE_Labs\\Lab3"), 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS) {
        MessageBox(NULL, _T("Thickness::Помилка при відкритті ключа."), title,
            MB_ICONERROR | MB_OK);
        RegCloseKey(key);
        return false;
    }
    if (RegSetValueEx(key, _T("Thickness"), 0, RRF_RT_REG_BINARY, (BYTE*)&width, sizeof(Gdiplus::REAL)) != ERROR_SUCCESS) {
        MessageBox(NULL, _T("Thickness::Помилка під час запису значення."), title,
            MB_ICONERROR | MB_OK);
        RegCloseKey(key);
        return false;
    }
    RegCloseKey(key);
    return true;
}
inline bool SaveColorInRegistry() {
    DWORD color = ::color.GetValue();
    HKEY key;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\KNURE_Labs\\Lab3"), 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS) {
        MessageBox(NULL, _T("Color::Помилка при відкритті ключа."), title,
            MB_ICONERROR | MB_OK);
        RegCloseKey(key);
        return false;
    }
    if (RegSetValueEx(key, _T("Color"), 0, RRF_RT_REG_DWORD, (BYTE*)&color, sizeof(DWORD)) != ERROR_SUCCESS) {
        MessageBox(NULL, _T("Color::Помилка під час запису значення."), title,
            MB_ICONERROR | MB_OK);
        RegCloseKey(key);
        return false;
    }
    RegCloseKey(key);
    return true;
}

inline void ReadFromRegistry()
{
    HKEY key;
    DWORD dword_type = RRF_RT_REG_DWORD;
    DWORD binary_type = RRF_RT_REG_BINARY;
    DWORD dword_size = sizeof(DWORD);
    DWORD greal_size = sizeof(Gdiplus::REAL);
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\KNURE_Labs\\Lab3"), 0, KEY_ALL_ACCESS, &key);
    if (result == ERROR_SUCCESS)
    {
        cosX = ReadDoubleArrayFromRegistry(L"cosX");
        sinX = ReadDoubleArrayFromRegistry(L"sinX");

        DWORD value;
        result = RegQueryValueEx(key, _T("Color"), NULL, &dword_type, (LPBYTE)&value, &dword_size);
        if (result == ERROR_SUCCESS) {
            color.SetValue(value);
            pen->SetColor(color);
        }
        Gdiplus::REAL width;
        result = RegQueryValueEx(key, _T("Thickness"), NULL, &binary_type, (LPBYTE)&width, &greal_size);
        if (result == ERROR_SUCCESS) {
            pen->SetWidth(width);
        }

        RegCloseKey(key);
    }
    else {
        MessageBox(NULL, _T("Помилка при читанні з реєстра."), title,
            MB_ICONERROR | MB_OK);
        return;
    }
}

inline void SaveDefaultToRegistry()
{
    HKEY key;
    DWORD dword_size = sizeof(DWORD);
    DWORD greal_size = sizeof(Gdiplus::REAL);
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\KNURE_Labs\\Lab3"), 0, KEY_ALL_ACCESS, &key);
    if (result == ERROR_SUCCESS)
    {
        const int numPoints = 50;
        std::vector<double> angles(numPoints);
        std::vector<double> cosX(numPoints);
        std::vector<double> sinX(numPoints);

        for (int i = 0; i < numPoints; ++i) {
            angles[i] = 2 * M_PI * i / (numPoints - 1);
        }

        for (int i = 0; i < numPoints; ++i) {
            cosX[i] = cos(angles[i]);
        }

        for (int i = 0; i < numPoints; ++i) {
            sinX[i] = sin(angles[i] + M_PI / 2);
        }

        std::vector<BYTE> cosXBytes(sizeof(double) * cosX.size());
        std::vector<BYTE> sinXBytes(sizeof(double) * sinX.size());

        for (size_t i = 0; i < cosX.size(); ++i) {
            double value = cosX[i];
            memcpy(&cosXBytes[i * sizeof(double)], &value, sizeof(double));
        }

        for (size_t i = 0; i < sinX.size(); ++i) {
            double value = sinX[i];
            memcpy(&sinXBytes[i * sizeof(double)], &value, sizeof(double));
        }

        RegSetValueEx(key, L"cosX", 0, REG_BINARY, cosXBytes.data(), cosXBytes.size());
        RegSetValueEx(key, L"sinX", 0, REG_BINARY, sinXBytes.data(), sinXBytes.size());
        SaveColorInRegistry();
        SaveThicknessInRegistry();

        RegCloseKey(key);
    }
    else if (result == ERROR_FILE_NOT_FOUND) {
        result = RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\KNURE_Labs\\Lab3"), 0,
            NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL);
        if (result != ERROR_SUCCESS)
        {
            MessageBox(NULL, _T("Помилка при створенні ключа реєстра."), title,
                MB_ICONERROR | MB_OK);
            return;
        }
        if (!SaveColorInRegistry() || !SaveThicknessInRegistry())
            return;
        const int numPoints = 50;
        std::vector<double> angles(numPoints);
        std::vector<double> cosX(numPoints);
        std::vector<double> sinX(numPoints);

        for (int i = 0; i < numPoints; ++i) {
            angles[i] = 2 * M_PI * i / (numPoints - 1);
        }

        for (int i = 0; i < numPoints; ++i) {
            cosX[i] = cos(angles[i]);
        }

        for (int i = 0; i < numPoints; ++i) {
            sinX[i] = sin(angles[i] + M_PI / 2);
        }

        std::vector<BYTE> cosXBytes(sizeof(double) * cosX.size());
        std::vector<BYTE> sinXBytes(sizeof(double) * sinX.size());

        for (size_t i = 0; i < cosX.size(); ++i) {
            double value = cosX[i];
            memcpy(&cosXBytes[i * sizeof(double)], &value, sizeof(double));
        }

        for (size_t i = 0; i < sinX.size(); ++i) {
            double value = sinX[i];
            memcpy(&sinXBytes[i * sizeof(double)], &value, sizeof(double));
        }

        RegSetValueEx(key, L"cosX", 0, REG_BINARY, cosXBytes.data(), cosXBytes.size());
        RegSetValueEx(key, L"sinX", 0, REG_BINARY, sinXBytes.data(), sinXBytes.size());
        RegCloseKey(key);
    }
    else {
        MessageBox(NULL, _T("Помилка при роботі з реєстром."), title,
            MB_ICONERROR | MB_OK);
        return;
    }
}

inline void SaveUserSettingsToRegistry(std::vector<double>& cosX, std::vector<double>& sinX, DWORD color, Gdiplus::REAL width)
{
    HKEY key;
    DWORD dword_type = RRF_RT_REG_DWORD;
    DWORD binary_type = RRF_RT_REG_BINARY;
    DWORD dword_size = sizeof(DWORD);
    DWORD greal_size = sizeof(Gdiplus::REAL);
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\KNURE_Labs\\Lab3"), 0, KEY_ALL_ACCESS, &key);
    if (result == ERROR_SUCCESS)
    {
        std::vector<BYTE> cosXBytes(sizeof(double) * cosX.size());
        std::vector<BYTE> sinXBytes(sizeof(double) * sinX.size());

        for (size_t i = 0; i < cosX.size(); ++i) {
            double value = cosX[i];
            memcpy(&cosXBytes[i * sizeof(double)], &value, sizeof(double));
        }

        for (size_t i = 0; i < sinX.size(); ++i) {
            double value = sinX[i];
            memcpy(&sinXBytes[i * sizeof(double)], &value, sizeof(double));
        }

        RegSetValueEx(key, _T("cosX"), 0, REG_BINARY, cosXBytes.data(), cosXBytes.size());
        RegSetValueEx(key, _T("sinX"), 0, REG_BINARY, sinXBytes.data(), sinXBytes.size());
        RegSetValueEx(key, _T("Color"), 0, RRF_RT_REG_DWORD, (BYTE*)&color, sizeof(DWORD));
        RegSetValueEx(key, _T("Thickness"), 0, RRF_RT_REG_BINARY, (BYTE*)&width, sizeof(Gdiplus::REAL));

        RegCloseKey(key);
    }
    else if (result == ERROR_FILE_NOT_FOUND) {
        result = RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\KNURE_Labs\\Lab3"), 0,
            NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL);
        if (result != ERROR_SUCCESS)
        {
            MessageBox(NULL, _T("Помилка при створенні ключа реєстра."), title,
                MB_ICONERROR | MB_OK);
            return;
        }
        std::vector<BYTE> cosXBytes(sizeof(double) * cosX.size());
        std::vector<BYTE> sinXBytes(sizeof(double) * sinX.size());

        for (size_t i = 0; i < cosX.size(); ++i) {
            double value = cosX[i];
            memcpy(&cosXBytes[i * sizeof(double)], &value, sizeof(double));
        }

        for (size_t i = 0; i < sinX.size(); ++i) {
            double value = sinX[i];
            memcpy(&sinXBytes[i * sizeof(double)], &value, sizeof(double));
        }

        RegSetValueEx(key, _T("cosX"), 0, REG_BINARY, cosXBytes.data(), cosXBytes.size());
        RegSetValueEx(key, _T("sinX"), 0, REG_BINARY, sinXBytes.data(), sinXBytes.size());
        RegSetValueEx(key, _T("Color"), 0, RRF_RT_REG_DWORD, (BYTE*)&color, sizeof(DWORD));
        RegSetValueEx(key, _T("Thickness"), 0, RRF_RT_REG_BINARY, (BYTE*)&width, sizeof(Gdiplus::REAL));
    }
    else {
        MessageBox(NULL, _T("Помилка при роботі з реєстром."), title,
            MB_ICONERROR | MB_OK);
        return;
    }
}
