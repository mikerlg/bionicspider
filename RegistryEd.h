#include <afx.h>

class CRegistryEd
  {
    protected:
      HKEY m_hKeyPath,m_hKeyOpen;
      CString m_strPath;

    public:
      CRegistryEd();
      ~CRegistryEd();

      void CloseKey();
      LONG SetPathKey(LPCTSTR,HKEY = HKEY_CURRENT_USER);
      LONG SetRegValue(LPCTSTR,DWORD);
      LONG SetRegStr(LPCTSTR,LPCTSTR,unsigned);
      LONG GetRegValue(LPCTSTR,DWORD&);
      LONG GetRegStr(LPCTSTR,CString&);
  };

