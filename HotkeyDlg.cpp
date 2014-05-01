#include "stdafx.h"
#include "HotkeyDlg.h"
#include "StringToken.h"

CHotkeyDlg::CHotkeyDlg(CBionicSpiderDlg *pPar) : CDialog(CHotkeyDlg::ID,pPar)
  {
      pParent = pPar;
      ASSERT(pParent);

      m_bHotGroupSel = TRUE;
      m_hHotkeyHwnd = NULL;
      m_bAltered = FALSE;
  }

CHotkeyDlg::~CHotkeyDlg()
  {
      DeleteObject(m_hFont);
  }

BOOL CHotkeyDlg::OnInitDialog()
  {
      CDialog::OnInitDialog();

      CHotKeyMan::Keydata kdata;
      CList<CString> lst;
      CString csHotkeys;
      UINT pos = 0;

          //Load up the hotkey list from the registry.
      LoadHotkeyList();

          //Hotkey edit position.
      RECT rc;
      rc.left = 156;
      rc.top = 15;
      rc.right = 160;
      rc.bottom = 20;

      m_hHotkeyHwnd = InitializeHotkey(GetSafeHwnd(),&rc);
      ASSERT(m_hHotkeyHwnd);

          //Select the very first group.
      CHotKeyMan::HotKeyID id;
      if(pParent->m_bionicTree.m_hotkeyGroups.GetDataByID(0,kdata,id))
          SetHotkeyStr(kdata);

      CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_HOTKEYS);
      ASSERT(pCheck);

      m_bEnable = pParent->GetHotkeyState();
      pCheck->SetCheck(m_bEnable);
      EnableCtrls(m_bEnable);
      if(m_bEnable)
          UpdateHotkeyBox();

      TCITEM tci;
      
      tci.mask = TCIF_TEXT;
      tci.iImage = -1;
      
      tci.pszText = _T("Groups");
      m_tab.InsertItem(HOTKEY_TAB_GROUPS,&tci);
      
      tci.pszText = _T("Misc.");
      m_tab.InsertItem(HOTKEY_TAB_MISC,&tci);

      ::SetFocus(m_hHotkeyHwnd);

      RECT rcToolTip = {0,0,100,100};
      CString tiptext;

      m_ttips.Create(this,TTS_ALWAYSTIP);
      m_ttips.SetMaxTipWidth(200);

      tiptext.LoadString(IDS_TOOLTIP_HOTKEY_EDIT);
      m_ttips.AddTool(CWnd::FromHandle(m_hHotkeyHwnd),tiptext,&rcToolTip,1);

      tiptext.LoadString(IDS_TOOLTIP_HOTKEY_SET);
      m_ttips.AddTool(GetDlgItem(IDC_BUTTON_HOTKEY),tiptext,&rcToolTip,IDS_TOOLTIP_HOTKEY_SET);

      tiptext.LoadString(IDS_TOOLTIP_HOTKEY_REMOVE);
      m_ttips.AddTool(GetDlgItem(IDC_BUTTON_HOTKEY_TOGGLE),tiptext,&rcToolTip,IDC_BUTTON_HOTKEY_TOGGLE);

      tiptext.LoadString(IDS_TOOLTIP_HOTKEY_REMOVE_ALL);
      m_ttips.AddTool(GetDlgItem(IDC_BUTTON_HOTKEY_REMOVE_ALL),tiptext,&rcToolTip,IDC_BUTTON_HOTKEY_REMOVE_ALL);

      tiptext.LoadString(IDS_TOOLTIP_HOTKEY_DEFAULT);
      m_ttips.AddTool(GetDlgItem(IDC_BUTTON_HOTKEY_DEFAULT),tiptext,&rcToolTip,IDC_BUTTON_HOTKEY_DEFAULT);

      tiptext.LoadString(IDS_TOOLTIP_HOTKEY_TABS);
      m_ttips.AddTool(GetDlgItem(IDC_TAB_HOTKEY_TYPE),tiptext,&rcToolTip,IDC_TAB_HOTKEY_TYPE);

      m_ttips.Activate(TRUE);

      return FALSE;
  }

BOOL CHotkeyDlg::PreTranslateMessage(MSG *pMsg)
  {
      switch(pMsg->message)
      {
          case WM_LBUTTONDOWN:
          case WM_LBUTTONUP:
          case WM_MOUSEMOVE:
              m_ttips.RelayEvent(pMsg);
      }
      return CDialog::PreTranslateMessage(pMsg);
  }

