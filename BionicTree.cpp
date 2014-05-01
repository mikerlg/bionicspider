#include "stdafx.h"
#include "BionicTree.h"
#include "RegistryEd.h"
#include "HotKeyMan.h"
#include "StringToken.h"
#include "ExBionicWindow.h"
#include <afxtempl.h>

const int CBionicTree::WM_TREE_RIGHT_CLICK = RegisterWindowMessage(_T("WM_TREE_RIGHT_CLICK"));
const int CBionicTree::WM_TREE_MOUSE_LEAVE = RegisterWindowMessage(_T("WM_TREE_MOUSE_LEAVE"));
const int CBionicTree::WM_TREE_UPDATE = RegisterWindowMessage(_T("WM_TREE_UPDATE"));
const int CBionicTree::WM_TREE_CHECK_POPUP = RegisterWindowMessage(_T("WM_TREE_CHECK_POPUP"));
const int CBionicTree::WM_TREE_CHECK_AUTOGROUP = RegisterWindowMessage(_T("WM_TREE_CHECK_AUTOGROUP"));

#define SHELL_CMD_NT _T("CMD.EXE")
#define SHELL_CMD_95 _T("COMMAND")
#define SWITCH_CURSOR_BOUNDARY 5

BOOL CALLBACK EnumChildExProc(HWND,LPARAM); //Enumeration callback for explorer windows.

CBionicTree::CBionicTree()
  {
      m_hRootNode = NULL;
      m_hItemDrop = NULL;
      m_hItemDrag = NULL;
      m_bDragging = FALSE;
      m_bTracking = FALSE;
      m_bMouseTrack = FALSE;
      m_pDragImage = NULL;
      m_bSwitchGroup = TRUE;
      m_bSwitchWindow = TRUE;
      m_bSwitchCursorToggle = TRUE;
      m_bHotkeyEnable = TRUE;

      m_nNumOfGroups = 0;
  }

CBionicTree::~CBionicTree()
  {
      m_treeImageList.DeleteImageList();
  }

//Get the type of node for hTreeItem.
CBionicTree::BT_TREE_NODE_TYPE CBionicTree::GetTreeNodeType(HTREEITEM hTreeItem /* = NULL*/)
  {
      if(NULL == hTreeItem)
          hTreeItem = GetSelectedItem();

      if(hTreeItem == m_hRootNode)
          return BT_NODE_ROOT;
      else if(GetParentItem(hTreeItem) == m_hRootNode)
          return BT_NODE_GROUP;

      return BT_NODE_WINDOW;
  }

HTREEITEM CBionicTree::GetDefaultNode(HTREEITEM hNode,
                                      BT_TREE_NODE_TYPE eDesiredNodeType)
  {
      BT_TREE_NODE_TYPE eSelectedNodeType;
      HTREEITEM hDesiredNode = hNode;

      if((eSelectedNodeType = GetTreeNodeType(hNode))!=eDesiredNodeType)
      {
          switch(eSelectedNodeType)
          {
              case BT_NODE_ROOT:

                  if(eDesiredNodeType == BT_NODE_GROUP)
                      hDesiredNode = GetChildItem(hNode);
                  else if(eDesiredNodeType == BT_NODE_WINDOW)
                      hDesiredNode = GetChildItem(GetChildItem(m_hRootNode)); //First window.

                  break;
                  
              case BT_NODE_GROUP:

                  if(eDesiredNodeType == BT_NODE_WINDOW)
                      hDesiredNode = GetChildItem(hNode);

                  break;
                      
              case BT_NODE_WINDOW:

                  if(eDesiredNodeType == BT_NODE_GROUP)
                      hDesiredNode = GetParentItem(hNode);

                  break;
          }
      }
      return hDesiredNode;
  }

//Enumerate through all groups under the root and apply doIt(...) to each.
BOOL CBionicTree::EnumAllGroups(BOOL (*pfDoIt)(HTREEITEM,CBionicTree*,void*),void *buf)
  {
      ASSERT(pfDoIt);
      
      HTREEITEM hCurrent = GetChildItem(GetRootItem());
      while(hCurrent)
      {
          if(pfDoIt(hCurrent,this,buf) == FALSE)
              return FALSE;

          hCurrent = GetNextItem(hCurrent,TVGN_NEXT);
      }
      return TRUE;
  }

BOOL CBionicTree::GetSwitchGroup()
  {
      return m_bSwitchGroup;
  }

void CBionicTree::SetSwitchGroup(BOOL bSwitchGroup)
  {
      m_bSwitchGroup = bSwitchGroup;
  }

BOOL CBionicTree::GetSwitchWindow()
  {
      return m_bSwitchWindow;
  }

void CBionicTree::SetSwitchWindow(BOOL bSwitchWindow)
  {
      m_bSwitchWindow = bSwitchWindow;
  }

int CBionicTree::GetNumOfGroups()
  {
      return m_nNumOfGroups;
  }

//START ROOT METHODS.
BOOL CBionicTree::EnumRefreshImageGroup(HTREEITEM hNode,
                                        CBionicTree *pTree,
                                        void *buf)
  {
      pTree->EnumGroup(hNode,BT_RefreshTreeImage,buf);

      return TRUE;
  }

//Enum. all groups - add group name to registry.
BOOL CBionicTree::EnumDumpGroup(HTREEITEM hNode,
                                CBionicTree *pTree,
                                void *buf)
  {
      CString *pGroupRegStr = (CString*)buf;
      ASSERT(pGroupRegStr);

      CString buffer = pTree->GetItemText(hNode);
      CString delim;
      
      delim.LoadString(IDS_DELIMINATOR);

      (*pGroupRegStr) += buffer;
      (*pGroupRegStr) += delim;

      return TRUE;
  }

void CBionicTree::RefreshImageList()
  {
      SetImageList(NULL,TVSIL_NORMAL);
      m_treeImageList.DeleteImageList();
      LoadTreeImages();//Load up folder images.

      TRACE("Loaded FOLDER IMAGES: %d\n",m_treeImageList.GetImageCount());
      
      EnumAllGroups(EnumRefreshImageGroup,0);

      TRACE("Total LOADED IMAGES: %d\n",m_treeImageList.GetImageCount());
  }

BOOL CBionicTree::EnumCountWindows(HTREEITEM hNode,
                                   CBionicTree *pTree,
                                   void *buf)
  {
      return pTree->EnumGroup(hNode,BT_CountWindow,buf);
  }

BOOL CBionicTree::EnumValidateWindow(HTREEITEM hNode,
                                     CBionicTree *pTree,
                                     void *buf)
  {
      return pTree->EnumGroup(hNode,BT_ValidateWindow,buf);
  }

BOOL CBionicTree::EnumIsWindow(HTREEITEM hNode,
                                     CBionicTree *pTree,
                                     void *buf)
  {
      return pTree->EnumGroup(hNode,BT_IsWindow,buf);
  }

BOOL CBionicTree::EnumFreeGroupWindows(HTREEITEM hNode,
                                       CBionicTree *pTree,
                                       void *buf)
  {
      pTree->EnumGroup(hNode,BT_FreeWindow,buf);

      return TRUE;
  }

BOOL CBionicTree::EnumShowGroupWindows(HTREEITEM hNode,
                                       CBionicTree *pTree,
                                       void *buf)
  {
      pTree->EnumGroup(hNode,BT_ShowWindow,buf);

      return TRUE;
  }

BOOL CBionicTree::EnumHideGroupWindows(HTREEITEM hNode,
                                       CBionicTree *pTree,
                                       void *buf)
  {
      pTree->EnumGroup(hNode,BT_HideWindow,buf);

      return TRUE;
  }

BOOL CBionicTree::EnumMaxGroupWindows(HTREEITEM hNode,
                                      CBionicTree *pTree,
                                      void *buf)
  {
      pTree->EnumGroup(hNode,BT_MaxWindow,buf);

      return TRUE;
  }

BOOL CBionicTree::EnumMinGroupWindows(HTREEITEM hNode,
                                      CBionicTree *pTree,
                                      void *buf)
  {
      pTree->EnumGroup(hNode,BT_MinWindow,buf);

      return TRUE;
  }

BOOL CBionicTree::EnumRestGroupWindows(HTREEITEM hNode,
                                       CBionicTree *pTree,
                                       void *buf)
  {
      pTree->EnumGroup(hNode,BT_RestWindow,buf);

      return TRUE;
  }

BOOL CBionicTree::EnumCollapseGroups(HTREEITEM hNode,
                                     CBionicTree *pTree,
                                     void *buf)
  {
      ASSERT(buf);
      UINT nCode = *(UINT*)buf;

      pTree->Expand(hNode,nCode);

      return TRUE;
  }

BOOL CBionicTree::EnumGroupSort(HTREEITEM hNode,
                                CBionicTree *pTree,
                                void *buf)
  {
      pTree->GroupSort(hNode);

      return TRUE;
  }

BOOL CBionicTree::EnumGroupSetForeground(HTREEITEM hNode,
                                         CBionicTree *pTree,
                                         void *buf)
  {
      return pTree->EnumGroup(hNode,BT_MarkForegroundWindow,buf);
  }

BOOL CBionicTree::EnumSetStateTrayIDWindows(HTREEITEM hNode,
                                            CBionicTree *pTree,
                                            void *buf)
  {
      return pTree->EnumGroup(hNode,BT_SetStateIDWindow,buf);
  }

BOOL CBionicTree::EnumRemoveGroupWindows(HTREEITEM hNode,
                                         CBionicTree *pTree,
                                         void *buf)
  {
      pTree->EnumGroup(hNode,BT_RemoveWindow,buf);

      return TRUE;
  }

BOOL CBionicTree::EnumCloseGroupWindows(HTREEITEM hNode,
                                        CBionicTree *pTree,
                                        void *buf)
  {
      pTree->EnumGroup(hNode,BT_CloseWindow,buf);

      return TRUE;
  }

BOOL CBionicTree::EnumGetGroupByNum(HTREEITEM hNode,
                                    CBionicTree *pTree,
                                    void *buf)
  {

      SBT_GroupNum *pdata = (SBT_GroupNum*)buf;
      ASSERT(buf);

      if(pdata->uiNum == pdata->uiCurrent)
      {
          pdata->hGroup = hNode;
          return FALSE;
      }
      else
          pdata->uiCurrent++;

      return TRUE;
  }

BOOL CBionicTree::EnumGetGroupNum(HTREEITEM hNode,
                                  CBionicTree *pTree,
                                  void *buf)
  {

      SBT_GroupNum *pdata = (SBT_GroupNum*)buf;
      ASSERT(buf);

      if(pdata->hGroup == hNode)
      {
          pdata->uiNum = pdata->uiCurrent;
          return FALSE;
      }
      else
          pdata->uiCurrent++;

      return TRUE;
  }

BOOL CBionicTree::EnumGetGroupsCount(HTREEITEM hNode,
                                     CBionicTree *pTree,
                                     void *buf)
  {
      int *pcount = (int*)buf;
      ASSERT(pcount);
      
      (*pcount)++;
      return TRUE;
  }

BOOL CBionicTree::EnumGetGroupNames(HTREEITEM hNode,
                                    CBionicTree *pTree,
                                    void *buf)
  {
      CList<CString> *pGroups = (CList<CString>*)buf;
      if(pGroups)
          pGroups->AddTail(pTree->GetItemText(hNode));

      return TRUE;
  }

