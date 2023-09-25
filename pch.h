
#pragma once

#include <wchar.h>	
#include "windows.h"
#include <string>
#include <vector>
#include <tchar.h>
#include "windowsx.h"
#include "winerror.h"
#include <strsafe.h>

#include "QkString.h"

#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))

#define ASSERT //
