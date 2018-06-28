#pragma once
#define ATLTRACE            __noop
#define ATLTRACE2           __noop

#include <atl.mini/atldef.h>
#include <WinReg.h>

class CRegKey
{
public:
    CRegKey() throw();
    CRegKey( CRegKey& key ) throw();
    explicit CRegKey(HKEY hKey) throw();
    ~CRegKey() throw();

    CRegKey& operator=( CRegKey& key ) throw();

    // Attributes
public:
    operator HKEY() const throw();
    HKEY m_hKey;
    REGSAM m_samWOW64;

    // Operations
public:
    LONG SetValue(LPCTSTR pszValueName, DWORD dwType, const void* pValue, ULONG nBytes) throw();
    LONG SetGUIDValue(LPCTSTR pszValueName, REFGUID guidValue) throw();
    LONG SetBinaryValue(LPCTSTR pszValueName, const void* pValue, ULONG nBytes) throw();
    LONG SetDWORDValue(LPCTSTR pszValueName, DWORD dwValue) throw();
    LONG SetQWORDValue(LPCTSTR pszValueName, ULONGLONG qwValue) throw();
    LONG SetStringValue(_In_opt_z_ LPCTSTR pszValueName, _In_opt_z_ LPCTSTR pszValue, _In_ DWORD dwType = REG_SZ) throw();
    LONG SetMultiStringValue(LPCTSTR pszValueName, LPCTSTR pszValue) throw();

    LONG QueryValue(_In_opt_z_ LPCTSTR pszValueName, DWORD* pdwType, void* pData, ULONG* pnBytes) throw();
    LONG QueryGUIDValue(_In_opt_z_ LPCTSTR pszValueName, GUID& guidValue) throw();
    LONG QueryBinaryValue(_In_opt_z_ LPCTSTR pszValueName, void* pValue, ULONG* pnBytes) throw();
    LONG QueryDWORDValue(_In_opt_z_ LPCTSTR pszValueName, DWORD& dwValue) throw();
    LONG QueryQWORDValue(_In_opt_z_ LPCTSTR pszValueName, ULONGLONG& qwValue) throw();
    LONG QueryStringValue(_In_opt_z_ LPCTSTR pszValueName, _Out_opt_z_cap_post_count_(*pnChars, *pnChars)  LPTSTR pszValue, _Inout_ ULONG* pnChars) throw();
    LONG QueryMultiStringValue(_In_opt_z_ LPCTSTR pszValueName, _Out_opt_z_cap_post_count_(*pnChars, *pnChars)  LPTSTR pszValue, _Inout_ ULONG* pnChars) throw();

    // Get the key's security attributes.
    LONG GetKeySecurity(SECURITY_INFORMATION si, PSECURITY_DESCRIPTOR psd, LPDWORD pnBytes) throw();
    // Set the key's security attributes.
    LONG SetKeySecurity(SECURITY_INFORMATION si, PSECURITY_DESCRIPTOR psd) throw();

    LONG SetKeyValue(LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL) throw();
    static LONG WINAPI SetValue(HKEY hKeyParent, LPCTSTR lpszKeyName,
        LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL);

    // Create a new registry key (or open an existing one).
    LONG Create(_In_ HKEY hKeyParent, _In_z_ LPCTSTR lpszKeyName,
        _In_opt_z_ LPTSTR lpszClass = REG_NONE, _In_ DWORD dwOptions = REG_OPTION_NON_VOLATILE,
        _In_ REGSAM samDesired = KEY_READ | KEY_WRITE,
        _In_opt_ LPSECURITY_ATTRIBUTES lpSecAttr = NULL,
        _Out_opt_ LPDWORD lpdwDisposition = NULL) throw();
    // Open an existing registry key.
    LONG Open(HKEY hKeyParent, LPCTSTR lpszKeyName,
        REGSAM samDesired = KEY_READ | KEY_WRITE) throw();
    // Close the registry key.
    LONG Close() throw();
    // Flush the key's data to disk.
    LONG Flush() throw();

    // Detach the CRegKey object from its HKEY.  Releases ownership.
    HKEY Detach() throw();
    // Attach the CRegKey object to an existing HKEY.  Takes ownership.
    void Attach(HKEY hKey) throw();

    // Enumerate the subkeys of the key.
    LONG EnumKey(_In_ DWORD iIndex, _Out_z_cap_post_count_(*pnNameLength, *pnNameLength) LPTSTR pszName, _Inout_ LPDWORD pnNameLength, _Out_opt_ FILETIME* pftLastWriteTime = NULL) throw();
    LONG NotifyChangeKeyValue(BOOL bWatchSubtree, DWORD dwNotifyFilter, HANDLE hEvent, BOOL bAsync = TRUE) throw();

