// ms example collector
#define __SNAPSHOT__



#ifdef __LOGON__
// Getting the Logon SID in C++
// https://docs.microsoft.com/en-us/previous-versions/aa446670(v=vs.85)
#include <windows.h>
#pragma comment(lib, "advapi32.lib")

BOOL GetLogonSID(HANDLE hToken, PSID *ppsid)
{
	BOOL bSuccess = FALSE;
	DWORD dwIndex;
	DWORD dwLength = 0;
	PTOKEN_GROUPS ptg = NULL;

	// Verify the parameter passed in is not NULL.
	if (NULL == ppsid)
		goto Cleanup;

	// Get required buffer size and allocate the TOKEN_GROUPS buffer.

	if (!GetTokenInformation(
		hToken,         // handle to the access token
		TokenGroups,    // get information about the token's groups 
		(LPVOID)ptg,   // pointer to TOKEN_GROUPS buffer
		0,              // size of buffer
		&dwLength       // receives required buffer size
	))
	{
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			goto Cleanup;

		ptg = (PTOKEN_GROUPS)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY, dwLength);

		if (ptg == NULL)
			goto Cleanup;
	}

	// Get the token group information from the access token.

	if (!GetTokenInformation(
		hToken,         // handle to the access token
		TokenGroups,    // get information about the token's groups 
		(LPVOID)ptg,   // pointer to TOKEN_GROUPS buffer
		dwLength,       // size of buffer
		&dwLength       // receives required buffer size
	))
	{
		goto Cleanup;
	}

	// Loop through the groups to find the logon SID.

	for (dwIndex = 0; dwIndex < ptg->GroupCount; dwIndex++)
		if ((ptg->Groups[dwIndex].Attributes & SE_GROUP_LOGON_ID)
			== SE_GROUP_LOGON_ID)
		{
			// Found the logon SID; make a copy of it.

			dwLength = GetLengthSid(ptg->Groups[dwIndex].Sid);
			*ppsid = (PSID)HeapAlloc(GetProcessHeap(),
				HEAP_ZERO_MEMORY, dwLength);
			if (*ppsid == NULL)
				goto Cleanup;
			if (!CopySid(dwLength, *ppsid, ptg->Groups[dwIndex].Sid))
			{
				HeapFree(GetProcessHeap(), 0, (LPVOID)*ppsid);
				goto Cleanup;
			}
			break;
		}

	bSuccess = TRUE;

Cleanup:

	// Free the buffer for the token groups.

	if (ptg != NULL)
		HeapFree(GetProcessHeap(), 0, (LPVOID)ptg);

	return bSuccess;
}

#include <windows.h>
#pragma comment(lib, "advapi32.lib")

VOID FreeLogonSID(PSID *ppsid)
{
	HeapFree(GetProcessHeap(), 0, (LPVOID)*ppsid);
}


// 在 C++ 中启动交互式客户端进程
// https://docs.microsoft.com/en-us/previous-versions/aa379608(v=vs.85)
#include <windows.h>
#include <Tchar.h>

#define DESKTOP_ALL (DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW | \
DESKTOP_CREATEMENU | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALRECORD | \
DESKTOP_JOURNALPLAYBACK | DESKTOP_ENUMERATE | DESKTOP_WRITEOBJECTS | \
DESKTOP_SWITCHDESKTOP | STANDARD_RIGHTS_REQUIRED)

#define WINSTA_ALL (WINSTA_ENUMDESKTOPS | WINSTA_READATTRIBUTES | \
WINSTA_ACCESSCLIPBOARD | WINSTA_CREATEDESKTOP | \
WINSTA_WRITEATTRIBUTES | WINSTA_ACCESSGLOBALATOMS | \
WINSTA_EXITWINDOWS | WINSTA_ENUMERATE | WINSTA_READSCREEN | \
STANDARD_RIGHTS_REQUIRED)

#define GENERIC_ACCESS (GENERIC_READ | GENERIC_WRITE | \
GENERIC_EXECUTE | GENERIC_ALL)

BOOL AddAceToWindowStation(HWINSTA hwinsta, PSID psid);

BOOL AddAceToDesktop(HDESK hdesk, PSID psid);

BOOL GetLogonSID(HANDLE hToken, PSID *ppsid);

VOID FreeLogonSID(PSID *ppsid);

BOOL StartInteractiveClientProcess(
	LPTSTR lpszUsername,    // client to log on
	LPTSTR lpszDomain,      // domain of client's account
	LPTSTR lpszPassword,    // client's password
	LPTSTR lpCommandLine    // command line to execute
)
{
	HANDLE      hToken;
	HDESK       hdesk = NULL;
	HWINSTA     hwinsta = NULL, hwinstaSave = NULL;
	PROCESS_INFORMATION pi;
	PSID pSid = NULL;
	STARTUPINFO si;
	BOOL bResult = FALSE;

	// Log the client on to the local computer.

	if (!LogonUser(
		lpszUsername,
		lpszDomain,
		lpszPassword,
		LOGON32_LOGON_INTERACTIVE,
		LOGON32_PROVIDER_DEFAULT,
		&hToken))
	{
		goto Cleanup;
	}

	// Save a handle to the caller's current window station.

	if ((hwinstaSave = GetProcessWindowStation()) == NULL)
		goto Cleanup;

	// Get a handle to the interactive window station.

	hwinsta = OpenWindowStation(
		_T("winsta0"),                   // the interactive window station 
		FALSE,                       // handle is not inheritable
		READ_CONTROL | WRITE_DAC);   // rights to read/write the DACL

	if (hwinsta == NULL)
		goto Cleanup;

	// To get the correct default desktop, set the caller's 
	// window station to the interactive window station.

	if (!SetProcessWindowStation(hwinsta))
		goto Cleanup;

	// Get a handle to the interactive desktop.

	hdesk = OpenDesktop(
		_T("default"),     // the interactive window station 
		0,             // no interaction with other desktop processes
		FALSE,         // handle is not inheritable
		READ_CONTROL | // request the rights to read and write the DACL
		WRITE_DAC |
		DESKTOP_WRITEOBJECTS |
		DESKTOP_READOBJECTS);

	// Restore the caller's window station.

	if (!SetProcessWindowStation(hwinstaSave))
		goto Cleanup;

	if (hdesk == NULL)
		goto Cleanup;

	// Get the SID for the client's logon session.

	if (!GetLogonSID(hToken, &pSid))
		goto Cleanup;

	// Allow logon SID full access to interactive window station.

	if (!AddAceToWindowStation(hwinsta, pSid))
		goto Cleanup;

	// Allow logon SID full access to interactive desktop.

	if (!AddAceToDesktop(hdesk, pSid))
		goto Cleanup;

	// Impersonate client to ensure access to executable file.

	if (!ImpersonateLoggedOnUser(hToken))
		goto Cleanup;

	// Initialize the STARTUPINFO structure.
	// Specify that the process runs in the interactive desktop.

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = TEXT("winsta0\\default");

	// Launch the process in the client's logon session.

	bResult = CreateProcessAsUser(
		hToken,            // client's access token
		NULL,              // file to execute
		lpCommandLine,     // command line
		NULL,              // pointer to process SECURITY_ATTRIBUTES
		NULL,              // pointer to thread SECURITY_ATTRIBUTES
		FALSE,             // handles are not inheritable
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,   // creation flags
		NULL,              // pointer to new environment block 
		NULL,              // name of current directory 
		&si,               // pointer to STARTUPINFO structure
		&pi                // receives information about new process
	);

	// End impersonation of client.

	RevertToSelf();

	if (bResult && pi.hProcess != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}

	if (pi.hThread != INVALID_HANDLE_VALUE)
		CloseHandle(pi.hThread);

Cleanup:

	if (hwinstaSave != NULL)
		SetProcessWindowStation(hwinstaSave);

	// Free the buffer for the logon SID.

	if (pSid)
		FreeLogonSID(&pSid);

	// Close the handles to the interactive window station and desktop.

	if (hwinsta)
		CloseWindowStation(hwinsta);

	if (hdesk)
		CloseDesktop(hdesk);

	// Close the handle to the client's access token.

	if (hToken != INVALID_HANDLE_VALUE)
		CloseHandle(hToken);

	return bResult;
}

BOOL AddAceToWindowStation(HWINSTA hwinsta, PSID psid)
{
	ACCESS_ALLOWED_ACE   *pace = NULL;
	ACL_SIZE_INFORMATION aclSizeInfo;
	BOOL                 bDaclExist;
	BOOL                 bDaclPresent;
	BOOL                 bSuccess = FALSE;
	DWORD                dwNewAclSize;
	DWORD                dwSidSize = 0;
	DWORD                dwSdSizeNeeded;
	PACL                 pacl;
	PACL                 pNewAcl = NULL;
	PSECURITY_DESCRIPTOR psd = NULL;
	PSECURITY_DESCRIPTOR psdNew = NULL;
	PVOID                pTempAce;
	SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
	unsigned int         i;

	__try
	{
		// Obtain the DACL for the window station.

		if (!GetUserObjectSecurity(
			hwinsta,
			&si,
			psd,
			dwSidSize,
			&dwSdSizeNeeded)
			)
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					dwSdSizeNeeded);

				if (psd == NULL)
					__leave;

				psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					dwSdSizeNeeded);

				if (psdNew == NULL)
					__leave;

				dwSidSize = dwSdSizeNeeded;

				if (!GetUserObjectSecurity(
					hwinsta,
					&si,
					psd,
					dwSidSize,
					&dwSdSizeNeeded)
					)
					__leave;
			}
			else
				__leave;

		// Create a new DACL.

		if (!InitializeSecurityDescriptor(
			psdNew,
			SECURITY_DESCRIPTOR_REVISION)
			)
			__leave;

		// Get the DACL from the security descriptor.

		if (!GetSecurityDescriptorDacl(
			psd,
			&bDaclPresent,
			&pacl,
			&bDaclExist)
			)
			__leave;

		// Initialize the ACL.

		ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
		aclSizeInfo.AclBytesInUse = sizeof(ACL);

		// Call only if the DACL is not NULL.

		if (pacl != NULL)
		{
			// get the file ACL size info
			if (!GetAclInformation(
				pacl,
				(LPVOID)&aclSizeInfo,
				sizeof(ACL_SIZE_INFORMATION),
				AclSizeInformation)
				)
				__leave;
		}

		// Compute the size of the new ACL.

		dwNewAclSize = aclSizeInfo.AclBytesInUse +
			(2 * sizeof(ACCESS_ALLOWED_ACE)) + (2 * GetLengthSid(psid)) -
			(2 * sizeof(DWORD));

		// Allocate memory for the new ACL.

		pNewAcl = (PACL)HeapAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			dwNewAclSize);

		if (pNewAcl == NULL)
			__leave;

		// Initialize the new DACL.

		if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
			__leave;

		// If DACL is present, copy it to a new DACL.

		if (bDaclPresent)
		{
			// Copy the ACEs to the new ACL.
			if (aclSizeInfo.AceCount)
			{
				for (i = 0; i < aclSizeInfo.AceCount; i++)
				{
					// Get an ACE.
					if (!GetAce(pacl, i, &pTempAce))
						__leave;

					// Add the ACE to the new ACL.
					if (!AddAce(
						pNewAcl,
						ACL_REVISION,
						MAXDWORD,
						pTempAce,
						((PACE_HEADER)pTempAce)->AceSize)
						)
						__leave;
				}
			}
		}

		// Add the first ACE to the window station.

		pace = (ACCESS_ALLOWED_ACE *)HeapAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) -
			sizeof(DWORD));

		if (pace == NULL)
			__leave;

		pace->Header.AceType = ACCESS_ALLOWED_ACE_TYPE;
		pace->Header.AceFlags = CONTAINER_INHERIT_ACE |
			INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE;
		pace->Header.AceSize = LOWORD(sizeof(ACCESS_ALLOWED_ACE) +
			GetLengthSid(psid) - sizeof(DWORD));
		pace->Mask = GENERIC_ACCESS;

		if (!CopySid(GetLengthSid(psid), &pace->SidStart, psid))
			__leave;

		if (!AddAce(
			pNewAcl,
			ACL_REVISION,
			MAXDWORD,
			(LPVOID)pace,
			pace->Header.AceSize)
			)
			__leave;

		// Add the second ACE to the window station.

		pace->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
		pace->Mask = WINSTA_ALL;

		if (!AddAce(
			pNewAcl,
			ACL_REVISION,
			MAXDWORD,
			(LPVOID)pace,
			pace->Header.AceSize)
			)
			__leave;

		// Set a new DACL for the security descriptor.

		if (!SetSecurityDescriptorDacl(
			psdNew,
			TRUE,
			pNewAcl,
			FALSE)
			)
			__leave;

		// Set the new security descriptor for the window station.

		if (!SetUserObjectSecurity(hwinsta, &si, psdNew))
			__leave;

		// Indicate success.

		bSuccess = TRUE;
	}
	__finally
	{
		// Free the allocated buffers.

		if (pace != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID)pace);

		if (pNewAcl != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

		if (psd != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

		if (psdNew != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);
	}

	return bSuccess;

}

