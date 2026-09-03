#include <windows.h>
#include <shlobj.h>
#include <string>
#include <vector>

#define IDI_ICON1 101
#define IDR_BUNDLE 102

std::wstring getLocalAppDataPath() {
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
        return std::wstring(path);
    }
    return L"";
}

bool fileExists(const std::wstring& path) {
    DWORD dwAttrib = GetFileAttributesW(path.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    std::wstring localAppData = getLocalAppDataPath();
    if (localAppData.empty()) {
        MessageBoxW(NULL, L"Could not determine LocalAppData folder.", L"Space2X Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    std::wstring targetDir = localAppData + L"\\Space2X\\app_v0.1.0";
    std::wstring targetExe = targetDir + L"\\space2x-ui.exe";

    // If targetExe does not exist, extract bundle from resource
    if (!fileExists(targetExe)) {
        HRSRC hRes = FindResourceW(hInstance, MAKEINTRESOURCEW(IDR_BUNDLE), MAKEINTRESOURCEW(10));
        if (!hRes) {
            MessageBoxW(NULL, L"Embedded application payload resource not found.", L"Space2X Error", MB_ICONERROR | MB_OK);
            return 1;
        }

        HGLOBAL hResData = LoadResource(hInstance, hRes);
        if (!hResData) return 1;

        DWORD resSize = SizeofResource(hInstance, hRes);
        void* pData = LockResource(hResData);
        if (!pData || resSize == 0) return 1;

        // Ensure directory exists
        CreateDirectoryW((localAppData + L"\\Space2X").c_str(), NULL);
        CreateDirectoryW(targetDir.c_str(), NULL);

        wchar_t tempPath[MAX_PATH];
        GetTempPathW(MAX_PATH, tempPath);
        std::wstring tempZip = std::wstring(tempPath) + L"space2x_bundle.zip";

        HANDLE hFile = CreateFileW(tempZip.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            MessageBoxW(NULL, L"Failed to create temporary archive file.", L"Space2X Error", MB_ICONERROR | MB_OK);
            return 1;
        }

        DWORD bytesWritten = 0;
        WriteFile(hFile, pData, resSize, &bytesWritten, NULL);
        CloseHandle(hFile);

        // Extract using built-in tar.exe
        std::wstring cmd = L"tar.exe -xf \"" + tempZip + L"\" -C \"" + targetDir + L"\"";
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = { 0 };
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        std::vector<wchar_t> cmdBuf(cmd.begin(), cmd.end());
        cmdBuf.push_back(0);

        if (CreateProcessW(NULL, cmdBuf.data(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, 30000);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }

        DeleteFileW(tempZip.c_str());
    }

    if (!fileExists(targetExe)) {
        MessageBoxW(NULL, L"Failed to unpack Space2X application components.", L"Space2X Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // Launch space2x-ui.exe
    STARTUPINFOW siTarget = { sizeof(siTarget) };
    PROCESS_INFORMATION piTarget = { 0 };
    siTarget.dwFlags = STARTF_USESHOWWINDOW;
    siTarget.wShowWindow = nCmdShow;

    std::wstring cmdLine = L"\"" + targetExe + L"\" " + GetCommandLineW();
    std::vector<wchar_t> cmdBuf(cmdLine.begin(), cmdLine.end());
    cmdBuf.push_back(0);

    if (CreateProcessW(targetExe.c_str(), cmdBuf.data(), NULL, NULL, FALSE, 0, NULL, targetDir.c_str(), &siTarget, &piTarget)) {
        CloseHandle(piTarget.hThread);
        CloseHandle(piTarget.hProcess);
        return 0;
    }

    MessageBoxW(NULL, L"Failed to start Space2X GUI.", L"Space2X Error", MB_ICONERROR | MB_OK);
    return 1;
}
