#ifndef _BIONICWINDOW_H_
#define _BIONICWINDOW_H_
#include <afxtempl.h>

typedef CList<UINT> WinTrayID;

class CBionicWindow
  {
    protected:

      HWND m_hWinHwnd;
      HICON m_hIcon;
      BOOL m_bIconDefault;
      BOOL m_bTopMost;
      BOOL m_bIsVisible;
      BOOL m_bTray;
      BOOL m_bPreserve;
      BOOL m_bForeground;
      UINT m_uiTrayID;
      RECT m_rcRecall;

      static int m_numOfWindows;
      static int m_numOfHidden;

      int m_nImageIndex; //Index of window icon for CTreeCtrl tree.
      void CleanUp();

    public:

      static WinTrayID sm_idList; //List that will hold valid IDs for new windows.
      static UINT sm_idCount;  //Current valid ID count for each window tray ID

      CBionicWindow();
      virtual ~CBionicWindow();

      virtual BOOL SetState(int nCmdShow);
      virtual void SetHandle(HWND);
      virtual void SetMemory();
      void Recall();

      BOOL GetState();
      BOOL ToggleAlwaysOnTop();
      BOOL GetAlwaysOnTop();
      BOOL GetPreserveName();
      void SetPreserveName(BOOL = TRUE);
      UINT GetTrayID();
      HICON RefreshIcon();//WM_GETICON msg.
      HWND GetHandle();
      void Close();
      void SetImageIndex(int);
      int GetImageIndex();
      void SetTrayState(BOOL); //Set tray state.
      BOOL GetTrayState();     //Is this window in the sys. tray?
      void MarkForeground(BOOL = TRUE);

      static int GetNumOfWindows();
      static int GetNumOfHidden();
  };
#endif
