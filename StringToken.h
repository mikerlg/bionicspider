#ifndef _STRINGTOKEN_H_
#define _STRINGTOKEN_H_
#include <afxtempl.h>

class CStringToken
  {
    public:
      static void TokenizeStr(const CString&,CList<CString>&,LPCTSTR);
  };
#endif
