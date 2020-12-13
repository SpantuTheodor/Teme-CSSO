#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <string.h>
#include <wchar.h>
#include <strsafe.h>

using namespace std;

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

void QueryKey(HKEY hkey, string path_reg, string path_dir)
{

    DWORD hkey_len = MAX_KEY_LENGTH;
    DWORD retCode;
    DWORD cchValue = MAX_VALUE_NAME;
    DWORD cSubKeys;
    DWORD cbMaxSubKey;
    DWORD cValues;
    DWORD cchMaxValue;
    DWORD cbMaxValueData;

    retCode = RegQueryInfoKey(
        hkey,            // key handle
        0,               // buffer for class name
        0,               // size of class string
        NULL,            // reserved
        &cSubKeys,       // number of subkeys
        &cbMaxSubKey,    // longest subkey size
        0,               // longest class string
        &cValues,        // number of values for this key
        &cchMaxValue,    // longest value name
        &cbMaxValueData, // longest value data
        0,               // security descriptor
        0);              // last write time

    if (cSubKeys)
    {
        for (DWORD index = 0; index < cSubKeys; index++)
        {
            TCHAR achKey[MAX_KEY_LENGTH]; // buffer for subkey name
            DWORD cbName;                 // size of name string
            cbName = MAX_KEY_LENGTH;

            retCode = RegEnumKeyEx(
                hkey,
                index,
                achKey,
                &cbName,
                0,
                0,
                0,
                0);

            if (retCode == ERROR_SUCCESS)
            {
                string next_path_dir = path_dir + "\\" + string(achKey);
                string next_path_reg = path_reg + "\\" + string(achKey);
                HKEY hKey;

                BOOL bDir;
                bDir = CreateDirectoryA(
                    next_path_dir.c_str(),
                    NULL);

                if (bDir == false) {
                    cout << "create directory failed, error number " << GetLastError() << endl;
                }

                if (RegOpenKeyEx(HKEY_CURRENT_USER,
                    next_path_reg.c_str(),
                    0,
                    KEY_READ,
                    &hKey) == ERROR_SUCCESS)
                {
                    QueryKey(hKey, next_path_reg, next_path_dir);
                }
                RegCloseKey(hKey);
            }
        }

    }

    if (cValues)
    {

        for (int index = 0; index < cValues; index++) {
            TCHAR  achValue[MAX_VALUE_NAME];
            DWORD cchValue = MAX_VALUE_NAME;
            cchValue = MAX_VALUE_NAME;
            achValue[0] = '\0';
            retCode = RegEnumValue(
                hkey,
                index,
                achValue,
                &cchValue,
                NULL,
                NULL,
                NULL,
                NULL);

            string next_path_file = path_dir + "\\" + string(achValue);
            if (retCode == ERROR_SUCCESS)
            {
                HANDLE hFile = CreateFile(
                    next_path_file.c_str(),     // Filename
                    GENERIC_ALL,                // Desired access
                    FILE_SHARE_READ,            // Share mode
                    NULL,                       // Security attributes
                    CREATE_NEW,                 // Creates a new file, only if it doesn't already exist
                    FILE_ATTRIBUTE_NORMAL,      // Flags and attributes
                    NULL);                      // Template file handle

            }
        }
    }

}

int main()
{
    HKEY hKey;
    string path_reg = "Software\\WinRAR";
    string path_dir = "D:\\WinRAR";

    BOOL bDir;
    bDir = CreateDirectoryA(
        "D:\\WinRAR",
        NULL);

    if (bDir == false) {
        cout << "create directory failed, error number " << GetLastError() << endl;
    }

    if (RegOpenKeyEx(HKEY_CURRENT_USER,
        TEXT("Software\\WinRAR"),
        0,
        KEY_READ,
        &hKey) == ERROR_SUCCESS)
    {
        QueryKey(hKey, path_reg, path_dir);
    }
    RegCloseKey(hKey);
}