BOOL CBionicTree::EnumBlacklistGroups(HTREEITEM hNode,
                                      CBionicTree *pTree,
                                      void *buf)
  {
      pTree->EnumGroup(hNode,BT_BlackListWindow,buf);

      return TRUE;
  }

//Get group hotkey data via its position index in the tree.
BOOL CBionicTree::EnumGetAllGroups(HTREEITEM hNode,
                                   CBionicTree *pTree,
                                   void *buf)
  {
      CList<HTREEITEM> *plist = (CList<HTREEITEM>*)buf;
      ASSERT(buf);
      
      plist->AddTail(hNode);

      return TRUE;
  }

//Gather up all group node handles into a list.
void CBionicTree::GetAllGroups(CList<HTREEITEM> &groups)
  {
      EnumAllGroups(EnumGetAllGroups,&groups);
  }

BOOL CBionicTree::ValidateWindow(HWND hWnd)
  {
      return EnumAllGroups(EnumValidateWindow,(HWND*)hWnd);
  }

//Make sure that all windows in the tree are still valid else remove them.
void CBionicTree::CheckAllWindows()
  {
      CList<HTREEITEM> items;
      POSITION pos;

      CBionicWindow *pWin;
      BOOL bUpdate = FALSE;
      HTREEITEM hParent = NULL;

      EnumAllGroups(EnumIsWindow,&items); //Scoop up all invalid windows into a list.

          //If we have at least one window node deletion, update the data area with parent group info.
      if((pos = items.GetHeadPosition()))
      {
          hParent = GetParentItem(items.GetAt(pos));
          ASSERT(hParent);

          bUpdate = TRUE;
      }
      while(pos)
      {
          HTREEITEM hNode = items.GetAt(pos);

          pWin = (CBionicWindow*)GetItemData(hNode);
          ASSERT(pWin);

          if(pWin)
          {
              delete pWin;
              pWin = 0;
          }
          DeleteItem(hNode);

          items.GetNext(pos);
      }
      if(bUpdate)
      {
          RefreshImageList();
          GetParent()->PostMessage(WM_TREE_UPDATE,
                                   (WPARAM)hParent,
                                   (LPARAM)BT_NODE_GROUP);
      }
  }

//For all root operations from main menu,
//do not focus the selection to the root node.
//Just update the current selected node.
void CBionicTree::UpdateFromRootMenu()
  {
      HTREEITEM hNode = GetSelectedItem();
      GetParent()->PostMessage(WM_TREE_UPDATE,
                               (WPARAM)hNode,
                               (LPARAM)GetTreeNodeType(hNode));
  }

void CBionicTree::HideAllGroupWindows()
  {
      winforeinfo winfore;
      winfore.start = 0;
      winfore.end = 1;

          //Enum. all desktop windows and mark the one window that has the focus.
      EnumWindows((WNDENUMPROC)EnumGetForegroundProc,(LPARAM)&winfore);
      MarkForegroundWindow(winfore.hFocused);

      EnumAllGroups(EnumHideGroupWindows,0);
      UpdateFromRootMenu();
  }

void CBionicTree::ShowAllGroupWindows()
  {
      EnumAllGroups(EnumShowGroupWindows,0);
      UpdateFromRootMenu();
  }

void CBionicTree::MaxAllGroupWindows()
  {
      EnumAllGroups(EnumMaxGroupWindows,0);
      UpdateFromRootMenu();
  }

void CBionicTree::MinAllGroupWindows()
  {
      EnumAllGroups(EnumMinGroupWindows,0);
      UpdateFromRootMenu();
  }

void CBionicTree::RestAllGroupWindows()
  {
      EnumAllGroups(EnumRestGroupWindows,0);
      UpdateFromRootMenu();
  }

void CBionicTree::CollapseAllGroups(BOOL bCollapse /* = TRUE */)
  {
      UINT nCode = (bCollapse) ? TVE_COLLAPSE : TVE_EXPAND;

      EnumAllGroups(EnumCollapseGroups,&nCode);
  }

void CBionicTree::FreeAllGroupWindows()
  {
      EnumAllGroups(EnumFreeGroupWindows,0);
  }

void CBionicTree::SortAllGroups()
  {
//    EnumAllGroups(EnumGroupSort,0);

      SortChildren(GetRootItem());
  }

void CBionicTree::MarkForegroundWindow(HWND hWnd)
  {
      EnumAllGroups(EnumGroupSetForeground,&hWnd);
  }

void CBionicTree::SetStateID(UINT uiID,
                             int nState)
  {
      SBT_WindowTrayID id;
      
      id.uiID = uiID;
      id.nState = nState;

      EnumAllGroups(EnumSetStateTrayIDWindows,&id);

          //If we found a window node matching the id we can refresh the interface data.
      if(id.hNode)
          GetParent()->PostMessage(WM_TREE_UPDATE,
                                   (WPARAM)id.hNode,
                                   (LPARAM)BT_NODE_WINDOW);
  }

void CBionicTree::RemoveAllGroupWindows()
  {
      CList<HTREEITEM> nodes;
      POSITION pos;

      if(GetParent()->SendMessage(WM_TREE_CHECK_AUTOGROUP))
          BlacklistAll();

      EnumAllGroups(EnumRemoveGroupWindows,&nodes);
      BOOL bUpdate = FALSE;

          //Delete all closed windows HTREEITEM's in tree.
      if((pos = nodes.GetHeadPosition()))
          bUpdate = TRUE;

      while(pos)
      {
          DeleteItem(nodes.GetAt(pos));
          nodes.GetNext(pos);
      }
      if(bUpdate)
      {
          RefreshImageList();
          GetParent()->PostMessage(WM_TREE_UPDATE,
                                   (WPARAM)GetRootItem(),
                                   (LPARAM)BT_NODE_ROOT);
      }
  }

void CBionicTree::CloseAllGroupWindows()
  {
      CList<HTREEITEM> nodes;
      POSITION pos;
      BOOL bUpdate = FALSE;
      
          //Close all windows and save thier HTREEITEM's in list.
      EnumAllGroups(EnumCloseGroupWindows,&nodes);

          //Delete all closed windows HTREEITEM's in tree.
      if((pos = nodes.GetHeadPosition()))
          bUpdate = TRUE;

      while(pos)
      {
          DeleteItem(nodes.GetAt(pos));
          nodes.GetNext(pos);
      }
      if(bUpdate)
      {
          RefreshImageList();
          GetParent()->PostMessage(WM_TREE_UPDATE,
                                   (WPARAM)GetRootItem(),
                                   (LPARAM)BT_NODE_ROOT);
      }
  }

HTREEITEM CBionicTree::InstallGroupFolder(LPCTSTR lpszName)
  {
      ASSERT(m_hRootNode);

      HTREEITEM hNewNode = NULL;
      TVINSERTSTRUCT tvs;
      TVITEM tvi;

      CString buffer = lpszName;
      CString delim;
      
      delim.LoadString(IDS_DELIMINATOR);

          //No REG. DELIM char in group text!
      if(buffer.Find(delim,0) != -1)
          buffer.Replace(delim,_T("-"));

      tvi.pszText = (LPTSTR)(LPCTSTR)buffer;
      tvi.mask = TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
      tvi.iImage = -1;          //Default 'blank' image to start out with.
      tvi.iSelectedImage = -1;

      tvi.lParam = 0; //Hotkey ATOM.

      tvs.hInsertAfter = m_hRootNode;
      tvs.hParent = m_hRootNode;
      tvs.item = tvi;

      if((hNewNode = InsertItem(&tvs)))
      {
          SetItemImage(hNewNode,BT_GROUP_ICON_INDEX,BT_GROUP_ICON_INDEX_SEL);
          Expand(m_hRootNode,TVE_EXPAND);
          Select(hNewNode,TVGN_CARET);

          m_nNumOfGroups++;

//           GetParent()->PostMessage(WM_TREE_UPDATE,
//                                    (WPARAM)hNewNode,
//                                    (LPARAM)BT_NODE_GROUP);
      }
      return hNewNode;
  }

BOOL CBionicTree::RemoveAllGroups()
  {
          //If we only have 2 nodes (root && group) then just return.
      if(BT_MINIMUM_NODES == GetCount())
          return FALSE;

          //Blacklist all windows under the root if we are in auto group mode.
      if(GetParent()->SendMessage(WM_TREE_CHECK_AUTOGROUP))
          BlacklistAll();

          //ENUM through all groups and release all *CBionicWindow
//    FreeAllGroupWindows();

      CList<HTREEITEM> groups;
      POSITION pos;

      GetAllGroups(groups);

      pos = groups.GetHeadPosition(); //Get the head position, the first group.
      groups.GetNext(pos);            //We need to keep the first group so just go to the next one.
      
      while(pos)
      {
          RemoveGroup((HTREEITEM)groups.GetAt(pos));
          groups.GetNext(pos);
      }
      RefreshImageList();
      m_nNumOfGroups = 1;

      GetParent()->PostMessage(WM_TREE_UPDATE,
                               (WPARAM)GetRootItem(),
                               (LPARAM)BT_NODE_ROOT);
      return TRUE;
  }

void CBionicTree::GetRootText(CString &cstrBuffer)
  {
      cstrBuffer = GetItemText(m_hRootNode);
  }

BOOL CBionicTree::RenameRootGroup(LPCSTR lpszText)
  {
      SelectItem(m_hRootNode);
      return SetItemText(m_hRootNode,lpszText);
  }

void CBionicTree::InstallRootNode()
  {
      TVINSERTSTRUCT tvs;
      TVITEM tvi;
      CString cstrRootDefault;

      cstrRootDefault.LoadString(IDS_ROOT_DEFAULT_NAME);

      tvs.hInsertAfter = TVI_LAST;
      tvi.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE;
      tvi.iImage = 0;
      tvi.iSelectedImage = 1;
      tvi.pszText = (LPSTR)(LPCTSTR)cstrRootDefault;

      tvs.hParent = TVI_ROOT;
      tvs.item = tvi;
      m_hRootNode = InsertItem(&tvs); //Store root HTREEITEM.    
  }

HTREEITEM CBionicTree::GetGroupByNum(UINT num)
  {
      SBT_GroupNum gdata;
      
      gdata.uiNum = num;
      gdata.uiCurrent = 0; //Start from the very first node.
      gdata.hGroup = NULL;
      
      EnumAllGroups(EnumGetGroupByNum,&gdata);

      return gdata.hGroup;
  }

int CBionicTree::GetGroupNum(HTREEITEM hGroup)
  {
      SBT_GroupNum gdata;
      int groupNum = -1;

      gdata.uiNum = 0;
      gdata.uiCurrent = 0; //Start from the very first node.
      gdata.hGroup = hGroup;
      
      if(!EnumAllGroups(EnumGetGroupNum,&gdata))
          groupNum = gdata.uiNum;

      return groupNum;
  }

int CBionicTree::GetGroupsCount()
  {
      int count = 0;
      EnumAllGroups(EnumGetGroupsCount,&count);
      return count;
  }

void CBionicTree::GetGroupNames(CList<CString> &groups)
  {
      EnumAllGroups(EnumGetGroupNames,&groups);
  }

//END ROOT METHODS.

