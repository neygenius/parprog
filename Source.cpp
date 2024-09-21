#include <iostream>
#include <windows.h>
#include <lm.h>
#include <string>
#include <vector>
#include <filesystem>

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "mpr.lib")

using namespace std;

// ��������� ��� �������� ������ � ��������
struct ServerInfo {
    wstring name;
};

// ������� ��� ��������� ������ ��������� �����
vector<ServerInfo> getRemoteMachines() {
    vector<ServerInfo> servers;

    PSERVER_INFO_100 pBuf = NULL;
    PSERVER_INFO_100 pTmpBuf;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    NET_API_STATUS nStatus;
    LPCWSTR pszServerName = NULL;  // NULL ��� ���������� ������
    DWORD dwLevel = 100;  // ������� 100, ����� �������� ������ ����� ��������
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwServerType = SV_TYPE_WORKSTATION | SV_TYPE_SERVER;  // ���� �������� (������� ������� � �������)

    nStatus = NetServerEnum(
        pszServerName,
        dwLevel,
        (LPBYTE*)&pBuf,
        dwPrefMaxLen,
        &dwEntriesRead,
        &dwTotalEntries,
        dwServerType,
        NULL,
        &dwResumeHandle
    );

    if ((nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) && pBuf != NULL) {
        pTmpBuf = pBuf;

        for (DWORD i = 0; i < dwEntriesRead; i++) {
            if (pTmpBuf == NULL) {
                break;
            }
            wstring serverName = L"\\\\" + wstring(pTmpBuf->sv100_name);
            servers.push_back({ serverName });
            pTmpBuf++;
        }
    }
    else {
        wcout << L"Failed to enumerate servers. Error: " << nStatus << endl;
    }

    if (pBuf != NULL) {
        NetApiBufferFree(pBuf);
    }

    return servers;
}

// ������� ��� ������ ��������� ������
wstring chooseRemoteMachine(const vector<ServerInfo>& servers) {
    int choice;
    wcout << L"Select remote machine:" << endl;
    for (size_t i = 0; i < servers.size(); i++) {
        wcout << i + 1 << L". " << servers[i].name << endl;
    }
    wcout << L"Enter number: ";
    cin >> choice;

    if (choice > 0 && choice <= servers.size()) {
        return servers[choice - 1].name;
    }
    else {
        wcout << L"Invalid choice, defaulting to local machine." << endl;
        return L"";
    }
}

// ������� ��� ����� ������� ������
void getUserCredentials(wstring& username, wstring& password) {
    wcout << L"Enter username: ";
    wcin.ignore();
    getline(wcin, username);

    wcout << L"Enter password: ";
    getline(wcin, password);
}

bool connectToNetworkResource(const std::wstring& networkPath, const std::wstring& userName, const std::wstring& password) {
    NETRESOURCE nr;
    DWORD result;

    ZeroMemory(&nr, sizeof(nr));
    nr.dwType = RESOURCETYPE_DISK;
    nr.lpLocalName = NULL;  // ��������� NULL, ���� �� ���������� ���� ��������
    nr.lpRemoteName = const_cast<LPWSTR>(networkPath.c_str());  // ��������� ������
    nr.lpProvider = NULL;   // ������ NULL, ���� �� ������������ ���������

    result = WNetAddConnection2(&nr, const_cast<LPWSTR>(password.c_str()), const_cast<LPWSTR>(userName.c_str()), 0);

    if (result == NO_ERROR) {
        std::wcout << L"Successfully connected to " << networkPath << std::endl;
        return true;
    }
    else {
        std::wcout << L"Failed to connect to " << networkPath << L". Error: " << result << std::endl;
        return false;
    }
}


// ������� ��� ������������ ����� ��������
void enumNetworkShares(const wchar_t* serverName) {
    PSHARE_INFO_502 pBuf = NULL;

    DWORD dwEntriesRead = 0, dwTotalEntries = 0, dwResumeHandle = 0;
    NET_API_STATUS nStatus;

    nStatus = NetShareEnum(
        const_cast<LPWSTR>(serverName),
        502,
        (LPBYTE*)&pBuf,
        MAX_PREFERRED_LENGTH,
        &dwEntriesRead,
        &dwTotalEntries,
        &dwResumeHandle
    );

    if (nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) {
        PSHARE_INFO_502 pTmpBuf = pBuf;
        for (DWORD i = 0; i < dwEntriesRead; i++) {
            if (pTmpBuf == NULL) break;
            wcout << L"Share name: " << pTmpBuf->shi502_netname << endl;
            wcout << L"Path: " << pTmpBuf->shi502_path << endl;
            wcout << L"--------------------------------------" << endl;
            pTmpBuf++;
        }
    }
    else {
        wcout << L"Error: " << nStatus << endl;
    }

    if (pBuf != NULL) {
        NetApiBufferFree(pBuf);
    }
}