BOOL AddAceToDesktop(HDESK hdesk, PSID psid)
{
	ACL_SIZE_INFORMATION aclSizeInfo;
	BOOL                 bDaclExist;
	BOOL                 bDaclPresent;
	BOOL                 bSuccess = FALSE;
	DWORD                dwNewAclSize;
	DWORD                dwSidSize = 0;
	DWORD                dwSdSizeNeeded;
	PACL                 pacl;
	PACL                 pNewAcl = NULL;
	PSECURITY_DESCRIPTOR psd = NULL;
	PSECURITY_DESCRIPTOR psdNew = NULL;
	PVOID                pTempAce;
	SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
	unsigned int         i;

	__try
	{
		// Obtain the security descriptor for the desktop object.

		if (!GetUserObjectSecurity(
			hdesk,
			&si,
			psd,
			dwSidSize,
			&dwSdSizeNeeded))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					dwSdSizeNeeded);

				if (psd == NULL)
					__leave;

				psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
					GetProcessHeap(),
					HEAP_ZERO_MEMORY,
					dwSdSizeNeeded);

				if (psdNew == NULL)
					__leave;

				dwSidSize = dwSdSizeNeeded;

				if (!GetUserObjectSecurity(
					hdesk,
					&si,
					psd,
					dwSidSize,
					&dwSdSizeNeeded)
					)
					__leave;
			}
			else
				__leave;
		}

		// Create a new security descriptor.

		if (!InitializeSecurityDescriptor(
			psdNew,
			SECURITY_DESCRIPTOR_REVISION)
			)
			__leave;

		// Obtain the DACL from the security descriptor.

		if (!GetSecurityDescriptorDacl(
			psd,
			&bDaclPresent,
			&pacl,
			&bDaclExist)
			)
			__leave;

		// Initialize.

		ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
		aclSizeInfo.AclBytesInUse = sizeof(ACL);

		// Call only if NULL DACL.

		if (pacl != NULL)
		{
			// Determine the size of the ACL information.

			if (!GetAclInformation(
				pacl,
				(LPVOID)&aclSizeInfo,
				sizeof(ACL_SIZE_INFORMATION),
				AclSizeInformation)
				)
				__leave;
		}

		// Compute the size of the new ACL.

		dwNewAclSize = aclSizeInfo.AclBytesInUse +
			sizeof(ACCESS_ALLOWED_ACE) +
			GetLengthSid(psid) - sizeof(DWORD);

		// Allocate buffer for the new ACL.

		pNewAcl = (PACL)HeapAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			dwNewAclSize);

		if (pNewAcl == NULL)
			__leave;

		// Initialize the new ACL.

		if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
			__leave;

		// If DACL is present, copy it to a new DACL.

		if (bDaclPresent)
		{
			// Copy the ACEs to the new ACL.
			if (aclSizeInfo.AceCount)
			{
				for (i = 0; i < aclSizeInfo.AceCount; i++)
				{
					// Get an ACE.
					if (!GetAce(pacl, i, &pTempAce))
						__leave;

					// Add the ACE to the new ACL.
					if (!AddAce(
						pNewAcl,
						ACL_REVISION,
						MAXDWORD,
						pTempAce,
						((PACE_HEADER)pTempAce)->AceSize)
						)
						__leave;
				}
			}
		}

		// Add ACE to the DACL.

		if (!AddAccessAllowedAce(
			pNewAcl,
			ACL_REVISION,
			DESKTOP_ALL,
			psid)
			)
			__leave;

		// Set new DACL to the new security descriptor.

		if (!SetSecurityDescriptorDacl(
			psdNew,
			TRUE,
			pNewAcl,
			FALSE)
			)
			__leave;

		// Set the new security descriptor for the desktop object.

		if (!SetUserObjectSecurity(hdesk, &si, psdNew))
			__leave;

		// Indicate success.

		bSuccess = TRUE;
	}
	__finally
	{
		// Free buffers.

		if (pNewAcl != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

		if (psd != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

		if (psdNew != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);
	}

	return bSuccess;
}

int main() {
	StartInteractiveClientProcess(L"Microsoft", L"", NULL, L"c://windows//notepad.exe");
	return 0;
}
#endif  // __LOGON__

#ifdef __SNAPSHOT__
// Taking a Snapshot and Viewing Processes
// https://docs.microsoft.com/en-us/windows/win32/toolhelp/taking-a-snapshot-and-viewing-processes
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

//  Forward declarations:
BOOL GetProcessList();
BOOL ListProcessModules(DWORD dwPID);
BOOL ListProcessThreads(DWORD dwOwnerPID);
void printError(TCHAR* msg);

int main(void)
{
	GetProcessList();
	system("pause");
	return 0;
}

BOOL GetProcessList()
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printError(TEXT("CreateToolhelp32Snapshot (of processes)"));
		return(FALSE);
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		printError(TEXT("Process32First")); // show cause of failure
		CloseHandle(hProcessSnap);          // clean the snapshot object
		return(FALSE);
	}

	// Now walk the snapshot of processes, and
	// display information about each process in turn
	do
	{
		_tprintf(TEXT("\n\n====================================================="));
		_tprintf(TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile);
		_tprintf(TEXT("\n-------------------------------------------------------"));

		// Retrieve the priority class.
		dwPriorityClass = 0;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if (hProcess == NULL)
			printError(TEXT("OpenProcess"));
		else
		{
			dwPriorityClass = GetPriorityClass(hProcess);
			if (!dwPriorityClass)
				printError(TEXT("GetPriorityClass"));
			CloseHandle(hProcess);
		}

		_tprintf(TEXT("\n  Process ID        = 0x%08X"), pe32.th32ProcessID);
		_tprintf(TEXT("\n  Thread count      = %d"), pe32.cntThreads);
		_tprintf(TEXT("\n  Parent process ID = 0x%08X"), pe32.th32ParentProcessID);
		_tprintf(TEXT("\n  Priority base     = %d"), pe32.pcPriClassBase);
		if (dwPriorityClass)
			_tprintf(TEXT("\n  Priority class    = %d"), dwPriorityClass);

		// List the modules and threads associated with this process
		ListProcessModules(pe32.th32ProcessID);
		ListProcessThreads(pe32.th32ProcessID);

	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return(TRUE);
}


BOOL ListProcessModules(DWORD dwPID)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	// Take a snapshot of all modules in the specified process.
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		printError(TEXT("CreateToolhelp32Snapshot (of modules)"));
		return(FALSE);
	}

	// Set the size of the structure before using it.
	me32.dwSize = sizeof(MODULEENTRY32);

	// Retrieve information about the first module,
	// and exit if unsuccessful
	if (!Module32First(hModuleSnap, &me32))
	{
		printError(TEXT("Module32First"));  // show cause of failure
		CloseHandle(hModuleSnap);           // clean the snapshot object
		return(FALSE);
	}

	// Now walk the module list of the process,
	// and display information about each module
	do
	{
		_tprintf(TEXT("\n\n     MODULE NAME:     %s"), me32.szModule);
		_tprintf(TEXT("\n     Executable     = %s"), me32.szExePath);
		_tprintf(TEXT("\n     Process ID     = 0x%08X"), me32.th32ProcessID);
		_tprintf(TEXT("\n     Ref count (g)  = 0x%04X"), me32.GlblcntUsage);
		_tprintf(TEXT("\n     Ref count (p)  = 0x%04X"), me32.ProccntUsage);
		_tprintf(TEXT("\n     Base address   = 0x%08X"), (DWORD)me32.modBaseAddr);
		_tprintf(TEXT("\n     Base size      = %d"), me32.modBaseSize);

	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
	return(TRUE);
}

BOOL ListProcessThreads(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return(FALSE);

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32);

	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if (!Thread32First(hThreadSnap, &te32))
	{
		printError(TEXT("Thread32First")); // show cause of failure
		CloseHandle(hThreadSnap);          // clean the snapshot object
		return(FALSE);
	}

	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			_tprintf(TEXT("\n\n     THREAD ID      = 0x%08X"), te32.th32ThreadID);
			_tprintf(TEXT("\n     Base priority  = %d"), te32.tpBasePri);
			_tprintf(TEXT("\n     Delta priority = %d"), te32.tpDeltaPri);
			_tprintf(TEXT("\n"));
		}
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return(TRUE);
}

void printError(TCHAR* msg)
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		sysMsg, 256, NULL);

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do { *p-- = 0; } while ((p >= sysMsg) &&
		((*p == '.') || (*p < 33)));

	// Display the message
	_tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);
}
#endif  // __SNAPSHOT__

#ifdef __ENUM_PROCESS
// Enumerating All Processes
// https://docs.microsoft.com/en-us/windows/win32/psapi/enumerating-all-processes
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

void PrintProcessNameAndID(DWORD processID)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

	// Get a handle to the process.

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	// Get the process name.

	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));
		}
	}

	// Print the process name and identifier.

	_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);

	// Release the handle to the process.

	CloseHandle(hProcess);
}

int main(void)
{
	// Get the list of process identifiers.

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return 1;
	}


	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.

	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			PrintProcessNameAndID(aProcesses[i]);
		}
	}

	return 0;
}
#endif // __ENUM_PROCESS

#ifdef __ENUM_PROCESS_MODULES__
// Enumerating All Modules For a Process
// https://docs.microsoft.com/en-us/windows/win32/psapi/enumerating-all-modules-for-a-process
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

int PrintModules(DWORD processID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	// Print the process identifier.

	printf("\nProcess ID: %u\n", processID);

	// Get a handle to the process.

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return 1;

	// Get a list of all the modules in this process.

	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.

			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				// Print the module name and handle value.

				_tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
			}
		}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);

	return 0;
}

int main(void)
{

	DWORD aProcesses[1024];
	DWORD cbNeeded;
	DWORD cProcesses;
	unsigned int i;

	// Get the list of process identifiers.

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return 1;

	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the names of the modules for each process.

	for (i = 0; i < cProcesses; i++)
	{
		PrintModules(aProcesses[i]);
	}

	return 0;
}
#endif // __ENUM_PROCESS_MODULES__

#ifdef __WAITALBE__
// Using Waitable Timers with an Asynchronous Procedure Call
// https://docs.microsoft.com/en-us/windows/win32/sync/using-a-waitable-timer-with-an-asynchronous-procedure-call
#define UNICODE 1
#define _UNICODE 1

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define _SECOND 10000000

typedef struct _MYDATA {
	TCHAR *szText;
	DWORD dwValue;
} MYDATA;

VOID CALLBACK TimerAPCProc(
	LPVOID lpArg,               // Data value
	DWORD dwTimerLowValue,      // Timer low value
	DWORD dwTimerHighValue)    // Timer high value

{
	// Formal parameters not used in this example.
	UNREFERENCED_PARAMETER(dwTimerLowValue);
	UNREFERENCED_PARAMETER(dwTimerHighValue);

	MYDATA *pMyData = (MYDATA *)lpArg;

	_tprintf(TEXT("Message: %s\nValue: %d\n\n"), pMyData->szText,
		pMyData->dwValue);
	MessageBeep(0);

}

int main(void)
{
	HANDLE          hTimer;
	BOOL            bSuccess;
	__int64         qwDueTime;
	LARGE_INTEGER   liDueTime;
	MYDATA          MyData;

	MyData.szText = TEXT("This is my data");
	MyData.dwValue = 100;

	hTimer = CreateWaitableTimer(
		NULL,                   // Default security attributes
		FALSE,                  // Create auto-reset timer
		TEXT("MyTimer"));       // Name of waitable timer
	if (hTimer != NULL)
	{
		__try
		{
			// Create an integer that will be used to signal the timer 
			// 5 seconds from now.
			qwDueTime = -5 * _SECOND;

			// Copy the relative time into a LARGE_INTEGER.
			liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
			liDueTime.HighPart = (LONG)(qwDueTime >> 32);

			bSuccess = SetWaitableTimer(
				hTimer,           // Handle to the timer object
				&liDueTime,       // When timer will become signaled
				2000,             // Periodic timer interval of 2 seconds
				TimerAPCProc,     // Completion routine
				&MyData,          // Argument to the completion routine
				FALSE);          // Do not restore a suspended system

			if (bSuccess)
			{
				for (; MyData.dwValue < 1000; MyData.dwValue += 100)
				{
					SleepEx(
						INFINITE,     // Wait forever
						TRUE);       // Put thread in an alertable state
				}

			}
			else
			{
				printf("SetWaitableTimer failed with error %d\n", GetLastError());
			}

		}
		__finally
		{
			CloseHandle(hTimer);
		}
	}
	else
	{
		printf("CreateWaitableTimer failed with error %d\n", GetLastError());
	}

	return 0;
}
#endif // __WAITALBE__

#ifdef __MUTEX__
// Using Mutex Objects
// https://docs.microsoft.com/en-us/windows/win32/sync/using-mutex-objects
#include <windows.h>
#include <stdio.h>

#define THREADCOUNT 2

HANDLE ghMutex;

DWORD WINAPI WriteToDatabase(LPVOID);

int main(void)
{
	HANDLE aThread[THREADCOUNT];
	DWORD ThreadID;
	int i;

	// Create a mutex with no initial owner

	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (ghMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	// Create worker threads

	for (i = 0; i < THREADCOUNT; i++)
	{
		aThread[i] = CreateThread(
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE)WriteToDatabase,
			NULL,       // no thread function arguments
			0,          // default creation flags
			&ThreadID); // receive thread identifier

		if (aThread[i] == NULL)
		{
			printf("CreateThread error: %d\n", GetLastError());
			return 1;
		}
	}

	// Wait for all threads to terminate

	WaitForMultipleObjects(THREADCOUNT, aThread, TRUE, INFINITE);

	// Close thread and mutex handles

	for (i = 0; i < THREADCOUNT; i++)
		CloseHandle(aThread[i]);

	CloseHandle(ghMutex);
	system("pause");
	return 0;
}

DWORD WINAPI WriteToDatabase(LPVOID lpParam)
{
	// lpParam not used in this example
	UNREFERENCED_PARAMETER(lpParam);

	DWORD dwCount = 0, dwWaitResult;

	// Request ownership of mutex.

	while (dwCount < 20)
	{
		dwWaitResult = WaitForSingleObject(
			ghMutex,    // handle to mutex
			INFINITE);  // no time-out interval

		switch (dwWaitResult)
		{
			// The thread got ownership of the mutex
		case WAIT_OBJECT_0:
			__try {
				// TODO: Write to the database
				printf("Thread %d writing to database...\n",
					GetCurrentThreadId());
				dwCount++;
			}

			__finally {
				// Release ownership of the mutex object
				if (!ReleaseMutex(ghMutex))
				{
					// Handle error.
				}
			}
			break;

			// The thread got ownership of an abandoned mutex
			// The database is in an indeterminate state
		case WAIT_ABANDONED:
			return FALSE;
		}
	}
	return TRUE;
}
#endif // __MUTEX__

#ifdef __NAMEPIPE__
// Named Pipe Server Using Completion Routines
// https://docs.microsoft.com/en-us/windows/win32/ipc/named-pipe-server-using-completion-routines
#include <windows.h> 
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096

typedef struct
{
	OVERLAPPED oOverlap;
	HANDLE hPipeInst;
	TCHAR chRequest[BUFSIZE];
	DWORD cbRead;
	TCHAR chReply[BUFSIZE];
	DWORD cbToWrite;
} PIPEINST, *LPPIPEINST;

VOID DisconnectAndClose(LPPIPEINST);
BOOL CreateAndConnectInstance(LPOVERLAPPED);
BOOL ConnectToNewClient(HANDLE, LPOVERLAPPED);
VOID GetAnswerToRequest(LPPIPEINST);

VOID WINAPI CompletedWriteRoutine(DWORD, DWORD, LPOVERLAPPED);
VOID WINAPI CompletedReadRoutine(DWORD, DWORD, LPOVERLAPPED);

HANDLE hPipe;

int _tmain(VOID)
{
	HANDLE hConnectEvent;
	OVERLAPPED oConnect;
	LPPIPEINST lpPipeInst;
	DWORD dwWait, cbRet;
	BOOL fSuccess, fPendingIO;

	// Create one event object for the connect operation. 

	hConnectEvent = CreateEvent(
		NULL,    // default security attribute
		TRUE,    // manual reset event 
		TRUE,    // initial state = signaled 
		NULL);   // unnamed event object 

	if (hConnectEvent == NULL)
	{
		printf("CreateEvent failed with %d.\n", GetLastError());
		return 0;
	}

	oConnect.hEvent = hConnectEvent;

	// Call a subroutine to create one instance, and wait for 
	// the client to connect. 

	fPendingIO = CreateAndConnectInstance(&oConnect);

	while (1)
	{
		// Wait for a client to connect, or for a read or write 
		// operation to be completed, which causes a completion 
		// routine to be queued for execution. 

		dwWait = WaitForSingleObjectEx(
			hConnectEvent,  // event object to wait for 
			INFINITE,       // waits indefinitely 
			TRUE);          // alertable wait enabled 

		switch (dwWait)
		{
			// The wait conditions are satisfied by a completed connect 
			// operation. 
		case 0:
			// If an operation is pending, get the result of the 
			// connect operation. 

			if (fPendingIO)
			{
				fSuccess = GetOverlappedResult(
					hPipe,     // pipe handle 
					&oConnect, // OVERLAPPED structure 
					&cbRet,    // bytes transferred 
					FALSE);    // does not wait 
				if (!fSuccess)
				{
					printf("ConnectNamedPipe (%d)\n", GetLastError());
					return 0;
				}
			}

			// Allocate storage for this instance. 

			lpPipeInst = (LPPIPEINST)GlobalAlloc(
				GPTR, sizeof(PIPEINST));
			if (lpPipeInst == NULL)
			{
				printf("GlobalAlloc failed (%d)\n", GetLastError());
				return 0;
			}

			lpPipeInst->hPipeInst = hPipe;

			// Start the read operation for this client. 
			// Note that this same routine is later used as a 
			// completion routine after a write operation. 

			lpPipeInst->cbToWrite = 0;
			CompletedWriteRoutine(0, 0, (LPOVERLAPPED)lpPipeInst);

			// Create new pipe instance for the next client. 

			fPendingIO = CreateAndConnectInstance(
				&oConnect);
			break;

			// The wait is satisfied by a completed read or write 
			// operation. This allows the system to execute the 
			// completion routine. 

		case WAIT_IO_COMPLETION:
			break;

			// An error occurred in the wait function. 

		default:
		{
			printf("WaitForSingleObjectEx (%d)\n", GetLastError());
			return 0;
		}
		}
	}
	return 0;
}

