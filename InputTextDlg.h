/***
  InputTextDlg.h
  InputTextDlg.cpp

  This MFC CDialog class will display
  a text box so that the user may enter
  a new group folder name.
 ***/
#ifndef __CINPUTTEXTDLG_H__
#define __CINPUTTEXTDLG_H__
#include <afxwin.h>
#include <afxcmn.h>
#include "resource.h"

class CInputTextDlg : public CDialog
  {
    protected:
      CString *m_pBuffer;             //pointer to the "input" CString
      const TCHAR *m_lpszTitle;       //the title of this CDialog
      const TCHAR *m_lpszDefaultText; //the default highlighted text in the edit box
      const TCHAR *m_lpszDisplayText; //the display text above the edit box.

    public:
      enum { ID=IDD_EDITDLG };
      CInputTextDlg(CWnd *pParent=NULL);
      BOOL OnInitDialog();
      void SetBuffer(CString*);
      void SetCaption(LPCTSTR,LPCTSTR=0,LPCTSTR=0,CString *buffer=NULL);

      afx_msg void OnOK();
      DECLARE_MESSAGE_MAP()
  };
#endif





