/////////////////////////////////////////////////////////////////////////////////////
//
//
#include "pch.h"

QkString QkString::_EmptyInstance = L"";

QkString::QkString() : m_pstr(m_szBuffer), _capacity(0), _isBuffer(0)
{
	m_szBuffer[0] = '\0';
	_size = 0;
}

QkString::QkString(const TCHAR ch) : m_pstr(m_szBuffer), _capacity(0), _isBuffer(0)
{
	m_szBuffer[0] = ch;
	m_szBuffer[1] = '\0';
	_size = 1;
}

QkString::QkString(const CHAR* ch) : m_pstr(m_szBuffer), _capacity(0), _isBuffer(0)
{
	_size = 0;
	*this = ch;
}

QkString::QkString(LPCTSTR lpsz, int nLen) : m_pstr(m_szBuffer), _capacity(0), _isBuffer(0)
{      
	ASSERT(!::IsBadStringPtr(lpsz,-1) || lpsz==NULL);
	m_szBuffer[0] = '\0';
	_size = 0;
	Assign(lpsz, nLen);
}

QkString::QkString(const QkString& src) : m_pstr(m_szBuffer), _capacity(0), _isBuffer(0)
{
	m_szBuffer[0] = '\0';
	_size = 0;
	Assign(src);
}

QkString::~QkString()
{
	if( m_pstr != m_szBuffer && _capacity) free(m_pstr);
}

size_t QkString::GetLength() const
{ 
	return _size; 
	//return (int) _tcslen(m_pstr); 
}

QkString::operator LPCTSTR() const 
{ 
	return m_pstr; 
}

bool QkString::EnsureCapacity(size_t nNewLength, bool release)
{
	if (_isBuffer && _capacity>=nNewLength) return true;
	if( nNewLength > MAX_LOCAL_STRING_LEN ) 
	{
		if( m_pstr == m_szBuffer ) 
		{
			m_pstr = static_cast<LPTSTR>(malloc((nNewLength + 1) * sizeof(TCHAR)));
			if (m_pstr)
			{
				_tcsncpy(m_pstr, m_szBuffer, GetLength());
				m_pstr[_size] = 0;
				_capacity = nNewLength;
				return true;
			}
			else
			{
				m_pstr = m_szBuffer;
				// malloc failed
			}
		}
		else if(_capacity<nNewLength)
		{
			size_t newCapacity = nNewLength*2 + 1;
			if (newCapacity<nNewLength)
				newCapacity = nNewLength;
			LPTSTR _pstr = static_cast<LPTSTR>(realloc(m_pstr, (newCapacity + 1) * sizeof(TCHAR)));
			if (_pstr)
			{
				m_pstr = _pstr;
				_capacity = newCapacity;
				return true;
			}
			// realloc failed.
		}
		else return true;
	}
	else {
		if( m_pstr != m_szBuffer && release ) 
		{
			// never happen ? 
			// will if Assign -> enssure cap -> ...
			// _tcscpy(m_szBuffer, m_pstr); // dont bother
			free(m_pstr);
			_capacity = 0;
			m_pstr = m_szBuffer;
		}
		return true;
	}
	// ensure capacity failed.
	return false;
}

size_t QkString::RecalcSize()
{
	size_t ret = _size;
	if(SUCCEEDED(StringCchLength(m_pstr, max(_capacity, MAX_LOCAL_STRING_LEN), &ret)))
		_size = ret;
	return ret;
}

void QkString::ReFit()
{
	size_t sz = RecalcSize();
	if (sz > MAX_LOCAL_STRING_LEN)
	{
		if (_capacity+MAX_LOCAL_STRING_LEN>sz)
		{
			LPTSTR _pstr = static_cast<LPTSTR>(realloc(m_pstr, (sz + 1) * sizeof(TCHAR)));
			if (_pstr)
			{
				m_pstr = _pstr;
				_capacity = sz;
			}
			// realloc failed.
		}
	}
	else if (m_pstr!=m_szBuffer)
	{
		_tcsncpy(m_szBuffer, m_pstr, sz);
		free(m_pstr);
		_capacity = 0;
		m_pstr = m_szBuffer;
	}
}

// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strcpy-wcscpy-mbscpy?view=msvc-160

// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strncpy-strncpy-l-wcsncpy-wcsncpy-l-mbsncpy-mbsncpy-l?view=msvc-160

// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/strncat-strncat-l-wcsncat-wcsncat-l-mbsncat-mbsncat-l?view=msvc-160

void QkString::Prepend(LPCTSTR pstr, int nLength)
{
	//QkString backup = *this;
	//Assign(pstr);
	//Append(backup, backup.GetLength());
	if( !pstr ) return;
	nLength = (nLength < 0 ? (int) _tcslen(pstr) : nLength);
	int nNewLength = GetLength() + nLength;
	if (EnsureCapacity(nNewLength, false))
	{
		wmemmove(m_pstr+nLength, m_pstr, GetLength());
		//TCHAR backup = m_pstr[nLength];
		_tcsncpy(m_pstr, pstr, nLength);
		//m_pstr[nLength] = backup;
		m_pstr[_size = nNewLength] = L'\0';
	}
}

void QkString::Append(LPCTSTR pstr, int nLength)
{
	nLength = (nLength < 0 ? (int) _tcslen(pstr) : nLength);
	int nNewLength = GetLength() + nLength;
	if (EnsureCapacity(nNewLength, false))
	{
		//todo
		_tcsncat(m_pstr, pstr, nLength);
		_size = nNewLength;
	}
}

void QkString::Assign(LPCTSTR pstr, int cchMax)
{
	if( pstr == NULL ) pstr = _T("");
	cchMax = (cchMax < 0 ? (int) _tcslen(pstr) : cchMax);
	if (EnsureCapacity(cchMax, true))
	{
		_tcsncpy(m_pstr, pstr, cchMax);
		m_pstr[cchMax] = '\0';
		_size = cchMax;
	}
}

bool QkString::IsEmpty() const 
{ 
	return m_pstr[0] == '\0'; 
}

void QkString::Empty() 
{ 
	if (_size)
	{
		_size = 0;
		if (!_isBuffer)
		{
			if( m_pstr != m_szBuffer ) 
			{
				free(m_pstr);
				_capacity = 0;
				m_pstr = m_szBuffer;
			}
		}
		m_pstr[0] = '\0'; 
	}
}

LPCTSTR QkString::GetData() const
{
	return m_pstr;
}

LPCSTR QkString::GetData(std::string & buffer, int offset, int length) const
{
	if(length==-1)
		length = GetLength() - offset;
	size_t max_cap = length*2.5;
	if (buffer.capacity()<max_cap)
	{
		buffer.resize(MAX(max_cap*1.25, max_cap));
	}
	max_cap = buffer.capacity() - 1;
	CHAR* data = (CHAR*)buffer.c_str();
	size_t len = ::WideCharToMultiByte(::GetACP(), 0, GetData()+offset, length, data, max_cap, NULL, NULL);
	if(len==0) {
		int max_cap_ = max_cap;
		max_cap = length*4;
		if (buffer.capacity()<max_cap)
		{
			buffer.resize(max_cap);
			max_cap = buffer.capacity() - 1;
			if(max_cap > max_cap_) {
				data = (CHAR*)buffer.c_str();
				len = ::WideCharToMultiByte(::GetACP(), 0, GetData()+offset, length, data, max_cap, NULL, NULL);
			}
		}
	}
	data[len] = '\0';
	//LogIs(2, "GetData::bufferSz=%d, len=%d", max_cap, len);
	return buffer.c_str();
}

LPCSTR QkString::GetData(std::string & buffer, LPCWSTR pStr)
{
	size_t length = lstrlen(pStr);
	size_t max_cap = length*2;
	if (buffer.capacity()<max_cap)
	{
		buffer.resize(MAX(max_cap*1.2, max_cap));
	}
	max_cap = buffer.capacity() - 1;
	CHAR* data = (CHAR*)buffer.c_str();
	size_t len = ::WideCharToMultiByte(::GetACP(), 0, pStr, length, data, max_cap, NULL, NULL);
	if(len==0) {
		int max_cap_ = max_cap;
		max_cap = length*4;
		if (buffer.capacity()<max_cap)
		{
			buffer.resize(max_cap);
			max_cap = buffer.capacity() - 1;
			if(max_cap > max_cap_) {
				data = (CHAR*)buffer.c_str();
				len = ::WideCharToMultiByte(::GetACP(), 0, pStr, length, data, max_cap, NULL, NULL);
			}
		}
	}
	data[len] = '\0';
	return data;
}

