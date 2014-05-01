#ifndef _CBIONICTREE_H_
#define _CBIONICTREE_H_
#include "resource.h"
#include "BionicWindow.h"
#include "HotKeyMan.h"
#include "FontMan.h"

class CBionicTree : public CTreeCtrl
  {
    public:
          //Notification messages that we will send to our parent dialog box.
      static const int WM_TREE_RIGHT_CLICK;
      static const int WM_TREE_UPDATE;
      static const int WM_TREE_CHECK_POPUP;
      static const int WM_TREE_CHECK_AUTOGROUP;
      static const int WM_TREE_MOUSE_LEAVE;

      CHotKeyMan m_hotkeyGroups;  //Hotkey manager for each group node.
      CImageList m_treeImageList; //PUBLIC image list object.

    private:

      CFontMan m_fontMan; //Font manager for the tree control.
      CFont m_font;       //Main font used for the tree text.

      struct SBT_GroupNum //Use this struct to get a group by it's order in the tree (use internally).
      {
          HTREEITEM hGroup;
          UINT uiNum;
          UINT uiCurrent;
      };

      HTREEITEM m_hRootNode;     //Root node handle.
      CImageList *m_pDragImage;

          //Drag/drop nodes.
      HTREEITEM m_hItemDrop;
      HTREEITEM m_hItemDrag;
      BOOL m_bDragging;
      BOOL m_bTracking;

      BOOL m_bMouseTrack;
      BOOL m_bSwitchGroup;
      BOOL m_bSwitchWindow;
      int m_nNumOfGroups;
      BOOL m_bSwitchCursorToggle; //Toggle this flag for switch cursor to prevent repeat actions.
      BOOL m_bHotkeyEnable;

      CList<HWND> m_blackList;

    public:

      struct SBT_TreeData
      {
          int numOfWindows;
          int numOfGroups;
          int numOfHidden;
      };
      struct SBT_WindowTrayID
      {
          HTREEITEM hNode;
          UINT uiID;
          int nState;

          SBT_WindowTrayID()
              {
                  hNode = NULL;
                  uiID = 0;
                  nState = 0;
              }
      };
      struct SBT_TrayData
      {
          HWND hWnd;
          HICON hIcon;
          UINT id;
          CString cstrBuffer;
      };
      struct SBT_GroupInfo
      {
          int ncount;
          int nvisible;
          CString hotkey;
          SBT_GroupInfo()
              {
                  ncount = 0;
                  nvisible = 0;
                  hotkey = "none";
              }
      };
      struct SBT_GroupHotkeyData //Passed in struct. to retrieve group ATOM via id.
      {
          ATOM atomID;
          int id;
          int current;
          SBT_GroupHotkeyData()
              {
                  atomID = 0;
                  id = 0;
                  current = 0;
              }
      };
      struct SBT_WindowInfo
      {
          BOOL bVisible;
          BOOL bOnTop;
          BOOL bTray;
          SBT_WindowInfo()
              {
                  bVisible = FALSE;
                  bOnTop = FALSE;
                  bTray = FALSE;
              }
      };
          //START CONSTANTS.
      enum
      {
          BT_AUTO_GROUP_ID = 1,
          BT_SWITCH_CURSOR_ID,
          BT_AUTO_GROUP_DELAY = 1000,
          BT_SWITCH_CURSOR_DELAY = 500,
          BT_MINIMUM_NODES = 2,
          BT_GROUP_ICON_INDEX = 2,
          BT_GROUP_ICON_INDEX_SEL = 3,
          ID = IDC_BIONICTREE
      };
      enum BT_TREE_NODE_TYPE
      {
          BT_NODE_ROOT = 1,
          BT_NODE_GROUP = 2,
          BT_NODE_WINDOW = 3
      };
      enum BT_TILE_CONFIG
      {
          BT_TILE_1X1H,
          BT_TILE_1X1V,
          BT_TILE_2X2
      };
          //END CONSTANTS.

      CBionicTree();
      ~CBionicTree();

          //START ROOT METHODS.
      HTREEITEM InstallGroupFolder(LPCTSTR); //Install a new group folder under the root.
      BOOL RemoveAllGroups();//Remove all groups under the root.
      BOOL RenameRootGroup(LPCTSTR);
      void GetRootText(CString&);
      void InstallRootNode();//Install a root node to the tree.
      void RefreshImageList();//Refresh image list from ROOT.

      void HideAllGroupWindows();  //Hide all groups under the root.
      void ShowAllGroupWindows();  //Hide all groups under the root.
      void FreeAllGroupWindows();
      void MinAllGroupWindows();
      void MaxAllGroupWindows();
      void RestAllGroupWindows();
      void CollapseAllGroups(BOOL = TRUE);
      void CloseAllGroupWindows();
      void RemoveAllGroupWindows();
      void CheckAllWindows();
      void SetStateID(UINT,int);
      void MoveGroupUp(BOOL = TRUE); //Hotkey - move group up or down.
      void SortAllGroups();
      void GetGroupNames(CList<CString>&);
      void RefreshGroupsHotkeyID();
      void InstallGroupsHotkeyID();
      void MarkForegroundWindow(HWND);
      int GetGroupsCount();
      HTREEITEM GetGroupByNum(UINT);

      static BOOL EnumDumpGroup(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumRefreshImageGroup(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumFreeGroupWindows(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumShowGroupWindows(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumHideGroupWindows(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumMinGroupWindows(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumMaxGroupWindows(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumRestGroupWindows(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumRemoveGroupWindows(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumCloseGroupWindows(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumValidateWindow(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumIsWindow(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumCountWindows(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumSetStateTrayIDWindows(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumGroupSort(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumGetGroupByNum(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumGetGroupNum(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumGetGroupNames(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumGetGroupsCount(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumBlacklistGroups(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumGetAllGroups(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumRefreshHotKeyIDGroups(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumSetHotKeyIDGroups(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumCollapseGroups(HTREEITEM,CBionicTree*,void*);
      static BOOL EnumGroupSetForeground(HTREEITEM,CBionicTree*,void*);
          //END ROOT METHODS.

          //START GROUP METHODS.
      void ShowGroupWindows(HTREEITEM = NULL);
      void HideGroupWindows(HTREEITEM = NULL);
      void RestGroupWindows();
      void MaxGroupWindows();
      void MinGroupWindows();
      void CloseGroupWindows();
      void RemoveGroupWindows(HTREEITEM = NULL);
      void TileGroupWindows(BT_TILE_CONFIG);

      BOOL RemoveSelectedGroup();
      BOOL RenameSelectedGroup(LPCTSTR);
      BOOL RemoveGroup(HTREEITEM);
      void GetNodeText(CString&,BT_TREE_NODE_TYPE);
      void GetGroupInfo(SBT_GroupInfo&,HTREEITEM = NULL); //Get the info for the current group.
      void GroupMemory();
      void GroupRecall();
      void GroupSort(HTREEITEM = NULL);

          //END GROUP METHODS.

          //START WINDOW METHODS.
    private:

      static BOOL BT_RefreshTreeImage(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_ValidateWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_FreeWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_HideWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_ShowWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_RestWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_MaxWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_MinWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_CloseWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_RemoveWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_CountWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_SetStateIDWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_IsWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_TileWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_WindowGroupInfo(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_GroupMemory(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_GroupRecall(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_BlackListWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);
      static BOOL BT_MarkForegroundWindow(HTREEITEM,CBionicTree*,CBionicWindow*,void*);

    public:

      void HideWindow();
      void ShowWindow();
      void MinWindow();
      void MaxWindow();
      void RestWindow();
      void ForegroundWindow();
      void RemoveWindow(BOOL = FALSE,HTREEITEM = NULL); //BOOL = close window? HTREEITEM = window node.
      BOOL GetTrayIconData(SBT_TrayData*); //Return default windows tray ID.
      BOOL RenameWindow(LPCTSTR);
      BOOL GetAlwaysOnTop();
      void ToggleAlwaysOnTop();
      BOOL WindowPositionMemory();
      void WindowPositionRecall();
      void WindowExConsole();
      void WindowExClone();
      void GetWindowInfo(SBT_WindowInfo &info); //Get info on the selected window node.

          //END WINDOW METHODS.

          //START ENUM METHODS.
    private:

      BOOL EnumAllGroups(BOOL (*doIt)(HTREEITEM,CBionicTree*,void*),void*);
      BOOL EnumGroup(HTREEITEM,BOOL (*doIt)(HTREEITEM,CBionicTree*,CBionicWindow*,void*),void*);

          //END ENUM METHODS.

          //START MISC. METHODS.

      void GetAllGroups(CList<HTREEITEM>&);    //Gather up all group handles into a list.
      void DragNodeToNode(HTREEITEM,HTREEITEM);//Called for drag and drop operations.
      void UpdateFromRootMenu();               //For all root operations, do not remove the selection to the root node, just update the current one.
      HTREEITEM CopyWindowToGroup(CBionicWindow*,HTREEITEM); //Copy a window node into a group node.
      BOOL GetDisplayInfo(CString&);                         //Get display info for the parent dialog.
      afx_msg void OnTimer(UINT);                            //Misc. timer operations.
      afx_msg LRESULT OnHotKey(WPARAM,LPARAM);               //Hotkey method for groups.
      static UINT AutoGroupThread(LPVOID);                   //Auto group thread that will be called by OnTimer();

      struct winforeinfo
      {
          HWND hFocused;
          int start;
          int end;
          winforeinfo()
              {
                  hFocused = NULL;
                  start = 0;
                  end = 0;
              }
      };
      static BOOL CALLBACK EnumGetForegroundProc(HWND,LPARAM);

    public:

      static BOOL CALLBACK EnumWinProc(HWND,LPARAM); //Enum. for all desktop windows.
      BOOL ValidateWindow(HWND);
      void InstallGroups();
      void DumpGroups();
      void AutoGroup(BOOL); //Set AUTO GROUP mode.
      void SetSwitchCursor(BOOL);
      HTREEITEM AddWindowToTree(HWND,HTREEITEM=NULL);
      void LoadTreeImages(); //Load up all images for this tree (root,group).
      BT_TREE_NODE_TYPE GetTreeNodeType(HTREEITEM=NULL); //Get the type of node (root,group,window).
      HTREEITEM GetDefaultNode(HTREEITEM,BT_TREE_NODE_TYPE);
      BOOL GetSwitchGroup();
      void SetSwitchGroup(BOOL);
      BOOL GetSwitchWindow();
      void SetSwitchWindow(BOOL);
      int GetNumOfWindows();
      int GetNumOfGroups();
      BOOL IsExplorerCheck(HWND = NULL);
      BOOL VerifyGroupRoot(); //Verify that there is only a ROOT and a single GROUP.
      void GetTreeData(SBT_TreeData*);
      int GetGroupNum(HTREEITEM);              //Given a group handle, return the groups index in the tree.

          //Blacklist methods - keep track of removed windows when "Auto group" is set.
      BOOL InsertBlacklist(HWND);
      BOOL RemoveBlacklisted(HWND);
      void TrimBlacklist(); //Remove all invalid HWNDs from the blacklist.
      BOOL IsBlacklisted(HWND);
      void InsertBlacklistGroup(HTREEITEM);
      void BlacklistAll();
      void KillBlacklist();

      void LoadGroupHotKeys();
      void LoadHotKeys();
      void LoadDefaultHotkeys();
      void EnableHotkeys(BOOL);

      void SetTreeFont(LOGFONT* = NULL,COLORREF = -1);
      void SaveTreeFont();
      COLORREF GetFontColor();
      void SetFullRowSelect(BOOL = TRUE);

          //END MISC. METHODS.

          //Reflected tree messages.
      afx_msg void OnTreeRightClick(NMHDR*,LRESULT*);
      afx_msg void OnTreeClick(NMHDR*,LRESULT*);
      afx_msg void OnTreeDblClick(NMHDR*,LRESULT*);
      afx_msg void OnBeginDrag(NMHDR*,LRESULT*);
      afx_msg void OnTreeSelChanged(NMHDR*,LRESULT*);
      afx_msg void OnLButtonUp(UINT,CPoint);
      afx_msg void OnMouseMove(UINT,CPoint);
      afx_msg LRESULT OnMouseLeave(WPARAM,LPARAM);

      DECLARE_MESSAGE_MAP()
};
#endif
