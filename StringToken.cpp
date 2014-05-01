#include "stdafx.h"
#include "StringToken.h"

void CStringToken::TokenizeStr(const CString &s,
                               CList<CString> &lst,
                               LPCTSTR lpszToken)
  {
      int len = s.GetLength();
      int index = 0;
      int offset = 0;

      for(int n = 0;
          n<len && index>-1;
          n++)
      {
          index = s.Find(lpszToken,n);
          offset = (index>-1) ? index-n : len-n;
          
          lst.AddTail(s.Mid(n,offset));
          n = index;
      }
  }