// CompletedWriteRoutine(DWORD, DWORD, LPOVERLAPPED) 
// This routine is called as a completion routine after writing to 
// the pipe, or when a new client has connected to a pipe instance.
// It starts another read operation. 

VOID WINAPI CompletedWriteRoutine(DWORD dwErr, DWORD cbWritten,
	LPOVERLAPPED lpOverLap)
{
	LPPIPEINST lpPipeInst;
	BOOL fRead = FALSE;

	// lpOverlap points to storage for this instance. 

	lpPipeInst = (LPPIPEINST)lpOverLap;

	// The write operation has finished, so read the next request (if 
	// there is no error). 

	if ((dwErr == 0) && (cbWritten == lpPipeInst->cbToWrite))
		fRead = ReadFileEx(
			lpPipeInst->hPipeInst,
			lpPipeInst->chRequest,
			BUFSIZE * sizeof(TCHAR),
			(LPOVERLAPPED)lpPipeInst,
			(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedReadRoutine);

	// Disconnect if an error occurred. 

	if (!fRead)
		DisconnectAndClose(lpPipeInst);
}

// CompletedReadRoutine(DWORD, DWORD, LPOVERLAPPED) 
// This routine is called as an I/O completion routine after reading 
// a request from the client. It gets data and writes it to the pipe. 

VOID WINAPI CompletedReadRoutine(DWORD dwErr, DWORD cbBytesRead,
	LPOVERLAPPED lpOverLap)
{
	LPPIPEINST lpPipeInst;
	BOOL fWrite = FALSE;

	// lpOverlap points to storage for this instance. 

	lpPipeInst = (LPPIPEINST)lpOverLap;

	// The read operation has finished, so write a response (if no 
	// error occurred). 

	if ((dwErr == 0) && (cbBytesRead != 0))
	{
		GetAnswerToRequest(lpPipeInst);

		fWrite = WriteFileEx(
			lpPipeInst->hPipeInst,
			lpPipeInst->chReply,
			lpPipeInst->cbToWrite,
			(LPOVERLAPPED)lpPipeInst,
			(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedWriteRoutine);
	}

	// Disconnect if an error occurred. 

	if (!fWrite)
		DisconnectAndClose(lpPipeInst);
}

// DisconnectAndClose(LPPIPEINST) 
// This routine is called when an error occurs or the client closes 
// its handle to the pipe. 

VOID DisconnectAndClose(LPPIPEINST lpPipeInst)
{
	// Disconnect the pipe instance. 

	if (!DisconnectNamedPipe(lpPipeInst->hPipeInst))
	{
		printf("DisconnectNamedPipe failed with %d.\n", GetLastError());
	}

	// Close the handle to the pipe instance. 

	CloseHandle(lpPipeInst->hPipeInst);

	// Release the storage for the pipe instance. 

	if (lpPipeInst != NULL)
		GlobalFree(lpPipeInst);
}

// CreateAndConnectInstance(LPOVERLAPPED) 
// This function creates a pipe instance and connects to the client. 
// It returns TRUE if the connect operation is pending, and FALSE if 
// the connection has been completed. 

BOOL CreateAndConnectInstance(LPOVERLAPPED lpoOverlap)
{
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	hPipe = CreateNamedPipe(
		lpszPipename,             // pipe name 
		PIPE_ACCESS_DUPLEX |      // read/write access 
		FILE_FLAG_OVERLAPPED,     // overlapped mode 
		PIPE_TYPE_MESSAGE |       // message-type pipe 
		PIPE_READMODE_MESSAGE |   // message read mode 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // unlimited instances 
		BUFSIZE * sizeof(TCHAR),    // output buffer size 
		BUFSIZE * sizeof(TCHAR),    // input buffer size 
		PIPE_TIMEOUT,             // client time-out 
		NULL);                    // default security attributes
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}

	// Call a subroutine to connect to the new client. 

	return ConnectToNewClient(hPipe, lpoOverlap);
}

BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
{
	BOOL fConnected, fPendingIO = FALSE;

	// Start an overlapped connection for this pipe instance. 
	fConnected = ConnectNamedPipe(hPipe, lpo);

	// Overlapped ConnectNamedPipe should return zero. 
	if (fConnected)
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}

	switch (GetLastError())
	{
		// The overlapped connection in progress. 
	case ERROR_IO_PENDING:
		fPendingIO = TRUE;
		break;

		// Client is already connected, so signal an event. 

	case ERROR_PIPE_CONNECTED:
		if (SetEvent(lpo->hEvent))
			break;

		// If an error occurs during the connect operation... 
	default:
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}
	}
	return fPendingIO;
}

VOID GetAnswerToRequest(LPPIPEINST pipe)
{
	_tprintf(TEXT("[%d] %s\n"), pipe->hPipeInst, pipe->chRequest);
	StringCchCopy(pipe->chReply, BUFSIZE, TEXT("Default answer from server"));
	pipe->cbToWrite = (lstrlen(pipe->chReply) + 1) * sizeof(TCHAR);
}
#endif // __NAMEPIPE__


// AWE Example
// https://docs.microsoft.com/en-us/windows/win32/memory/awe-example
// The following sample program illustrates the Address Windowing Extensions.
#ifdef __AWE_EXAMPLE__
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define MEMORY_REQUESTED 1024*1024 // request a megabyte

BOOL
LoggedSetLockPagesPrivilege(HANDLE hProcess,
	BOOL bEnable);

void _cdecl main()
{
	BOOL bResult;                   // generic Boolean value
	ULONG_PTR NumberOfPages;        // number of pages to request
	ULONG_PTR NumberOfPagesInitial; // initial number of pages requested
	ULONG_PTR *aPFNs;               // page info; holds opaque data
	PVOID lpMemReserved;            // AWE window
	SYSTEM_INFO sSysInfo;           // useful system information
	int PFNArraySize;               // memory to request for PFN array

	GetSystemInfo(&sSysInfo);  // fill the system information structure

	_tprintf(_T("This computer has page size %d.\n"), sSysInfo.dwPageSize);

	// Calculate the number of pages of memory to request.

	NumberOfPages = MEMORY_REQUESTED / sSysInfo.dwPageSize;
	_tprintf(_T("Requesting %d pages of memory.\n"), NumberOfPages);

	// Calculate the size of the user PFN array.

	PFNArraySize = NumberOfPages * sizeof(ULONG_PTR);

	_tprintf(_T("Requesting a PFN array of %d bytes.\n"), PFNArraySize);

	aPFNs = (ULONG_PTR *)HeapAlloc(GetProcessHeap(), 0, PFNArraySize);

	if (aPFNs == NULL)
	{
		_tprintf(_T("Failed to allocate on heap.\n"));
		return;
	}

	// Enable the privilege.

	if (!LoggedSetLockPagesPrivilege(GetCurrentProcess(), TRUE))
	{
		return;
	}

	// Allocate the physical memory.

	NumberOfPagesInitial = NumberOfPages;
	bResult = AllocateUserPhysicalPages(GetCurrentProcess(),
		&NumberOfPages,
		aPFNs);

	if (bResult != TRUE)
	{
		_tprintf(_T("Cannot allocate physical pages (%u)\n"), GetLastError());
		return;
	}

	if (NumberOfPagesInitial != NumberOfPages)
	{
		_tprintf(_T("Allocated only %p pages.\n"), NumberOfPages);
		return;
	}

	// Reserve the virtual memory.

	lpMemReserved = VirtualAlloc(NULL,
		MEMORY_REQUESTED,
		MEM_RESERVE | MEM_PHYSICAL,
		PAGE_READWRITE);

	if (lpMemReserved == NULL)
	{
		_tprintf(_T("Cannot reserve memory.\n"));
		return;
	}

	// Map the physical memory into the window.

	bResult = MapUserPhysicalPages(lpMemReserved,
		NumberOfPages,
		aPFNs);

	if (bResult != TRUE)
	{
		_tprintf(_T("MapUserPhysicalPages failed (%u)\n"), GetLastError());
		return;
	}

	// unmap

	bResult = MapUserPhysicalPages(lpMemReserved,
		NumberOfPages,
		NULL);

	if (bResult != TRUE)
	{
		_tprintf(_T("MapUserPhysicalPages failed (%u)\n"), GetLastError());
		return;
	}

	// Free the physical pages.

	bResult = FreeUserPhysicalPages(GetCurrentProcess(),
		&NumberOfPages,
		aPFNs);

	if (bResult != TRUE)
	{
		_tprintf(_T("Cannot free physical pages, error %u.\n"), GetLastError());
		return;
	}

	// Free virtual memory.

	bResult = VirtualFree(lpMemReserved,
		0,
		MEM_RELEASE);

	// Release the aPFNs array.

	bResult = HeapFree(GetProcessHeap(), 0, aPFNs);

	if (bResult != TRUE)
	{
		_tprintf(_T("Call to HeapFree has failed (%u)\n"), GetLastError());
	}

}

/*****************************************************************
LoggedSetLockPagesPrivilege: a function to obtain or
release the privilege of locking physical pages.

Inputs:

HANDLE hProcess: Handle for the process for which the
privilege is needed

BOOL bEnable: Enable (TRUE) or disable?

Return value: TRUE indicates success, FALSE failure.

*****************************************************************/
BOOL
LoggedSetLockPagesPrivilege(HANDLE hProcess,
	BOOL bEnable)
{
	struct {
		DWORD Count;
		LUID_AND_ATTRIBUTES Privilege[1];
	} Info;

	HANDLE Token;
	BOOL Result;

	// Open the token.

	Result = OpenProcessToken(hProcess,
		TOKEN_ADJUST_PRIVILEGES,
		&Token);

	if (Result != TRUE)
	{
		_tprintf(_T("Cannot open process token.\n"));
		return FALSE;
	}

	// Enable or disable?

	Info.Count = 1;
	if (bEnable)
	{
		Info.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else
	{
		Info.Privilege[0].Attributes = 0;
	}

	// Get the LUID.

	Result = LookupPrivilegeValue(NULL,
		SE_LOCK_MEMORY_NAME,
		&(Info.Privilege[0].Luid));

	if (Result != TRUE)
	{
		_tprintf(_T("Cannot get privilege for %s.\n"), SE_LOCK_MEMORY_NAME);
		return FALSE;
	}

	// Adjust the privilege.

	Result = AdjustTokenPrivileges(Token, FALSE,
		(PTOKEN_PRIVILEGES)&Info,
		0, NULL, NULL);

	// Check the result.

	if (Result != TRUE)
	{
		_tprintf(_T("Cannot adjust token privileges (%u)\n"), GetLastError());
		return FALSE;
	}
	else
	{
		if (GetLastError() != ERROR_SUCCESS)
		{
			_tprintf(_T("Cannot enable the SE_LOCK_MEMORY_NAME privilege; "));
			_tprintf(_T("please check the local policy.\n"));
			return FALSE;
		}
	}

	CloseHandle(Token);

	return TRUE;
}
#endif // __AWE_EXAMPLE__




/* ********************* Using Pipes start ***************************** */

#ifdef __USING_PIPES_START__


// Using Pipes
// https://docs.microsoft.com/en-us/windows/win32/ipc/using-pipes
// The following examples demonstrate the use of pipes to pass a continuous stream of data between processes:
// 
// Multithreaded pipe server
// Named pipe server using overlapped I / O
// Named pipe server using completion routines
// Named pipe client
// Transactions on named pipes


// Multithreaded Pipe Server
// https://docs.microsoft.com/en-us/windows/win32/ipc/multithreaded-pipe-server
// The following example is a multithreaded pipe server. 
// It has a main thread with a loop that creates a pipe instance and waits for a pipe client to connect. 
// When a pipe client connects, the pipe server creates a thread to service that client 
// and then continues to execute the loop in the main thread. 
// It is possible for a pipe client to connect successfully to the pipe instance in the interval 
// between calls to the CreateNamedPipe and ConnectNamedPipe functions. 
// If this happens, ConnectNamedPipe returns zero, and GetLastError returns ERROR_PIPE_CONNECTED.
// 
// The thread created to service each pipe instance reads requests from the pipe 
// and writes replies to the pipe until the pipe client closes its handle.
// When this happens, the thread flushes the pipe, disconnects, closes its pipe handle, 
// and terminates.The main thread will run until an error occurs or the process is ended.
// 
// This pipe server can be used with the pipe client described in Named Pipe Client.
#ifdef __MULTITHREAD_PIPE_SERVER__
#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>

#define BUFSIZE 512

DWORD WINAPI InstanceThread(LPVOID);
VOID GetAnswerToRequest(LPTSTR, LPTSTR, LPDWORD);

int _tmain(VOID)
{
	BOOL   fConnected = FALSE;
	DWORD  dwThreadId = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
	LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	// The main loop creates an instance of the named pipe and 
	// then waits for a client to connect to it. When the client 
	// connects, a thread is created to handle communications 
	// with that client, and this loop is free to wait for the
	// next client connect request. It is an infinite loop.

	for (;;)
	{
		_tprintf(TEXT("\nPipe Server: Main thread awaiting client connection on %s\n"), lpszPipename);
		hPipe = CreateNamedPipe(
			lpszPipename,             // pipe name 
			PIPE_ACCESS_DUPLEX,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // max. instances  
			BUFSIZE,                  // output buffer size 
			BUFSIZE,                  // input buffer size 
			0,                        // client time-out 
			NULL);                    // default security attribute 

		if (hPipe == INVALID_HANDLE_VALUE)
		{
			_tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError());
			return -1;
		}

		// Wait for the client to connect; if it succeeds, 
		// the function returns a nonzero value. If the function
		// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

		fConnected = ConnectNamedPipe(hPipe, NULL) ?
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (fConnected)
		{
			printf("Client connected, creating a processing thread.\n");

			// Create a thread for this client. 
			hThread = CreateThread(
				NULL,              // no security attribute 
				0,                 // default stack size 
				InstanceThread,    // thread proc
				(LPVOID)hPipe,    // thread parameter 
				0,                 // not suspended 
				&dwThreadId);      // returns thread ID 

			if (hThread == NULL)
			{
				_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
				return -1;
			}
			else CloseHandle(hThread);
		}
		else
			// The client could not connect, so close the pipe. 
			CloseHandle(hPipe);
	}

	return 0;
}