    LONG DeleteSubKey(LPCTSTR lpszSubKey) throw();
    LONG RecurseDeleteKey(LPCTSTR lpszKey) throw();
    LONG DeleteValue(LPCTSTR lpszValue) throw();
};

inline CRegKey::CRegKey() throw() : 
m_hKey( NULL ), m_samWOW64(0)
{
}

inline CRegKey::CRegKey( CRegKey& key ) throw() :
m_hKey( NULL ), m_samWOW64(key.m_samWOW64)
{
    Attach( key.Detach() );
}

inline CRegKey::CRegKey(HKEY hKey) throw() :
m_hKey(hKey), m_samWOW64(0)
{
}

inline CRegKey::~CRegKey() throw()
{Close();}

inline CRegKey& CRegKey::operator=( CRegKey& key ) throw()
{
    if(m_hKey!=key.m_hKey)
    {
        Close();
        Attach( key.Detach() );
        m_samWOW64 = key.m_samWOW64;
    }
    return( *this );
}

inline CRegKey::operator HKEY() const throw()
{return m_hKey;}

inline HKEY CRegKey::Detach() throw()
{
    HKEY hKey = m_hKey;
    m_hKey = NULL;
    m_samWOW64 = 0;
    return hKey;
}

inline void CRegKey::Attach(HKEY hKey) throw()
{
    ATLASSUME(m_hKey == NULL);
    m_hKey = hKey;
    m_samWOW64 = 0;
}

inline LONG CRegKey::DeleteSubKey(LPCTSTR lpszSubKey) throw()
{
    ATLASSUME(m_hKey != NULL);

#if WINVER >= 0x0501
    typedef LSTATUS (WINAPI * PFNRegDeleteKeyEx)(HKEY, LPCTSTR, REGSAM, DWORD);
    static bool bInitialized = false;
    static PFNRegDeleteKeyEx pfnRegDeleteKeyEx = NULL;

    if (!bInitialized)
    {
        HMODULE hAdvapi32 = GetModuleHandle(_T("Advapi32.dll"));
        if (hAdvapi32 != NULL)
        {
#ifdef _UNICODE
            pfnRegDeleteKeyEx = (PFNRegDeleteKeyEx)GetProcAddress(hAdvapi32, "RegDeleteKeyExW");
#else
            pfnRegDeleteKeyEx = (PFNRegDeleteKeyEx)GetProcAddress(hAdvapi32, "RegDeleteKeyExA");
#endif	// _UNICODE
        }
        bInitialized = true;
    }

    if (pfnRegDeleteKeyEx != NULL)
    {
        return pfnRegDeleteKeyEx(m_hKey, lpszSubKey, m_samWOW64, 0);
    }

#endif	// WINVER

    return RegDeleteKey(m_hKey, lpszSubKey);
}

inline LONG CRegKey::DeleteValue(LPCTSTR lpszValue) throw()
{
    ATLASSUME(m_hKey != NULL);
    return RegDeleteValue(m_hKey, (LPTSTR)lpszValue);
}

inline LONG CRegKey::Close() throw()
{
    LONG lRes = ERROR_SUCCESS;
    if (m_hKey != NULL)
    {
        lRes = RegCloseKey(m_hKey);
        m_hKey = NULL;
    }
    m_samWOW64 = 0;
    return lRes;
}

inline LONG CRegKey::Flush() throw()
{
    ATLASSUME(m_hKey != NULL);

    return ::RegFlushKey(m_hKey);
}

inline LONG CRegKey::EnumKey(_In_ DWORD iIndex, _Out_z_cap_post_count_(*pnNameLength, *pnNameLength) LPTSTR pszName, _Inout_ LPDWORD pnNameLength, _Out_opt_ FILETIME* pftLastWriteTime) throw()
{
    FILETIME ftLastWriteTime;

    ATLASSUME(m_hKey != NULL);
    if (pftLastWriteTime == NULL)
    {
        pftLastWriteTime = &ftLastWriteTime;
    }

    return ::RegEnumKeyEx(m_hKey, iIndex, pszName, pnNameLength, NULL, NULL, NULL, pftLastWriteTime);
}

inline LONG CRegKey::NotifyChangeKeyValue(BOOL bWatchSubtree, DWORD dwNotifyFilter, HANDLE hEvent, BOOL bAsync) throw()
{
    ATLASSUME(m_hKey != NULL);
    ATLASSERT((hEvent != NULL) || !bAsync);

    return ::RegNotifyChangeKeyValue(m_hKey, bWatchSubtree, dwNotifyFilter, hEvent, bAsync);
}

