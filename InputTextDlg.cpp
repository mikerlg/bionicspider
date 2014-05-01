#include "stdafx.h"
#include "InputTextDlg.h"

CInputTextDlg::CInputTextDlg(CWnd *pParent)
                             : CDialog(CInputTextDlg::ID,pParent)
  {
      m_pBuffer = 0;
      m_lpszTitle = 0;
      m_lpszDefaultText = 0;
  }

BOOL CInputTextDlg::OnInitDialog()
  {
      CDialog::OnInitDialog();

      CEdit *pEdit=(CEdit*)GetDlgItem(IDC_EDIT_INPUT);
      CStatic *pStatic=(CStatic*)GetDlgItem(IDC_STATIC_EDIT_TEXT);

      ASSERT(NULL!=pEdit);
      ASSERT(NULL!=pStatic);

//    SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ROOT_GROUP),TRUE);
      SetWindowText(m_lpszTitle);

      if(m_lpszDisplayText)
          pStatic->SetWindowText(m_lpszDisplayText);

      if(m_lpszDefaultText)
      {
          pEdit->SetWindowText(m_lpszDefaultText);
          pEdit->SetSel(0,-1);
      }
      GetDlgItem(IDC_EDIT_INPUT)->SetFocus();

      return 0;
  }

void CInputTextDlg::SetBuffer(CString *pCstr)
  {
      m_pBuffer = pCstr;
  }

void CInputTextDlg::SetCaption(LPCTSTR lpszCap,
                               LPCTSTR lpszDefaultText,
                               LPCTSTR lpszDisplayText,
                               CString *CStrBuffer)
  {
      m_lpszTitle = lpszCap;
      m_lpszDefaultText = lpszDefaultText;
      m_lpszDisplayText = lpszDisplayText;
      if(CStrBuffer)
          m_pBuffer = CStrBuffer;
  }

BEGIN_MESSAGE_MAP(CInputTextDlg,CDialog)
END_MESSAGE_MAP()

afx_msg void CInputTextDlg::OnOK()
  {
      if(m_pBuffer)
      {
          CEdit *pEdit=(CEdit*)GetDlgItem(IDC_EDIT_INPUT);
          CString str;
          pEdit->GetWindowText(str);
          *m_pBuffer = str;
      }
      CDialog::OnOK();
  }