bool createDirectory(const wstring& path) {
    // ���������� CreateDirectory �� Windows API
    return CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

bool createNetworkShare(const wchar_t* serverName, const std::wstring& shareName, const std::wstring& path, const std::wstring& description) {
    SHARE_INFO_502 shareInfo;
    DWORD result;

    ZeroMemory(&shareInfo, sizeof(shareInfo));
    shareInfo.shi502_netname = const_cast<LPWSTR>(shareName.c_str());  // ��� ������ �������
    shareInfo.shi502_type = STYPE_DISKTREE;  // ��� ������� (����)
    shareInfo.shi502_remark = const_cast<LPWSTR>(description.c_str()); // �������� ������ �������
    shareInfo.shi502_permissions = ACCESS_ALL; // ����� ������� (������ �����)
    shareInfo.shi502_max_uses = -1; // �� ������������ ���������� �����������
    shareInfo.shi502_path = const_cast<LPWSTR>(path.c_str()); // ���� � ����� �����
    shareInfo.shi502_passwd = NULL; // ������ (���� �� ������������)

    result = NetShareAdd(const_cast<LPWSTR>(serverName), 502, (LPBYTE)&shareInfo, NULL);

    if (result == NERR_Success) {
        std::wcout << L"Successfully added share: " << shareName << std::endl;
        return true;
    }
    else {
        std::wcout << L"Failed to add share. Error: " << result << std::endl;
        // ������� ��������� ��������� �� ������
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            result,
            0,
            (LPWSTR)&lpMsgBuf,
            0,
            NULL
        );

        LocalFree(lpMsgBuf);
        return false;
    }
}

bool disconnectFromNetworkResource(const std::wstring& networkPath, bool forceClose) {
    DWORD result = WNetCancelConnection2(networkPath.c_str(), 0, forceClose);

    if (result == NO_ERROR) {
        std::wcout << L"Successfully disconnected from " << networkPath << std::endl;
        return true;
    }
    else {
        std::wcout << L"Failed to disconnect from " << networkPath << L". Error: " << result << std::endl;
        return false;
    }
}

int main() {
    vector<ServerInfo> servers = getRemoteMachines();

    int choice;
    wstring serverName;
    wstring username;
    wstring password;
    bool exitFlag = false;



    while (!exitFlag) {
        wcout << L"1. View network shares" << endl;
        wcout << L"2. Delete a network share" << endl;
        wcout << L"0. Exit" << endl;
        wcout << L"Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            serverName = chooseRemoteMachine(servers);

            enumNetworkShares(serverName.empty() ? NULL : serverName.c_str());

            break;
        }
        case 2: {
            wstring networkPath;
            wcout << L"Enter network path to disconnect: ";
            wcin.ignore();
            getline(wcin, networkPath);

            bool forceClose = true;  // Set to true to force close the connection
            disconnectFromNetworkResource(networkPath, forceClose);
            break;
        }
        case 0:
            exitFlag = true;
            break;
        default:
            wcout << L"Invalid choice. Please try again." << endl;
        }
    }



    //NETRESOURCE nr;
    //memset(&nr, 0, sizeof(nr));
    //nr.dwType = RESOURCETYPE_DISK;
    //nr.lpRemoteName = const_cast<LPWSTR>(L"\\\\DESKTOP-MGCLDV2");

    //DWORD result = WNetAddConnection2(&nr, L"1234", L"test", 0);
    //if (result != NO_ERROR) {
    //    wcout << L"Failed to connect. Error: " << result << endl;
    //}
    //else {
    //    // ������ ����� ������������ NetShareEnum
    //    PSHARE_INFO_502 pBuf = NULL;
    //    DWORD dwEntriesRead = 0, dwTotalEntries = 0, dwResumeHandle = 0;
    //    NET_API_STATUS nStatus;

    //    nStatus = NetShareEnum(
    //        const_cast<LPWSTR>(L"\\\\DESKTOP-MGCLDV2"),
    //        502,
    //        (LPBYTE*)&pBuf,
    //        MAX_PREFERRED_LENGTH,
    //        &dwEntriesRead,
    //        &dwTotalEntries,
    //        &dwResumeHandle
    //    );

    //    if (nStatus == NERR_Success || nStatus == ERROR_MORE_DATA) {
    //        PSHARE_INFO_502 pTmpBuf = pBuf;
    //        for (DWORD i = 0; i < dwEntriesRead; i++) {
    //            if (pTmpBuf == NULL) break;
    //            wcout << L"Share name: " << pTmpBuf->shi502_netname << endl;
    //            wcout << L"Path: " << pTmpBuf->shi502_path << endl;
    //            wcout << L"--------------------------------------" << endl;
    //            pTmpBuf++;
    //        }
    //    }
    //    else {
    //        wcout << L"Error: " << nStatus << endl;
    //    }

    //    if (pBuf != NULL) {
    //        NetApiBufferFree(pBuf);
    //    }
    //}


    //SHARE_INFO_502 si502;
    //NET_API_STATUS nStatus;
    //DWORD parmErr;

    //// �������� ���������
    //ZeroMemory(&si502, sizeof(si502));


    //// ��������� ��������� SHARE_INFO_502
    //si502.shi502_netname = const_cast<LPWSTR>(L"Test");
    //si502.shi502_type = STYPE_DISKTREE; // ��� ������ �������
    //si502.shi502_remark = NULL; // �����������, ���� �����
    //si502.shi502_permissions = ACCESS_READ; // ����� �������
    //si502.shi502_max_uses = -1; // �������������� ���������� �����������
    //si502.shi502_current_uses = 0; // ������� ���������� �����������
    //si502.shi502_path = const_cast<LPWSTR>(L"C:/Server");
    //si502.shi502_passwd = NULL; // ������, ���� �����
    //si502.shi502_reserved = 0;
    //si502.shi502_security_descriptor = NULL; // ���������� ������������, ���� �����

    //// ����� NetShareAdd ��� ���������� ������ �������
    //nStatus = NetShareAdd(
    //    serverName.empty() ? NULL : const_cast<LPWSTR>(serverName.c_str()),
    //    502,
    //    (LPBYTE)&si502,
    //    &parmErr
    //);

    return 0;
}