inline LONG CRegKey::Create(_In_ HKEY hKeyParent, _In_z_ LPCTSTR lpszKeyName,
                            _In_opt_z_ LPTSTR lpszClass, _In_ DWORD dwOptions, _In_ REGSAM samDesired,
                            _In_opt_ LPSECURITY_ATTRIBUTES lpSecAttr, _Out_opt_ LPDWORD lpdwDisposition) throw()
{
    ATLASSERT(hKeyParent != NULL);
    DWORD dw;
    HKEY hKey = NULL;
    LONG lRes = RegCreateKeyEx(hKeyParent, lpszKeyName, 0,
        lpszClass, dwOptions, samDesired, lpSecAttr, &hKey, &dw);
    if (lpdwDisposition != NULL)
        *lpdwDisposition = dw;
    if (lRes == ERROR_SUCCESS)
    {
        lRes = Close();
        m_hKey = hKey;
#if WINVER >= 0x0501
        m_samWOW64 = samDesired & (KEY_WOW64_32KEY | KEY_WOW64_64KEY);
#endif
    }
    return lRes;
}

inline LONG CRegKey::Open(HKEY hKeyParent, LPCTSTR lpszKeyName, REGSAM samDesired) throw()
{
    ATLASSUME(hKeyParent != NULL);
    HKEY hKey = NULL;
    LONG lRes = RegOpenKeyEx(hKeyParent, lpszKeyName, 0, samDesired, &hKey);
    if (lRes == ERROR_SUCCESS)
    {
        lRes = Close();
        ATLASSERT(lRes == ERROR_SUCCESS);
        m_hKey = hKey;
#if WINVER >= 0x0501
        m_samWOW64 = samDesired & (KEY_WOW64_32KEY | KEY_WOW64_64KEY);
#endif
    }
    return lRes;
}


inline LONG CRegKey::QueryValue(LPCTSTR pszValueName, DWORD* pdwType, void* pData, ULONG* pnBytes) throw()
{
    ATLASSUME(m_hKey != NULL);

    return( ::RegQueryValueEx(m_hKey, pszValueName, NULL, pdwType, static_cast< LPBYTE >( pData ), pnBytes) );
}

inline LONG CRegKey::QueryDWORDValue(LPCTSTR pszValueName, DWORD& dwValue) throw()
{
    LONG lRes;
    ULONG nBytes;
    DWORD dwType;

    ATLASSUME(m_hKey != NULL);

    nBytes = sizeof(DWORD);
    lRes = ::RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(&dwValue),
        &nBytes);
    if (lRes != ERROR_SUCCESS)
        return lRes;
    if (dwType != REG_DWORD)
        return ERROR_INVALID_DATA;

    return ERROR_SUCCESS;
}
inline LONG CRegKey::QueryQWORDValue(LPCTSTR pszValueName, ULONGLONG& qwValue) throw()
{
    LONG lRes;
    ULONG nBytes;
    DWORD dwType;

    ATLASSUME(m_hKey != NULL);

    nBytes = sizeof(ULONGLONG);
    lRes = ::RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(&qwValue),
        &nBytes);
    if (lRes != ERROR_SUCCESS)
        return lRes;
    if (dwType != REG_QWORD)
        return ERROR_INVALID_DATA;

    return ERROR_SUCCESS;
}

inline LONG CRegKey::QueryBinaryValue(LPCTSTR pszValueName, void* pValue, ULONG* pnBytes) throw()
{
    LONG lRes;
    DWORD dwType;

    ATLASSERT(pnBytes != NULL);
    ATLASSUME(m_hKey != NULL);

    lRes = ::RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pValue),
        pnBytes);
    if (lRes != ERROR_SUCCESS)
        return lRes;
    if (dwType != REG_BINARY)
        return ERROR_INVALID_DATA;

    return ERROR_SUCCESS;
}

