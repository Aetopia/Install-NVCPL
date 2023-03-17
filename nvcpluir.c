#include <windows.h>
#include <shlobj.h>

BOOL DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    static DWORD dwBytesNeeded;
    static SERVICE_STATUS_PROCESS ssp;
    static SC_HANDLE hSCManager, hService;

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
            do
            {
                QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded);
            } while (ssp.dwCurrentState != SERVICE_RUNNING);
            ChangeServiceConfigW(hService, SERVICE_NO_CHANGE, SERVICE_DISABLED, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        }
        else if (fdwReason == DLL_PROCESS_DETACH)
        {
            ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp);
            do
            {
                QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded);
            } while (ssp.dwCurrentState != SERVICE_STOPPED);
            CloseServiceHandle(hSCManager);
            CloseServiceHandle(hService);
        };
    return TRUE;
}