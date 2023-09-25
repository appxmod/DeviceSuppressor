
#pragma once
//#include "pch.h"

class QkString
{
public:
	enum { MAX_LOCAL_STRING_LEN = 15 };

	QkString();
	QkString(const TCHAR ch);
	QkString(const CHAR* ch);
	QkString(const QkString& src);
	QkString(LPCTSTR lpsz, int nLen = -1);
	~QkString();

	static QkString & EmptyInstance(LPCTSTR lpstrId=0)
	{
		if (!lpstrId)
		{
			if (_EmptyInstance.GetLength()>0)
			{
				// 放空 ： 释放栈区数据，保存类结构中的本地数据。
				_EmptyInstance.Empty();
			}
		}
		else
		{
			_EmptyInstance = lpstrId;
		}
		return _EmptyInstance;
	};
	void Empty();
	size_t GetLength() const;
	bool IsEmpty() const;
	TCHAR GetAt(int nIndex) const;

	bool EnsureCapacity(size_t newSz, bool release=false);
	void AsBuffer(bool isBuffer=true){
		_isBuffer = isBuffer;
	}
	size_t Capacity(){
		return m_pstr==m_szBuffer?MAX_LOCAL_STRING_LEN:_capacity;
	};
	size_t RecalcSize();
	void ReFit();

	void Prepend(QkString & other) {
		Prepend(other, other.GetLength());
	};
	void Prepend(LPCTSTR pstr, int nLength = -1);
	void Append(QkString & other) {
		Append(other, other.GetLength());
	};
	void Append(LPCTSTR pstr, int nLength = -1);
	void Assign(QkString & other) {
		Assign(other, other.GetLength());
	};
	void Assign(LPCTSTR pstr, int nLength = -1);
	//QkString& AssignThenReturn(LPCTSTR pstr, int nLength = -1) {
	//	Assign(pstr, nLength);
	//	return *this;
	//};
	LPCTSTR GetData() const;
	LPCSTR GetData(std::string & buffer, int offset=0, int length=-1) const;
	static LPCSTR GetData(std::string & buffer, LPCWSTR pStr);

	void SetAt(int nIndex, TCHAR ch);
	operator LPCTSTR() const;

	TCHAR operator[] (int nIndex) const;
	const QkString& operator=(const QkString& src);
	const QkString& operator=(const TCHAR ch);
	const QkString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
	const QkString& operator=(LPCSTR lpStr);
	const QkString& operator+=(LPCSTR lpStr);
#else
	const QkString& operator=(LPCWSTR lpwStr);
	const QkString& operator+=(LPCWSTR lpwStr);
#endif
	QkString operator+(const QkString& src) const;
	QkString operator+(LPCTSTR pstr) const;
	const QkString& operator+=(const QkString& src);
	const QkString& operator+=(LPCTSTR pstr);
	const QkString& operator+=(const TCHAR ch);

	bool operator == (LPCTSTR str) const;
	bool operator != (LPCTSTR str) const;
	bool operator == (const QkString & str) const;
	bool operator != (const QkString & str) const;
	bool operator <= (LPCTSTR str) const;
	bool operator <  (LPCTSTR str) const;
	bool operator >= (LPCTSTR str) const;
	bool operator >  (LPCTSTR str) const;

	int Compare(LPCTSTR pstr) const;
	int CompareNoCase(LPCTSTR pstr) const;

	void MakeUpper();
	void MakeLower();

	QkString Left(int nLength) const;
	QkString Mid(int iPos, int nLength = -1) const;
	void MidFast(int iPos, int nLength = -1);
	//QkString * MidFake(int iPos, int nLength = -1);
	QkString Right(int nLength) const;

	int Find(TCHAR ch, int iPos = 0) const;
	int Find(LPCTSTR pstr, int iPos = 0) const;
	int ReverseFind(TCHAR ch) const;
	int ReverseFind(LPCTSTR pstr, int iPos = 0) const;
	int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);
	void Trim();
	void Split(const QkString & delim, std::vector<QkString> & ret);

	bool StartWith(const QkString & prefix, bool ignoreCase=false, int toffset=0) const;
	bool EndWith(const QkString & other, bool ignoreCase=false) const;

	int __cdecl Format(LPCTSTR pstrFormat, ...);
	int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

protected:
	int __cdecl InnerFormat(LPCTSTR pstrFormat, va_list Args);

protected:
	LPTSTR m_pstr;
	TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
	size_t _size;
	size_t _capacity;
	bool _isBuffer;
	static QkString _EmptyInstance;
};

static std::vector<QkString> StrSplit(QkString text, QkString sp)
{
	std::vector<QkString> vResults;
	int pos = text.Find(sp, 0);
	while (pos >= 0)
	{
		QkString t = text.Left(pos);
		vResults.push_back(t);
		text = text.Right(text.GetLength() - pos - sp.GetLength());
		pos = text.Find(sp);
	}
	vResults.push_back(text);
	return vResults;
}