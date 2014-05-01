#include "stdafx.h"
#include "RegistryEd.h"

CRegistryEd::CRegistryEd()
  {
      m_hKeyPath = NULL;
      m_hKeyOpen = NULL;
  }

CRegistryEd::~CRegistryEd()
  {
      CloseKey();
  }

LONG CRegistryEd::SetPathKey(LPCTSTR lpszPath,
                             HKEY hKey)
  {
      ULONG ulResult = 0;

      m_strPath = lpszPath;
      m_hKeyPath = hKey;

      return RegCreateKeyEx(m_hKeyPath,
                            m_strPath,
                            0,_T(""),0,KEY_ALL_ACCESS,
                            NULL,&m_hKeyOpen,&ulResult);
  }

LONG CRegistryEd::SetRegValue(LPCTSTR lpszKey,
                              DWORD dwValue)
  {
      return RegSetValueEx(m_hKeyOpen,
                           lpszKey,0,
                           REG_DWORD,
                           (LPBYTE)&dwValue,
                           sizeof(DWORD));
  }

LONG CRegistryEd::SetRegStr(LPCTSTR lpszValue,
                            LPCTSTR lpszKey,
                            unsigned sizeOfBuffer)
  {
      return RegSetValueEx(m_hKeyOpen,
                           lpszKey,0,
                           REG_SZ,
                           (LPBYTE)lpszValue,
                           sizeOfBuffer);
  }

LONG CRegistryEd::GetRegValue(LPCTSTR lpszKey,
                              DWORD &dwValue)
  {
      ULONG ulDataType,ulDataSize;

      ulDataSize = sizeof(DWORD);
      return RegQueryValueEx(m_hKeyOpen,
                             lpszKey,
                             NULL,
                             &ulDataType,
                             (LPBYTE)&dwValue,
                             &ulDataSize);
  }

LONG CRegistryEd::GetRegStr(LPCTSTR lpszKey,
                            CString &cstrValue)
  {
      TCHAR cBuffer[1024];
      ULONG ulDataType,ulDataSize;
      LONG lReturn;

      ulDataSize = 1024;
      lReturn = RegQueryValueEx(m_hKeyOpen,
                                lpszKey,
                                NULL,
                                &ulDataType,
                                (LPBYTE)cBuffer,
                                &ulDataSize);
      cstrValue = cBuffer;
      return lReturn;
  }

void CRegistryEd::CloseKey()
  {
      RegCloseKey(m_hKeyOpen);
  }