//Set the hotkey string for the hotkey control.
LRESULT CHotkeyDlg::SetHotkeyStr(const CHotKeyMan::Keydata& kdata)
  {
      UINT mod = 0;

          //Stupid bug in hotkey api - do the converson from MOD_CONTROL to HOTKEYF_CONTROL etc.
      if(kdata.modKeys & MOD_ALT)
          mod |= HOTKEYF_ALT;
      
      if(kdata.modKeys & MOD_CONTROL)
          mod |= HOTKEYF_CONTROL;

      if(kdata.modKeys & MOD_SHIFT)
          mod |= HOTKEYF_SHIFT;

      return ::SendMessage(m_hHotkeyHwnd,HKM_SETHOTKEY,
                           MAKEWORD(kdata.vk,mod),0);
  }

//Get hotkey data for the hotkey control.
int CHotkeyDlg::GetHotkeyData(CHotKeyMan::Keydata &kdata)
  {
      LRESULT hotkey;
      DWORD vk,mod;

      hotkey = ::SendMessage(m_hHotkeyHwnd,HKM_GETHOTKEY,0,0);

      vk = LOBYTE(LOWORD(hotkey));
      mod = HIBYTE(LOWORD(hotkey));

      if(mod & HOTKEYF_CONTROL)
          kdata.modKeys |= MOD_CONTROL;

      if(mod & HOTKEYF_SHIFT)
          kdata.modKeys |= MOD_SHIFT;

      if(mod & HOTKEYF_ALT)
          kdata.modKeys |= MOD_ALT;

      kdata.vk = vk;

      return 0;
  }

HWND WINAPI CHotkeyDlg::InitializeHotkey(HWND hWnd,
                                         RECT *prc)
  { 
      HWND hwndHot = NULL;
    
          // Ensure that the common control DLL is loaded. 
      InitCommonControls(); 
      hwndHot = CreateWindowEx(0,                        // no extended styles 
                               HOTKEY_CLASS,             // class name 
                               "",                       // no title (caption) 
                               WS_CHILD | WS_VISIBLE,    // style 
                               prc->left, prc->top,      // position 
                               prc->right, prc->bottom,  // size 
                               hWnd,                     // parent window 
                               NULL,                     // uses class menu 
                               AfxGetInstanceHandle(),                  // instance 
                               NULL);                    // no WM_CREATE parameter 

      m_hFont = CreateFont(16,0,0,0,FW_NORMAL,
                           0,0,0,ANSI_CHARSET,
                           OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS,
                           DEFAULT_QUALITY,
                           DEFAULT_PITCH | FF_DONTCARE,
                           "Arial");

      ::SendMessage(hwndHot,WM_SETFONT,(WPARAM)m_hFont,TRUE);

      ::SetFocus(hwndHot);
          // Set rules for invalid key combinations. If the user does not supply a
          // modifier key, use ALT as a modifier. If the user supplies SHIFT as a 
          // modifier key, use SHIFT + ALT instead.
      ::SendMessage(hwndHot, 
                    HKM_SETRULES, 
                    (WPARAM) HKCOMB_NONE | HKCOMB_S,   // invalid key combinations 
                    MAKELPARAM(HOTKEYF_ALT, 0));       // add ALT to invalid entries 
 
          // Set CTRL + ALT + A as the default hot key for this window. 
          // 0x41 is the virtual key code for 'A'. 
      ::SendMessage(hwndHot,HKM_SETHOTKEY,
                    MAKEWORD(0x41, HOTKEYF_CONTROL | HOTKEYF_ALT),0);
      
      return hwndHot;
  }

void CHotkeyDlg::DoDataExchange(CDataExchange *pDX)
  {
      CDialog::DoDataExchange(pDX);

      DDX_Control(pDX,IDC_TAB_HOTKEY_TYPE,m_tab);
      DDX_Control(pDX,IDC_LIST_HOTKEYS,m_hotkeylist);
      DDX_Control(pDX,IDC_BUTTON_HOTKEY,m_sethotkey);
      DDX_Control(pDX,IDC_BUTTON_HOTKEY_TOGGLE,m_togglehotkey);
  }