TCHAR QkString::GetAt(int nIndex) const
{
	return m_pstr[nIndex];
}

TCHAR QkString::operator[] (int nIndex) const
{ 
	return m_pstr[nIndex];
}   

const QkString& QkString::operator=(const QkString& src)
{      
	Assign(src, src.GetLength());
	return *this;
}

const QkString& QkString::operator=(LPCTSTR lpStr)
{      
	if ( lpStr )
	{
		ASSERT(!::IsBadStringPtr(lpStr,-1));
		Assign(lpStr);
	}
	else
	{
		Empty();
	}
	return *this;
}

#ifdef _UNICODE

const QkString& QkString::operator=(LPCSTR lpStr)
{
	if ( lpStr )
	{
		ASSERT(!::IsBadStringPtrA(lpStr,-1));
		int cchStr = (int) strlen(lpStr) + 1;
		LPWSTR pwstr = (LPWSTR) _malloca(cchStr*sizeof(TCHAR));
		if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
		Assign(pwstr);
		_freea(pwstr);
	}
	else
	{
		Empty();
	}
	return *this;
}

const QkString& QkString::operator+=(LPCSTR lpStr)
{
	if ( lpStr )
	{
		ASSERT(!::IsBadStringPtrA(lpStr,-1));
		int cchStr = (int) strlen(lpStr) + 1;
		LPWSTR pwstr = (LPWSTR) _malloca(cchStr*sizeof(TCHAR));
		if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
		Append(pwstr);
		_freea(pwstr);
	}

	return *this;
}

#else

const QkString& QkString::operator=(LPCWSTR lpwStr)
{      
	if ( lpwStr )
	{
		ASSERT(!::IsBadStringPtrW(lpwStr,-1));
		int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
		LPSTR pstr = (LPSTR) _alloca(cchStr);
		if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
		Assign(pstr);
	}
	else
	{
		Empty();
	}

	return *this;
}

const QkString& QkString::operator+=(LPCWSTR lpwStr)
{
	if ( lpwStr )
	{
		ASSERT(!::IsBadStringPtrW(lpwStr,-1));
		int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
		LPSTR pstr = (LPSTR) _alloca(cchStr);
		if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
		Append(pstr);
	}

	return *this;
}

#endif // _UNICODE

const QkString& QkString::operator=(const TCHAR ch)
{
	Empty();
	m_szBuffer[0] = ch;
	m_szBuffer[1] = '\0';
	return *this;
}

QkString QkString::operator+(const QkString& src) const
{
	QkString sTemp = *this;
	sTemp.Append(src);
	return sTemp;
}

QkString QkString::operator+(LPCTSTR lpStr) const
{
	if ( lpStr )
	{
		ASSERT(!::IsBadStringPtr(lpStr,-1));
		QkString sTemp = *this;
		sTemp.Append(lpStr);
		return sTemp;
	}

	return *this;
}

const QkString& QkString::operator+=(const QkString& src)
{      
	Append(src);
	return *this;
}

const QkString& QkString::operator+=(LPCTSTR lpStr)
{      
	if ( lpStr )
	{
		ASSERT(!::IsBadStringPtr(lpStr,-1));
		Append(lpStr);
	}

	return *this;
}

const QkString& QkString::operator+=(const TCHAR ch)
{      
	TCHAR str[] = { ch, '\0' };
	Append(str);
	return *this;
}

bool QkString::operator == (LPCTSTR str) const { return (Compare(str) == 0); };
bool QkString::operator != (LPCTSTR str) const { return (Compare(str) != 0); };
bool QkString::operator == (const QkString & str) const{ return _size==str._size && _tcsncmp(m_pstr, str, _size) == 0; };
bool QkString::operator != (const QkString & str) const{ return _size!=str._size || _tcsncmp(m_pstr, str, _size) != 0; };
bool QkString::operator <= (LPCTSTR str) const { return (Compare(str) <= 0); };
bool QkString::operator <  (LPCTSTR str) const { return (Compare(str) <  0); };
bool QkString::operator >= (LPCTSTR str) const { return (Compare(str) >= 0); };
bool QkString::operator >  (LPCTSTR str) const { return (Compare(str) >  0); };

void QkString::SetAt(int nIndex, TCHAR ch)
{
	ASSERT(nIndex>=0 && nIndex<GetLength());
	m_pstr[nIndex] = ch;
}

