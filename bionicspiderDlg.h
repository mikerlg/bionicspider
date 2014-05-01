/*
  BionicSpider Lite v2.0.0
  by Mike LeonGuerrero
*/
#ifndef _BIONICSPIDERDLG_H_
#define _BIONICSPIDERDLG_H_
#include "BionicTree.h"
#include "HotKeyMan.h"
#include "afxwin.h"

class CBionicSpiderDlg : public CDialog
  {
    public:

      CBionicSpiderDlg(CWnd* pParent = NULL);	// standard constructor
      ~CBionicSpiderDlg();

      static const int WM_NOTIFYICON;            //Msg. for tray icon.
      static const int WM_TASKBARCREATED;        //Msg. sent from windows when explorer.exe crashes - restore tray icon.

      CBionicTree m_bionicTree;
      CHotKeyMan m_hotkey;

      CStatic m_rootInfoSelection;
      CStatic m_info;
      CStatic m_infoIcon;

      enum
      {
          IDD = IDD_BIONICSPIDER_DIALOG,

          BS_BIONICTREE_DEFAULT_POS_X = 1,
          BS_BIONICTREE_DEFAULT_POS_Y = 105,
          BS_TRAY_ICON_ID = 0,
          BS_MENUMAP_IMAGES = 3
      };

      static void AlertMsgBox(HWND,LPCTSTR); //Static to call from other modules.
      static BOOL ConfirmMsgBox(HWND,LPCTSTR);
      BOOL CleanUp();

    private:

      CBitmap m_menuMaps[BS_MENUMAP_IMAGES];
      CToolTipCtrl m_ttip;
      CStatusBar m_status;

      HACCEL m_hAccelTable;
      HICON m_hIcon;
      HMENU m_hMainMenu;
      HWND m_hTrayHandle;

//HOTKEY DATA
      enum HOTKEY_CODES
      {
          HOTKEY_GROUP_UP = 0,
          HOTKEY_GROUP_DOWN,
          HOTKEY_ROOT_HIDE_SHOW_ALL,
          HOTKEY_GROUP_HIDE_SHOW,
          HOTKEY_FOREGROUND_SPIDER
      };
      enum
      {
          HOTKEY_TOGGLE_HIDE_ALL,
          HOTKEY_TOGGLE_HIDE_GROUP,
          HOTKEY_TOGGLE_SIZE
      };
      BOOL m_bHotkeyToggle[HOTKEY_TOGGLE_SIZE];
      BOOL m_bHotkeyDlgInit;
//END HOTKEY DATA

//DOCKING AppBar attributes.
      struct DOCKING_OPTIONS
      {
          UINT uSide;
          RECT rcEdges[4];
          DWORD cxWidth;
          DWORD cyHeight;
          BOOL fAutoHide;
          BOOL fOnTop;
          BOOL fHiding;
      };
      DOCKING_OPTIONS m_dockingOpt;
      RECT m_rcAppBar;            // Current rect of the appbar

      BOOL AppBar_SetSide(UINT uSide);
      void AppBar_QuerySetPos(UINT,LPRECT,PAPPBARDATA,BOOL);
      void AppBar_QueryPos(LPRECT lprc);
      void AppBar_Size();
      void AppBar_Register();
      void AppBar_UnRegister();
//END DOCKING AppBar attributes.

//MENU SETTINGS 
      enum
      {
          MENU_ALWAYS_ON_TOP,
          MENU_WARNINGS,
          MENU_TRAY_POPUP,
          MENU_FULLROWSEL,
          MENU_ANIMATE,
          MENU_AUTOGROUP,
          MENU_DOCKING_REGISTRED,
          MENU_DOCKED_LEFT,
          MENU_SWITCH_GROUP,
          MENU_SWITCH_WINDOW,
          MENU_SWITCH_CURSOR,
          MENU_HOTKEYS,
          MENU_SETTINGS_SIZE
      };
      BOOL m_bMenuSettings[MENU_SETTINGS_SIZE];
//END MENU SETTINGS 

//ADD MISC. METHODS
      void DumpMenuSettings();
      void LoadMenuSettings();

      void Docking();
      void Undock(); //Clean up heap docking object etc.
      void PopContextMenu(int);
      void SetAlwaysOnTop(BOOL);
      void InitPopupTray(BOOL);
      void UpdateSelInfo(HTREEITEM,CBionicTree::BT_TREE_NODE_TYPE); //Update the info. on the selection side.
      BOOL GetInputText(LPCTSTR,LPCTSTR,LPCTSTR,CString&);
      BOOL TrayIcon(HWND,LPCTSTR,DWORD,HICON,UINT);
      LRESULT OnNotifyIcon(WPARAM,LPARAM);
      HWND GetTrayHwnd();
      BOOL CenterWndOnScreen(HWND);
      void UpdateGroupListMenu(); //Update the "Group(s)" slide out menu.
      BOOL OnHelpInfo(HELPINFO* pHelpInfo);

#ifdef LITE_UPGRADE      
      void upgrade_msg();
#endif
//END ADD MISC. METHODS

      virtual BOOL OnCommand(WPARAM,LPARAM);

          //Registered right click msg -> CTreeCtrl will send these to parent.
      afx_msg LRESULT OnTreeRightClick(WPARAM,LPARAM);
      afx_msg LRESULT OnTreeMouseLeave(WPARAM,LPARAM);
      afx_msg LRESULT OnTreeCheckPopup(WPARAM,LPARAM); //Check if we have the tray popup setting set.
      afx_msg LRESULT OnTreeCheckAutoGroup(WPARAM,LPARAM); //Check if we need to blacklist windows - Auto group setting.

      afx_msg LRESULT OnHotKey(WPARAM,LPARAM);
      afx_msg LRESULT OnSeekerWindow(WPARAM,LPARAM); //Msg. from the seeker.
      afx_msg LRESULT OnTaskBarCreated(WPARAM,LPARAM); //EXPLORER.EXE crashed. :(
      afx_msg LRESULT OnUpdateInfo(WPARAM,LPARAM); //Message from tree to update data.

          // Generated message map functions
      virtual BOOL OnInitDialog();
      afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
      afx_msg void OnPaint();
      afx_msg void OnSize(UINT,int,int);
      afx_msg void OnSizing(UINT,LPRECT);
      afx_msg HCURSOR OnQueryDragIcon();
      virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
      BOOL PreTranslateMessage(MSG*);

      DECLARE_MESSAGE_MAP()

          public:

          //START MISC. PUBLIC INTERFACE.
      BOOL GetHotkeyState();
      void SetHotkeyState(BOOL);
      void LoadHotKeys();
      void ShowHideSpider(); //Toggle spider's visibility.
          //END MISC. PUBLIC INTERFACE.

      afx_msg void OnClose();
      afx_msg void OnBionicSpiderExit();
      afx_msg void OnInitMenuPopup(CMenu*,UINT,BOOL);
      afx_msg void OnUpdateTilewindows1x1hort(CCmdUI *pCmdUI);
      afx_msg void OnUpdateTileWindows1x1hort(CCmdUI *pCmdUI);
      afx_msg void OnUpdateTileWindows1x1vert(CCmdUI *pCmdUI);
      afx_msg void OnUpdateTileWindows2x2(CCmdUI *pCmdUI);
      afx_msg void OnDockingToggle();
      afx_msg void OnUpdateDockingEnableDocking(CCmdUI *pCmdUI);
      afx_msg void OnUpdateDockingRight(CCmdUI *pCmdUI);
      afx_msg void OnUpdateDockingLeft(CCmdUI *pCmdUI);
      afx_msg void OnDockingRight();
      afx_msg void OnDockingLeft();
      afx_msg void OnRootAddGroup();
      afx_msg void OnBionicspiderAlwaysOnTop();
      afx_msg void OnUpdateBionicspiderAlwaysOnTop(CCmdUI *pCmdUI);
      afx_msg void OnBionicspiderWarnings();
      afx_msg void OnUpdateBionicspiderWarnings(CCmdUI *pCmdUI);
      afx_msg void OnRootHideAllGroups();
      afx_msg void OnRootShowAllGroups();
      afx_msg void OnRootMaximizeAllGroups();
      afx_msg void OnRootMinimizeAllGroups();
      afx_msg void OnRootRestoreAllGroups();
      afx_msg void OnRootSortAllGroups();
      afx_msg void OnRootCloseAllGroups();
      afx_msg void OnRootRemoveWindowsInAllGroups();
      afx_msg void OnRootRemoveAllGroups();
      afx_msg void OnGroupHideAllWindows();
      afx_msg void OnGroupShowAllWindows();
      afx_msg void OnGroupMaximizeAllWindows();
      afx_msg void OnGroupMinimizeAllWindows();
      afx_msg void OnGroupRestoreAllWindows();
      afx_msg void OnTileWindows1x1Hort();
      afx_msg void OnTileWindows1x1Vert();
      afx_msg void OnTileWindows2x2();
      afx_msg void OnGroupPositionsGroupMemory();
      afx_msg void OnGroupPositionsRecallAll();
      afx_msg void OnGroupAddWindowByTitle();
      afx_msg void OnGroupSortAllWindowsInGroup();
      afx_msg void OnGroupCloseAllWindowsInGroup();
      afx_msg void OnGroupRemoveAllWindowsInGroup();
      afx_msg void OnGroupRemoveThisGroup();
      afx_msg void OnGroupRenameThisGroup();
      afx_msg void OnWindowHideWindow();
      afx_msg void OnWindowShowWindow();
      afx_msg void OnWindowForegroundWindow();
      afx_msg void OnWindowMaximize();
      afx_msg void OnWindowMinimize();
      afx_msg void OnWindowRestore();
      afx_msg void OnWindowAlwaysOnTop();
      afx_msg void OnWindowClose();
      afx_msg void OnExplorerConsole();
      afx_msg void OnExplorerClone();
      afx_msg void OnWindowRename();
      afx_msg void OnWindowRemove();
      afx_msg void OnHelpContents();
      afx_msg void OnHelpAbout();
      afx_msg void OnBionicspiderSwitchGroup();
      afx_msg void OnUpdateBionicspiderSwitchGroup(CCmdUI *pCmdUI);
      afx_msg void OnBionicspiderSwitchWindows();
      afx_msg void OnUpdateBionicspiderSwitchWindows(CCmdUI *pCmdUI);
      afx_msg void OnBionicspiderTrayPopUp();
      afx_msg void OnUpdateBionicspiderTrayPopUp(CCmdUI *pCmdUI);
      afx_msg void OnWindowMinimizeToTray();
      afx_msg void OnBionicspiderHotkeys();
      afx_msg void OnBionicspiderAutogroup();
      afx_msg void OnUpdateBionicspiderAutogroup(CCmdUI *pCmdUI);
      afx_msg void OnUpdateWindowAlwaysOnTop(CCmdUI *pCmdUI);
      afx_msg void OnHelpCheckForUpdate();
      afx_msg void OnPositioningWindowMemory();
      afx_msg void OnPositioningWindowRecall();
      afx_msg void OnUpdateExplorerConsole(CCmdUI *pCmdUI);
      afx_msg void OnUpdateExplorerClone(CCmdUI *pCmdUI);
      afx_msg void OnBionicspiderSwitchCursor();
      afx_msg void OnUpdateBionicspiderSwitchCursor(CCmdUI *pCmdUI);
      afx_msg void OnHelpEnterkey();
      afx_msg void OnBionicspiderHide();
      afx_msg void OnBionicspiderFonts();
      afx_msg void OnBionicspiderFullrow();
      afx_msg void OnUpdateBionicspiderFullRow(CCmdUI *pCmdUI);
      afx_msg void OnBionicspiderAnimate();
      afx_msg void OnUpdateBionicspiderAnimate(CCmdUI *pCmdUI);
      afx_msg void OnRootCollapse();
      afx_msg void OnRootExpand();
      afx_msg void OnMenuGroupOne();
};
#endif