BEGIN_MESSAGE_MAP(CHotkeyDlg,CDialog)
ON_COMMAND(IDC_BUTTON_HOTKEY,OnSetHotkey)
ON_LBN_SELCHANGE(IDC_LIST_HOTKEYS,OnSelChange)
ON_BN_CLICKED(IDC_BUTTON_HOTKEY_REMOVE_ALL,OnBnClickedHotkeyRemoveAll)
ON_BN_CLICKED(IDC_BUTTON_HOTKEY_DEFAULT,OnBnClickedHotkeyDefault)
ON_BN_CLICKED(IDC_CHECK_HOTKEYS,OnBnClickedCheckHotkeys)
ON_BN_CLICKED(IDC_BUTTON_HOTKEY_TOGGLE,OnBnClickedHotkeyToggle)
ON_NOTIFY(TCN_SELCHANGE,IDC_TAB_HOTKEY_TYPE,OnTabChange)
ON_WM_HELPINFO()
ON_WM_CLOSE()
END_MESSAGE_MAP()

afx_msg void CHotkeyDlg::OnSetHotkey()
  {
      CString buffer;
      buffer.LoadString(IDS_HOTKEY_WARNING);

      if(CBionicSpiderDlg::ConfirmMsgBox(GetSafeHwnd(),buffer))
      {
          CHotKeyMan::Keydata kd;

              //Get hotkey data from the hotkey control.
          GetHotkeyData(kd);

              //INVALID if we have no mod. keys etc. Allow for function keys F1-F12 (112-123 ascii).
          if(kd.modKeys == 0 && (kd.vk < 112 || kd.vk > 123))
          {
              buffer.LoadString(IDS_INVALID_HOTKEY);

              CBionicSpiderDlg::AlertMsgBox(GetSafeHwnd(),buffer);
              return;
          }
          if(kd.modKeys & MOD_CONTROL)
          {
              kd.keys += HOTKEYMAN_CTRL;
              kd.keys += HOTKEYMAN_DELIM;
          }
          if(kd.modKeys & MOD_ALT)
          {
              kd.keys += HOTKEYMAN_ALT;
              kd.keys += HOTKEYMAN_DELIM;
          }
          if(kd.modKeys & MOD_SHIFT)
          {
              kd.keys += HOTKEYMAN_SHIFT;
              kd.keys += HOTKEYMAN_DELIM;
          }
              //Concat. the virtual key that will trigger this hotkey to the hotkey string.
          buffer.Format("%d",kd.vk);
          kd.keys += buffer;

          TRACE("NEW HOTKEY: %s\n",kd.keys);

          kd.id =  m_hotkeylist.GetCurSel();
          if(m_bHotGroupSel)
          {
              CHotKeyMan::HotKeyID hotID = pParent->m_bionicTree.m_hotkeyGroups.RegHotkey(kd);
              
              HTREEITEM hGroup = pParent->m_bionicTree.GetGroupByNum(kd.id);
              ASSERT(hGroup);

                  //For each group hotkey we need to store the hotkey ID into the node lparam.
              if(hotID)
              {
                  TRACE("Installing hotkey into node: %d\n",kd.id);
                  pParent->m_bionicTree.SetItemData(hGroup,hotID);
              }
          }
          else
              pParent->m_hotkey.RegHotkey(kd);
      }
      m_bAltered = TRUE;
      ::SetFocus(m_hHotkeyHwnd);
  }

void CHotkeyDlg::UpdateHotkeyBox()
  {
      CHotKeyMan::Keydata kdata;
      CHotKeyMan::HotKeyID hotID;
      CHotKeyMan *pHotkeys = 0;

      int id = m_hotkeylist.GetCurSel();
      BOOL bEnable = FALSE;

      if(m_bHotGroupSel)
          pHotkeys = &pParent->m_bionicTree.m_hotkeyGroups;
      else
          pHotkeys = &pParent->m_hotkey;
      
      if(pHotkeys->GetDataByID(id,kdata,hotID))
      {
          SetHotkeyStr(kdata);
          bEnable = TRUE;
      }
      ToggleHotkeysCtrls(bEnable);
  }
afx_msg void CHotkeyDlg::OnSelChange()
  {
      UpdateHotkeyBox();
  }

afx_msg void CHotkeyDlg::OnBnClickedHotkeyDefault()
  {
      CString s;
      s.LoadString(IDS_HOTKEY_DEFAULT_WARNING);

      if(CBionicSpiderDlg::ConfirmMsgBox(GetSafeHwnd(),s))
      {
          TRACE("Installing default HOTKEYS\n");

          pParent->m_hotkey.LoadDefaultData();
          pParent->m_bionicTree.LoadDefaultHotkeys();

          UpdateHotkeyBox();
          m_bAltered = TRUE;
      }
  }