int QkString::Compare(LPCTSTR lpsz) const 
{ 
	return _tcscmp(m_pstr, lpsz); 
}

int QkString::CompareNoCase(LPCTSTR lpsz) const 
{ 
	return _tcsicmp(m_pstr, lpsz); 
}

void QkString::MakeUpper() 
{ 
	_tcsupr(m_pstr); 
}

void QkString::MakeLower() 
{ 
	_tcslwr(m_pstr); 
}

QkString QkString::Left(int iLength) const
{
	if( iLength < 0 ) iLength = 0;
	if( iLength > GetLength() ) iLength = GetLength();
	return QkString(m_pstr, iLength);
}

void QkString::MidFast(int iPos, int iLength)
{
	if( iLength < 0 ) iLength = GetLength() - iPos;
	if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
	if (iLength<GetLength())
	{
		if (iPos>0)
		{
			wmemmove(m_pstr, m_pstr+iPos, iLength);
		}
		m_pstr[iLength] = '\0';
		_size = iLength;
	}
}

QkString QkString::Mid(int iPos, int iLength) const
{
	if( iLength < 0 ) iLength = GetLength() - iPos;
	if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
	if( iLength <= 0 ) return QkString();
	return QkString(m_pstr + iPos, iLength);
}

QkString QkString::Right(int iLength) const
{
	int iPos = GetLength() - iLength;
	if( iPos < 0 ) {
		iPos = 0;
		iLength = GetLength();
	}
	return QkString(m_pstr + iPos, iLength);
}

int QkString::Find(TCHAR ch, int iPos /*= 0*/) const
{
	ASSERT(iPos>=0 && iPos<=GetLength());
	if( iPos != 0 && (iPos < 0 || iPos >= GetLength()) ) return -1;
	LPCTSTR p = _tcschr(m_pstr + iPos, ch);
	if( p == NULL ) return -1;
	return (int)(p - m_pstr);
}

int QkString::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
{
	ASSERT(!::IsBadStringPtr(pstrSub,-1));
	ASSERT(iPos>=0 && iPos<=GetLength());
	if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;
	LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
	if( p == NULL ) return -1;
	return (int)(p - m_pstr);
}

int QkString::ReverseFind(TCHAR ch) const
{
	LPCTSTR p = _tcsrchr(m_pstr, ch);
	if( p == NULL ) return -1;
	return (int)(p - m_pstr);
}

int QkString::ReverseFind(LPCTSTR pstrSub, int iPos /*= 0*/) const
{
	ASSERT(!::IsBadStringPtr(pstrSub,-1));
	ASSERT(iPos>=0 && iPos<=GetLength());
	if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;

	int subLength = _tcslen(pstrSub);
	int startPos = iPos == 0 ? GetLength() - subLength : iPos - subLength;

	while (startPos >= 0)
	{
		if (_tcsncmp(m_pstr + startPos, pstrSub, subLength) == 0)
			return startPos;

		startPos--;
	}

	return -1;
}

int QkString::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
{
	//QkString sTemp;
	//int nCount = 0;
	//int iPos = Find(pstrFrom);
	//if( iPos < 0 ) return 0;
	//int cchFrom = (int) _tcslen(pstrFrom);
	//int cchTo = (int) _tcslen(pstrTo);
	//while( iPos >= 0 ) {
	//	sTemp = Left(iPos);
	//	sTemp += pstrTo;
	//	sTemp += Mid(iPos + cchFrom);
	//	Assign(sTemp);
	//	iPos = Find(pstrFrom, iPos + cchTo);
	//	nCount++;
	//}
	//return nCount;
	QkString sTemp;
	int last_find_start = 0;
	int nCount = 0;
	int iPos = Find(pstrFrom);
	if( iPos < 0 ) return 0;
	int cchFrom = (int) _tcslen(pstrFrom);
	int cchTo = (int) _tcslen(pstrTo);
	while( iPos >= 0 ) {
		sTemp.Append(m_pstr+last_find_start, iPos-last_find_start);
		sTemp.Append(pstrTo, cchTo);
		iPos = Find(pstrFrom, last_find_start = iPos + cchFrom);
		nCount++;
	}
	if (nCount)
	{
		sTemp.Append(m_pstr+last_find_start, iPos-last_find_start);
		Assign(sTemp);
	}
	return nCount;
}

