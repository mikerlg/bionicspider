#include "stdafx.h"
#include "resource.h"
#include "BionicWindow.h"

int CBionicWindow::m_numOfWindows = 0;
int CBionicWindow::m_numOfHidden = 0;

UINT CBionicWindow::sm_idCount = 0;
WinTrayID CBionicWindow::sm_idList;

CBionicWindow::CBionicWindow()
  {
      m_hWinHwnd = NULL;
      m_hIcon = NULL;
      m_bIconDefault = FALSE;
      m_bTopMost = FALSE;
      m_bIsVisible = TRUE;
      m_bTray = FALSE;
      m_bPreserve = FALSE;
      m_bForeground = FALSE;

      m_nImageIndex = -1;

      m_rcRecall.left = 0;
      m_rcRecall.top = 0;
      m_rcRecall.right = 0;
      m_rcRecall.bottom = 0;

      m_numOfWindows++;

          //Valid ID list is empty, so just increm. count.
      if(sm_idList.IsEmpty())
      {
          ++sm_idCount;
          ASSERT(sm_idCount != 0); //If 0, then we have the same ID as main tray icon. :(

          m_uiTrayID = sm_idCount;
          TRACE("Tray ID: %d\n",m_uiTrayID);
      }
      else
      {
              //Get first available tray ID
          m_uiTrayID = sm_idList.GetAt(sm_idList.GetHeadPosition());
          sm_idList.RemoveHead(); //Remove this ID.
          TRACE("GETTING Tray ID from list: %d\n",m_uiTrayID);
      }
  }

CBionicWindow::~CBionicWindow()
  {
      TRACE("DESTRUCTOR: CBionicWindow.\n");
      CleanUp();
  }

void CBionicWindow::CleanUp()
  {
          //Add tray ID to valid ID list.
      TRACE("ADDING tray ID to list: %d\n",m_uiTrayID);
      sm_idList.AddHead(m_uiTrayID);

      m_numOfWindows--;

      if(!GetState())
          SetState(SW_SHOW);

      if(m_hIcon &&
         m_bIconDefault == FALSE)
      {
          TRACE("Destroying window icon.\n");
          DestroyIcon(m_hIcon);
      }
  }

HWND CBionicWindow::GetHandle()
  {
      return m_hWinHwnd;
  }

void CBionicWindow::SetHandle(HWND hWnd)
  {
      m_hWinHwnd = hWnd;

          //Get default recall postion.
      ::GetWindowRect(hWnd,&m_rcRecall);
  }

int CBionicWindow::GetImageIndex()
  {
      return m_nImageIndex;
  }

void CBionicWindow::SetImageIndex(int nIndex)
  {
      m_nImageIndex = nIndex;
  }

HICON CBionicWindow::RefreshIcon()
  {
          //If window is closed, return the default icon.
      if(!IsWindow(m_hWinHwnd))
          return AfxGetApp()->LoadIcon(IDR_MAINFRAME);

      if(m_hIcon)
          return m_hIcon;

      if((m_hIcon = CopyIcon((HICON)::SendMessage(m_hWinHwnd,WM_GETICON,ICON_BIG,0))))
          return m_hIcon;

      if((m_hIcon = CopyIcon((HICON)::SendMessage(m_hWinHwnd,WM_GETICON,ICON_SMALL,0))))
          return m_hIcon;

      if((m_hIcon = CopyIcon((HICON)::GetClassLong(m_hWinHwnd,GCL_HICON))))
          return m_hIcon;

      if((m_hIcon = CopyIcon((HICON)::GetClassLong(m_hWinHwnd,GCL_HICONSM))))
          return m_hIcon;

//    if((m_hIcon = CopyIcon((HICON)::SendMessage(m_hWinHwnd,WM_GETICON,ICON_BIG,0)))   ||
//       (m_hIcon = CopyIcon((HICON)::SendMessage(m_hWinHwnd,WM_GETICON,ICON_SMALL,0))) ||
//       (m_hIcon = CopyIcon((HICON)::GetClassLong(m_hWinHwnd,GCL_HICON)))              ||
//       (m_hIcon = CopyIcon((HICON)::GetClassLong(m_hWinHwnd,GCL_HICONSM))))
//        return m_hIcon;

      m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); //If all else fails, use the default icon.
      m_bIconDefault = TRUE;
      return m_hIcon;
  }

UINT CBionicWindow::GetTrayID()
  {
      return m_uiTrayID;
  }

BOOL CBionicWindow::SetState(int nCmdShow)
  {
      BOOL bFeedback;

      switch(nCmdShow)
      {
          case SW_SHOW:

              m_bIsVisible = TRUE;
              m_numOfHidden--;

              bFeedback = ::SetWindowPos(m_hWinHwnd,NULL,0,0,0,0,
                                         SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER |
                                         SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
              if(m_bForeground)
              {
                  TRACE("Setting foreground window.\n");
                  SetForegroundWindow(m_hWinHwnd);
                  m_bForeground = FALSE;
              }
              break;

          case SW_HIDE:

              m_bIsVisible = FALSE;
              m_numOfHidden++;

              bFeedback = ::SetWindowPos(m_hWinHwnd,NULL,0,0,0,0,
                                         SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER |
                                         SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
              break;
              
          default:
              bFeedback = ::ShowWindow(m_hWinHwnd,nCmdShow);
      }
      return bFeedback;
  }

void CBionicWindow::Close()
  {
      PostMessage(m_hWinHwnd,WM_CLOSE,0,0);
  }

BOOL CBionicWindow::GetState()
  {
      return m_bIsVisible;
  }

BOOL CBionicWindow::GetAlwaysOnTop()
  {
      return m_bTopMost;
  }

BOOL CBionicWindow::ToggleAlwaysOnTop()
  {
      m_bTopMost = !m_bTopMost;
      UINT vflag = (m_bIsVisible) ? SWP_SHOWWINDOW : SWP_HIDEWINDOW;

      return SetWindowPos(m_hWinHwnd,(m_bTopMost) ? HWND_TOPMOST : HWND_NOTOPMOST,
                          0,0,0,0,SWP_NOMOVE | SWP_NOSIZE |
                          vflag);
  }

void CBionicWindow::SetMemory()
  {
      ::GetWindowRect(m_hWinHwnd,&m_rcRecall);

      m_rcRecall.right = m_rcRecall.right - m_rcRecall.left;
      m_rcRecall.bottom = m_rcRecall.bottom - m_rcRecall.top;
  }

void CBionicWindow::Recall()
  {
      ::SetWindowPos(m_hWinHwnd,0, //Ignore z-order via SWP_NOZORDER.
                     m_rcRecall.left,
                     m_rcRecall.top,
                     m_rcRecall.right,
                     m_rcRecall.bottom,
                     SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOOWNERZORDER);
  }

void CBionicWindow::SetTrayState(BOOL state)
  {
      m_bTray = state;
  }

BOOL CBionicWindow::GetTrayState()
  {
      return m_bTray;
  }

int CBionicWindow::GetNumOfWindows()
  {
      return m_numOfWindows;
  }

int CBionicWindow::GetNumOfHidden()
  {
      return m_numOfHidden;
  }

BOOL CBionicWindow::GetPreserveName()
  {
      return m_bPreserve;
  }

void CBionicWindow::SetPreserveName(BOOL bPreserve)
  {
      m_bPreserve = bPreserve;
  }

void CBionicWindow::MarkForeground(BOOL bForeground /* = TRUE */)
  {
      m_bForeground = bForeground;
  }