DWORD WINAPI InstanceThread(LPVOID lpvParam)
// This routine is a thread processing function to read from and reply to a client
// via the open pipe connection passed from the main loop. Note this allows
// the main loop to continue executing, potentially creating more threads of
// of this procedure to run concurrently, depending on the number of incoming
// client connections.
{
	HANDLE hHeap = GetProcessHeap();
	TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));
	TCHAR* pchReply = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE * sizeof(TCHAR));

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;

	// Do some extra error checking since the app will keep running even if this
	// thread fails.

	if (lpvParam == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL value in lpvParam.\n");
		printf("   InstanceThread exitting.\n");
		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}

	if (pchRequest == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL heap allocation.\n");
		printf("   InstanceThread exitting.\n");
		if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
		return (DWORD)-1;
	}

	if (pchReply == NULL)
	{
		printf("\nERROR - Pipe Server Failure:\n");
		printf("   InstanceThread got an unexpected NULL heap allocation.\n");
		printf("   InstanceThread exitting.\n");
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		return (DWORD)-1;
	}

	// Print verbose messages. In production code, this should be for debugging only.
	printf("InstanceThread created, receiving and processing messages.\n");

	// The thread's parameter is a handle to a pipe object instance. 

	hPipe = (HANDLE)lpvParam;

	// Loop until done reading
	while (1)
	{
		// Read client requests from the pipe. This simplistic code only allows messages
		// up to BUFSIZE characters in length.
		fSuccess = ReadFile(
			hPipe,        // handle to pipe 
			pchRequest,    // buffer to receive data 
			BUFSIZE * sizeof(TCHAR), // size of buffer 
			&cbBytesRead, // number of bytes read 
			NULL);        // not overlapped I/O 

		if (!fSuccess || cbBytesRead == 0)
		{
			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				_tprintf(TEXT("InstanceThread: client disconnected.\n"));
			}
			else
			{
				_tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError());
			}
			break;
		}

		// Process the incoming message.
		GetAnswerToRequest(pchRequest, pchReply, &cbReplyBytes);

		// Write the reply to the pipe. 
		fSuccess = WriteFile(
			hPipe,        // handle to pipe 
			pchReply,     // buffer to write from 
			cbReplyBytes, // number of bytes to write 
			&cbWritten,   // number of bytes written 
			NULL);        // not overlapped I/O 

		if (!fSuccess || cbReplyBytes != cbWritten)
		{
			_tprintf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError());
			break;
		}
	}

	// Flush the pipe to allow the client to read the pipe's contents 
	// before disconnecting. Then disconnect the pipe, and close the 
	// handle to this pipe instance. 

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchReply);

	printf("InstanceThread exiting.\n");
	return 1;
}

VOID GetAnswerToRequest(LPTSTR pchRequest,
	LPTSTR pchReply,
	LPDWORD pchBytes)
	// This routine is a simple function to print the client request to the console
	// and populate the reply buffer with a default data string. This is where you
	// would put the actual client request processing code that runs in the context
	// of an instance thread. Keep in mind the main thread will continue to wait for
	// and receive other client connections while the instance thread is working.
{
	_tprintf(TEXT("Client Request String:\"%s\"\n"), pchRequest);

	// Check the outgoing message to make sure it's not too long for the buffer.
	if (FAILED(StringCchCopy(pchReply, BUFSIZE, TEXT("default answer from server"))))
	{
		*pchBytes = 0;
		pchReply[0] = 0;
		printf("StringCchCopy failed, no outgoing message.\n");
		return;
	}
	*pchBytes = (lstrlen(pchReply) + 1) * sizeof(TCHAR);
}
#endif // __MULTITHREAD_PIPE_SERVER__


// Named Pipe Server Using Overlapped I/O
// https://docs.microsoft.com/en-us/windows/win32/ipc/named-pipe-server-using-overlapped-i-o
// The following is an example of a single-threaded pipe server that uses overlapped operations to 
// service simultaneous connections to multiple pipe clients. The pipe server creates a fixed number of 
// pipe instances. Each pipe instance can be connected to a separate pipe client. 
// When a pipe client has finished using its pipe instance, the server disconnects from the 
// client and reuses the pipe instance to connect to a new client. This pipe server can be used with 
// the pipe client described in Named Pipe Client.
// 
// The OVERLAPPED structure is specified as a parameter in each ReadFile, WriteFile, and ConnectNamedPipe 
// operation on the pipe instance.Although the example shows simultaneous operations on different pipe instances, 
// it avoids simultaneous operations on a single pipe instance by using the event object in the OVERLAPPED structure.
// Because the same event object is used for read, write, and connect operations for each instance, 
// there is no way to know which operation's completion caused the event to be set to 
// the signaled state for simultaneous operations using the same pipe instance.
// 
// The event handles for each pipe instance are stored in an array that is passed to the 
// WaitForMultipleObjects function.This function waits for one of the events to be signaled, 
// and returns the array index of the event that caused the wait operation to complete.
// The example in this topic uses this array index to retrieve a structure containing information for the pipe instance.
// The server uses the fPendingIO member of the structure to keep track of whether the 
// most recent I / O operation on the instance was pending, which requires a call to the GetOverlappedResult function.
// The server uses the dwState member of the structure to determine the next operation that must be performed for the pipe instance.
// 
// Overlapped ReadFile, WriteFile, and ConnectNamedPipe operations can finish by the time the function returns.
// Otherwise, if the operation is pending, the event object in the specified OVERLAPPED structure is set to 
// the nonsignaled state before the function returns.When the pending operation finishes, the system sets the 
// state of the event object to signaled.The state of the event object is not changed if the operation finishes 
// before the function returns.
// 
// Because the example uses manual - reset event objects, 
// the state of an event object is not changed to nonsignaled by the WaitForMultipleObjects function.
// This is important, because the example relies on the event objects remaining in the signaled state, 
// except when there is a pending operation.
// 
// If the operation has already finished when ReadFile, WriteFile, or ConnectNamedPipe returns, 
// the function's return value indicates the result. For read and write operations, 
// the number of bytes transferred is also returned. If the operation is still pending, 
// the ReadFile, WriteFile, or ConnectNamedPipe function returns zero and the GetLastError function returns ERROR_IO_PENDING. 
// In this case, use the GetOverlappedResult function to retrieve the results after the operation has finished. 
// GetOverlappedResult returns only the results of pending operations. 
// It does not report the results of operations that were completed before the overlapped ReadFile, 
// WriteFile, or ConnectNamedPipe function returned.
// 
// Before disconnecting from a client, you must wait for a signal indicating the client has finished. 
// (Flushing the file buffers would defeat the purpose of overlapped I / O, 
// because the flush operation would block the execution of the server thread while 
// it waits for the client to empty the pipe.) In this example, the signal is the error generated by 
// trying to read from the pipe after the pipe client closes its handle.
#ifdef __NAME_PIPE_USING_OVERLAPPED_IO_
#include <windows.h> 
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#define CONNECTING_STATE 0 
#define READING_STATE 1 
#define WRITING_STATE 2 
#define INSTANCES 4 
#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096

typedef struct
{
	OVERLAPPED oOverlap;
	HANDLE hPipeInst;
	TCHAR chRequest[BUFSIZE];
	DWORD cbRead;
	TCHAR chReply[BUFSIZE];
	DWORD cbToWrite;
	DWORD dwState;
	BOOL fPendingIO;
} PIPEINST, *LPPIPEINST;


VOID DisconnectAndReconnect(DWORD);
BOOL ConnectToNewClient(HANDLE, LPOVERLAPPED);
VOID GetAnswerToRequest(LPPIPEINST);

PIPEINST Pipe[INSTANCES];
HANDLE hEvents[INSTANCES];

int _tmain(VOID)
{
	DWORD i, dwWait, cbRet, dwErr;
	BOOL fSuccess;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	// The initial loop creates several instances of a named pipe 
	// along with an event object for each instance.  An 
	// overlapped ConnectNamedPipe operation is started for 
	// each instance. 

	for (i = 0; i < INSTANCES; i++)
	{

		// Create an event object for this instance. 

		hEvents[i] = CreateEvent(
			NULL,    // default security attribute 
			TRUE,    // manual-reset event 
			TRUE,    // initial state = signaled 
			NULL);   // unnamed event object 

		if (hEvents[i] == NULL)
		{
			printf("CreateEvent failed with %d.\n", GetLastError());
			return 0;
		}

		Pipe[i].oOverlap.hEvent = hEvents[i];

		Pipe[i].hPipeInst = CreateNamedPipe(
			lpszPipename,            // pipe name 
			PIPE_ACCESS_DUPLEX |     // read/write access 
			FILE_FLAG_OVERLAPPED,    // overlapped mode 
			PIPE_TYPE_MESSAGE |      // message-type pipe 
			PIPE_READMODE_MESSAGE |  // message-read mode 
			PIPE_WAIT,               // blocking mode 
			INSTANCES,               // number of instances 
			BUFSIZE * sizeof(TCHAR),   // output buffer size 
			BUFSIZE * sizeof(TCHAR),   // input buffer size 
			PIPE_TIMEOUT,            // client time-out 
			NULL);                   // default security attributes 

		if (Pipe[i].hPipeInst == INVALID_HANDLE_VALUE)
		{
			printf("CreateNamedPipe failed with %d.\n", GetLastError());
			return 0;
		}

		// Call the subroutine to connect to the new client

		Pipe[i].fPendingIO = ConnectToNewClient(
			Pipe[i].hPipeInst,
			&Pipe[i].oOverlap);

		Pipe[i].dwState = Pipe[i].fPendingIO ?
			CONNECTING_STATE : // still connecting 
			READING_STATE;     // ready to read 
	}

	while (1)
	{
		// Wait for the event object to be signaled, indicating 
		// completion of an overlapped read, write, or 
		// connect operation. 

		dwWait = WaitForMultipleObjects(
			INSTANCES,    // number of event objects 
			hEvents,      // array of event objects 
			FALSE,        // does not wait for all 
			INFINITE);    // waits indefinitely 

						  // dwWait shows which pipe completed the operation. 

		i = dwWait - WAIT_OBJECT_0;  // determines which pipe 
		if (i < 0 || i >(INSTANCES - 1))
		{
			printf("Index out of range.\n");
			return 0;
		}

		// Get the result if the operation was pending. 

		if (Pipe[i].fPendingIO)
		{
			fSuccess = GetOverlappedResult(
				Pipe[i].hPipeInst, // handle to pipe 
				&Pipe[i].oOverlap, // OVERLAPPED structure 
				&cbRet,            // bytes transferred 
				FALSE);            // do not wait 

			switch (Pipe[i].dwState)
			{
				// Pending connect operation 
			case CONNECTING_STATE:
				if (!fSuccess)
				{
					printf("Error %d.\n", GetLastError());
					return 0;
				}
				Pipe[i].dwState = READING_STATE;
				break;

				// Pending read operation 
			case READING_STATE:
				if (!fSuccess || cbRet == 0)
				{
					DisconnectAndReconnect(i);
					continue;
				}
				Pipe[i].cbRead = cbRet;
				Pipe[i].dwState = WRITING_STATE;
				break;

				// Pending write operation 
			case WRITING_STATE:
				if (!fSuccess || cbRet != Pipe[i].cbToWrite)
				{
					DisconnectAndReconnect(i);
					continue;
				}
				Pipe[i].dwState = READING_STATE;
				break;

			default:
			{
				printf("Invalid pipe state.\n");
				return 0;
			}
			}
		}

		// The pipe state determines which operation to do next. 

		switch (Pipe[i].dwState)
		{
			// READING_STATE: 
			// The pipe instance is connected to the client 
			// and is ready to read a request from the client. 

		case READING_STATE:
			fSuccess = ReadFile(
				Pipe[i].hPipeInst,
				Pipe[i].chRequest,
				BUFSIZE * sizeof(TCHAR),
				&Pipe[i].cbRead,
				&Pipe[i].oOverlap);

			// The read operation completed successfully. 

			if (fSuccess && Pipe[i].cbRead != 0)
			{
				Pipe[i].fPendingIO = FALSE;
				Pipe[i].dwState = WRITING_STATE;
				continue;
			}

			// The read operation is still pending. 

			dwErr = GetLastError();
			if (!fSuccess && (dwErr == ERROR_IO_PENDING))
			{
				Pipe[i].fPendingIO = TRUE;
				continue;
			}

			// An error occurred; disconnect from the client. 

			DisconnectAndReconnect(i);
			break;

			// WRITING_STATE: 
			// The request was successfully read from the client. 
			// Get the reply data and write it to the client. 

		case WRITING_STATE:
			GetAnswerToRequest(&Pipe[i]);

			fSuccess = WriteFile(
				Pipe[i].hPipeInst,
				Pipe[i].chReply,
				Pipe[i].cbToWrite,
				&cbRet,
				&Pipe[i].oOverlap);

			// The write operation completed successfully. 

			if (fSuccess && cbRet == Pipe[i].cbToWrite)
			{
				Pipe[i].fPendingIO = FALSE;
				Pipe[i].dwState = READING_STATE;
				continue;
			}

			// The write operation is still pending. 

			dwErr = GetLastError();
			if (!fSuccess && (dwErr == ERROR_IO_PENDING))
			{
				Pipe[i].fPendingIO = TRUE;
				continue;
			}

			// An error occurred; disconnect from the client. 

			DisconnectAndReconnect(i);
			break;

		default:
		{
			printf("Invalid pipe state.\n");
			return 0;
		}
		}
	}

	return 0;
}


// DisconnectAndReconnect(DWORD) 
// This function is called when an error occurs or when the client 
// closes its handle to the pipe. Disconnect from this client, then 
// call ConnectNamedPipe to wait for another client to connect. 

VOID DisconnectAndReconnect(DWORD i)
{
	// Disconnect the pipe instance. 

	if (!DisconnectNamedPipe(Pipe[i].hPipeInst))
	{
		printf("DisconnectNamedPipe failed with %d.\n", GetLastError());
	}

	// Call a subroutine to connect to the new client. 

	Pipe[i].fPendingIO = ConnectToNewClient(
		Pipe[i].hPipeInst,
		&Pipe[i].oOverlap);

	Pipe[i].dwState = Pipe[i].fPendingIO ?
		CONNECTING_STATE : // still connecting 
		READING_STATE;     // ready to read 
}

// ConnectToNewClient(HANDLE, LPOVERLAPPED) 
// This function is called to start an overlapped connect operation. 
// It returns TRUE if an operation is pending or FALSE if the 
// connection has been completed. 

BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
{
	BOOL fConnected, fPendingIO = FALSE;

	// Start an overlapped connection for this pipe instance. 
	fConnected = ConnectNamedPipe(hPipe, lpo);

	// Overlapped ConnectNamedPipe should return zero. 
	if (fConnected)
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}

	switch (GetLastError())
	{
		// The overlapped connection in progress. 
	case ERROR_IO_PENDING:
		fPendingIO = TRUE;
		break;

		// Client is already connected, so signal an event. 

	case ERROR_PIPE_CONNECTED:
		if (SetEvent(lpo->hEvent))
			break;

		// If an error occurs during the connect operation... 
	default:
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}
	}

	return fPendingIO;
}