//START GROUP METHODS.
BOOL CBionicTree::EnumGroup(HTREEITEM hNode,
                            BOOL (*pfDoIt)(HTREEITEM,
                                           CBionicTree*,
                                           CBionicWindow*,
                                           void*),
                            void *buf)
  {
      ASSERT(hNode);

      HTREEITEM hCurrent = GetChildItem(hNode);
      CBionicWindow *pWin = 0;

      while(hCurrent)
      {
          pWin = (CBionicWindow*)GetItemData(hCurrent);
          ASSERT(pWin);
          
          if(pWin)
          {
              if(pfDoIt(hCurrent,this,pWin,buf) == FALSE)
                  return FALSE;
          }
          hCurrent = GetNextItem(hCurrent,TVGN_NEXT);
      }
      return TRUE;
  }

void CBionicTree::ShowGroupWindows(HTREEITEM hGroup /* = NULL */)
  {
      if(!hGroup)
      {
          hGroup = GetDefaultNode(GetSelectedItem(),BT_NODE_GROUP);
          ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));
      }
      EnumGroup(hGroup,BT_ShowWindow,0);
      GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hGroup,(LPARAM)BT_NODE_GROUP);
  }

void CBionicTree::HideGroupWindows(HTREEITEM hGroup /* = NULL */)
  {
      if(!hGroup)
      {
          hGroup = GetDefaultNode(GetSelectedItem(),BT_NODE_GROUP);
          ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));
      }
      EnumGroup(hGroup,BT_HideWindow,0);
      GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hGroup,(LPARAM)BT_NODE_GROUP);
  }

void CBionicTree::RestGroupWindows()
  {
      HTREEITEM hGroup = GetSelectedItem();

      hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);
      ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));

      EnumGroup(hGroup,BT_RestWindow,0);
      GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hGroup,(LPARAM)BT_NODE_GROUP);
  }

void CBionicTree::MaxGroupWindows()
  {
      HTREEITEM hGroup = GetSelectedItem();

      hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);
      ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));

      EnumGroup(hGroup,BT_MaxWindow,0);
      GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hGroup,(LPARAM)BT_NODE_GROUP);
  }

void CBionicTree::MinGroupWindows()
  {
      HTREEITEM hGroup = GetSelectedItem();

      hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);
      ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));

      EnumGroup(hGroup,BT_MinWindow,0);
      GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hGroup,(LPARAM)BT_NODE_GROUP);
  }

void CBionicTree::RemoveGroupWindows(HTREEITEM hGroup /* = NULL */)
  {
      if(!hGroup)
      {
          hGroup = GetDefaultNode(GetSelectedItem(),BT_NODE_GROUP);
          ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));
      }
      CList<HTREEITEM> winhwnd;
      POSITION pos;

      if(GetParent()->SendMessage(WM_TREE_CHECK_AUTOGROUP))
          InsertBlacklistGroup(hGroup);

          //Store each windows HTREEITEM handle in the list.
      EnumGroup(hGroup,BT_RemoveWindow,&winhwnd);

      BOOL bUpdate = FALSE;

      if((pos = winhwnd.GetHeadPosition()))
          bUpdate = TRUE;

      while(pos)
      {
          DeleteItem(winhwnd.GetAt(pos));
          winhwnd.GetNext(pos);
      }
      if(bUpdate)
      {
          RefreshImageList();
          GetParent()->PostMessage(WM_TREE_UPDATE,
                                   (WPARAM)hGroup,
                                   (LPARAM)BT_NODE_GROUP);
      }
  }

void CBionicTree::CloseGroupWindows()
  {
      HTREEITEM hGroup = GetSelectedItem();
      CList<HTREEITEM> nodes;
      POSITION pos;

      hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);
      ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));

          //Store each windows HTREEITEM handle in the list.
      EnumGroup(hGroup,BT_CloseWindow,&nodes);

      pos = nodes.GetHeadPosition();
      while(pos)
      {
          DeleteItem(nodes.GetAt(pos));
          nodes.GetNext(pos);
      }
      RefreshImageList();
      GetParent()->PostMessage(WM_TREE_UPDATE,
                               (WPARAM)hGroup,
                               (LPARAM)BT_NODE_GROUP);
  }

BOOL CBionicTree::RemoveGroup(HTREEITEM hGroup)
  {
      BOOL bFeedback = FALSE;

      if(ItemHasChildren(hGroup))
      {
          TRACE("::RemoveGroup() - Removing children.\n");
          EnumGroup(hGroup,BT_FreeWindow,0);
      }
      CHotKeyMan::HotKeyID hotID;

          //If this group node has a hotkey installed, unregister it before we destroy it.
      if((hotID = (CHotKeyMan::HotKeyID)GetItemData(hGroup)))
          m_hotkeyGroups.UnregHotkey(hotID);

      bFeedback = DeleteItem(hGroup);

      RefreshImageList();
      m_nNumOfGroups--;

          //Since we removed a group node, we need to resync. all hotkey IDs.
      RefreshGroupsHotkeyID();

      return bFeedback;
  }

BOOL CBionicTree::RemoveSelectedGroup()
  {
      HTREEITEM hGroup = GetSelectedItem();
      BOOL bFeedback = FALSE;

          //At least 2 groups (root && default group)
      if(GetCount() == BT_MINIMUM_NODES)
          return bFeedback;

      hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);
      ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));

      if(GetParent()->SendMessage(WM_TREE_CHECK_AUTOGROUP))
          InsertBlacklistGroup(hGroup);

      bFeedback = RemoveGroup(hGroup);

      GetParent()->PostMessage(WM_TREE_UPDATE,
                               (WPARAM)GetRootItem(),
                               (LPARAM)BT_NODE_ROOT);
      return bFeedback;
  }

void CBionicTree::GetNodeText(CString &cstrBuffer,
                              BT_TREE_NODE_TYPE eNodeType)
  {
      HTREEITEM hNode = GetDefaultNode(GetSelectedItem(),eNodeType);
      cstrBuffer = GetItemText(hNode);
  }

BOOL CBionicTree::RenameSelectedGroup(LPCTSTR lpszText)
  {
      HTREEITEM hGroup = GetSelectedItem();
      BOOL bSuccess = FALSE;

      CString buffer = lpszText;
      CString delim;
      
      delim.LoadString(IDS_DELIMINATOR);

          //No REG. DELIM char in group text!
      if(buffer.Find(delim,0) != -1)
          buffer.Replace(delim,_T("-"));

      hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);
      ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));

      SelectItem(hGroup);
      bSuccess = SetItemText(hGroup,buffer);

      return bSuccess;
  }

void CBionicTree::TileGroupWindows(BT_TILE_CONFIG config)
  {
      HTREEITEM hGroup = GetSelectedItem();

      hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);
      ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));

      ShowGroupWindows(hGroup);

      LPARAM param = MAKELPARAM(0,config);
      EnumGroup(hGroup,BT_TileWindow,&param);
  }

void CBionicTree::GroupMemory()
  {
      HTREEITEM hGroup = GetSelectedItem();
      hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);

      EnumGroup(hGroup,BT_GroupMemory,0);
  }

void CBionicTree::GroupRecall()
  {
      HTREEITEM hGroup = GetSelectedItem();
      hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);

      EnumGroup(hGroup,BT_GroupRecall,0);
  }

void CBionicTree::GroupSort(HTREEITEM hGroup /* = NULL*/)
  {
      if(!hGroup)
      {
          hGroup = GetSelectedItem();
          hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);
      }
      SortChildren(hGroup);
  }

//END GROUP METHODS.

//START WINDOW METHODS.

//START WINDOW ENUM. METHODS.
BOOL CBionicTree::BT_WindowGroupInfo(HTREEITEM hNode,
                                     CBionicTree *pParentTree,
                                     CBionicWindow *pWin,
                                     void *buf)
  {
      SBT_GroupInfo *pinfo = (SBT_GroupInfo*)buf;

      pinfo->ncount++;

      if(pWin && pWin->GetState())
          pinfo->nvisible++;

      return TRUE;
  }

BOOL CBionicTree::BT_CountWindow(HTREEITEM hNode,
                                 CBionicTree *pParentTree,
                                 CBionicWindow *pWin,
                                 void *buf)
  {
          //Increment the window count.
      int *pn = (int*)buf;
      (*pn)++;

      return TRUE;
  }

BOOL CBionicTree::BT_SetStateIDWindow(HTREEITEM hNode,
                                      CBionicTree *pParentTree,
                                      CBionicWindow *pWin,
                                      void *buf)
  {
      if(buf)
      {
          SBT_WindowTrayID *id = (SBT_WindowTrayID*)buf;
          if(id->uiID == pWin->GetTrayID())
          {
              if(id->nState == SW_SHOW)
                  BT_ShowWindow(hNode,pParentTree,pWin,0);
              else
                  BT_HideWindow(hNode,pParentTree,pWin,0);

              id->hNode = hNode; //Store this windows hNode to refresh the interface data.
              pWin->SetTrayState((id->nState == SW_SHOW) ? FALSE : TRUE);

              return FALSE;
          }
      }
      return TRUE;
  }

BOOL CBionicTree::BT_ValidateWindow(HTREEITEM hNode,
                                    CBionicTree *pParentTree,
                                    CBionicWindow *pWin,
                                    void *buf)
  {
      HWND hWndValidate = *(HWND*)&buf;

      ASSERT(hWndValidate);
      ASSERT(pWin);

      if(pWin->GetHandle() == hWndValidate)
          return FALSE;

      return TRUE;
  }

BOOL CBionicTree::BT_FreeWindow(HTREEITEM hNode,
                                CBionicTree *pParentTree,
                                CBionicWindow *pWin,
                                void *buf)
  {
      ASSERT(pParentTree);
      ASSERT(pWin);
      ASSERT((CBionicWindow*)pParentTree->GetItemData(hNode) == pWin);

      if(pWin)
      {
          delete pWin;
          pWin = 0;
      }
      else
          return FALSE;

      return TRUE;
  }

BOOL CBionicTree::BT_RefreshTreeImage(HTREEITEM hNode,
                                      CBionicTree *pParentTree,
                                      CBionicWindow *pWin,
                                      void *buf)
  {
      ASSERT(pParentTree);
      ASSERT(BT_NODE_WINDOW == pParentTree->GetTreeNodeType(hNode));
      ASSERT(pWin);

      HICON hIcon = NULL;

      hIcon = pWin->RefreshIcon();
      ASSERT(hIcon);

      pParentTree->m_treeImageList.Add(hIcon);

      int nIndex = pParentTree->m_treeImageList.GetImageCount()-1;
      pWin->SetImageIndex(nIndex);
      pParentTree->SetItemImage(hNode,nIndex,nIndex);

      return TRUE;
  }

BOOL CBionicTree::BT_HideWindow(HTREEITEM hNode,
                                CBionicTree *pParentTree,
                                CBionicWindow *pWin,
                                void *buf)
  {
      ASSERT(pWin);

      if(pWin->GetState())
          pWin->SetState(SW_HIDE);

      return TRUE;
  }

BOOL CBionicTree::BT_ShowWindow(HTREEITEM hNode,
                                CBionicTree *pParentTree,
                                CBionicWindow *pWin,
                                void *buf)
  {
      ASSERT(pWin);

      if(!pWin->GetState())
          pWin->SetState(SW_SHOW);

      return TRUE;
  }

BOOL CBionicTree::BT_RestWindow(HTREEITEM hNode,
                                CBionicTree *pParentTree,
                                CBionicWindow *pWin,
                                void *buf)
  {
      ASSERT(pWin);

      if(!pWin->GetState())
          pWin->SetState(SW_SHOW);

      pWin->SetState(SW_RESTORE);

      return TRUE;
  }

