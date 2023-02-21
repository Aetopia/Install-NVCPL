#include <windows.h>
#include <shlobj.h>
#include <libgen.h>

int main(int argc, char *argv[])
{
    if (CreateMutex(NULL, TRUE, "NVCPL") &&
        GetLastError() == ERROR_ALREADY_EXISTS)
        return 0;
    if (!IsUserAnAdmin())
    {
        ShellExecute(0, "runas", argv[0], NULL, NULL, SW_SHOWNORMAL);
        return 0;
    }
    SetCurrentDirectory(dirname(argv[0]));

    DWORD dwBytesNeeded;
    SERVICE_STATUS_PROCESS ssp;
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS),
              hService = OpenService(hSCManager, "NVDisplay.ContainerLocalSystem", SERVICE_ALL_ACCESS);
    SHELLEXECUTEINFO sei = {.cbSize = sizeof(SHELLEXECUTEINFO),
                            .lpVerb = "open",
                            .lpFile = "nvcplui.exe",
                            .nShow = SW_SHOWNORMAL,
                            .fMask = SEE_MASK_NOCLOSEPROCESS};

    if (hService)
    {
        ChangeServiceConfig(hService, SERVICE_NO_CHANGE, SERVICE_DEMAND_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        StartService(hService, 0, NULL);
        while (TRUE)
        {
            QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded);
            if (ssp.dwCurrentState == SERVICE_RUNNING)
                break;
        }

        ShellExecuteEx(&sei);
        WaitForSingleObject(sei.hProcess, INFINITE);

        ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp);
        while (TRUE)
        {
            QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded);
            if (ssp.dwCurrentState == SERVICE_STOPPED)
                break;
        }

        ChangeServiceConfig(hService, SERVICE_NO_CHANGE, SERVICE_DISABLED, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    };
    CloseServiceHandle(hService);

    return 0;
}