VOID GetAnswerToRequest(LPPIPEINST pipe)
{
	_tprintf(TEXT("[%d] %s\n"), pipe->hPipeInst, pipe->chRequest);
	StringCchCopy(pipe->chReply, BUFSIZE, TEXT("Default answer from server"));
	pipe->cbToWrite = (lstrlen(pipe->chReply) + 1) * sizeof(TCHAR);
}
#endif // __NAME_PIPE_USING_OVERLAPPED_IO_


// Named Pipe Server Using Completion Routines
// https://docs.microsoft.com/en-us/windows/win32/ipc/named-pipe-server-using-completion-routines
// The following example is a single-threaded pipe server that creates a message-type pipe and 
// uses overlapped operations. It uses the extended functions ReadFileEx and WriteFileEx to perform 
// overlapped I/O using a completion routine, which is queued for execution when the operation is finished. 
// The pipe server uses the WaitForSingleObjectEx function, which performs an alertable wait operation that 
// returns when a completion routine is ready to execute. The wait function also returns when an event object 
// is signaled, which in this example indicates that the overlapped ConnectNamedPipe operation has finished 
// (a new client has connected). This pipe server can be used with the pipe client described in Named Pipe Client.
// 
// Initially, the pipe server creates a single instance of the pipe and starts an overlapped ConnectNamedPipe operation.
// When a client connects, the server allocates a structure to provide storage for that pipe instance and then calls 
// the ReadFileEx function to start a sequence of I / O operations to handle communications with the client.
// Each operation specifies a completion routine that performs the next operation in the sequence.
// The sequence terminates when the client is disconnected and the pipe instance closed.After starting the 
// sequence of operations for the new client, the server creates another pipe instance and waits for the 
// next client to connect.
// 
// The parameters of the ReadFileEx and WriteFileEx functions specify a completion routine and a pointer to 
// an OVERLAPPED structure.This pointer is passed to the completion routine in its lpOverLap parameter.
// Because the OVERLAPPED structure points to the first member in the structure allocated for each pipe instance, 
// the completion routine can use its lpOverLap parameter to access the structure for the pipe instance.
#ifdef __NAMED_PIPE_USEING_COMPLETION_ROUTINES__
#include <windows.h> 
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096

typedef struct
{
	OVERLAPPED oOverlap;
	HANDLE hPipeInst;
	TCHAR chRequest[BUFSIZE];
	DWORD cbRead;
	TCHAR chReply[BUFSIZE];
	DWORD cbToWrite;
} PIPEINST, *LPPIPEINST;

VOID DisconnectAndClose(LPPIPEINST);
BOOL CreateAndConnectInstance(LPOVERLAPPED);
BOOL ConnectToNewClient(HANDLE, LPOVERLAPPED);
VOID GetAnswerToRequest(LPPIPEINST);

VOID WINAPI CompletedWriteRoutine(DWORD, DWORD, LPOVERLAPPED);
VOID WINAPI CompletedReadRoutine(DWORD, DWORD, LPOVERLAPPED);

HANDLE hPipe;

int _tmain(VOID)
{
	HANDLE hConnectEvent;
	OVERLAPPED oConnect;
	LPPIPEINST lpPipeInst;
	DWORD dwWait, cbRet;
	BOOL fSuccess, fPendingIO;

	// Create one event object for the connect operation. 

	hConnectEvent = CreateEvent(
		NULL,    // default security attribute
		TRUE,    // manual reset event 
		TRUE,    // initial state = signaled 
		NULL);   // unnamed event object 

	if (hConnectEvent == NULL)
	{
		printf("CreateEvent failed with %d.\n", GetLastError());
		return 0;
	}

	oConnect.hEvent = hConnectEvent;

	// Call a subroutine to create one instance, and wait for 
	// the client to connect. 

	fPendingIO = CreateAndConnectInstance(&oConnect);

	while (1)
	{
		// Wait for a client to connect, or for a read or write 
		// operation to be completed, which causes a completion 
		// routine to be queued for execution. 

		dwWait = WaitForSingleObjectEx(
			hConnectEvent,  // event object to wait for 
			INFINITE,       // waits indefinitely 
			TRUE);          // alertable wait enabled 

		switch (dwWait)
		{
			// The wait conditions are satisfied by a completed connect 
			// operation. 
		case 0:
			// If an operation is pending, get the result of the 
			// connect operation. 

			if (fPendingIO)
			{
				fSuccess = GetOverlappedResult(
					hPipe,     // pipe handle 
					&oConnect, // OVERLAPPED structure 
					&cbRet,    // bytes transferred 
					FALSE);    // does not wait 
				if (!fSuccess)
				{
					printf("ConnectNamedPipe (%d)\n", GetLastError());
					return 0;
				}
			}

			// Allocate storage for this instance. 

			lpPipeInst = (LPPIPEINST)GlobalAlloc(
				GPTR, sizeof(PIPEINST));
			if (lpPipeInst == NULL)
			{
				printf("GlobalAlloc failed (%d)\n", GetLastError());
				return 0;
			}

			lpPipeInst->hPipeInst = hPipe;

			// Start the read operation for this client. 
			// Note that this same routine is later used as a 
			// completion routine after a write operation. 

			lpPipeInst->cbToWrite = 0;
			CompletedWriteRoutine(0, 0, (LPOVERLAPPED)lpPipeInst);

			// Create new pipe instance for the next client. 

			fPendingIO = CreateAndConnectInstance(
				&oConnect);
			break;

			// The wait is satisfied by a completed read or write 
			// operation. This allows the system to execute the 
			// completion routine. 

		case WAIT_IO_COMPLETION:
			break;

			// An error occurred in the wait function. 

		default:
		{
			printf("WaitForSingleObjectEx (%d)\n", GetLastError());
			return 0;
		}
		}
	}
	return 0;
}

// CompletedWriteRoutine(DWORD, DWORD, LPOVERLAPPED) 
// This routine is called as a completion routine after writing to 
// the pipe, or when a new client has connected to a pipe instance.
// It starts another read operation. 

VOID WINAPI CompletedWriteRoutine(DWORD dwErr, DWORD cbWritten,
	LPOVERLAPPED lpOverLap)
{
	LPPIPEINST lpPipeInst;
	BOOL fRead = FALSE;

	// lpOverlap points to storage for this instance. 

	lpPipeInst = (LPPIPEINST)lpOverLap;

	// The write operation has finished, so read the next request (if 
	// there is no error). 

	if ((dwErr == 0) && (cbWritten == lpPipeInst->cbToWrite))
		fRead = ReadFileEx(
			lpPipeInst->hPipeInst,
			lpPipeInst->chRequest,
			BUFSIZE * sizeof(TCHAR),
			(LPOVERLAPPED)lpPipeInst,
			(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedReadRoutine);

	// Disconnect if an error occurred. 

	if (!fRead)
		DisconnectAndClose(lpPipeInst);
}

// CompletedReadRoutine(DWORD, DWORD, LPOVERLAPPED) 
// This routine is called as an I/O completion routine after reading 
// a request from the client. It gets data and writes it to the pipe. 

VOID WINAPI CompletedReadRoutine(DWORD dwErr, DWORD cbBytesRead,
	LPOVERLAPPED lpOverLap)
{
	LPPIPEINST lpPipeInst;
	BOOL fWrite = FALSE;

	// lpOverlap points to storage for this instance. 

	lpPipeInst = (LPPIPEINST)lpOverLap;

	// The read operation has finished, so write a response (if no 
	// error occurred). 

	if ((dwErr == 0) && (cbBytesRead != 0))
	{
		GetAnswerToRequest(lpPipeInst);

		fWrite = WriteFileEx(
			lpPipeInst->hPipeInst,
			lpPipeInst->chReply,
			lpPipeInst->cbToWrite,
			(LPOVERLAPPED)lpPipeInst,
			(LPOVERLAPPED_COMPLETION_ROUTINE)CompletedWriteRoutine);
	}

	// Disconnect if an error occurred. 

	if (!fWrite)
		DisconnectAndClose(lpPipeInst);
}

// DisconnectAndClose(LPPIPEINST) 
// This routine is called when an error occurs or the client closes 
// its handle to the pipe. 

VOID DisconnectAndClose(LPPIPEINST lpPipeInst)
{
	// Disconnect the pipe instance. 

	if (!DisconnectNamedPipe(lpPipeInst->hPipeInst))
	{
		printf("DisconnectNamedPipe failed with %d.\n", GetLastError());
	}

	// Close the handle to the pipe instance. 

	CloseHandle(lpPipeInst->hPipeInst);

	// Release the storage for the pipe instance. 

	if (lpPipeInst != NULL)
		GlobalFree(lpPipeInst);
}

// CreateAndConnectInstance(LPOVERLAPPED) 
// This function creates a pipe instance and connects to the client. 
// It returns TRUE if the connect operation is pending, and FALSE if 
// the connection has been completed. 

BOOL CreateAndConnectInstance(LPOVERLAPPED lpoOverlap)
{
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	hPipe = CreateNamedPipe(
		lpszPipename,             // pipe name 
		PIPE_ACCESS_DUPLEX |      // read/write access 
		FILE_FLAG_OVERLAPPED,     // overlapped mode 
		PIPE_TYPE_MESSAGE |       // message-type pipe 
		PIPE_READMODE_MESSAGE |   // message read mode 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // unlimited instances 
		BUFSIZE * sizeof(TCHAR),    // output buffer size 
		BUFSIZE * sizeof(TCHAR),    // input buffer size 
		PIPE_TIMEOUT,             // client time-out 
		NULL);                    // default security attributes
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}

	// Call a subroutine to connect to the new client. 

	return ConnectToNewClient(hPipe, lpoOverlap);
}

BOOL ConnectToNewClient(HANDLE hPipe, LPOVERLAPPED lpo)
{
	BOOL fConnected, fPendingIO = FALSE;

	// Start an overlapped connection for this pipe instance. 
	fConnected = ConnectNamedPipe(hPipe, lpo);

	// Overlapped ConnectNamedPipe should return zero. 
	if (fConnected)
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}

	switch (GetLastError())
	{
		// The overlapped connection in progress. 
	case ERROR_IO_PENDING:
		fPendingIO = TRUE;
		break;

		// Client is already connected, so signal an event. 

	case ERROR_PIPE_CONNECTED:
		if (SetEvent(lpo->hEvent))
			break;

		// If an error occurs during the connect operation... 
	default:
	{
		printf("ConnectNamedPipe failed with %d.\n", GetLastError());
		return 0;
	}
	}
	return fPendingIO;
}

VOID GetAnswerToRequest(LPPIPEINST pipe)
{
	_tprintf(TEXT("[%d] %s\n"), pipe->hPipeInst, pipe->chRequest);
	StringCchCopy(pipe->chReply, BUFSIZE, TEXT("Default answer from server"));
	pipe->cbToWrite = (lstrlen(pipe->chReply) + 1) * sizeof(TCHAR);
}
#endif // __NAMED_PIPE_USEING_COMPLETION_ROUTINES__


// Named Pipe Client
// https://docs.microsoft.com/en-us/windows/win32/ipc/named-pipe-client
// A named pipe client uses the CreateFile function to open a handle to a named pipe. 
// If the pipe exists but all of its instances are busy, CreateFile returns INVALID_HANDLE_VALUE and 
// the GetLastError function returns ERROR_PIPE_BUSY. When this happens, the named pipe client uses the 
// WaitNamedPipe function to wait for an instance of the named pipe to become available.
// 
// The CreateFile function fails if the access specified is incompatible with the access 
// specified(duplex, outbound, or inbound) when the server created the pipe.For a duplex pipe, 
// the client can specify read, write, or read / write access; for an outbound pipe(write - only server), 
// the client must specify read - only access; and for an inbound pipe(read - only server), 
// the client must specify write - only access.
// 
// The handle returned by CreateFile defaults to byte - read mode, blocking - wait mode, overlapped mode disabled, 
// and write - through mode disabled.The pipe client can use CreateFile to enable overlapped mode by 
// specifying FILE_FLAG_OVERLAPPED or to enable write - through mode by specifying FILE_FLAG_WRITE_THROUGH.
// The client can use the SetNamedPipeHandleState function to enable nonblocking mode by specifying PIPE_NOWAIT or 
// to enable message - read mode by specifying PIPE_READMODE_MESSAGE.
// 
// The following example shows a pipe client that opens a named pipe, sets the pipe handle to message - read mode, 
// uses the WriteFile function to send a request to the server, and uses the ReadFile function to read the server's reply. 
// This pipe client can be used with any of the message-type servers listed at the bottom of this topic. 
// With a byte-type server, however, this pipe client fails when it calls SetNamedPipeHandleState to change 
// to message-read mode. Because the client is reading from the pipe in message-read mode, it is possible 
// for the ReadFile operation to return zero after reading a partial message. This happens when the message 
// is larger than the read buffer. In this situation, GetLastError returns ERROR_MORE_DATA, and the client 
// can read the remainder of the message using additional calls to ReadFile.
// 
// This pipe client can be used with any of the pipe servers listed under See Also.
#ifdef __NAME_PIPE_CLIENT__
#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUFSIZE 512