BOOL CBionicTree::BT_MaxWindow(HTREEITEM hNode,
                               CBionicTree *pParentTree,
                               CBionicWindow *pWin,
                               void *buf)
  {
      ASSERT(pWin);

      if(!pWin->GetState())
          pWin->SetState(SW_SHOW);

      pWin->SetState(SW_MAXIMIZE);

      return TRUE;
  }

BOOL CBionicTree::BT_MinWindow(HTREEITEM hNode,
                               CBionicTree *pParentTree,
                               CBionicWindow *pWin,
                               void *buf)
  {
      ASSERT(pParentTree);
      ASSERT(pWin);

      if(!pWin->GetState())
          pWin->SetState(SW_SHOW);

      pWin->SetState(SW_MINIMIZE);

      return TRUE;
  }

BOOL CBionicTree::BT_CloseWindow(HTREEITEM hNode,
                                 CBionicTree *pParentTree,
                                 CBionicWindow *pWin,
                                 void *buf)
  {
      ASSERT(pWin);

          //We may need to store this windows HTREEITEM handle to delete it later...
      if(buf)
      {
          CList<HTREEITEM>* pLst = (CList<HTREEITEM>*)buf;
          pLst->AddTail(hNode);
      }
      pWin->Close();
      BT_FreeWindow(hNode,pParentTree,pWin,0);

      return TRUE;
  }

BOOL CBionicTree::BT_RemoveWindow(HTREEITEM hNode,
                                  CBionicTree *pParentTree,
                                  CBionicWindow *pWin,
                                  void *buf)
  {
      ASSERT(pWin);

      if(buf)
      {
          CList<HTREEITEM>* pLst = (CList<HTREEITEM>*)buf;
          pLst->AddTail(hNode);
      }
      BT_FreeWindow(hNode,pParentTree,pWin,0);

      return TRUE;
  }

BOOL CBionicTree::BT_IsWindow(HTREEITEM hNode,
                              CBionicTree *pParentTree,
                              CBionicWindow *pWin,
                              void *buf)
  {
      ASSERT(pParentTree);
      ASSERT(pWin);
      ASSERT(buf);

      HWND hWnd = pWin->GetHandle();

          //Make sure window is still running else remove it from the tree.
      if(FALSE == IsWindow(hWnd))
      {
          CList<HTREEITEM> *pitems = (CList<HTREEITEM>*)buf;
          ASSERT(pitems);

          pitems->AddTail(hNode);
      }
      else if(!pWin->GetPreserveName()) //If we don't need to preserve the window text, refresh it.
      {
              //Valid window node - refresh node text via caption bar.
          TCHAR buffer[255];

          ::GetWindowText(hWnd,buffer,255);
          pParentTree->SetItemText(hNode,buffer);
      }
      return TRUE;
  }

BOOL CBionicTree::BT_TileWindow(HTREEITEM hNode,
                                CBionicTree *pParentTree,
                                CBionicWindow *pWin,
                                void *buf)
  {
      LPARAM *pTileInfo = (LPARAM*)buf;
      ASSERT(pTileInfo);

      DWORD nTileConfig = 0;
      DWORD nTileNum = 1;

      nTileNum = LOWORD(*pTileInfo); //Extract the current window tile num.
      nTileConfig = HIWORD(*pTileInfo); //Tile config - 1x1V, 1x1H, or 2x2
  
      RECT rcWorkArea;
      DWORD screenx,screeny,tempx,tempy;

      RECT rcClient;
      DWORD left,top,right,bottom;

      SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
      screenx = rcWorkArea.right;
      screeny = rcWorkArea.bottom;

      ASSERT(pWin);
      HWND hWin = pWin->GetHandle();

      switch(nTileConfig)
      {
          case BT_TILE_1X1H:

              if(nTileNum >= 2)
                  return FALSE;

              tempx = screenx;
              tempy = screeny/2;

                  //First window.
              if(nTileNum == 0)
              {
                  ::GetClientRect(hWin,&rcClient);

                  left = rcWorkArea.left;
                  top = rcWorkArea.top;
              }
              else
              {
                  left = rcWorkArea.left;
                  top = tempy;
              }
              right = tempx;
              bottom = tempy;

              break;

          case BT_TILE_1X1V:

              if(nTileNum >= 2)
                  return FALSE;

              tempx = screenx/2;
              tempy = screeny;

              if(nTileNum == 0)
              {
                  ::GetClientRect(hWin,&rcClient);

                  left = rcWorkArea.left;
                  top = rcWorkArea.top;
              }
              else
              {
                  left = tempx;
                  top = rcWorkArea.top;
              }
              right = tempx;
              bottom = tempy;

              break;
              
          case BT_TILE_2X2:

              if(nTileNum >= 4)
                  return FALSE;

              tempx = screenx/2;
              tempy = screeny/2;

              if(nTileNum == 0)
              {
                  ::GetClientRect(hWin,&rcClient);

                  left = rcWorkArea.left;
                  top = rcWorkArea.top;
                  right = tempx;
                  bottom = tempy;
              }
              else if(nTileNum == 1)
              {
                  left = tempx;
                  top = rcWorkArea.top;
                  right = tempx;
                  bottom = tempy;
              }
              else if(nTileNum == 2)
              {
                  left = tempx;
                  top = tempy;
                  right = tempx;
                  bottom = tempy;
              }
              else
              {
                  left = rcWorkArea.left;
                  top = tempy;
                  right = tempx;
                  bottom = tempy;
              }
              break;
      }
      ::SetWindowPos(hWin,
                     HWND_TOP,
                     left,top,
                     right,bottom,
                     SWP_SHOWWINDOW);

          //Increase the tile count and pack it back into void* for the next tile.
      *pTileInfo = MAKELPARAM(++nTileNum,nTileConfig);

      return TRUE;
  }

BOOL CBionicTree::BT_GroupMemory(HTREEITEM hNode,
                                 CBionicTree *pParentTree,
                                 CBionicWindow *pWin,
                                 void *buf)
  {
      ASSERT(pWin);
      if(pWin)
          pWin->SetMemory();
      
      return TRUE;
  }

BOOL CBionicTree::BT_GroupRecall(HTREEITEM hNode,
                                 CBionicTree *pParentTree,
                                 CBionicWindow *pWin,
                                 void *buf)
  {
      ASSERT(pWin);
      if(pWin)
          pWin->Recall();

      return TRUE;
  }

BOOL CBionicTree::BT_BlackListWindow(HTREEITEM hNode,
                                     CBionicTree *pParentTree,
                                     CBionicWindow *pWin,
                                     void *buf)
  {
      CList<HWND> *pBlacklist = (CList<HWND>*)buf;
      ASSERT(buf);
      
      pBlacklist->AddTail(pWin->GetHandle());

      return TRUE;
  }

BOOL CBionicTree::BT_MarkForegroundWindow(HTREEITEM hNode,
                                          CBionicTree *pParentTree,
                                          CBionicWindow *pWin,
                                          void *buf)
  {
      HWND hWnd = *(HWND*)buf;
      ASSERT(hWnd);
      
      if(pWin->GetHandle() == hWnd)
      {
#ifdef _DEBUG
          char buffer[255];
          ::GetWindowText(pWin->GetHandle(),buffer,255);
          TRACE("Foreground mark: %s\n",buffer);
#endif

          pWin->MarkForeground();
          return FALSE;
      }
      return TRUE;
  }

//END WINDOW ENUM. METHODS.

void CBionicTree::HideWindow()
  {
      HTREEITEM hNode = GetSelectedItem();
      CBionicWindow *pWin = 0;

      hNode = GetDefaultNode(hNode,BT_NODE_WINDOW);
      if(hNode)
      {
          pWin = (CBionicWindow*)GetItemData(hNode);
          ASSERT(pWin);
      
          if(pWin)
          {
              BT_HideWindow(hNode,this,pWin,0);
              GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hNode,(LPARAM)BT_NODE_WINDOW);
          }
      }
  }

void CBionicTree::ShowWindow()
  {
      HTREEITEM hNode = GetSelectedItem();
      CBionicWindow *pWin = 0;

      hNode = GetDefaultNode(hNode,BT_NODE_WINDOW);
      if(hNode)
      {
          pWin = (CBionicWindow*)GetItemData(hNode);
          ASSERT(pWin);
      
          if(pWin)
          {
              BT_ShowWindow(hNode,this,pWin,0);
              GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hNode,(LPARAM)BT_NODE_WINDOW);
          }
      }
  }

void CBionicTree::MinWindow()
  {
      HTREEITEM hNode = GetSelectedItem();
      CBionicWindow *pWin = 0;

      hNode = GetDefaultNode(hNode,BT_NODE_WINDOW);
      if(hNode)
      {
          pWin = (CBionicWindow*)GetItemData(hNode);
          ASSERT(pWin);
      
          if(pWin)
          {
              pWin->SetState(SW_MINIMIZE);
              GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hNode,(LPARAM)BT_NODE_WINDOW);
          }
      }
  }

void CBionicTree::MaxWindow()
  {
      HTREEITEM hNode = GetSelectedItem();
      CBionicWindow *pWin = 0;

      hNode = GetDefaultNode(hNode,BT_NODE_WINDOW);
      if(hNode)
      {
          pWin = (CBionicWindow*)GetItemData(hNode);
          ASSERT(pWin);
      
          if(pWin)
          {
              pWin->SetState(SW_MAXIMIZE);
              GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hNode,(LPARAM)BT_NODE_WINDOW);
          }
      }
  }

void CBionicTree::RestWindow()
  {
      HTREEITEM hNode = GetSelectedItem();
      CBionicWindow *pWin = 0;

      hNode = GetDefaultNode(hNode,BT_NODE_WINDOW);
      if(hNode)
      {
          pWin = (CBionicWindow*)GetItemData(hNode);
          ASSERT(pWin);
      
          if(pWin)
          {
              pWin->SetState(SW_RESTORE);
              GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hNode,(LPARAM)BT_NODE_WINDOW);
          }
      }
  }

BOOL CBionicTree::GetTrayIconData(SBT_TrayData *pTrayData)
  {
      HTREEITEM hNode = GetSelectedItem();
      CBionicWindow *pWin = 0;
      BOOL bSuccess = FALSE;

      hNode = GetDefaultNode(hNode,BT_NODE_WINDOW);
      if(hNode)
      {
          pWin = (CBionicWindow*)GetItemData(hNode);
          ASSERT(pWin);
      
          if(pWin)
          {
              TCHAR cBuffer[255];

              pTrayData->id = pWin->GetTrayID();
              pTrayData->hIcon = pWin->RefreshIcon();
              pTrayData->hWnd = pWin->GetHandle();

              ::GetWindowText(pTrayData->hWnd,cBuffer,255);
              pTrayData->cstrBuffer = cBuffer;

              BT_HideWindow(hNode,this,pWin,0);

              pWin->SetTrayState(TRUE);
              bSuccess = TRUE;

              GetParent()->PostMessage(WM_TREE_UPDATE,
                                       (WPARAM)hNode,
                                       (LPARAM)BT_NODE_WINDOW);
          }
      }
      return bSuccess;
  }