#pragma warning(push)
#pragma warning(disable: 6053)
/* prefast noise VSW 496818 */
inline LONG CRegKey::QueryStringValue(LPCTSTR pszValueName, LPTSTR pszValue, ULONG* pnChars) throw()
{
    LONG lRes;
    DWORD dwType;
    ULONG nBytes;

    ATLASSUME(m_hKey != NULL);
    ATLASSERT(pnChars != NULL);

    nBytes = (*pnChars)*sizeof(TCHAR);
    *pnChars = 0;
    lRes = ::RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pszValue),
        &nBytes);

    if (lRes != ERROR_SUCCESS)
    {
        return lRes;
    }

    if(dwType != REG_SZ && dwType != REG_EXPAND_SZ)
    {
        return ERROR_INVALID_DATA;
    }

    if (pszValue != NULL)
    {
        if(nBytes!=0)
        {
#pragma warning(suppress:6385) // suppress noisy code analysis warning due to annotation on RegQueryValueEx
            if ((nBytes % sizeof(TCHAR) != 0) || (pszValue[nBytes / sizeof(TCHAR) -1] != 0))
            {
                return ERROR_INVALID_DATA;
            }
        }
        else
        {
            pszValue[0]=_T('\0');
        }
    }

    *pnChars = nBytes/sizeof(TCHAR);

    return ERROR_SUCCESS;
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 6053)
/* prefast noise VSW 496818 */
inline LONG CRegKey::QueryMultiStringValue(LPCTSTR pszValueName, LPTSTR pszValue, ULONG* pnChars) throw()
{
    LONG lRes;
    DWORD dwType;
    ULONG nBytes;

    ATLASSUME(m_hKey != NULL);
    ATLASSERT(pnChars != NULL);

    if (pszValue != NULL && *pnChars < 2)
        return ERROR_INSUFFICIENT_BUFFER;

    nBytes = (*pnChars)*sizeof(TCHAR);
    *pnChars = 0;

    lRes = ::RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pszValue),
        &nBytes);
    if (lRes != ERROR_SUCCESS)
        return lRes;
    if (dwType != REG_MULTI_SZ)
        return ERROR_INVALID_DATA;
    if (pszValue != NULL && (nBytes % sizeof(TCHAR) != 0 || nBytes / sizeof(TCHAR) < 1 || pszValue[nBytes / sizeof(TCHAR) -1] != 0 || ((nBytes/sizeof(TCHAR))>1 && pszValue[nBytes / sizeof(TCHAR) - 2] != 0)))
        return ERROR_INVALID_DATA;

    *pnChars = nBytes/sizeof(TCHAR);

    return ERROR_SUCCESS;
}
#pragma warning(pop)

inline LONG CRegKey::QueryGUIDValue(LPCTSTR pszValueName, GUID& guidValue) throw()
{
    TCHAR szGUID[64];
    LONG lRes;
    ULONG nCount;
    HRESULT hr;

    ATLASSUME(m_hKey != NULL);

    guidValue = GUID_NULL;

    nCount = 64;
    lRes = QueryStringValue(pszValueName, szGUID, &nCount);

    if (lRes != ERROR_SUCCESS)
        return lRes;

    if(szGUID[0] != _T('{'))
        return ERROR_INVALID_DATA;

	SStringW strGuid = S_CT2W(szGUID);

	LPOLESTR oleGuid = strGuid.LockBuffer();
    hr = ::CLSIDFromString(oleGuid, &guidValue);
	strGuid.UnlockBuffer();
    if (FAILED(hr))
        return ERROR_INVALID_DATA;

    return ERROR_SUCCESS;
}

inline LONG WINAPI CRegKey::SetValue(HKEY hKeyParent, LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName)
{
    ATLASSERT(lpszValue != NULL);
    CRegKey key;
    LONG lRes = key.Create(hKeyParent, lpszKeyName);
    if (lRes == ERROR_SUCCESS)
        lRes = key.SetStringValue(lpszValueName, lpszValue);
    return lRes;
}

inline LONG CRegKey::SetKeyValue(LPCTSTR lpszKeyName, LPCTSTR lpszValue, LPCTSTR lpszValueName) throw()
{
    ATLASSERT(lpszValue != NULL);
    CRegKey key;
    LONG lRes = key.Create(m_hKey, lpszKeyName, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE | m_samWOW64);
    if (lRes == ERROR_SUCCESS)
        lRes = key.SetStringValue(lpszValueName, lpszValue);
    return lRes;
}


inline LONG CRegKey::SetValue(LPCTSTR pszValueName, DWORD dwType, const void* pValue, ULONG nBytes) throw()
{
    ATLASSUME(m_hKey != NULL);
    return ::RegSetValueEx(m_hKey, pszValueName, NULL, dwType, static_cast<const BYTE*>(pValue), nBytes);
}

inline LONG CRegKey::SetBinaryValue(LPCTSTR pszValueName, const void* pData, ULONG nBytes) throw()
{
    ATLASSUME(m_hKey != NULL);
    return ::RegSetValueEx(m_hKey, pszValueName, NULL, REG_BINARY, reinterpret_cast<const BYTE*>(pData), nBytes);
}