afx_msg void CHotkeyDlg::OnBnClickedHotkeyRemoveAll()
  {
      CString s;
      s.LoadString(IDS_HOTKEY_REMOVE_ALL);

      if(CBionicSpiderDlg::ConfirmMsgBox(GetSafeHwnd(),s))
      {
          pParent->m_hotkey.UnregKeys();
          pParent->m_bionicTree.m_hotkeyGroups.UnregKeys();

          ToggleHotkeysCtrls(FALSE);
          m_bAltered = TRUE;
      }
  }

//Enable or disable ALL controls.
void CHotkeyDlg::EnableCtrls(BOOL bState)
  {
      CButton *pSetDefault = (CButton*)GetDlgItem(IDC_BUTTON_HOTKEY_DEFAULT);
      CButton *pSetHot = (CButton*)GetDlgItem(IDC_BUTTON_HOTKEY);
      CButton *pToggle = (CButton*)GetDlgItem(IDC_BUTTON_HOTKEY_TOGGLE);
      CButton *pRemove = (CButton*)GetDlgItem(IDC_BUTTON_HOTKEY_REMOVE_ALL);

      ASSERT(pSetDefault);
      ASSERT(pSetHot);
      ASSERT(pToggle);
      ASSERT(pRemove);
      
      pRemove->EnableWindow(bState);
      pToggle->EnableWindow(bState);
      pSetHot->EnableWindow(bState);
      pSetDefault->EnableWindow(bState);

          //Handle hotkey edit control.
      m_hotkeylist.EnableWindow(bState);

      if(!bState)
          ::SendMessage(m_hHotkeyHwnd,HKM_SETHOTKEY,0,0); //Blank out hotkey control text with "None"

      ::EnableWindow(m_hHotkeyHwnd,bState);

      m_bEnable = bState;
  }

//If there is a valid hotkey, then we should enable ctrls if not then only enable the "Enable" button.
void CHotkeyDlg::ToggleHotkeysCtrls(BOOL bEnable)
  {
      CString enable,remove;
      CString buffer;
      
      enable.LoadString(IDS_HOTKEYDLG_ENABLE);
      remove.LoadString(IDS_HOTKEYDLG_REMOVE);

      ::EnableWindow(m_hHotkeyHwnd,bEnable);
      m_sethotkey.EnableWindow(bEnable);

      buffer = (bEnable) ? remove : enable;
      m_togglehotkey.SetWindowText(buffer);

      if(bEnable)
          ::SetFocus(m_hHotkeyHwnd);
      else
          ::SendMessage(m_hHotkeyHwnd,HKM_SETHOTKEY,0,0); //Blank out hotkey control text with "None"
  }

//Do we have to disable or enable?
afx_msg void CHotkeyDlg::OnBnClickedHotkeyToggle()
  {
      CHotKeyMan::Keydata kdata;
      CHotKeyMan::HotKeyID hotID;
      CHotKeyMan *pHotkey = 0;

      int id = m_hotkeylist.GetCurSel();
      BOOL bEnable = FALSE;

      if(m_bHotGroupSel)
          pHotkey = &pParent->m_bionicTree.m_hotkeyGroups;
      else
          pHotkey = &pParent->m_hotkey;
      
          //If we have an ID, then we remove this hotkey.
      if(pHotkey->GetDataByID(id,kdata,hotID))
      {
          CString s;
          s.LoadString(IDS_HOTKEY_REMOVE);

          if(CBionicSpiderDlg::ConfirmMsgBox(GetSafeHwnd(),s))
              pHotkey->UnregHotkey(hotID);
          else
              return;
      }
      else
          bEnable = TRUE;

      m_bAltered = TRUE;
      ToggleHotkeysCtrls(bEnable);      
  }

//Enable or disable hotkeys checkbox.
afx_msg void CHotkeyDlg::OnBnClickedCheckHotkeys()
  {
      CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_HOTKEYS);
      ASSERT(pCheck);

      BOOL bCheckState = pCheck->GetCheck();
      BOOL bEnableCtrls = FALSE;

      if(bCheckState)
      {
              //Check if we have not yet loaded any hotkeys, if so, load them for the display strings.
          if(!pParent->m_hotkey.RegisteredHotkeys())
              pParent->LoadHotKeys();

          if(!pParent->m_bionicTree.m_hotkeyGroups.RegisteredHotkeys())
              pParent->m_bionicTree.LoadHotKeys();

          bEnableCtrls = TRUE;
          UpdateHotkeyBox();
      }
      EnableCtrls(bEnableCtrls);
      m_bAltered = TRUE;
  }