void CBionicTree::ForegroundWindow()
  {
      HTREEITEM hNode = GetSelectedItem();
      CBionicWindow *pWin = 0;

      hNode = GetDefaultNode(hNode,BT_NODE_WINDOW);
      if(hNode)
      {
          pWin = (CBionicWindow*)GetItemData(hNode);
          ASSERT(pWin);
      
          if(pWin)
              ::SetForegroundWindow(pWin->GetHandle());
      }      
  }

void CBionicTree::RemoveWindow(BOOL bClose,    /* = FALSE */
                               HTREEITEM hNode /* = NULL */)
  {
      if(!hNode)
          hNode = GetDefaultNode(GetSelectedItem(),BT_NODE_WINDOW);

      if(hNode)
      {
          CBionicWindow *pWin = (CBionicWindow*)GetItemData(hNode);
          ASSERT(pWin);

              //Ask parent if we need to blacklist this HWND - Is autogroup mode set?
          if(GetParent()->SendMessage(WM_TREE_CHECK_AUTOGROUP))
              m_blackList.AddTail(pWin->GetHandle());

          HTREEITEM hParentGroup = GetParentItem(hNode);
          if(pWin)
          {
              if(bClose)
                  pWin->Close();

              BT_FreeWindow(hNode,this,pWin,0);
          }
          DeleteItem(hNode);

          RefreshImageList();
          GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hParentGroup,(LPARAM)BT_NODE_GROUP);
      }
  }

BOOL CBionicTree::RenameWindow(LPCTSTR lpszText)
  {
      HTREEITEM hWin = GetSelectedItem();
      hWin = GetDefaultNode(hWin,BT_NODE_WINDOW);

      CBionicWindow *pWin = (CBionicWindow*)GetItemData(hWin);
      ASSERT(pWin);
      
      pWin->SetPreserveName(TRUE);
      SelectItem(hWin);

      return SetItemText(hWin,lpszText);
  }

BOOL CBionicTree::GetAlwaysOnTop()
  {
      HTREEITEM hWin = GetSelectedItem();
      BOOL bTopMost = FALSE;

      if((hWin = GetDefaultNode(hWin,BT_NODE_WINDOW)))
      {
          CBionicWindow *pWin = (CBionicWindow*)GetItemData(hWin);
          ASSERT(pWin);
          bTopMost = pWin->GetAlwaysOnTop();
      }
      return bTopMost;
  }

void CBionicTree::ToggleAlwaysOnTop()
  {
      HTREEITEM hWin = GetSelectedItem();

      if((hWin = GetDefaultNode(hWin,BT_NODE_WINDOW)))
      {
          CBionicWindow *pWin = (CBionicWindow*)GetItemData(hWin);
          ASSERT(pWin);

          if(pWin)
              pWin->ToggleAlwaysOnTop();
      }
  }

BOOL CBionicTree::WindowPositionMemory()
  {
      HTREEITEM hWin = GetSelectedItem();

      if((hWin = GetDefaultNode(hWin,BT_NODE_WINDOW)))
      {
          CBionicWindow *pWin = (CBionicWindow*)GetItemData(hWin);
          ASSERT(pWin);

          if(pWin)
              pWin->SetMemory();

          return TRUE;
      }
      return FALSE;
  }

void CBionicTree::WindowPositionRecall()
  {
      HTREEITEM hWin = GetSelectedItem();

      if((hWin = GetDefaultNode(hWin,BT_NODE_WINDOW)))
      {
          CBionicWindow *pWin = (CBionicWindow*)GetItemData(hWin);
          ASSERT(pWin);

          if(pWin)
              pWin->Recall();
      }
  }

BOOL CALLBACK EnumChildExProc(HWND hChild,
                              LPARAM lParam)
  {
      TCHAR cClassName[5];
      TCHAR *pBuffer;
         
      if(GetClassName(hChild,cClassName,5)>0 &&
         lstrcmp(cClassName,_T("Edit"))==0)
      {
          pBuffer = (TCHAR*)lParam;
          SendMessage(hChild,WM_GETTEXT,(WPARAM)MAX_PATH,(LPARAM)pBuffer);
          return FALSE;
      }
      return TRUE;
  }

void CBionicTree::WindowExConsole()
  {
      HTREEITEM hWin = GetSelectedItem();

      if((hWin = GetDefaultNode(hWin,BT_NODE_WINDOW)))
      {
          CBionicWindow *pWin = (CBionicWindow*)GetItemData(hWin);
          ASSERT(pWin);

          if(pWin)
          {
              TCHAR cBuffer[MAX_PATH] = _T("INVALID");
              TCHAR cDefaultDirectory[MAX_PATH];

              GetCurrentDirectory(MAX_PATH,cDefaultDirectory);
              EnumChildWindows(pWin->GetHandle(),
                               (WNDENUMPROC)EnumChildExProc,(LPARAM)cBuffer);

              if(lstrcmp(_T("INVALID"),cBuffer) != 0 &&
                 SetCurrentDirectory(cBuffer))
              {
                  OSVERSIONINFO ov;
                  CString cmd;

                  ZeroMemory(&ov,sizeof(OSVERSIONINFO));
                  ov.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
                  GetVersionEx(&ov);

                  cmd = (VER_PLATFORM_WIN32_NT == ov.dwPlatformId) ? SHELL_CMD_NT : SHELL_CMD_95;
                  
                  ShellExecute(NULL,NULL,cmd,NULL,NULL,SW_SHOW);
                  SetCurrentDirectory(cDefaultDirectory);           //return to app's originial directory
              }
          }
      }
  }

void CBionicTree::WindowExClone()
  {
      HTREEITEM hWin = GetSelectedItem();

      if((hWin = GetDefaultNode(hWin,BT_NODE_WINDOW)))
      {
          CBionicWindow *pWin = (CBionicWindow*)GetItemData(hWin);
          ASSERT(pWin);

          TCHAR cBuffer[MAX_PATH] = _T("INVALID");
          CString strPath;
          
          EnumChildWindows(pWin->GetHandle(),
                           (WNDENUMPROC)EnumChildExProc,(LPARAM)cBuffer);

          if(lstrcmp(_T("INVALID"),cBuffer)!=0)
          {
              strPath.Format(_T("/e,%s"),cBuffer);
              ShellExecute(0,0,_T("EXPLORER.EXE"),
                           strPath,0,SW_SHOW);
          }
      }
  }

//END WINDOW METHODS.

//Load up image list for this sub-classed tree.
void CBionicTree::LoadTreeImages()
  {
//    m_treeImageList.Create(16,16,ILC_COLOR16,0,4);

      m_treeImageList.Create(16,16,ILC_COLOR16,4,64);
      m_treeImageList.SetBkColor(GetSysColor(COLOR_WINDOW));

      m_treeImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_ROOT));
      m_treeImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_ROOT_OPEN));
      m_treeImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_FOLDER));
      m_treeImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_FOLDER_OPEN));

      SetImageList(&m_treeImageList,TVSIL_NORMAL);
  }

BEGIN_MESSAGE_MAP(CBionicTree,CTreeCtrl)
ON_NOTIFY_REFLECT(NM_RCLICK,OnTreeRightClick)
ON_NOTIFY_REFLECT(NM_CLICK,OnTreeClick)
ON_NOTIFY_REFLECT(NM_DBLCLK,OnTreeDblClick)
ON_NOTIFY_REFLECT(TVN_BEGINDRAG,OnBeginDrag)
ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
ON_MESSAGE(WM_HOTKEY,OnHotKey)
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_TIMER()
END_MESSAGE_MAP()

afx_msg void CBionicTree::OnTreeRightClick(NMHDR *pNMHDR,
                                           LRESULT *pResult)
  {
      pResult = 0;

      HTREEITEM hTreeItem = 0;
      POINT pt;
      UINT flag;

      GetCursorPos(&pt);
      ScreenToClient(&pt);
      if((hTreeItem = HitTest(pt,&flag)))//notify parent about right click
      {
          SelectItem(hTreeItem);
          GetParent()->PostMessage(WM_TREE_RIGHT_CLICK,
                                   (WPARAM)hTreeItem,
                                   (LPARAM)GetTreeNodeType(hTreeItem));
      }
  }

//Double clicking will have a default hide operation.
afx_msg void CBionicTree::OnTreeDblClick(NMHDR *pNMHDR,
                                         LRESULT *pResult)
  {
      CPoint pt(GetMessagePos());
      ScreenToClient(&pt);

      HTREEITEM hItem = NULL;
      UINT nFlags = 0;

      hItem = HitTest(pt,&nFlags);

      BT_TREE_NODE_TYPE eNodeType = GetTreeNodeType(hItem);
          //Act upon only icon and label clicks...
      if(hItem && ((TVHT_ONITEMICON | TVHT_ONITEMLABEL) & nFlags))
      {
          switch(eNodeType)
          {
              case BT_NODE_ROOT:
                  EnumAllGroups(EnumHideGroupWindows,0);
                  break;

              case BT_NODE_GROUP:
                  EnumGroup(hItem,BT_HideWindow,0);
                  break;

              case BT_NODE_WINDOW:

                  CBionicWindow *pWin = (CBionicWindow*)GetItemData(hItem);
                  ASSERT(pWin);
               
                  if(pWin && pWin->GetState())
                      BT_HideWindow(hItem,this,pWin,0);

                  break;
          }
          GetParent()->PostMessage(WM_TREE_UPDATE,
                                   (WPARAM)hItem,
                                   (LPARAM)eNodeType);
      }
      *pResult = 1; //Prevent expanding/collapsing of node.
  }

afx_msg void CBionicTree::OnTreeClick(NMHDR *pNMHDR,
                                      LRESULT *pResult)
  {
      CPoint pt(GetMessagePos());
      ScreenToClient(&pt);

      HTREEITEM hItem = NULL;
      UINT nFlags = 0;

      hItem = HitTest(pt,&nFlags);

      BT_TREE_NODE_TYPE eNodeType = GetTreeNodeType(hItem);
          //Act upon only icon, label clicks, and clicking in the indentation area of node.
      if(hItem && ((TVHT_ONITEMICON | TVHT_ONITEMLABEL | TVHT_ONITEMINDENT | TVHT_ONITEMRIGHT) & nFlags))
      {
              //If we are in group switch mode and we have a group node selected...
          if(m_bSwitchGroup && BT_NODE_WINDOW != eNodeType)
          {
              if(eNodeType == BT_NODE_GROUP ||
                 eNodeType == BT_NODE_ROOT)
              {
                      //This should only be called from root menu...
                      //HideAllGroupWindows();

                  winforeinfo winfore;
                  winfore.start = 0;
                  winfore.end = 1;

                      //Enum. all desktop windows and mark the one window that has the focus.
                  EnumWindows((WNDENUMPROC)EnumGetForegroundProc,(LPARAM)&winfore);
                  MarkForegroundWindow(winfore.hFocused);

                  EnumAllGroups(EnumHideGroupWindows,0);

                  if(eNodeType == BT_NODE_GROUP)
                      EnumGroup(hItem,BT_ShowWindow,0);
              }
          }
          else if(m_bSwitchWindow && BT_NODE_WINDOW == eNodeType)
          {
              CBionicWindow *pWin = (CBionicWindow*)GetItemData(hItem);
              ASSERT(pWin);
               
              if(pWin)
              {
                  WINDOWPLACEMENT wstate;
                  HWND hWnd = pWin->GetHandle();

                      //If window is minimized, restore then make visible.
                  if(::GetWindowPlacement(hWnd,&wstate) &&
                     SW_SHOWMINIMIZED == wstate.showCmd)
                      pWin->SetState(SW_RESTORE);

                  if(!pWin->GetState())
                      BT_ShowWindow(hItem,this,pWin,0);

                  ::SetForegroundWindow(hWnd);
              }
          }
          GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hItem,(LPARAM)eNodeType);
      }
  }

