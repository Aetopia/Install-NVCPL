#include <windows.h>
#include <shlobj.h>
#include <wtsapi32.h>

BOOL DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    static WTS_PROCESS_INFOW *pWPI;
    static DWORD dwBytesNeeded, dwCount;
    static SERVICE_STATUS_PROCESS ssp;
    static SC_HANDLE hSCManager, hService;
    static HANDLE hProcess;

    if (IsUserAnAdmin())
        if (fdwReason == DLL_PROCESS_ATTACH)
        {
            if (CreateMutexW(NULL, TRUE, L"NVIDIA Control Panel") &&
                GetLastError() == ERROR_ALREADY_EXISTS)
                TerminateProcess(GetCurrentProcess(), 0);
            hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS),
            hService = OpenServiceW(hSCManager, L"NVDisplay.ContainerLocalSystem", SERVICE_ALL_ACCESS);
            ChangeServiceConfigW(hService, SERVICE_NO_CHANGE, SERVICE_DEMAND_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            StartServiceW(hService, 0, NULL);
            ChangeServiceConfigW(hService, SERVICE_NO_CHANGE, SERVICE_DISABLED, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            do
                QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded);
            while (ssp.dwCurrentState != SERVICE_RUNNING);
        }
        else if (fdwReason == DLL_PROCESS_DETACH)
        {
            ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp);
            CloseServiceHandle(hSCManager);
            CloseServiceHandle(hService);
            WTSEnumerateProcessesW(WTS_CURRENT_SERVER, 0, 1, &pWPI, &dwCount);
            for (DWORD i = 0; i < dwCount; i++)
            {
                if (wcscmp(pWPI[i].pProcessName, L"NVDisplay.Container.exe"))
                    continue;
                hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pWPI[i].ProcessId);
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            };
        };
    return TRUE;
}