inline LONG CRegKey::SetDWORDValue(LPCTSTR pszValueName, DWORD dwValue) throw()
{
    ATLASSUME(m_hKey != NULL);
    return ::RegSetValueEx(m_hKey, pszValueName, NULL, REG_DWORD, reinterpret_cast<const BYTE*>(&dwValue), sizeof(DWORD));
}

inline LONG CRegKey::SetQWORDValue(LPCTSTR pszValueName, ULONGLONG qwValue) throw()
{
    ATLASSUME(m_hKey != NULL);
    return ::RegSetValueEx(m_hKey, pszValueName, NULL, REG_QWORD, reinterpret_cast<const BYTE*>(&qwValue), sizeof(ULONGLONG));
}

inline LONG CRegKey::SetStringValue(_In_opt_z_ LPCTSTR pszValueName, _In_opt_z_ LPCTSTR pszValue, _In_ DWORD dwType) throw()
{
    ATLASSUME(m_hKey != NULL);
    //ATLENSURE_RETURN_VAL(pszValue != NULL, ERROR_INVALID_DATA);
    ATLASSERT((dwType == REG_SZ) || (dwType == REG_EXPAND_SZ));

    return ::RegSetValueEx(m_hKey, pszValueName, NULL, dwType, reinterpret_cast<const BYTE*>(pszValue), (lstrlen(pszValue)+1)*sizeof(TCHAR));
}

inline LONG CRegKey::SetMultiStringValue(LPCTSTR pszValueName, LPCTSTR pszValue) throw()
{
    LPCTSTR pszTemp;
    ULONG nBytes;
    ULONG nLength;

    ATLASSUME(m_hKey != NULL);
    //ATLENSURE_RETURN_VAL(pszValue != NULL, ERROR_INVALID_DATA);

    // Find the total length (in bytes) of all of the strings, including the
    // terminating '\0' of each string, and the second '\0' that terminates
    // the list.
    nBytes = 0;
    pszTemp = pszValue;
    do
    {
        nLength = lstrlen(pszTemp)+1;
        pszTemp += nLength;
        nBytes += nLength*sizeof(TCHAR);
    } while (nLength != 1);

    return ::RegSetValueEx(m_hKey, pszValueName, NULL, REG_MULTI_SZ, reinterpret_cast<const BYTE*>(pszValue),
        nBytes);
}

inline LONG CRegKey::SetGUIDValue(LPCTSTR pszValueName, REFGUID guidValue) throw()
{
    OLECHAR szGUID[64];

    ATLASSUME(m_hKey != NULL);

    ::StringFromGUID2(guidValue, szGUID, 64);

	SStringT strGuid = S_CW2T(szGUID);
    return SetStringValue(pszValueName, strGuid);
}

inline LONG CRegKey::GetKeySecurity(SECURITY_INFORMATION si, PSECURITY_DESCRIPTOR psd, LPDWORD pnBytes) throw()
{
    ATLASSUME(m_hKey != NULL);
    ATLASSUME(pnBytes != NULL);

    return ::RegGetKeySecurity(m_hKey, si, psd, pnBytes);
}

inline LONG CRegKey::SetKeySecurity(SECURITY_INFORMATION si, PSECURITY_DESCRIPTOR psd) throw()
{
    ATLASSUME(m_hKey != NULL);
    ATLASSUME(psd != NULL);

    return ::RegSetKeySecurity(m_hKey, si, psd);
}

inline LONG CRegKey::RecurseDeleteKey(LPCTSTR lpszKey) throw()
{
    CRegKey key;
    LONG lRes = key.Open(m_hKey, lpszKey, KEY_READ | KEY_WRITE | m_samWOW64);
    if (lRes != ERROR_SUCCESS)
    {
        if (lRes != ERROR_FILE_NOT_FOUND && lRes != ERROR_PATH_NOT_FOUND)
        {
            ATLTRACE(_T("CRegKey::RecurseDeleteKey : Failed to Open Key %s(Error = %d)\n"), lpszKey, lRes);
        }
        return lRes;
    }
    FILETIME time;
    DWORD dwSize = 256;
    TCHAR szBuffer[256];
    while (RegEnumKeyEx(key.m_hKey, 0, szBuffer, &dwSize, NULL, NULL, NULL,
        &time)==ERROR_SUCCESS)
    {
        lRes = key.RecurseDeleteKey(szBuffer);
        if (lRes != ERROR_SUCCESS)
            return lRes;
        dwSize = 256;
    }
    key.Close();
    return DeleteSubKey(lpszKey);
}