afx_msg void CBionicTree::OnTreeSelChanged(NMHDR *pNMHDR,
                                           LRESULT *pResult)
  {}

//Handle dragging/dropping of nodes.
afx_msg void CBionicTree::OnBeginDrag(NMHDR *pNMHDR,
                                      LRESULT *pResult)
  {
      *pResult = 0;

      NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW*)pNMHDR;
//    CImageList *m_pDragImage = NULL;

      if(m_pDragImage)
      {
          delete m_pDragImage;
          m_pDragImage = NULL;
      }
      POINT cursorPos(pNMTreeView->ptDrag);
      HTREEITEM hTreeItem = HitTest(cursorPos);
      HTREEITEM hTreeParent = NULL;

      if(!(hTreeParent = GetParentItem(hTreeItem)))
          return;

      m_hItemDrag = pNMTreeView->itemNew.hItem;
      m_hItemDrop = NULL;

      SelectItem(hTreeItem);
      m_pDragImage = CreateDragImage(hTreeItem);
      m_bDragging = TRUE;
      m_pDragImage->BeginDrag(0,CPoint(15,15));
      ClientToScreen(&cursorPos);
      m_pDragImage->DragEnter(0,cursorPos);
      SetCapture();
  }

afx_msg void CBionicTree::OnLButtonUp(UINT nFlags,
                                      CPoint point)
  {
      CTreeCtrl::OnLButtonUp(nFlags,point);
      if(m_bDragging)
      {
          m_bDragging = FALSE;
          CImageList::DragLeave(this);
          CImageList::EndDrag();
          ReleaseCapture();
          SelectDropTarget(NULL);

          if(m_pDragImage)
          {
              delete m_pDragImage;
              m_pDragImage = NULL;
          }
          CRect rc;
          POINT pt = point;
          ClientToScreen(&pt);

          GetParent()->GetWindowRect(rc); //Check if we are dragging outside of our parent's client area.
          BT_TREE_NODE_TYPE type = GetTreeNodeType(m_hItemDrop);

          if(rc.PtInRect(pt)== FALSE &&
             BT_NODE_ROOT != type)
          {
              HTREEITEM hGroup;

              if(BT_NODE_WINDOW == type)
              {
                  hGroup = GetParentItem(m_hItemDrop);
                  RemoveWindow(FALSE,m_hItemDrop);
              }
              else if(BT_NODE_GROUP == type)
              {
                  hGroup = m_hItemDrop;
                  RemoveGroupWindows(hGroup);
              }
              GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hGroup,BT_NODE_GROUP);
          }
          else //else we are still inside the client area of the CTreeCtrl || our parent
          {
                  //If drag/drop is the same node, just update the selection information
              if(m_hItemDrag == m_hItemDrop)
              {
                  GetParent()->PostMessage(WM_TREE_UPDATE,
                                           (WPARAM)m_hItemDrag,
                                           (LPARAM)GetTreeNodeType(m_hItemDrag));
                  return;
              }
              DragNodeToNode(m_hItemDrag,m_hItemDrop);
          }
      }
  }

void CBionicTree::DragNodeToNode(HTREEITEM hDrag,
                                 HTREEITEM hDrop)
  {
      BT_TREE_NODE_TYPE eNodeType = GetTreeNodeType(hDrag);
      BT_TREE_NODE_TYPE eNodeTypeDrop = GetTreeNodeType(hDrop);

      CBionicWindow *pWin = 0;

      if(eNodeType == BT_NODE_GROUP &&
         eNodeTypeDrop == BT_NODE_WINDOW)
          return;

      switch(eNodeTypeDrop)
      {
          case BT_NODE_GROUP:
          {
              if(eNodeType == BT_NODE_WINDOW)
              {
                  pWin = (CBionicWindow*)GetItemData(hDrag);
                  ASSERT(pWin!=0);

                  CopyWindowToGroup(pWin,hDrop);
                  DeleteItem(hDrag);
              }
              else if(eNodeType == BT_NODE_GROUP)
              {
                  if(ItemHasChildren(hDrag))
                  {
                      HTREEITEM hDragChild = GetNextItem(hDrag,TVGN_CHILD);
                      HTREEITEM hTemp = NULL;
                      while(NULL!=hDragChild)
                      {
                          pWin = (CBionicWindow*)GetItemData(hDragChild);
                          ASSERT(pWin!=0);

                          CopyWindowToGroup(pWin,hDrop);
                          hTemp = hDragChild;
                          hDragChild = GetNextItem(hDragChild,TVGN_NEXT);
                          DeleteItem(hTemp);
                      }
                  }
              }
          }
          break;

          case BT_NODE_WINDOW:

              HTREEITEM hGroup = GetParentItem(hDrop);
              if(hGroup) //(hGroup == GetParentItem(hDrag))
              {
                  HTREEITEM hNewNode,hInsertBefore;
                  TVINSERTSTRUCT tvs;
                  TCHAR cBuffer[255];
                  TVITEM tvi;

                  pWin = (CBionicWindow*)GetItemData(hDrag);
                  ASSERT(pWin!=0);

                  ::GetWindowText(pWin->GetHandle(),cBuffer,255);
                  tvi.pszText = (LPTSTR)cBuffer;
                  tvi.mask = TVIF_PARAM | TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
                  tvi.lParam = (LPARAM)pWin;
                  tvi.iImage = -1;
                  tvi.iSelectedImage = -1;

                  hInsertBefore = GetNextItem(hDrop,TVGN_PREVIOUS);

                      //If we are dragging one node down...
                  if(hInsertBefore == hDrag)
                      tvs.hInsertAfter = GetNextItem(hDrag,TVGN_NEXT);
                  else
                  {
                          //If we are dragging one node up...
                      if(NULL == hInsertBefore)
                          tvs.hInsertAfter = TVI_FIRST;
                      else
                          tvs.hInsertAfter = hInsertBefore;
                  }
                  tvs.hParent = hGroup;
                  tvs.item = tvi;
                  
                  if((hNewNode = InsertItem(&tvs)))
                  {
                      int nIndex = pWin->GetImageIndex();
                      SetItemImage(hNewNode,nIndex,nIndex);
                      SelectItem(hNewNode);
                      DeleteItem(hDrag);

                      GetParent()->PostMessage(WM_TREE_UPDATE,
                                               (WPARAM)hNewNode,
                                               (LPARAM)BT_NODE_WINDOW);
                  }
              }
              break;
      }
  }

LRESULT CBionicTree::OnMouseLeave(WPARAM wParam,
                                  LPARAM lParam)
  {
      ASSERT(m_bMouseTrack);

      m_bMouseTrack = FALSE;
//    Invalidate();
      GetParent()->PostMessage(WM_TREE_MOUSE_LEAVE,0);

      return 0;
  }

afx_msg void CBionicTree::OnMouseMove(UINT nFlags,
                                      CPoint point)
  {
      if(m_bDragging)
      {
          HTREEITEM hitem;
          UINT flags;
          POINT pt = point;
          ClientToScreen(&pt);
          CImageList::DragMove(pt);
          if((hitem = HitTest(point,&flags)))
          {
              CImageList::DragShowNolock(FALSE);
              SelectDropTarget(hitem);
              m_hItemDrop = hitem;
              CImageList::DragShowNolock(TRUE);
          }
      }
      if(GetParent()->SendMessage(WM_TREE_CHECK_POPUP))
      {
          if(!m_bMouseTrack)
          {
              TRACKMOUSEEVENT t = {sizeof(TRACKMOUSEEVENT),TME_LEAVE,m_hWnd,0};

              if(::_TrackMouseEvent(&t))
              {
                  m_bMouseTrack = TRUE;
    //            Invalidate();
              }
          }
      }
      CTreeCtrl::OnMouseMove(nFlags,point);
  }

HTREEITEM CBionicTree::CopyWindowToGroup(CBionicWindow *pWin,
                                         HTREEITEM hNode)
  {
      HTREEITEM hNewNode = NULL;
      TVINSERTSTRUCT tvs;
      TCHAR cBuffer[255];
      TVITEM tvi;

      ASSERT(BT_NODE_GROUP == GetTreeNodeType(hNode));
      ASSERT(pWin);

      if(BT_NODE_GROUP == GetTreeNodeType(hNode))
      {

          ::GetWindowText(pWin->GetHandle(),cBuffer,255);
          tvi.pszText = (LPTSTR)cBuffer;
          tvi.mask = TVIF_PARAM | TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
          tvi.lParam = (LPARAM)pWin;
          tvi.iImage = -1;
          tvi.iSelectedImage = -1;

          tvs.hInsertAfter = hNode;
          tvs.hParent = hNode;
          tvs.item = tvi;

          if((hNewNode = InsertItem(&tvs)))
          {
              int nIndex = pWin->GetImageIndex();
              SetItemImage(hNewNode,nIndex,nIndex);
          
              Expand(hNode,TVE_EXPAND); //Expand the group.
              SelectItem(hNewNode);     //Select new window node.

              GetParent()->PostMessage(WM_TREE_UPDATE,
                                       (WPARAM)hNewNode,
                                       (LPARAM)BT_NODE_WINDOW);
          }
      }
      return hNewNode;
  }

BOOL CBionicTree::IsExplorerCheck(HWND hWnd /* = NULL */)
  {
      TCHAR cClassName[15];
      
      if(!hWnd)
      {
          HTREEITEM hNode = GetSelectedItem();
          hNode = GetDefaultNode(hNode,BT_NODE_WINDOW);
          
          if(hNode)
          {
              CBionicWindow *pWin = (CBionicWindow*)GetItemData(hNode);
              ASSERT(pWin);
              
              if(pWin)
                  hWnd = pWin->GetHandle();
          }
      }
      if(hWnd && ::GetClassName(hWnd,cClassName,15)>0 &&
         lstrcmp(cClassName,_T("CabinetWClass"))==0 ||
         lstrcmp(cClassName,_T("ExploreWClass"))==0 ||
         lstrcmp(cClassName,_T("IEFrame"))==0)
      {
          return TRUE;
      }
      return FALSE;
  }

//Verify that there is only one ROOT and a single GROUP under the tree.
BOOL CBionicTree::VerifyGroupRoot()
  {
      HTREEITEM hGroup = GetChildItem(m_hRootNode);
      if(NULL == (hGroup = GetNextItem(hGroup,TVGN_NEXT)))
          return TRUE;

      return FALSE;
  }

