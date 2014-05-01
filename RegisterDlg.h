/*
  Register dialog box
*/
#ifndef _REGISTERDLG_H_
#define _REGISTERDLG_H_
#include <afxwin.h>
#include <afxcmn.h>
#include "resource.h"

class CRegisterDlg : public CDialog
  {
    public:

      enum { ID = IDD_REGISTER };
      CRegisterDlg(CWnd* pParent = NULL);

      BOOL OnInitDialog();
      BOOL InstallKeyCode(LPCTSTR,LPCTSTR);

      afx_msg void OnOK();
  };
#endif
