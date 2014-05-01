#include "stdafx.h"
#include "ExBionicWindow.h"

CExBionicWindow::CExBionicWindow()
  {
      m_rectExClass.left = 0;
      m_rectExClass.top = 0;
      m_rectExClass.right = 0;
      m_rectExClass.bottom = 0;

      m_rcRecall.left = 0;
      m_rcRecall.top = 0;
      m_rcRecall.right = 0;
      m_rcRecall.bottom = 0;
  }

CExBionicWindow::~CExBionicWindow()
  {
      TRACE("DESTRUCTOR: CExBionicWindow\n");

      if(FALSE == m_bIsVisible)
          SetState(SW_SHOW);
  }

void CExBionicWindow::SetHandle(HWND hWnd)
  {
      m_hWinHwnd = hWnd;

      ::GetWindowRect(m_hWinHwnd,&m_rectExClass);
      ::CopyMemory(&m_rcRecall,&m_rectExClass,sizeof(RECT));
  }

BOOL CExBionicWindow::SetState(int nCmdShow)
  {
      TRACE("ExBionicWindow::SetState(int)\n");

      BOOL bSuccess = TRUE;

          //As long as we are not hiding this ex. window, make sure we are visible before we do max, min, or rest.
      if(SW_HIDE != nCmdShow &&
         !m_bIsVisible)
      {
          EnableWindow(m_hWinHwnd,TRUE);
          ::SetWindowPos(m_hWinHwnd,0,
                         m_rectExClass.left,
                         m_rectExClass.top,
                         m_rectExClass.right,
                         m_rectExClass.bottom,
                         SWP_HIDEWINDOW | SWP_NOSIZE |
                         SWP_NOZORDER | SWP_NOACTIVATE);

          ::SetWindowPos(m_hWinHwnd,0,0,0,0,0,
                         SWP_SHOWWINDOW | SWP_NOSIZE |
                         SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

          m_numOfHidden--;
          m_bIsVisible = TRUE;

          if(m_bForeground)
          {
              TRACE("Setting foreground window.\n");
              SetForegroundWindow(m_hWinHwnd);
              m_bForeground = FALSE;
          }
              //If we only want visibility we are done here.
          if(SW_SHOW == nCmdShow)
              return bSuccess;
          
      }

          //We want to hide this ex. window.
      if(nCmdShow == SW_HIDE &&
         m_bIsVisible == TRUE)
      {
          GetWindowRect(m_hWinHwnd,&m_rectExClass);
          m_rectExClass.right = m_rectExClass.right-m_rectExClass.left;
          m_rectExClass.bottom = m_rectExClass.bottom-m_rectExClass.top;

          MoveWindow(m_hWinHwnd,
                     3000,3000,
                     m_rectExClass.right,
                     m_rectExClass.bottom,
                     TRUE);

          EnableWindow(m_hWinHwnd,FALSE);
          m_bIsVisible = FALSE;

          m_numOfHidden++;

//Explorer window taskbar button removal.
          HWND hTray, hReBar32, hMSTask;

          hTray = FindWindow(_T("Shell_TrayWnd"),NULL);
          hReBar32 = FindWindowEx(hTray,NULL,_T("ReBarWindow32"),NULL);
          hMSTask = FindWindowEx(hReBar32,NULL,_T("MSTaskSwWClass"),NULL);
          PostMessage(hMSTask,RegisterWindowMessage(_T("SHELLHOOK")),(WPARAM)2,(LPARAM)m_hWinHwnd);
      }
      else //Else we just want to max, min or rest. so call base class to handle it.
          bSuccess = CBionicWindow::SetState(nCmdShow);

      return bSuccess;
  }

void CExBionicWindow::SetMemory()
  {
          //If we are dealing with a hidden EXPLORER class, we can't just save its
          //current position because its far off screen, so just copy over its visible
          //'on screen' position from ex. rect.

      if(!m_bIsVisible)
          CopyMemory(&m_rcRecall,&m_rectExClass,sizeof(RECT));
      else
      {
          ::GetWindowRect(m_hWinHwnd,&m_rcRecall);
          m_rcRecall.right = m_rcRecall.right - m_rcRecall.left;
          m_rcRecall.bottom = m_rcRecall.bottom - m_rcRecall.top;
      }
  }