HTREEITEM CBionicTree::AddWindowToTree(HWND hWnd,
                                       HTREEITEM hDesiredNode/*= NULL*/)
  {
      HTREEITEM hNewNode = NULL;

      if(::IsWindow(hWnd))
      {
          CBionicWindow *pBWin = 0;

              //Check if we have an explorer window (handle ex. windows with care)
          if(IsExplorerCheck(hWnd))
          {
              TRACE("FOUND EXPLORER WINDOW.\n");
              pBWin = new CExBionicWindow();
          }
          else
              pBWin = new CBionicWindow();

          if(pBWin)
          {
              TVINSERTSTRUCT tvs;
              TCHAR cBuffer[255];
              HTREEITEM hNode;
              TVITEM tvi;
              HICON hIcon;

              pBWin->SetHandle(hWnd);
      
              hNode = (NULL == hDesiredNode) ? GetSelectedItem() : hDesiredNode;
              hNode = GetDefaultNode(hNode,BT_NODE_GROUP);//Make sure we select a group, if not then default.

              ASSERT(BT_NODE_GROUP == GetTreeNodeType(hNode));

                  //Assure we are adding into a group node.
              if(BT_NODE_GROUP != GetTreeNodeType(hNode))
                  return NULL;

              ::GetWindowText(hWnd,cBuffer,255);
              tvi.pszText = (LPTSTR)cBuffer;
              tvi.mask = TVIF_PARAM | TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
              tvi.lParam = (LPARAM)pBWin;
              tvi.iImage = -1;
              tvi.iSelectedImage = -1;

              tvs.hInsertAfter = hNode;
              tvs.hParent = hNode;
              tvs.item = tvi;

              if((hNewNode = InsertItem(&tvs)))
              {
                  hIcon = pBWin->RefreshIcon();
                  ASSERT(hIcon);

                  m_treeImageList.Add(hIcon);
          
                  int nIndex = m_treeImageList.GetImageCount()-1;
                  pBWin->SetImageIndex(nIndex);
                  SetItemImage(hNewNode,nIndex,nIndex);
          
                  Expand(hNode,TVE_EXPAND); //Expand the group.
                  SelectItem(hNewNode);     //Select new window node.
              }
              else
              {
                  delete pBWin;
                  pBWin = 0;
              }
          }
          GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hNewNode,
                                   (LPARAM)GetTreeNodeType(hNewNode));
      
      }
      return hNewNode;
  }

// HTREEITEM CBionicTree::AddWindowToTree(HWND hWnd,
//                                        HTREEITEM hDesiredNode/*= NULL*/)
//   {
//       ASSERT(::IsWindow(hWnd));

//       HTREEITEM hNewNode = NULL;
//       CBionicWindow *pBWin = 0;

//           //Check if we have an explorer window (handle ex. windows with care)
//       if(IsExplorerCheck(hWnd))
//       {
//           TRACE("FOUND EXPLORER WINDOW.\n");
//           pBWin = new CExBionicWindow();
//       }
//       else
//           pBWin = new CBionicWindow();

//       if(pBWin)
//       {
//           TVINSERTSTRUCT tvs;
//           TCHAR cBuffer[255];
//           HTREEITEM hNode;
//           TVITEM tvi;
//           HICON hIcon;

//           pBWin->SetHandle(hWnd);
      
//           hNode = (NULL == hDesiredNode) ? GetSelectedItem() : hDesiredNode;
//           hNode = GetDefaultNode(hNode,BT_NODE_GROUP);//Make sure we select a group, if not then default.

//           ASSERT(BT_NODE_GROUP == GetTreeNodeType(hNode));

//               //Assure we are adding into a group node.
//           if(BT_NODE_GROUP != GetTreeNodeType(hNode))
//               return NULL;

//           ::GetWindowText(hWnd,cBuffer,255);
//           tvi.pszText = (LPTSTR)cBuffer;
//           tvi.mask = TVIF_PARAM | TVIF_TEXT | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
//           tvi.lParam = (LPARAM)pBWin;
//           tvi.iImage = -1;
//           tvi.iSelectedImage = -1;

//           tvs.hInsertAfter = hNode;
//           tvs.hParent = hNode;
//           tvs.item = tvi;

//           if((hNewNode = InsertItem(&tvs)))
//           {
//               hIcon = pBWin->RefreshIcon();
//               ASSERT(hIcon);

//               m_treeImageList.Add(hIcon);
          
//               int nIndex = m_treeImageList.GetImageCount()-1;
//               pBWin->SetImageIndex(nIndex);
//               SetItemImage(hNewNode,nIndex,nIndex);
          
//               Expand(hNode,TVE_EXPAND); //Expand the group.
//               SelectItem(hNewNode);     //Select new window node.
//           }
//           else
//           {
//               delete pBWin;
//               pBWin = 0;
//           }
//       }
//       GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hNewNode,
//                                (LPARAM)GetTreeNodeType(hNewNode));

//       return hNewNode;
//   }

//START TIMER AUTO GROUPING METHOD.
BOOL CALLBACK CBionicTree::EnumWinProc(HWND hWnd,
                                       LPARAM lParam)
  {
      CBionicTree *pMe = (CBionicTree*)lParam;
      ASSERT(pMe);

      if((::GetWindowLong(hWnd,GWL_STYLE) &
          (WS_OVERLAPPEDWINDOW|WS_VISIBLE))==(WS_OVERLAPPEDWINDOW|WS_VISIBLE))
      {
          if(NULL == ::GetParent(hWnd))
          {
                  //Make sure this HWND is not under the tree && is NOT blacklisted.
              if(pMe->ValidateWindow(hWnd) &&
                 !pMe->IsBlacklisted(hWnd))
              {
                  pMe->AddWindowToTree(hWnd);
              }
          }
      }
      return TRUE;
  }

void CBionicTree::AutoGroup(BOOL bSetTimer)
  {
      if(bSetTimer)
          SetTimer(BT_AUTO_GROUP_ID,BT_AUTO_GROUP_DELAY,NULL);
      else
      {
          KillTimer(BT_AUTO_GROUP_ID);
          KillBlacklist();
      }
  }

void CBionicTree::SetSwitchCursor(BOOL bSetTimer)
  {
      if(bSetTimer)
          SetTimer(BT_SWITCH_CURSOR_ID,BT_SWITCH_CURSOR_DELAY,NULL);
      else
          KillTimer(BT_SWITCH_CURSOR_ID);
  }

//Install folders from registry (last saved).
void CBionicTree::InstallGroups()
  {
      CRegistryEd reg;
      CString regPath;
      CString regKey;
      CString delim;
      
      CList<CString> groups;
      POSITION pos;

      delim.LoadString(IDS_DELIMINATOR);
      
      regPath.LoadString(IDS_REGISTRY_PATH);
      regKey.LoadString(IDS_GROUPS_REGKEY);
      
      if(reg.SetPathKey(regPath) == ERROR_SUCCESS)
      {
          CString buffer;
              //If we dont already have this reg key, then create it.
          if(reg.GetRegStr(regKey,buffer) != ERROR_SUCCESS)
          {
              TRACE("Creating DEFAULT GROUPS registry key.\n");

              buffer.LoadString(IDS_DEFAULT_GROUP);
              reg.SetRegStr(buffer,regKey,buffer.GetLength()+1);
          }
          CStringToken::TokenizeStr(buffer,groups,delim);

          pos = groups.GetHeadPosition();
          int index = 0;

          while(pos)
          {
              HTREEITEM hGroup = InstallGroupFolder((LPCTSTR)groups.GetAt(pos));
              ASSERT(hGroup);

              CHotKeyMan::Keydata kdata;
              CHotKeyMan::HotKeyID id;

                  //Check if we have a group hotkey by this index, if so, save the ATOM into the group node.
              if(m_hotkeyGroups.GetDataByID(index,kdata,id))
              {
                  TRACE("Setting group HOTKEY: %d\n",index);
                  SetItemData(hGroup,id);
              }
              index++;
              groups.GetNext(pos);
          }
      }
  }

void CBionicTree::DumpGroups()
  {
      CString groupRegStr;
      
      EnumAllGroups(EnumDumpGroup,&groupRegStr);

      CRegistryEd reg;
      CString regPath;
      CString regKey;

      regPath.LoadString(IDS_REGISTRY_PATH);
      regKey.LoadString(IDS_GROUPS_REGKEY);
      
      if(reg.SetPathKey(regPath) == ERROR_SUCCESS)
          reg.SetRegStr(groupRegStr,regKey,groupRegStr.GetLength()+1);

      TRACE("SAVED GROUPS: %s\n",groupRegStr);
  }

void CBionicTree::MoveGroupUp(BOOL bGroupUp)
  {
      HTREEITEM hGroup = GetSelectedItem();
      HTREEITEM hNewGroup,hTemp;
      
      if(bGroupUp)
      {
          hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);
          ASSERT(BT_NODE_GROUP == GetTreeNodeType(hGroup));

          if(NULL == (hNewGroup = GetPrevSiblingItem(hGroup)))
          {
              hTemp = hGroup;
              do
              {
                  hNewGroup = hTemp;
                  hTemp = GetNextSiblingItem(hNewGroup);

              } while(hTemp);
          }
      }
      else
      {
          HTREEITEM hRootTest = hGroup;
          hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);

          if(hRootTest == GetRootItem())
              hNewGroup = hGroup;
          else
          {
              if(NULL == (hNewGroup = GetNextSiblingItem(hGroup)))
              {
                  hTemp = hGroup;
                  do
                  {
                      hNewGroup = hTemp;
                      hTemp = GetPrevSiblingItem(hNewGroup);

                  } while(hTemp);
              }
          }
      }
      EnumAllGroups(EnumHideGroupWindows,0);
      EnumGroup(hNewGroup,BT_ShowWindow,0);

      SelectItem(hNewGroup);
      GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hNewGroup,(LPARAM)BT_NODE_GROUP);
  }

UINT CBionicTree::AutoGroupThread(LPVOID ptree)
  {
      CBionicTree *pTree = (CBionicTree*)ptree;
      ASSERT(pTree);

          //Enumerate tree nodes to look for closed windows.
      pTree->CheckAllWindows();

      EnumChildWindows(::GetDesktopWindow(),
                       (WNDENUMPROC)CBionicTree::EnumWinProc,
                       (LPARAM)pTree);
      return 0;
  }

afx_msg void CBionicTree::OnTimer(UINT id)
  {
      switch(id)
      {
          case BT_AUTO_GROUP_ID:

                  //Start auto grouping thread.
              AfxBeginThread(AutoGroupThread,this);
              break;

          case BT_SWITCH_CURSOR_ID:
          {
              int x = GetSystemMetrics(SM_CXSCREEN);
              int y = GetSystemMetrics(SM_CYSCREEN);
              int groupn = -1;

              HTREEITEM hGroup = NULL;
              POINT pt = {0,0};

              GetCursorPos(&pt);

              if(pt.x > -1 && pt.x <= SWITCH_CURSOR_BOUNDARY &&
                 pt.y > -1 && pt.y <= SWITCH_CURSOR_BOUNDARY)
              {
                  if(m_bSwitchCursorToggle)
                  {
                      groupn = 0;
                      m_bSwitchCursorToggle = FALSE;
                  }
              }
              else if(pt.x >= x - SWITCH_CURSOR_BOUNDARY &&
                      pt.y <= SWITCH_CURSOR_BOUNDARY)
              {
                  if(m_bSwitchCursorToggle)
                  {
                      groupn = 1;
                      m_bSwitchCursorToggle = FALSE;
                  }
              }
              else if(pt.y >= y - SWITCH_CURSOR_BOUNDARY &&
                      pt.x <= SWITCH_CURSOR_BOUNDARY)
              {
                  if(m_bSwitchCursorToggle)
                  {
                      groupn = 2;
                      m_bSwitchCursorToggle = FALSE;
                  }
              }
              else if(pt.x >= x - SWITCH_CURSOR_BOUNDARY &&
                      pt.y >= y - SWITCH_CURSOR_BOUNDARY)
              {
                  if(m_bSwitchCursorToggle)
                  {
                      groupn = 3;
                      m_bSwitchCursorToggle = FALSE;
                  }
              }
              else
                  m_bSwitchCursorToggle = TRUE;

              if(groupn != -1 &&
                 (hGroup = GetGroupByNum(groupn)))
              {
                  HideAllGroupWindows();
                  ShowGroupWindows(hGroup);
                  SelectItem(hGroup);
              }
          }
          break;
      }
  }

