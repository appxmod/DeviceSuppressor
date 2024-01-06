#include "pch.h"



#include <stdio.h>
#include <Windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>

#pragma comment (lib, "Newdev.lib")
#pragma comment (lib, "Setupapi.lib")
/* for Devpkey */
#define INITGUID

/* dependencies */
#pragma comment (lib, "SetupAPI")
#include <windows.h>
#include <SetupAPI.h>
#include <Devpkey.h>


#include <vector>
#include <string>
#include <strsafe.h>
#include <iostream>


#include <iostream>
using namespace std;



size_t StrLength(const CHAR* data, int max)
{
	size_t ret = 0;
	if(SUCCEEDED(StringCchLengthA(data, max, &ret))) {}
	return ret;
}

size_t StrLength(const TCHAR* data, int max)
{
	size_t ret = 0;
	if(SUCCEEDED(StringCchLengthW(data, max, &ret))) {}
	return ret;
}

bool StrStartWith(const CHAR* data, const CHAR* data1, bool ignoreCase, int offset) //const
{
	int pos = offset;
	int pos1 = 0;
	int len = StrLength(data, 4096);
	int len1 = StrLength(data1, 4096);
	if (pos==-1) pos=len-len1; // end with
	if (pos < 0 || pos+len1 > len) return false;
	if (ignoreCase) 
	{
		while (--len1 >= 0) 
			if (tolower(data[pos++]) != tolower(data1[pos1++])) 
				return false;
	}
	else while (--len1 >= 0)
		if (data[pos++] != data1[pos1++])
			return false;
	return true;
}

bool StrStartWith(const TCHAR* data, const TCHAR* data1, bool ignoreCase, int offset) //const
{
	int pos = offset;
	int pos1 = 0;
	int len = StrLength(data, 4096);
	int len1 = StrLength(data1, 4096);
	if (pos==-1) pos=len-len1; // end with
	if (pos < 0 || pos+len1 > len) return false;
	if (ignoreCase) 
	{
		while (--len1 >= 0) 
			if (tolower(data[pos++]) != tolower(data1[pos1++])) 
				return false;
	}
	else while (--len1 >= 0)
		if (data[pos++] != data1[pos1++])
			return false;
	return true;
}


bool isMatch(string text, string pattern)
{
	int n = text.length();
	int m = pattern.length();
	int i = 0, j = 0, startIndex = -1, match = 0;
#define lower(...) tolower(##__VA_ARGS__)
	while (i < n)
	{
		if (j < m && (pattern[j] == '?' || lower(pattern[j]) == lower(text[i])))
		{
			// Characters match or '?' in pattern matches any character.
			i++;
			j++;
		}
		else if (j < m && pattern[j] == '*')
		{
			// Wildcard character '*', mark the current position in the pattern and the text as a proper match.
			startIndex = j;
			match = i;
			j++;
		}
		else if (startIndex != -1)
		{
			// No match, but a previous wildcard was found. Backtrack to the last '*' character position and try for a different match.
			j = startIndex + 1;
			match++;
			i = match;
		}
		else
		{
			// If none of the above cases comply, the pattern does not match.
			return false;
		}
	}

	// Consume any remaining '*' characters in the given pattern.
	while (j < m && pattern[j] == '*')
	{
		j++;
	}

	// If we have reached the end of both the pattern and the text, the pattern matches the text.
	return lower(j) == lower(m);
}



bool isMatch(wstring text, wstring pattern)
{
	int n = text.length();
	int m = pattern.length();
	int i = 0, j = 0, startIndex = -1, match = 0;
#define lower(...) tolower(##__VA_ARGS__)
	while (i < n)
	{
		if (j < m && (pattern[j] == '?' || lower(pattern[j]) == lower(text[i])))
		{
			// Characters match or '?' in pattern matches any character.
			i++;
			j++;
		}
		else if (j < m && pattern[j] == '*')
		{
			// Wildcard character '*', mark the current position in the pattern and the text as a proper match.
			startIndex = j;
			match = i;
			j++;
		}
		else if (startIndex != -1)
		{
			// No match, but a previous wildcard was found. Backtrack to the last '*' character position and try for a different match.
			j = startIndex + 1;
			match++;
			i = match;
		}
		else
		{
			// If none of the above cases comply, the pattern does not match.
			return false;
		}
	}

	// Consume any remaining '*' characters in the given pattern.
	while (j < m && pattern[j] == '*')
	{
		j++;
	}

	// If we have reached the end of both the pattern and the text, the pattern matches the text.
	return lower(j) == lower(m);
}