void QkString::Trim()
{
	int st = 0;
	int ed = GetLength();
	while (st < ed && m_pstr[st] <= L' ') st++;
	// todo skip other empty chars
	while (st < ed && m_pstr[ed - 1] <= L' ') ed--;
	// todo skip other empty chars
	if (st > 0 || ed < GetLength())
	{
		//Assign(Mid(st, len-st));
		if (st)
		{
			wmemmove(m_pstr, m_pstr+st, ed-st);
		}
		m_pstr[ed-st] = 0;
		_size = ed-st;
	}
}

bool QkString::StartWith(const QkString & other, bool ignoreCase, int offset) const
{
	int pos = offset;
	int pos1 = 0;
	int len = GetLength();
	int len1 = other.GetLength();
	if (pos==-1) pos=len-len1; // end with
	if (pos < 0 || pos+len1 > len) return false;
	LPCTSTR data = GetData();
	LPCTSTR data1 = other.GetData();
	if (ignoreCase) 
	{
		while (--len1 >= 0) 
			if (_totlower(data[pos++]) != _totlower(data1[pos1++])) 
				return false;
	}
	else while (--len1 >= 0)
		if (data[pos++] != data1[pos1++])
			return false;
	return true;
}

bool QkString::EndWith(const QkString & other, bool ignoreCase) const
{
	return StartWith(other, ignoreCase, -1);
}

void QkString::Split(const QkString & delim, std::vector<QkString> & ret)
{
	int last = 0;  
	int index=Find(delim, last);  
	while (index!=-1)  
	{  
		ret.push_back(Mid(last,index-last));
		index=Find(delim, last=index+delim.GetLength());  
	}  
	if (GetLength()>last+1)  
	{  
		ret.push_back(Mid(last,GetLength()-last));  
	}  
}

int QkString::Format(LPCTSTR pstrFormat, ...)
{
	int nRet;
	va_list Args;

	va_start(Args, pstrFormat);
	nRet = InnerFormat(pstrFormat, Args);
	va_end(Args);

	return nRet;
}

int QkString::SmallFormat(LPCTSTR pstrFormat, ...)
{
	QkString sFormat = pstrFormat;
	TCHAR szBuffer[64] = { 0 };
	va_list argList;
	va_start(argList, pstrFormat);
	int iRet = ::_vsntprintf(szBuffer, sizeof(szBuffer), sFormat, argList);
	va_end(argList);
	Assign(szBuffer);
	return iRet;
}

int QkString::InnerFormat(LPCTSTR pstrFormat, va_list Args)
{
#if _MSC_VER <= 1400
	TCHAR *szBuffer = NULL;
	int size = 512, nLen, counts;
	szBuffer = (TCHAR*)malloc(size);
	ZeroMemory(szBuffer, size);
	while (TRUE){
		counts = size / sizeof(TCHAR);
		nLen = _vsntprintf (szBuffer, counts, pstrFormat, Args);
		if (nLen != -1 && nLen < counts){
			break;
		}
		if (nLen == -1){
			size *= 2;
		}else{
			size += 1 * sizeof(TCHAR);
		}

		if ((szBuffer = (TCHAR*)realloc(szBuffer, size)) != NULL){
			ZeroMemory(szBuffer, size);
		}else{
			break;
		}
	}

	Assign(szBuffer);
	free(szBuffer);
	return nLen;
#else
	int nLen, totalLen;
	TCHAR *szBuffer;
	nLen = _vsntprintf(NULL, 0, pstrFormat, Args);
	totalLen = (nLen + 1)*sizeof(TCHAR);
	szBuffer = (TCHAR*)malloc(totalLen);
	ZeroMemory(szBuffer, totalLen);
	nLen = _vsntprintf(szBuffer, nLen + 1, pstrFormat, Args);
	Assign(szBuffer);
	free(szBuffer);
	return nLen;

#endif
}

/////////////////////////////////////////////////////////////////////////////
//
//

static UINT HashKey(LPCTSTR Key)
{
	UINT i = 0;
	SIZE_T len = _tcslen(Key);
	while( len-- > 0 ) i = (i << 5) + i + Key[len];
	return i;
}

static UINT HashKey(const QkString& Key)
{
	UINT i = 0;
	SIZE_T len = Key.GetLength();
	LPCTSTR KeyData = Key;
	while( len-- > 0 ) i = (i << 5) + i + KeyData[len];
	return i;
};