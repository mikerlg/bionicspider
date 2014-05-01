// bionicspider.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "bionicspider.h"
#include "bionicspiderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBionicSpiderApp

BEGIN_MESSAGE_MAP(CBionicSpiderApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBionicSpiderApp construction

CBionicSpiderApp::CBionicSpiderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBionicSpiderApp object

CBionicSpiderApp theApp;


// CBionicSpiderApp initialization

BOOL CBionicSpiderApp::InitInstance()
  {
          // InitCommonControls() is required on Windows XP if an application
          // manifest specifies use of ComCtl32.dll version 6 or later to enable
          // visual styles.  Otherwise, any window creation will fail.
      InitCommonControls();

      CWinApp::InitInstance();

          // Standard initialization
          // If you are not using these features and wish to reduce the size
          // of your final executable, you should remove from the following
          // the specific initialization routines you do not need
          // Change the registry key under which our settings are stored
          // TODO: You should modify this string to be something appropriate
          // such as the name of your company or organization
      SetRegistryKey(_T("Local AppWizard-Generated Applications"));

          //Set up a invisible parent window to remove button from taskbar.
      if(NULL == m_wndOwner.m_hWnd)
      {
          LPCTSTR pstrOwnerClass = AfxRegisterWndClass(0);

          if(!m_wndOwner.CreateEx(0,pstrOwnerClass, _T(""),
                                  WS_POPUP,CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  NULL,0))
              return FALSE;
      }
      m_wndOwner.SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),TRUE);

      CBionicSpiderDlg dlg(&m_wndOwner);

      m_pMainWnd = &dlg;
      dlg.DoModal();

          // Destroy the invisible window
      if (NULL!=m_wndOwner.m_hWnd)
          m_wndOwner.DestroyWindow();

          // Since the dialog has been closed, return FALSE so that we exit the
          //  application, rather than start the application's message pump.
      return FALSE;
  }