int Register_GetValue(const char* sub_key, const char* key, char* value) {
	BYTE* buffer = NULL;
	HKEY hkey = nullptr;
	LSTATUS res = ::RegOpenKeyExA(HKEY_CURRENT_USER, sub_key, 0, KEY_READ, &hkey);
	if (res != ERROR_SUCCESS) {
		return -1;
	}
	std::shared_ptr<void> close_key(nullptr, [&](void*) {
		if (hkey != nullptr) {
			::RegCloseKey(hkey);
			hkey = nullptr;
		}
	});
	DWORD type = REG_SZ;
	DWORD size = 0;
	res = ::RegQueryValueExA(hkey, key, 0, &type, nullptr, &size);
	if (res != ERROR_SUCCESS || size <= 0) {
		return -1;
	}
	std::vector<BYTE> value_data(size);
	res = ::RegQueryValueExA(hkey, key, 0, &type, value_data.data(), &size);
	if (res != ERROR_SUCCESS) {
		return -1;
	}

	buffer = &value_data[0];
	strcpy_s(value, value_data.size(), (char*)buffer);
	return 1;
}

VOID SetDeviceState(LPCWSTR lpszHardwareId, DWORD dwState, int Id_Class_Desc)
{				
	//OutputDebugString(L"SetDeviceState=");
	//OutputDebugString(lpszHardwareId);
	//OutputDebugString(L"\n");

	//std::cout<<"by="<<Id_Class_Desc<<std::endl;
	//wprintf(lpszHardwareId);
	if(Id_Class_Desc==3)
	{
		//int set = GetSystemMetrics(SM_CMONITORS)  ;
		//std::cout<<set<<std::endl;
		const char* keyPath = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Accessibility\\ATConfig\\colorfiltering\\";
		char value[1024] = { 0 };
		int ret = Register_GetValue(keyPath, "Active", value);
		std::cerr << ret << value << std::endl;
		return;
	}
	HDEVINFO devInfo;
	if (devInfo = SetupDiGetClassDevsW(NULL, NULL, NULL, DIGCF_ALLCLASSES))
	{
		DWORD dwBuffersize;
		SP_DEVINFO_DATA devData;
		DEVPROPTYPE devProptype;
		LPWSTR devBuffer;

		devData.cbSize = sizeof(SP_DEVINFO_DATA);
		bool wildcard = StrStartWith(lpszHardwareId, L"?", false, 0);
		if(wildcard) lpszHardwareId++;

		auto spec = Id_Class_Desc==0?&DEVPKEY_Device_HardwareIds
			: Id_Class_Desc==1?&DEVPKEY_Device_Class
			: &DEVPKEY_Device_DeviceDesc;

		for (int i = 0; ; i++)
		{
			SetupDiEnumDeviceInfo(devInfo, i, &devData);
			if (GetLastError() == ERROR_NO_MORE_ITEMS) break;

			/* calculate buffer size */
			SetupDiGetDevicePropertyW(devInfo, &devData, spec, &devProptype, NULL, 0, &dwBuffersize, 0);

			/* real call */
			if (devBuffer = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, dwBuffersize))
			{
				SetupDiGetDevicePropertyW(devInfo, &devData, spec, &devProptype, (PBYTE)devBuffer, dwBuffersize, NULL, 0);
				//OutputDebugString(devBuffer);
				//OutputDebugString(L"\n");
				if(dwState==-1)
				{
					//std::cout<<devBuffer<<std::endl;
					wprintf((const TCHAR*)devBuffer);
					std::cout<<std::endl;
				}
				else 
				{
					bool found = false;
					if(wildcard)
					{
						found = isMatch(devBuffer, lpszHardwareId);
					}
					else
					{
						// _tcsicmp lstrcmpW
						found = _tcsicmp(devBuffer, lpszHardwareId) == 0;
					}
					if(found)
					{
						/* matched */
						SP_CLASSINSTALL_HEADER ciHeader;
						ciHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
						ciHeader.InstallFunction = DIF_PROPERTYCHANGE;

						SP_PROPCHANGE_PARAMS pcParams;
						pcParams.ClassInstallHeader = ciHeader;
						pcParams.StateChange = dwState;
						pcParams.Scope = DICS_FLAG_GLOBAL;
						pcParams.HwProfile = 0;

						SetupDiSetClassInstallParamsW(devInfo, &devData, (PSP_CLASSINSTALL_HEADER)&pcParams, sizeof(SP_PROPCHANGE_PARAMS));
						SetupDiChangeState(devInfo, &devData);
					}
				}
				HeapFree (GetProcessHeap(), 0, devBuffer);
			}
		}
		SetupDiDestroyDeviceInfoList(devInfo);
	}
}