int _tmain(int argc, TCHAR *argv[])
{
	HANDLE hPipe;
	LPTSTR lpvMessage = TEXT("Default message from client.");
	TCHAR  chBuf[BUFSIZE];
	BOOL   fSuccess = FALSE;
	DWORD  cbRead, cbToWrite, cbWritten, dwMode;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	if (argc > 1)
		lpvMessage = argv[1];

	// Try to open a named pipe; wait for it, if necessary. 

	while (1)
	{
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

							// Break if the pipe handle is valid. 

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
	}

	// The pipe connected; change to message-read mode. 

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	// Send a message to the pipe server. 

	cbToWrite = (lstrlen(lpvMessage) + 1) * sizeof(TCHAR);
	_tprintf(TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage);

	fSuccess = WriteFile(
		hPipe,                  // pipe handle 
		lpvMessage,             // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 

	if (!fSuccess)
	{
		_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	printf("\nMessage sent to server, receiving reply as follows:\n");

	do
	{
		// Read from the pipe. 

		fSuccess = ReadFile(
			hPipe,    // pipe handle 
			chBuf,    // buffer to receive reply 
			BUFSIZE * sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

		_tprintf(TEXT("\"%s\"\n"), chBuf);
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
	{
		_tprintf(TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError());
		return -1;
	}

	printf("\n<End of message, press ENTER to terminate connection and exit>");
	_getch();

	CloseHandle(hPipe);

	return 0;
}
#endif // __NAME_PIPE_CLIENT__


// Transactions on Named Pipes
// https://docs.microsoft.com/en-us/windows/win32/ipc/transactions-on-named-pipes
// A named pipe transaction is a client/server communication that combines a write operation and a read 
// operation into a single network operation. A transaction can be used only on a duplex, message-type pipe. 
// Transactions improve the performance of network communications between a client and a remote server. 
// Processes can use the TransactNamedPipe and CallNamedPipe functions to perform named pipe transactions.
// 
// The TransactNamedPipe function is most commonly used by a pipe client to write a request message to the 
// named pipe server and read the server's response message. The pipe client must 
// specify GENERIC_READ | GENERIC_WRITE access when it opens its pipe handle by calling the CreateFile function. 
// Then, the pipe client sets the pipe handle to message-read mode by calling the SetNamedPipeHandleState function. 
// If the read buffer specified in the call to TransactNamedPipe is not large enough to hold the entire message 
// written by the server, the function returns zero and GetLastError returns ERROR_MORE_DATA. The client can 
// read the remainder of the message by calling either the ReadFile, ReadFileEx, or PeekNamedPipe function.
// 
// TransactNamedPipe is typically called by pipe clients, but can also be used by a pipe server.
// 
// The following example shows a pipe client using TransactNamedPipe.This pipe client can be used with any of 
// the pipe servers listed under See Also.
// 
//
// A pipe client uses CallNamedPipe to combine the CreateFile, WaitNamedPipe (if necessary), TransactNamedPipe, 
// and CloseHandle function calls into a single call. Because the pipe handle is closed before the function 
// returns, any additional bytes in the message are lost if the message is larger than the specified size of 
// the read buffer. The following example is the previous example rewritten to use CallNamedPipe.
#ifdef __TRANSACTIONS_ON_NAMED_PIPES__
#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUFSIZE 512

int _tmain(int argc, TCHAR *argv[])
{
	HANDLE hPipe;
	LPTSTR lpszWrite = TEXT("Default message from client");
	TCHAR chReadBuf[BUFSIZE];
	BOOL fSuccess;
	DWORD cbRead, dwMode;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	if (argc > 1)
	{
		lpszWrite = argv[1];
	}

	// Try to open a named pipe; wait for it, if necessary. 
	while (1)
	{
		hPipe = CreateFile(
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

							// Break if the pipe handle is valid. 
		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			printf("Could not open pipe\n");
			return 0;
		}

		// All pipe instances are busy, so wait for 20 seconds. 
		if (!WaitNamedPipe(lpszPipename, 20000))
		{
			printf("Could not open pipe\n");
			return 0;
		}
	}

	// The pipe connected; change to message-read mode. 
	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess)
	{
		printf("SetNamedPipeHandleState failed.\n");
		return 0;
	}

	// Send a message to the pipe server and read the response. 
	fSuccess = TransactNamedPipe(
		hPipe,                  // pipe handle 
		lpszWrite,              // message to server
		(lstrlen(lpszWrite) + 1) * sizeof(TCHAR), // message length 
		chReadBuf,              // buffer to receive reply
		BUFSIZE * sizeof(TCHAR),  // size of read buffer
		&cbRead,                // bytes read
		NULL);                  // not overlapped 

	if (!fSuccess && (GetLastError() != ERROR_MORE_DATA))
	{
		printf("TransactNamedPipe failed.\n");
		return 0;
	}

	while (1)
	{
		_tprintf(TEXT("%s\n"), chReadBuf);

		// Break if TransactNamedPipe or ReadFile is successful
		if (fSuccess)
			break;

		// Read from the pipe if there is more data in the message.
		fSuccess = ReadFile(
			hPipe,      // pipe handle 
			chReadBuf,  // buffer to receive reply 
			BUFSIZE * sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 

					  // Exit if an error other than ERROR_MORE_DATA occurs.
		if (!fSuccess && (GetLastError() != ERROR_MORE_DATA))
			break;
		else _tprintf(TEXT("%s\n"), chReadBuf);
	}

	_getch();

	CloseHandle(hPipe);

	return 0;
}
#endif // __TRANSACTIONS_ON_NAMED_PIPES__
#ifdef __TRANSACTIONS_ON_NAMED_PIPES__
#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUFSIZE 512

int _tmain(int argc, TCHAR *argv[])
{
	LPTSTR lpszWrite = TEXT("Default message from client");
	TCHAR chReadBuf[BUFSIZE];
	BOOL fSuccess;
	DWORD cbRead;
	LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

	if (argc > 1)
	{
		lpszWrite = argv[1];
	}

	fSuccess = CallNamedPipe(
		lpszPipename,        // pipe name 
		lpszWrite,           // message to server 
		(lstrlen(lpszWrite) + 1) * sizeof(TCHAR), // message length 
		chReadBuf,              // buffer to receive reply 
		BUFSIZE * sizeof(TCHAR),  // size of read buffer 
		&cbRead,                // number of bytes read 
		20000);                 // waits for 20 seconds 

	if (fSuccess || GetLastError() == ERROR_MORE_DATA)
	{
		_tprintf(TEXT("%s\n"), chReadBuf);

		// The pipe is closed; no more data can be read. 

		if (!fSuccess)
		{
			printf("\nExtra data in message was lost\n");
		}
	}

	_getch();

	return 0;
}
#endif // __DEFFFF__

#endif // __USING_PIPES_START__

/* ********************* Using Pipes end ***************************** */



/* ********************* Using Synchronization start ***************************** */

#ifdef __USING_SYNCHRONIZATION__

// Using Synchronization
// https://docs.microsoft.com/en-us/windows/win32/sync/using-synchronization
// Waiting for multiple objects
// Using named objects
// Using event objects
// Using mutex objects
// Using semaphore objects
// Using waitable timer objects
// Using waitable timers with an asynchronous procedure call
// Using critical section objects
// Using condition variables
// Using one - time initialization
// Using singly linked lists
// Using timer queues


// Waiting for Multiple Objects
// https://docs.microsoft.com/en-us/windows/win32/sync/waiting-for-multiple-objects
// The following example uses the CreateEvent function to create two event objects 
// and the CreateThread function to create a thread. 
// It then uses the WaitForMultipleObjects function to wait for the thread to 
// set the state of one of the objects to signaled using the SetEvent function.
// For an example that waits for a single object, see Using Mutex Objects.
#ifdef __MULTIPLE_OBJECT__
#include <windows.h>
#include <stdio.h>

HANDLE ghEvents[2];

DWORD WINAPI ThreadProc(LPVOID);

int main(void)
{
	HANDLE hThread;
	DWORD i, dwEvent, dwThreadID;

	// Create two event objects

	for (i = 0; i < 2; i++)
	{
		ghEvents[i] = CreateEvent(
			NULL,   // default security attributes
			FALSE,  // auto-reset event object
			FALSE,  // initial state is nonsignaled
			NULL);  // unnamed object

		if (ghEvents[i] == NULL)
		{
			printf("CreateEvent error: %d\n", GetLastError());
			ExitProcess(0);
		}
	}

	// Create a thread

	hThread = CreateThread(
		NULL,         // default security attributes
		0,            // default stack size
		(LPTHREAD_START_ROUTINE)ThreadProc,
		NULL,         // no thread function arguments
		0,            // default creation flags
		&dwThreadID); // receive thread identifier

	if (hThread == NULL)
	{
		printf("CreateThread error: %d\n", GetLastError());
		return 1;
	}

	// Wait for the thread to signal one of the event objects

	dwEvent = WaitForMultipleObjects(
		2,           // number of objects in array
		ghEvents,     // array of objects
		FALSE,       // wait for any object
		5000);       // five-second wait

					 // The return value indicates which event is signaled

	switch (dwEvent)
	{
		// ghEvents[0] was signaled
	case WAIT_OBJECT_0 + 0:
		// TODO: Perform tasks required by this event
		printf("First event was signaled.\n");
		break;

		// ghEvents[1] was signaled
	case WAIT_OBJECT_0 + 1:
		// TODO: Perform tasks required by this event
		printf("Second event was signaled.\n");
		break;

	case WAIT_TIMEOUT:
		printf("Wait timed out.\n");
		break;

		// Return value is invalid.
	default:
		printf("Wait error: %d\n", GetLastError());
		ExitProcess(0);
	}

	// Close event handles

	for (i = 0; i < 2; i++)
		CloseHandle(ghEvents[i]);

	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{

	// lpParam not used in this example
	UNREFERENCED_PARAMETER(lpParam);

	// Set one event to the signaled state

	if (!SetEvent(ghEvents[0]))
	{
		printf("SetEvent failed (%d)\n", GetLastError());
		return 1;
	}
	return 0;
}
#endif // __MULTIPLE_OBJECT__


// Using Named Objects
// https://docs.microsoft.com/en-us/windows/win32/sync/using-named-objects
// 
// First Process
// he first process uses the CreateMutex function to create the mutex object. 
// Note that this function succeeds even if there is an existing object with the same name.
//
// Second Process
// The second process uses the OpenMutex function to open a handle to the existing mutex. 
// This function fails if a mutex object with the specified name does not exist. 
// The access parameter requests full access to the mutex object, 
// which is necessary for the handle to be used in any of the wait functions.
#ifdef __NAMED_OBJECT__

#ifdef __FIRST_PROCESS__
#include <windows.h>
#include <stdio.h>

// This process opens a handle to a mutex created by another process.

int main(void)
{
	HANDLE hMutex;

	hMutex = OpenMutex(
		MUTEX_ALL_ACCESS,            // request full access
		FALSE,                       // handle not inheritable
		TEXT("NameOfMutexObject"));  // object name

	if (hMutex == NULL)
		printf("OpenMutex error: %d\n", GetLastError());
	else printf("OpenMutex successfully opened the mutex.\n");

	CloseHandle(hMutex);

	return 0;
}
#endif // __FIRST_PROCESS__

#ifdef __SECOND_PROCESS__
#include <windows.h>
#include <stdio.h>

// This process opens a handle to a mutex created by another process.

int main(void)
{
	HANDLE hMutex;

	hMutex = OpenMutex(
		MUTEX_ALL_ACCESS,            // request full access
		FALSE,                       // handle not inheritable
		TEXT("NameOfMutexObject"));  // object name

	if (hMutex == NULL)
		printf("OpenMutex error: %d\n", GetLastError());
	else printf("OpenMutex successfully opened the mutex.\n");

	CloseHandle(hMutex);

	return 0;
}
#endif // __SECOND_PROCESS__

#endif // __NAMED_OBJECT__


// Using Event Objects (Synchronization)
// https://docs.microsoft.com/en-us/windows/win32/sync/using-event-objects
// Applications can use event objects in a number of situations to 
// notify a waiting thread of the occurrence of an event. 
// For example, overlapped I/O operations on files, named pipes, 
// and communications devices use an event object to signal their completion. 
// For more information about the use of event objects in overlapped I/O operations, 
// see Synchronization and Overlapped Input and Output.
//
// The following example uses event objects to prevent several threads 
// from reading from a shared memory buffer while a master thread is writing to that buffer.
// First, the master thread uses the CreateEvent function to create a manual - reset event object 
// whose initial state is nonsignaled.Then it creates several reader threads.
// The master thread performs a write operation and then sets the event object to 
// the signaled state when it has finished writing.
// 
// Before starting a read operation, each reader thread uses WaitForSingleObject to 
// wait for the manual - reset event object to be signaled.When WaitForSingleObject returns, 
// this indicates that the main thread is ready for it to begin its read operation.
#ifdef __EVENT_OBJECTS__
#include <windows.h>
#include <stdio.h>

#define THREADCOUNT 4 

HANDLE ghWriteEvent;
HANDLE ghThreads[THREADCOUNT];

DWORD WINAPI ThreadProc(LPVOID);

void CreateEventsAndThreads(void)
{
	int i;
	DWORD dwThreadID;

	// Create a manual-reset event object. The write thread sets this
	// object to the signaled state when it finishes writing to a 
	// shared buffer. 

	ghWriteEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT("WriteEvent")  // object name
	);

	if (ghWriteEvent == NULL)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return;
	}

	// Create multiple threads to read from the buffer.

	for (i = 0; i < THREADCOUNT; i++)
	{
		// TODO: More complex scenarios may require use of a parameter
		//   to the thread procedure, such as an event per thread to  
		//   be used for synchronization.
		ghThreads[i] = CreateThread(
			NULL,              // default security
			0,                 // default stack size
			ThreadProc,        // name of the thread function
			NULL,              // no thread parameters
			0,                 // default startup flags
			&dwThreadID);

		if (ghThreads[i] == NULL)
		{
			printf("CreateThread failed (%d)\n", GetLastError());
			return;
		}
	}
}

void WriteToBuffer(VOID)
{
	// TODO: Write to the shared buffer.

	printf("Main thread writing to the shared buffer...\n");

	// Set ghWriteEvent to signaled

	if (!SetEvent(ghWriteEvent))
	{
		printf("SetEvent failed (%d)\n", GetLastError());
		return;
	}
}

void CloseEvents()
{
	// Close all event handles (currently, only one global handle).

	CloseHandle(ghWriteEvent);
}

int main(void)
{
	DWORD dwWaitResult;

	// TODO: Create the shared buffer

	// Create events and THREADCOUNT threads to read from the buffer

	CreateEventsAndThreads();

	// At this point, the reader threads have started and are most
	// likely waiting for the global event to be signaled. However, 
	// it is safe to write to the buffer because the event is a 
	// manual-reset event.

	WriteToBuffer();

	printf("Main thread waiting for threads to exit...\n");

	// The handle for each thread is signaled when the thread is
	// terminated.
	dwWaitResult = WaitForMultipleObjects(
		THREADCOUNT,   // number of handles in array
		ghThreads,     // array of thread handles
		TRUE,          // wait until all are signaled
		INFINITE);

	switch (dwWaitResult)
	{
		// All thread objects were signaled
	case WAIT_OBJECT_0:
		printf("All threads ended, cleaning up for application exit...\n");
		break;

		// An error occurred
	default:
		printf("WaitForMultipleObjects failed (%d)\n", GetLastError());
		return 1;
	}

	// Close the events to clean up

	CloseEvents();

	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	// lpParam not used in this example.
	UNREFERENCED_PARAMETER(lpParam);

	DWORD dwWaitResult;

	printf("Thread %d waiting for write event...\n", GetCurrentThreadId());

	dwWaitResult = WaitForSingleObject(
		ghWriteEvent, // event handle
		INFINITE);    // indefinite wait

	switch (dwWaitResult)
	{
		// Event object was signaled
	case WAIT_OBJECT_0:
		//
		// TODO: Read from the shared buffer
		//
		printf("Thread %d reading from buffer\n",
			GetCurrentThreadId());
		break;

		// An error occurred
	default:
		printf("Wait error (%d)\n", GetLastError());
		return 0;
	}

	// Now that we are done reading the buffer, we could use another
	// event to signal that this thread is no longer reading. This
	// example simply uses the thread handle for synchronization (the
	// handle is signaled when the thread terminates.)

	printf("Thread %d exiting\n", GetCurrentThreadId());
	return 1;
}
#endif // __EVENT_OBJECTS__


// Using Mutex Objects
// https://docs.microsoft.com/en-us/windows/win32/sync/using-mutex-objects
// You can use a mutex object to protect a shared resource from simultaneous access 
// by multiple threads or processes. Each thread must wait for ownership of the mutex 
// before it can execute the code that accesses the shared resource. 
// For example, if several threads share access to a database, 
// the threads can use a mutex object to permit only one thread at a time to write to the database.
//
// The following example uses the CreateMutex function to create a mutex object 
// and the CreateThread function to create worker threads.
// 
// When a thread of this process writes to the database, it first requests ownership of the mutex 
// using the WaitForSingleObject function.If the thread obtains ownership of the mutex, 
// it writes to the database and then releases its ownership of the mutex using the ReleaseMutex function.
// 
// This example uses structured exception handling to ensure that the thread properly releases the mutex object.
// The __finally block of code is executed no matter how the __try block terminates
// (unless the __try block includes a call to the TerminateThread function).
// This prevents the mutex object from being abandoned inadvertently.
// 
// If a mutex is abandoned, the thread that owned the mutex did not properly release it before terminating.
// In this case, the status of the shared resource is indeterminate, 
// and continuing to use the mutex can obscure a potentially serious error.
// Some applications might attempt to restore the resource to a consistent state; 
// this example simply returns an error and stops using the mutex.
// For more information, see Mutex Objects.
#ifdef __MUTEX_OBJECT__
#include <windows.h>
#include <stdio.h>

#define THREADCOUNT 2

HANDLE ghMutex;

DWORD WINAPI WriteToDatabase(LPVOID);

int main(void)
{
	HANDLE aThread[THREADCOUNT];
	DWORD ThreadID;
	int i;

	// Create a mutex with no initial owner

	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (ghMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	// Create worker threads

	for (i = 0; i < THREADCOUNT; i++)
	{
		aThread[i] = CreateThread(
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE)WriteToDatabase,
			NULL,       // no thread function arguments
			0,          // default creation flags
			&ThreadID); // receive thread identifier

		if (aThread[i] == NULL)
		{
			printf("CreateThread error: %d\n", GetLastError());
			return 1;
		}
	}

	// Wait for all threads to terminate

	WaitForMultipleObjects(THREADCOUNT, aThread, TRUE, INFINITE);

	// Close thread and mutex handles

	for (i = 0; i < THREADCOUNT; i++)
		CloseHandle(aThread[i]);

	CloseHandle(ghMutex);

	return 0;
}

DWORD WINAPI WriteToDatabase(LPVOID lpParam)
{
	// lpParam not used in this example
	UNREFERENCED_PARAMETER(lpParam);

	DWORD dwCount = 0, dwWaitResult;

	// Request ownership of mutex.

	while (dwCount < 20)
	{
		dwWaitResult = WaitForSingleObject(
			ghMutex,    // handle to mutex
			INFINITE);  // no time-out interval

		switch (dwWaitResult)
		{
			// The thread got ownership of the mutex
		case WAIT_OBJECT_0:
			__try {
				// TODO: Write to the database
				printf("Thread %d writing to database...\n",
					GetCurrentThreadId());
				dwCount++;
			}

			__finally {
				// Release ownership of the mutex object
				if (!ReleaseMutex(ghMutex))
				{
					// Handle error.
				}
			}
			break;

			// The thread got ownership of an abandoned mutex
			// The database is in an indeterminate state
		case WAIT_ABANDONED:
			return FALSE;
		}
	}
	return TRUE;
}
#endif // __MUTEX_OBJECT__


// Using Semaphore Objects
// https://docs.microsoft.com/en-us/windows/win32/sync/using-semaphore-objects
// The following example uses a semaphore object to limit the number of threads that can perform a particular task. 
// First, it uses the CreateSemaphore function to create the semaphore and to specify initial and maximum counts, 
// then it uses the CreateThread function to create the threads.
// 
// Before a thread attempts to perform the task, 
// it uses the WaitForSingleObject function to determine whether the semaphore's current count permits it to do so. 
// The wait function's time - out parameter is set to zero, 
// so the function returns immediately if the semaphore is in the nonsignaled state.
// WaitForSingleObject decrements the semaphore's count by one.
// 
// When a thread completes the task, 
// it uses the ReleaseSemaphore function to increment the semaphore's count, 
// thus enabling another waiting thread to perform the task.
#ifdef __SEMAPHORE_OBJECT__
#include <windows.h>
#include <stdio.h>

#define MAX_SEM_COUNT 10
#define THREADCOUNT 12

HANDLE ghSemaphore;

DWORD WINAPI ThreadProc(LPVOID);

int main(void)
{
	HANDLE aThread[THREADCOUNT];
	DWORD ThreadID;
	int i;

	// Create a semaphore with initial and max counts of MAX_SEM_COUNT

	ghSemaphore = CreateSemaphore(
		NULL,           // default security attributes
		MAX_SEM_COUNT,  // initial count
		MAX_SEM_COUNT,  // maximum count
		NULL);          // unnamed semaphore

	if (ghSemaphore == NULL)
	{
		printf("CreateSemaphore error: %d\n", GetLastError());
		return 1;
	}

	// Create worker threads

	for (i = 0; i < THREADCOUNT; i++)
	{
		aThread[i] = CreateThread(
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE)ThreadProc,
			NULL,       // no thread function arguments
			0,          // default creation flags
			&ThreadID); // receive thread identifier

		if (aThread[i] == NULL)
		{
			printf("CreateThread error: %d\n", GetLastError());
			return 1;
		}
	}

	// Wait for all threads to terminate

	WaitForMultipleObjects(THREADCOUNT, aThread, TRUE, INFINITE);

	// Close thread and semaphore handles

	for (i = 0; i < THREADCOUNT; i++)
		CloseHandle(aThread[i]);

	CloseHandle(ghSemaphore);

	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{

	// lpParam not used in this example
	UNREFERENCED_PARAMETER(lpParam);

	DWORD dwWaitResult;
	BOOL bContinue = TRUE;

	while (bContinue)
	{
		// Try to enter the semaphore gate.

		dwWaitResult = WaitForSingleObject(
			ghSemaphore,   // handle to semaphore
			0L);           // zero-second time-out interval

		switch (dwWaitResult)
		{
			// The semaphore object was signaled.
		case WAIT_OBJECT_0:
			// TODO: Perform task
			printf("Thread %d: wait succeeded\n", GetCurrentThreadId());
			bContinue = FALSE;

			// Simulate thread spending time on task
			Sleep(5);

			// Release the semaphore when task is finished

			if (!ReleaseSemaphore(
				ghSemaphore,  // handle to semaphore
				1,            // increase count by one
				NULL))       // not interested in previous count
			{
				printf("ReleaseSemaphore error: %d\n", GetLastError());
			}
			break;

			// The semaphore was nonsignaled, so a time-out occurred.
		case WAIT_TIMEOUT:
			printf("Thread %d: wait timed out\n", GetCurrentThreadId());
			break;
		}
	}
	return TRUE;
}
#endif // __SEMAPHORE_OBJECT__


// Using Waitable Timer Objects
// https://docs.microsoft.com/en-us/windows/win32/sync/using-waitable-timer-objects
// The following example creates a timer that will be signaled after a 10 second delay. 
// First, the code uses the CreateWaitableTimer function to create a waitable timer object. 
// Then it uses the SetWaitableTimer function to set the timer. 
// The code uses the WaitForSingleObject function to determine when the timer has been signaled.
#ifdef __WAITABLE_TIMER_OBJECTS__
#include <windows.h>
#include <stdio.h>

int main()
{
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -100000000LL;

	// Create an unnamed waitable timer.
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (NULL == hTimer)
	{
		printf("CreateWaitableTimer failed (%d)\n", GetLastError());
		return 1;
	}

	printf("Waiting for 10 seconds...\n");

	// Set a timer to wait for 10 seconds.
	if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
	{
		printf("SetWaitableTimer failed (%d)\n", GetLastError());
		return 2;
	}

	// Wait for the timer.

	if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
		printf("WaitForSingleObject failed (%d)\n", GetLastError());
	else printf("Timer was signaled.\n");

	return 0;
}
#endif // __WAITABLE_TIMER_OBJECTS__

// Using Waitable Timers with an Asynchronous Procedure Call
// https://docs.microsoft.com/en-us/windows/win32/sync/using-a-waitable-timer-with-an-asynchronous-procedure-call
// The following example associates an asynchronous procedure call (APC) function, 
// also known as a completion routine, with a waitable timer when the timer is set. 
// The address of the completion routine is the fourth parameter to the SetWaitableTimer function. 
// The fifth parameter is a void pointer that you can use to pass arguments to the completion routine.
// 
// The completion routine will be executed by the same thread that called SetWaitableTimer.
// This thread must be in an alertable state to execute the completion routine.
// It accomplishes this by calling the SleepEx function, which is an alertable function.
// 
// Each thread has an APC queue.If there is an entry in the thread's APC queue at 
// the time that one of the alertable functions is called, 
// the thread is not put to sleep. Instead, 
// the entry is removed from the APC queue and the completion routine is called.
// 
// If no entry exists in the APC queue, the thread is suspended until the wait is satisfied.
// The wait can be satisfied by adding an entry to the APC queue, by a timeout, 
// or by a handle becoming signaled.If the wait is satisfied by an entry in the APC queue, 
// the thread is awakened and the completion routine is called.In this case, 
// the return value of the function is WAIT_IO_COMPLETION.
// 
// After the completion routine is executed, the system checks for another entry in the APC queue to process.
// An alertable function will return only after all APC entries have been processed.
// Therefore, if entries are being added to the APC queue faster than they can be processed, 
// it is possible that a call to an alertable function will never return.
// This is especially possible with waitable timers, 
// if the period is shorter than the amount of time required to execute the completion routine.
// 
// When you are using a waitable timer with an APC, 
// the thread that sets the timer should not wait on the handle of the timer.
// By doing so, you would cause the thread to wake up as a result of the timer becoming signaled 
// rather than as the result of an entry being added to the APC queue.
// As a result, the thread is no longer in an alertable state and the completion routine is not called.
// In the following code, the call to SleepEx awakens the thread when an entry is added 
// to the thread's APC queue after the timer is set to the signaled state.
#ifdef __WATEABLE_TIMERS_WITH_ASYN__
#define UNICODE 1
#define _UNICODE 1

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define _SECOND 10000000

typedef struct _MYDATA {
	TCHAR *szText;
	DWORD dwValue;
} MYDATA;

VOID CALLBACK TimerAPCProc(
	LPVOID lpArg,               // Data value
	DWORD dwTimerLowValue,      // Timer low value
	DWORD dwTimerHighValue)    // Timer high value

{
	// Formal parameters not used in this example.
	UNREFERENCED_PARAMETER(dwTimerLowValue);
	UNREFERENCED_PARAMETER(dwTimerHighValue);

	MYDATA *pMyData = (MYDATA *)lpArg;

	_tprintf(TEXT("Message: %s\nValue: %d\n\n"), pMyData->szText,
		pMyData->dwValue);
	MessageBeep(0);

}

int main(void)
{
	HANDLE          hTimer;
	BOOL            bSuccess;
	__int64         qwDueTime;
	LARGE_INTEGER   liDueTime;
	MYDATA          MyData;

	MyData.szText = TEXT("This is my data");
	MyData.dwValue = 100;

	hTimer = CreateWaitableTimer(
		NULL,                   // Default security attributes
		FALSE,                  // Create auto-reset timer
		TEXT("MyTimer"));       // Name of waitable timer
	if (hTimer != NULL)
	{
		__try
		{
			// Create an integer that will be used to signal the timer 
			// 5 seconds from now.
			qwDueTime = -5 * _SECOND;

			// Copy the relative time into a LARGE_INTEGER.
			liDueTime.LowPart = (DWORD)(qwDueTime & 0xFFFFFFFF);
			liDueTime.HighPart = (LONG)(qwDueTime >> 32);

			bSuccess = SetWaitableTimer(
				hTimer,           // Handle to the timer object
				&liDueTime,       // When timer will become signaled
				2000,             // Periodic timer interval of 2 seconds
				TimerAPCProc,     // Completion routine
				&MyData,          // Argument to the completion routine
				FALSE);          // Do not restore a suspended system

			if (bSuccess)
			{
				for (; MyData.dwValue < 1000; MyData.dwValue += 100)
				{
					SleepEx(
						INFINITE,     // Wait forever
						TRUE);       // Put thread in an alertable state
				}

			}
			else
			{
				printf("SetWaitableTimer failed with error %d\n", GetLastError());
			}

		}
		__finally
		{
			CloseHandle(hTimer);
		}
	}
	else
	{
		printf("CreateWaitableTimer failed with error %d\n", GetLastError());
	}

	return 0;
}
#endif // __WATEABLE_TIMERS_WITH_ASYN__


// Using Critical Section Objects
// https://docs.microsoft.com/en-us/windows/win32/sync/using-critical-section-objects
// The following example shows how a thread initializes, enters, and releases a critical section. 
// It uses the InitializeCriticalSectionAndSpinCount, EnterCriticalSection, LeaveCriticalSection, 
// and DeleteCriticalSection functions.
#ifdef __CRITICAL_SECTION_OBJECT__
// Global variable
CRITICAL_SECTION CriticalSection;

int main(void)
{
	...

		// Initialize the critical section one time only.
		if (!InitializeCriticalSectionAndSpinCount(&CriticalSection,
			0x00000400))
			return;
	...

		// Release resources used by the critical section object.
		DeleteCriticalSection(&CriticalSection);
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	...

		// Request ownership of the critical section.
		EnterCriticalSection(&CriticalSection);

	// Access the shared resource.

	// Release ownership of the critical section.
	LeaveCriticalSection(&CriticalSection);

	...
		return 1;
}
#endif // __CRITICAL_SECTION_OBJECT__


// Using Condition Variables
// https://docs.microsoft.com/en-us/windows/win32/sync/using-condition-variables
// The following code implements a producer/consumer queue. 
// The queue is represented as a bounded circular buffer, and is protected by a critical section. 
// The code uses two condition variables: one used by producers (BufferNotFull) and one used by consumers (BufferNotEmpty).
// 
// The code calls the InitializeConditionVariable function to create the condition variables.
// The consumer threads call the SleepConditionVariableCS function to wait for items to be added 
// to the queue and the WakeConditionVariable function to signal the producer that it is ready for more items.
// The producer threads call SleepConditionVariableCS to wait for the consumer to remove items from the queue 
// and WakeConditionVariable to signal the consumer that there are more items in the queue.
// 
// Windows Server 2003 and Windows XP : Condition variables are not supported.
#ifdef __CONDITION_VARIABLES__
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 10
#define PRODUCER_SLEEP_TIME_MS 500
#define CONSUMER_SLEEP_TIME_MS 2000

LONG Buffer[BUFFER_SIZE];
LONG LastItemProduced;
ULONG QueueSize;
ULONG QueueStartOffset;

ULONG TotalItemsProduced;
ULONG TotalItemsConsumed;

CONDITION_VARIABLE BufferNotEmpty;
CONDITION_VARIABLE BufferNotFull;
CRITICAL_SECTION   BufferLock;

BOOL StopRequested;

DWORD WINAPI ProducerThreadProc(PVOID p)
{
	ULONG ProducerId = (ULONG)(ULONG_PTR)p;

	while (true)
	{
		// Produce a new item.

		Sleep(rand() % PRODUCER_SLEEP_TIME_MS);

		ULONG Item = InterlockedIncrement(&LastItemProduced);

		EnterCriticalSection(&BufferLock);

		while (QueueSize == BUFFER_SIZE && StopRequested == FALSE)
		{
			// Buffer is full - sleep so consumers can get items.
			SleepConditionVariableCS(&BufferNotFull, &BufferLock, INFINITE);
		}

		if (StopRequested == TRUE)
		{
			LeaveCriticalSection(&BufferLock);
			break;
		}

		// Insert the item at the end of the queue and increment size.

		Buffer[(QueueStartOffset + QueueSize) % BUFFER_SIZE] = Item;
		QueueSize++;
		TotalItemsProduced++;

		printf("Producer %u: item %2d, queue size %2u\r\n", ProducerId, Item, QueueSize);

		LeaveCriticalSection(&BufferLock);

		// If a consumer is waiting, wake it.

		WakeConditionVariable(&BufferNotEmpty);
	}

	printf("Producer %u exiting\r\n", ProducerId);
	return 0;
}

DWORD WINAPI ConsumerThreadProc(PVOID p)
{
	ULONG ConsumerId = (ULONG)(ULONG_PTR)p;

	while (true)
	{
		EnterCriticalSection(&BufferLock);

		while (QueueSize == 0 && StopRequested == FALSE)
		{
			// Buffer is empty - sleep so producers can create items.
			SleepConditionVariableCS(&BufferNotEmpty, &BufferLock, INFINITE);
		}

		if (StopRequested == TRUE && QueueSize == 0)
		{
			LeaveCriticalSection(&BufferLock);
			break;
		}

		// Consume the first available item.

		LONG Item = Buffer[QueueStartOffset];

		QueueSize--;
		QueueStartOffset++;
		TotalItemsConsumed++;

		if (QueueStartOffset == BUFFER_SIZE)
		{
			QueueStartOffset = 0;
		}

		printf("Consumer %u: item %2d, queue size %2u\r\n",
			ConsumerId, Item, QueueSize);

		LeaveCriticalSection(&BufferLock);

		// If a producer is waiting, wake it.

		WakeConditionVariable(&BufferNotFull);

		// Simulate processing of the item.

		Sleep(rand() % CONSUMER_SLEEP_TIME_MS);
	}

	printf("Consumer %u exiting\r\n", ConsumerId);
	return 0;
}

int main(void)
{
	InitializeConditionVariable(&BufferNotEmpty);
	InitializeConditionVariable(&BufferNotFull);

	InitializeCriticalSection(&BufferLock);

	DWORD id;
	HANDLE hProducer1 = CreateThread(NULL, 0, ProducerThreadProc, (PVOID)1, 0, &id);
	HANDLE hConsumer1 = CreateThread(NULL, 0, ConsumerThreadProc, (PVOID)1, 0, &id);
	HANDLE hConsumer2 = CreateThread(NULL, 0, ConsumerThreadProc, (PVOID)2, 0, &id);

	puts("Press enter to stop...");
	getchar();

	EnterCriticalSection(&BufferLock);
	StopRequested = TRUE;
	LeaveCriticalSection(&BufferLock);

	WakeAllConditionVariable(&BufferNotFull);
	WakeAllConditionVariable(&BufferNotEmpty);

	WaitForSingleObject(hProducer1, INFINITE);
	WaitForSingleObject(hConsumer1, INFINITE);
	WaitForSingleObject(hConsumer2, INFINITE);

	printf("TotalItemsProduced: %u, TotalItemsConsumed: %u\r\n",
		TotalItemsProduced, TotalItemsConsumed);
	return 0;
}
#endif // __CONDITION_VARIABLES__


// Using One-Time Initialization	
// https://docs.microsoft.com/en-us/windows/win32/sync/using-one-time-initialization
// The following examples demonstrate the use of the one-time initialization functions.
// 
// 
// Synchronous Example
// In this example, the g_InitOnce global variable is the one-time initialization structure. 
// It is initialized statically using INIT_ONCE_STATIC_INIT.
// 
// The OpenEventHandleSync function returns a handle to an event that is created only once.
// It calls the InitOnceExecuteOnce function to execute the initialization code contained 
// in the InitHandleFunction callback function.If the callback function succeeds, 
// OpenEventHandleSync returns the event handle returned in lpContext; otherwise, 
// it returns INVALID_HANDLE_VALUE.
// 
// The InitHandleFunction function is the one - time initialization callback function.
// InitHandleFunction calls the CreateEvent function to create the event 
// and returns the event handle in the lpContext parameter.
//
//
// Asynchronous Example
// In this example, the g_InitOnce global variable is the one-time initialization structure. 
// It is initialized statically using INIT_ONCE_STATIC_INIT.
// 
// The OpenEventHandleAsync function returns a handle to an event that is created only once.
// OpenEventHandleAsync calls the InitOnceBeginInitialize function to enter the initializing state.
// 
// If the call succeeds, the code checks the value of the fPending parameter 
// to determine whether to create the event or simply return a handle to the event created by another thread.
// If fPending is FALSE, initialization has already completed so OpenEventHandleAsync returns 
// the event handle returned in the lpContext parameter.Otherwise, it calls the CreateEvent function 
// to create the event and the InitOnceComplete function to complete the initialization.
// 
// If the call to InitOnceComplete succeeds, OpenEventHandleAsync returns the new event handle.
// Otherwise, it closes the event handle and calls InitOnceBeginInitialize with INIT_ONCE_CHECK_ONLY 
// to determine whether initialization failed or was completed by another thread.
// 
// If the initialization was completed by another thread, OpenEventHandleAsync returns 
// the event handle returned in lpContext.Otherwise, it returns INVALID_HANDLE_VALUE.
#ifdef __SYNCHRONOUS_EXAMPLE__
#define _WIN32_WINNT 0x0600
#include <windows.h>

// Global variable for one-time initialization structure
INIT_ONCE g_InitOnce = INIT_ONCE_STATIC_INIT; // Static initialization

											  // Initialization callback function 
BOOL CALLBACK InitHandleFunction(
	PINIT_ONCE InitOnce,
	PVOID Parameter,
	PVOID *lpContext);

// Returns a handle to an event object that is created only once
HANDLE OpenEventHandleSync()
{
	PVOID lpContext;
	BOOL  bStatus;

	// Execute the initialization callback function 
	bStatus = InitOnceExecuteOnce(&g_InitOnce,          // One-time initialization structure
		InitHandleFunction,   // Pointer to initialization callback function
		NULL,                 // Optional parameter to callback function (not used)
		&lpContext);          // Receives pointer to event object stored in g_InitOnce

							  // InitOnceExecuteOnce function succeeded. Return event object.
	if (bStatus)
	{
		return (HANDLE)lpContext;
	}
	else
	{
		return (INVALID_HANDLE_VALUE);
	}
}

// Initialization callback function that creates the event object 
BOOL CALLBACK InitHandleFunction(
	PINIT_ONCE InitOnce,        // Pointer to one-time initialization structure        
	PVOID Parameter,            // Optional parameter passed by InitOnceExecuteOnce            
	PVOID *lpContext)           // Receives pointer to event object           
{
	HANDLE hEvent;

	// Create event object
	hEvent = CreateEvent(NULL,    // Default security descriptor
		TRUE,    // Manual-reset event object
		TRUE,    // Initial state of object is signaled 
		NULL);   // Object is unnamed

				 // Event object creation failed.
	if (NULL == hEvent)
	{
		return FALSE;
	}
	// Event object creation succeeded.
	else
	{
		*lpContext = hEvent;
		return TRUE;
	}
}
#endif // __SYNCHRONOUS_EXAMPLE__
#ifdef __ASYNCHRONOUS_EXAMPLE__
#define _WIN32_WINNT 0x0600
#include <windows.h>

// Global variable for one-time initialization structure
INIT_ONCE g_InitOnce = INIT_ONCE_STATIC_INIT; // Static initialization

											  // Returns a handle to an event object that is created only once
HANDLE OpenEventHandleAsync()
{
	PVOID  lpContext;
	BOOL   fStatus;
	BOOL   fPending;
	HANDLE hEvent;

	// Begin one-time initialization
	fStatus = InitOnceBeginInitialize(&g_InitOnce,       // Pointer to one-time initialization structure
		INIT_ONCE_ASYNC,   // Asynchronous one-time initialization
		&fPending,         // Receives initialization status
		&lpContext);       // Receives pointer to data in g_InitOnce  

						   // InitOnceBeginInitialize function failed.
	if (!fStatus)
	{
		return (INVALID_HANDLE_VALUE);
	}

	// Initialization has already completed and lpContext contains event object.
	if (!fPending)
	{
		return (HANDLE)lpContext;
	}

	// Create event object for one-time initialization.
	hEvent = CreateEvent(NULL,    // Default security descriptor
		TRUE,    // Manual-reset event object
		TRUE,    // Initial state of object is signaled 
		NULL);   // Object is unnamed

				 // Event object creation failed.
	if (NULL == hEvent)
	{
		return (INVALID_HANDLE_VALUE);
	}

	// Complete one-time initialization.
	fStatus = InitOnceComplete(&g_InitOnce,             // Pointer to one-time initialization structure
		INIT_ONCE_ASYNC,         // Asynchronous initialization
		(PVOID)hEvent);          // Pointer to event object to be stored in g_InitOnce

								 // InitOnceComplete function succeeded. Return event object.
	if (fStatus)
	{
		return hEvent;
	}

	// Initialization has already completed. Free the local event.
	CloseHandle(hEvent);


	// Retrieve the final context data.
	fStatus = InitOnceBeginInitialize(&g_InitOnce,            // Pointer to one-time initialization structure
		INIT_ONCE_CHECK_ONLY,   // Check whether initialization is complete
		&fPending,              // Receives initialization status
		&lpContext);            // Receives pointer to event object in g_InitOnce

								// Initialization is complete. Return handle.
	if (fStatus && !fPending)
	{
		return (HANDLE)lpContext;
	}
	else
	{
		return INVALID_HANDLE_VALUE;
	}
}
#endif // __ASYNCHRONOUS_EXAMPLE__


// Using Singly Linked Lists
// https://docs.microsoft.com/en-us/windows/win32/sync/using-singly-linked-lists
// The following example uses the InitializeSListHead function to initialize a singly linked list 
// and the InterlockedPushEntrySList function to insert 10 items. 
// The example uses the InterlockedPopEntrySList function to remove 10 items and 
// the InterlockedFlushSList function to verify that the list is empty.
#ifdef __SINGLY_LINKED_LISTS__
#include <windows.h>
#include <malloc.h>
#include <stdio.h>

// Structure to be used for a list item; the first member is the 
// SLIST_ENTRY structure, and additional members are used for data.
// Here, the data is simply a signature for testing purposes. 


typedef struct _PROGRAM_ITEM {
	SLIST_ENTRY ItemEntry;
	ULONG Signature;
} PROGRAM_ITEM, *PPROGRAM_ITEM;

int main()
{
	ULONG Count;
	PSLIST_ENTRY pFirstEntry, pListEntry;
	PSLIST_HEADER pListHead;
	PPROGRAM_ITEM pProgramItem;

	// Initialize the list header to a MEMORY_ALLOCATION_ALIGNMENT boundary.
	pListHead = (PSLIST_HEADER)_aligned_malloc(sizeof(SLIST_HEADER),
		MEMORY_ALLOCATION_ALIGNMENT);
	if (NULL == pListHead)
	{
		printf("Memory allocation failed.\n");
		return -1;
	}
	InitializeSListHead(pListHead);

	// Insert 10 items into the list.
	for (Count = 1; Count <= 10; Count += 1)
	{
		pProgramItem = (PPROGRAM_ITEM)_aligned_malloc(sizeof(PROGRAM_ITEM),
			MEMORY_ALLOCATION_ALIGNMENT);
		if (NULL == pProgramItem)
		{
			printf("Memory allocation failed.\n");
			return -1;
		}
		pProgramItem->Signature = Count;
		pFirstEntry = InterlockedPushEntrySList(pListHead,
			&(pProgramItem->ItemEntry));
	}

	// Remove 10 items from the list and display the signature.
	for (Count = 10; Count >= 1; Count -= 1)
	{
		pListEntry = InterlockedPopEntrySList(pListHead);

		if (NULL == pListEntry)
		{
			printf("List is empty.\n");
			return -1;
		}

		pProgramItem = (PPROGRAM_ITEM)pListEntry;
		printf("Signature is %d\n", pProgramItem->Signature);

		// This example assumes that the SLIST_ENTRY structure is the 
		// first member of the structure. If your structure does not 
		// follow this convention, you must compute the starting address 
		// of the structure before calling the free function.

		_aligned_free(pListEntry);
	}

	// Flush the list and verify that the items are gone.
	pListEntry = InterlockedFlushSList(pListHead);
	pFirstEntry = InterlockedPopEntrySList(pListHead);
	if (pFirstEntry != NULL)
	{
		printf("Error: List is not empty.\n");
		return -1;
	}

	_aligned_free(pListHead);

	return 1;
}
#endif // __SINGLY_LINKED_LISTS__


// Using Timer Queues
// https://docs.microsoft.com/en-us/windows/win32/sync/using-timer-queues
// The following example creates a timer routine that will be executed 
// by a thread from a timer queue after a 10 second delay. First, the code uses the CreateEvent function 
// to create an event object that is signaled when the timer-queue thread completes. 
// Then it creates a timer queue and a timer-queue timer, 
// using the CreateTimerQueue and CreateTimerQueueTimer functions, respectively. 
// The code uses the WaitForSingleObject function to determine when the timer routine has completed. 
// Finally, the code calls DeleteTimerQueue to clean up.
// 
// For more information on the timer routine, see WaitOrTimerCallback.
#ifdef __TIMER_QUEUES__
#include <windows.h>
#include <stdio.h>

HANDLE gDoneEvent;

VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	if (lpParam == NULL)
	{
		printf("TimerRoutine lpParam is NULL\n");
	}
	else
	{
		// lpParam points to the argument; in this case it is an int

		printf("Timer routine called. Parameter is %d.\n",
			*(int*)lpParam);
		if (TimerOrWaitFired)
		{
			printf("The wait timed out.\n");
		}
		else
		{
			printf("The wait event was signaled.\n");
		}
	}

	SetEvent(gDoneEvent);
}

int main()
{
	HANDLE hTimer = NULL;
	HANDLE hTimerQueue = NULL;
	int arg = 123;

	// Use an event object to track the TimerRoutine execution
	gDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == gDoneEvent)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return 1;
	}

	// Create the timer queue.
	hTimerQueue = CreateTimerQueue();
	if (NULL == hTimerQueue)
	{
		printf("CreateTimerQueue failed (%d)\n", GetLastError());
		return 2;
	}

	// Set a timer to call the timer routine in 10 seconds.
	if (!CreateTimerQueueTimer(&hTimer, hTimerQueue,
		(WAITORTIMERCALLBACK)TimerRoutine, &arg, 10000, 0, 0))
	{
		printf("CreateTimerQueueTimer failed (%d)\n", GetLastError());
		return 3;
	}

	// TODO: Do other useful work here 

	printf("Call timer routine in 10 seconds...\n");

	// Wait for the timer-queue thread to complete using an event 
	// object. The thread will signal the event at that time.

	if (WaitForSingleObject(gDoneEvent, INFINITE) != WAIT_OBJECT_0)
		printf("WaitForSingleObject failed (%d)\n", GetLastError());

	CloseHandle(gDoneEvent);

	// Delete all timers in the timer queue.
	if (!DeleteTimerQueue(hTimerQueue))
		printf("DeleteTimerQueue failed (%d)\n", GetLastError());

	return 0;
}
#endif // __TIMER_QUEUES__

#endif // __USING_SYNCHRONIZATION__

/* ********************* Using Synchronization end ***************************** */