afx_msg void CHotkeyDlg::OnOK()
  {
      if(m_bAltered)
      {
          CButton *pCheck = (CButton*)GetDlgItem(IDC_CHECK_HOTKEYS);
          ASSERT(pCheck);

          BOOL bCheckState = pCheck->GetCheck();

              //Load up hotkey data if the initial state was disabled and we want to enable them now.
          if(bCheckState &&
             !pParent->GetHotkeyState())
          {
              if(!pParent->m_hotkey.RegisteredHotkeys())
                  pParent->LoadHotKeys();

              if(!pParent->m_bionicTree.m_hotkeyGroups.RegisteredHotkeys())
                  pParent->m_bionicTree.LoadHotKeys();
          }
          else if(!bCheckState) //Else, we want to disable hotkeys.
          {
              pParent->m_hotkey.UnregKeys(FALSE);
              pParent->m_bionicTree.m_hotkeyGroups.UnregKeys(FALSE);
          }
          TRACE("CHotKeyDlg::OnOK() - Dumping HOTKEY settings to registry.\n");

          pParent->m_bionicTree.m_hotkeyGroups.SetRegistryString();
          pParent->m_hotkey.SetRegistryString();

          pParent->SetHotkeyState(m_bEnable);
      }
      CDialog::OnOK();
  }

//Closed or cancel was depressed, revert all changes made - re-read all registry hotkey data.
void CHotkeyDlg::RevertChanges()
  {
          //If the hotkeys were set initially, just reload old settings from the registry, else just return.
      if(m_bAltered && pParent->GetHotkeyState())
      {
          TRACE("CHotkeyDlg::RevertChanges() - Reverting all changes.\n");

              //Unregister all hotkeys and re-install config. from the registry.
          pParent->m_hotkey.UnregKeys(TRUE); //Pass true to destroy current hotkey map.
          pParent->m_bionicTree.m_hotkeyGroups.UnregKeys(TRUE);

          pParent->LoadHotKeys();
          pParent->m_bionicTree.LoadHotKeys();
      }
      else if(m_bAltered && !pParent->GetHotkeyState()) //Else, just unregister the hotkeys - destroy the map.
      {
          pParent->m_hotkey.UnregKeys(TRUE);
          pParent->m_bionicTree.m_hotkeyGroups.UnregKeys(TRUE);
      }
  }

afx_msg void CHotkeyDlg::OnClose()
  {
      RevertChanges();
      CDialog::OnClose();
  }

afx_msg void CHotkeyDlg::OnCancel()
  {
      RevertChanges();
      CDialog::OnCancel();
  }

afx_msg void CHotkeyDlg::OnTabChange(NMHDR *hdr,
                                     LRESULT *notused)
  {
      m_bHotGroupSel = (HOTKEY_TAB_GROUPS == m_tab.GetCurSel()) ? TRUE : FALSE;
      LoadHotkeyList();

      if(m_bEnable)
          UpdateHotkeyBox();
  }

void CHotkeyDlg::LoadHotkeyList()
  {
      POSITION pos;
      int increm = 0;

          //Blank out CListCtrl.
      m_hotkeylist.ResetContent();

      CList<CString> groups;
      pParent->m_bionicTree.GetGroupNames(groups);

          //If we are in the group hotkey mode then we must load the listbox with the group strings.
      if(m_bHotGroupSel)
      {
          pos = groups.GetHeadPosition();
          while(pos)
          {
              m_hotkeylist.InsertString(increm++,groups.GetAt(pos));
              groups.GetNext(pos);
          }
      }
      else //We must load up the default hotkeys that do not concern an individual group.
      {
          CList<CString> hotkeys;
          CString buffer;

          buffer.LoadString(IDS_HOTKEY_LIST);
          CStringToken::TokenizeStr(buffer,hotkeys,HOTKEYMAN_DELIM);//Split up each hotkey info. string and insert into the list box.

          pos = hotkeys.GetHeadPosition();
          while(pos)
          {
              m_hotkeylist.InsertString(increm++,hotkeys.GetAt(pos));
              hotkeys.GetNext(pos);
          }
      }
      m_hotkeylist.SetCurSel(0);
  }

BOOL CHotkeyDlg::OnHelpInfo(HELPINFO *pHelpInfo)
  {
      return TRUE;
  }
