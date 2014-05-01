#include "stdafx.h"
#include "UpdateDlg.h"
#include "bionicspiderDlg.h"

CUpdateDlg::CUpdateDlg(CWnd *pParent) : CDialog(CUpdateDlg::ID,pParent) {}

BOOL CUpdateDlg::OnInitDialog()
  {
      CDialog::OnInitDialog();

      CString buffer;

      buffer.LoadString(IDS_APP_NAME);
      SetWindowText(buffer);

      buffer.LoadString(IDS_WEB_URL_UPDATE);
      m_webaddy.SetBackColor(::GetSysColor(COLOR_3DFACE));
      m_webaddy.SetWindowText(buffer);
      m_webaddy.SetURL(buffer);

      buffer.LoadString(IDS_UPDATECHECK_CHECKING);
      SetUpdateText(buffer);

      ((CStatic*)GetDlgItem(IDOK))->EnableWindow(FALSE);
      AfxBeginThread(CheckUpdateThread,this);

      return TRUE;
  }

void CUpdateDlg::DoDataExchange(CDataExchange *pDX)
  {
      CDialog::DoDataExchange(pDX);
      DDX_Control(pDX,IDC_UPDATE_ADDRESS,m_webaddy);
  }

BOOL CUpdateDlg::PreTranslateMessage(MSG *pMsg)
  {
      if(pMsg->message == WM_KEYDOWN)
      {
          if(pMsg->wParam == VK_RETURN ||
             pMsg->wParam == VK_ESCAPE)
          {
              ::TranslateMessage(pMsg);
              ::DispatchMessage(pMsg);
              return TRUE;                    
          }
      }
      return CDialog::PreTranslateMessage(pMsg);
  }

void CUpdateDlg::SetUpdateText(LPCTSTR lpszMsg)
  {
      ((CStatic*)GetDlgItem(IDC_UPDATE_MSG))->SetWindowText(lpszMsg);
  }

UINT CUpdateDlg::CheckUpdateThread(LPVOID pdlg)
  {
      CUpdateDlg *pDlg = (CUpdateDlg*)pdlg;
      ASSERT(pDlg);

      CString buffer;
      CString ver;

      buffer.LoadString(IDS_UPDATE_SCRIPT_URL);
      ver.LoadString(IDS_BIONICSPIDER_VERSION_NUM);
      buffer += ver;

      switch(pDlg->UpdateCheck(buffer))
      {
          case CUpdateDlg::BS_UPDATECHECK_NOUPDATE:
              buffer.LoadString(IDS_UPDATECHECK_FALSE);
              break;
              
          case CUpdateDlg::BS_UPDATECHECK_UPDATE:
              buffer.LoadString(IDS_UPDATECHECK_TRUE);
              break;

          default:
              buffer.LoadString(IDS_UPDATECHECK_NONET);
      }
      pDlg->SetUpdateText(buffer);
      ((CStatic*)pDlg->GetDlgItem(IDOK))->EnableWindow(TRUE);

      return 0;
  }

int CUpdateDlg::UpdateCheck(LPCTSTR lpszURL)
  {
      CInternetSession netSession(NULL);
      CInternetFile *pFile = NULL;
      CString htmlBuffer;

      int update = BS_UPDATECHECK_NONET;
      try
      {
          pFile = (CInternetFile*)netSession.OpenURL(lpszURL,1,
                                                     INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_DONT_CACHE,
                                                     NULL,0);
      }
      catch(CInternetException *pEx)
      {
          pFile = NULL;
          pEx->Delete();
      }
      if(pFile)
      {
          if(pFile->ReadString(htmlBuffer))
              update = (htmlBuffer == _T("1")) ? BS_UPDATECHECK_UPDATE : BS_UPDATECHECK_NOUPDATE;

          pFile->Close();
          delete pFile;
          pFile = NULL;
      }
      netSession.Close();

      return update;
  }