void CBionicTree::GetTreeData(SBT_TreeData *pData)
  {
//    pData->numOfWindows = m_nNumOfWindows;
//    pData->numOfGroups = m_nNumOfGroups;
//    pData->numOfHidden = m_nNumOfHidden;

      pData->numOfGroups = m_nNumOfGroups;
      pData->numOfWindows = CBionicWindow::GetNumOfWindows();
      pData->numOfHidden = CBionicWindow::GetNumOfHidden();
  }

BOOL CBionicTree::GetDisplayInfo(CString &msg)
  {
      HTREEITEM hNode = GetSelectedItem();
      BT_TREE_NODE_TYPE type = GetTreeNodeType(hNode);
      
      CString info, buffer;

      switch(type)
      {
          case BT_NODE_ROOT:
              msg = _T("ROOT");
              break;

          case BT_NODE_GROUP:
              msg = _T("GROUP");
              break;
              
          case BT_NODE_WINDOW:
              msg = _T("WINDOW");
              break;
      }
      return TRUE;
  }

//Get data for the selected group (# of windows, hidden etc.)
void CBionicTree::GetGroupInfo(SBT_GroupInfo &info,
                               HTREEITEM group /* = NULL */)
  {
      HTREEITEM hGroup = NULL;
      
      hGroup = (!group) ? GetSelectedItem() : group;
      hGroup = GetDefaultNode(hGroup,BT_NODE_GROUP);

      EnumGroup(hGroup,BT_WindowGroupInfo,&info);

      CHotKeyMan::HotKeyID id = (CHotKeyMan::HotKeyID)GetItemData(hGroup);
      if(id)
      {
          CHotKeyMan::Keydata kd;
          if(m_hotkeyGroups.GetDataByHotKeyID(id,kd))
          {
              info.hotkey = "";
              m_hotkeyGroups.GetKeyStrDisplay(kd,info.hotkey);
          }
      }
  }

void CBionicTree::GetWindowInfo(SBT_WindowInfo &info)
  {
      HTREEITEM hNode = GetSelectedItem();
      hNode = GetDefaultNode(hNode,BT_NODE_WINDOW);

      if(hNode)
      {
          CBionicWindow *pWin = (CBionicWindow*)GetItemData(hNode);
          ASSERT(pWin);
          
          if(pWin)
          {
              info.bVisible = pWin->GetState();
              info.bOnTop = pWin->GetAlwaysOnTop();
              info.bTray = pWin->GetTrayState();
          }
      }
  }

BOOL CBionicTree::InsertBlacklist(HWND hWnd)
  {
      if(m_blackList.Find(hWnd))
          return FALSE;
      
      m_blackList.AddTail(hWnd);

      return TRUE;
  }

BOOL CBionicTree::RemoveBlacklisted(HWND hWnd)
  {
      POSITION pos;
      
      if((pos = m_blackList.Find(hWnd)))
      {
          m_blackList.RemoveAt(pos);
          return TRUE;
      }
      return FALSE;
  }

void CBionicTree::TrimBlacklist()
  {
      POSITION pos = m_blackList.GetHeadPosition();
      while(pos)
      {
          POSITION delpos = pos;
          m_blackList.GetNext(pos);

          if(!IsWindow(m_blackList.GetAt(delpos)))
              m_blackList.RemoveAt(delpos);
      }
  }

BOOL CBionicTree::IsBlacklisted(HWND hWnd)
  {
      if(m_blackList.Find(hWnd))
          return TRUE;

      return FALSE;
  }

void CBionicTree::InsertBlacklistGroup(HTREEITEM hGroup)
  {
      EnumGroup(hGroup,BT_BlackListWindow,&m_blackList);
  }

void CBionicTree::BlacklistAll()
  {
      EnumAllGroups(EnumBlacklistGroups,&m_blackList);
  }

void CBionicTree::KillBlacklist()
  {
      m_blackList.RemoveAll();
  }

//START HOTKEY CODE.

void CBionicTree::RefreshGroupsHotkeyID()
  {
      int index = 0;
      EnumAllGroups(EnumRefreshHotKeyIDGroups,&index);
  }

BOOL CBionicTree::EnumRefreshHotKeyIDGroups(HTREEITEM hNode,
                                            CBionicTree *pParent,
                                            void *buf)
  {
      int *pindex = (int*)buf;
      ASSERT(buf);
      ASSERT(pParent);
      
      CHotKeyMan::HotKeyID hotID;
      CHotKeyMan::Keydata kd;
      
          //Check if this group node has a hotkey installed.
      if((hotID = (CHotKeyMan::HotKeyID)pParent->GetItemData(hNode)))
      {
              //If so, change the id in correlation to the index.
          if(pParent->m_hotkeyGroups.GetDataByHotKeyID(hotID,kd))
          {
              TRACE("Changing group hotkey ID: %d\n",*pindex);
              VERIFY(pParent->m_hotkeyGroups.SetDataID(hotID,*pindex));
          }
      }

      (*pindex)++;
      return TRUE;
  }

//Enumerate through all group nodes and check if there is a hotkey by the node index, if so, save it into lparam.
void CBionicTree::InstallGroupsHotkeyID()
  {
      int index = 0;
      EnumAllGroups(EnumSetHotKeyIDGroups,&index);
  }

BOOL CBionicTree::EnumSetHotKeyIDGroups(HTREEITEM hNode,
                                        CBionicTree *pParent,
                                        void *buf)
  {
      int *pindex = (int*)buf;
      ASSERT(buf);
      ASSERT(pParent);
      
      CHotKeyMan::HotKeyID hotID;
      CHotKeyMan::Keydata kd;
      
      if(pParent->m_hotkeyGroups.GetDataByID(*pindex,kd,hotID))
      {
          TRACE("Installing group hotkey ID: %d\n",*pindex);
          pParent->SetItemData(hNode,hotID);
      }
      (*pindex)++;
      return TRUE;
  }

void CBionicTree::LoadHotKeys()
  {
      CString regPath;
      CString regKey;
      CString regDefault;

      regPath.LoadString(IDS_REGISTRY_PATH);
      regKey.LoadString(IDS_HOTKEY_REGKEY_GROUPS);
      regDefault.LoadString(IDS_HOTKEY_DEFAULT_GROUPS);

      m_hotkeyGroups.Load(GetSafeHwnd(),regPath,regKey,regDefault);
      m_hotkeyGroups.LoadHotKeyData();
      InstallGroupsHotkeyID();
  }

void CBionicTree::LoadDefaultHotkeys()
  {
      m_hotkeyGroups.LoadDefaultData();
      InstallGroupsHotkeyID();
  }

void CBionicTree::EnableHotkeys(BOOL bEnable)
  {
      m_bHotkeyEnable = bEnable;

          //Check if we have a group node selected, if so, update hotkey display string if we have changed it.
      HTREEITEM hNode = GetSelectedItem();
      if(BT_NODE_GROUP == GetTreeNodeType(hNode))
          GetParent()->PostMessage(WM_TREE_UPDATE,
                                   (WPARAM)hNode,
                                   (LPARAM)BT_NODE_GROUP);
  }

afx_msg LRESULT CBionicTree::OnHotKey(WPARAM wParam,
                                      LPARAM lParam)
  {
      if(m_bHotkeyEnable)
      {
          CHotKeyMan::HotKeyID hotkeyID = (CHotKeyMan::HotKeyID)wParam;
          CHotKeyMan::Keydata kdata;

          if(m_hotkeyGroups.GetDataByHotKeyID(hotkeyID,kdata))
          {
              TRACE("CBionicTree:OnHotKey(): Hiding group: %d\n",kdata.id);

              HTREEITEM hGroup = GetGroupByNum(kdata.id);

              HideAllGroupWindows();
              ShowGroupWindows(hGroup);
              GetParent()->PostMessage(WM_TREE_UPDATE,(WPARAM)hGroup,(LPARAM)BT_NODE_GROUP);
          }
      }
      return 0;
  }
//END HOTKEY CODE.

//START FONT CODE.
//Load up the passed in font, if NULL, load up defaults.
void CBionicTree::SetTreeFont(LOGFONT *pfont,
                              COLORREF c)
  {
      LOGFONT *plf = pfont;
      LOGFONT lf;
      COLORREF fontColor = c;

      if(!plf)
      {
          CString regPath,regKey,regKeyColor,regDefault;

          regPath.LoadString(IDS_REGISTRY_PATH);
          regKey.LoadString(IDS_FONT_REGKEY);
          regKeyColor.LoadString(IDS_FONT_COLOR_REGKEY);
          regDefault.LoadString(IDS_FONT_DEFAULT);

          m_fontMan.Load(regPath,regKey,regKeyColor,regDefault);
          m_fontMan.LoadFontFromReg(&lf);

          plf = &lf;
      }
          //If default value is pass in, get the value from the font manager.
      if(fontColor == -1)
          fontColor = m_fontMan.GetFontColor();
      else
          m_fontMan.SetFontColor(fontColor); //Else, set the font color to pass in argument.

      m_font.DeleteObject();
      m_font.CreateFont(plf->lfHeight,
                        plf->lfWidth,
                        plf->lfEscapement,
                        plf->lfOrientation,
                        plf->lfWeight,
                        plf->lfItalic,
                        plf->lfUnderline,
                        plf->lfStrikeOut,
                        plf->lfCharSet,
                        plf->lfOutPrecision,
                        plf->lfClipPrecision,
                        plf->lfQuality,
                        plf->lfPitchAndFamily,
                        plf->lfFaceName);

      SetFont(&m_font);
      SetTextColor(fontColor);
      Invalidate();
  }

//Save tree font into the registry.
void CBionicTree::SaveTreeFont()
  {
      CFont *pfont = 0;
      LOGFONT lf;
          
      pfont = GetFont();
      ASSERT(pfont);

      if(pfont)
      {
          pfont->GetLogFont(&lf);
          m_fontMan.SaveFontInReg(&lf);
      }
  }

COLORREF CBionicTree::GetFontColor()
  {
      return m_fontMan.GetFontColor();
  }
//END FONT CODE.

void CBionicTree::SetFullRowSelect(BOOL bEnable)
  {
      DWORD remove,add;
      
      if(bEnable)
      {
          remove = TVS_HASLINES;
          add = TVS_FULLROWSELECT;
      }
      else
      {
          remove = TVS_FULLROWSELECT;
          add = TVS_HASLINES;
      }
      ModifyStyle(remove,add);
  }

BOOL CALLBACK CBionicTree::EnumGetForegroundProc(HWND hWnd,
                                                 LPARAM lParam)
  {
      winforeinfo *p = (winforeinfo*)lParam;
      ASSERT(p);

      if((::GetWindow(hWnd,GW_OWNER) == NULL)
         && (::IsWindowVisible(hWnd)))
      {
          if(p->start == p->end)
          {
              p->hFocused = hWnd;
              return FALSE;
          }
          p->start++;
      }
      return TRUE;
  }
