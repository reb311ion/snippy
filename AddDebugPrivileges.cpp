int AddDebugPrivileges() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;

	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES, &hToken);

	if( !LookupPrivilegeValueA( NULL, "SeDebugPrivilege", &tp.Privileges[0].Luid ) ) {
		CloseHandle(hToken);
		return 1;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if( !AdjustTokenPrivileges( hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0) ) {
		CloseHandle(hToken);
		return 1;
	}

	CloseHandle(hToken);
	return 0;
}