int mainx(int argc, void * argv[])
{
	string str = "baaababxx";
	string pattern = "*****ba*****ab??";
	if (isMatch(str, pattern))
		cout << "Yes" << endl;
	else
		cout << "No" << endl;
	return 0;
}


int main(int argc, void * argv[])
{
	//std::vector<std::string> arguments;

	QkString tmp;
	QkString tmpBy;
	tmp.AsBuffer();
	tmpBy.AsBuffer();
	int ret = 0;
	for (int i = 0; i < argc; i++)
	{
		//arguments.push_back(std::string((CHAR*)argv[i]));
		//OutputDebugStringA(arguments[arguments.size()-1].data());
		//OutputDebugString(L"\n哈哈哈\n");
		auto arg = (CHAR*) argv[i];
		bool enable = StrStartWith(arg, "enable", false, 0);
		if(enable || StrStartWith(arg, "disable", false, 0)) {
			arg += enable?6:7;
			int idx = strchr(arg, '=') - arg;
			if(idx >= 0) {
				// prepare id name
				tmp = arg + idx + 1;
				if(tmp.StartWith('\"') && tmp.EndWith('\"')) {
					tmp.MidFast(1, tmp.GetLength()-2);
				}
				tmp.Trim();

				// prepare by what spec
				int by = 0;
				tmpBy.Empty();
				if(idx > 0) {
					arg[idx] = '\0';
					tmpBy = arg;
					by = tmpBy.StartWith(L"bydesc")?2
						:tmpBy.StartWith(L"byclass")?1
						:0
						;
				}
				SetDeviceState(tmp, enable?DICS_ENABLE:DICS_DISABLE, by);
			}
		}
		else if(StrStartWith(arg, "print", false, 0)) {
			int by = 0;
			if(StrStartWith(arg, "=", false, 5)) {
				tmpBy = arg+6;
				by = tmpBy.StartWith(L"byset")?3
					: tmpBy.StartWith(L"bydesc")?2
					:tmpBy.StartWith(L"byclass")?1
					:0
					;
			}
			SetDeviceState(tmp, -1, by);
		}
		else if(StrStartWith(arg, "delete", false, 0)) {
			arg += 6;
			int idx = strchr(arg, '=') - arg;
			if(idx >= 0) {
				// prepare id name
				tmp = arg + idx + 1;
				if(tmp.StartWith('\"') && tmp.EndWith('\"')) {
					tmp.MidFast(1, tmp.GetLength()-2);
				}
				tmp.Trim();
				std::string buffer;
				extern int MoveToTrash(PCZZSTR path, BOOL bNoUI);

				//OutputDebugString(L"MoveToTrash\n");
				//OutputDebugString(tmp);
				auto path = tmp.GetData(buffer);
				std::cout<<"deleting::"<<path<<std::endl;
				ret = MoveToTrash(path, true);
				std::cout<<"deleted="<<ret<<std::endl;
			}
		}
	}
	// enable="USB\VID_12C9&PID_1028&REV_0210&MI_00"
	
	//SetDeviceState(L"USB\\VID_12C9&PID_1028&REV_0210&MI_00", DICS_DISABLE);
	//SetDeviceState(L"USB\\VID_12C9&PID_1028&REV_0210&MI_00", DICS_ENABLE);

	return ret;
}

// print=byset
// delete="D:/tmp/New folder (4)/1.png"
