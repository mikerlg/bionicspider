#ifndef _HOTKEYDLG_H_
#define _HOTKEYDLG_H_
#include "HotKeyMan.h"
#include "BionicTree.h"
#include "bionicspiderDlg.h"
#include "resource.h"

class CHotkeyDlg : public CDialog
  {
    private:
      
      CBionicSpiderDlg *pParent;
      CToolTipCtrl m_ttips;
      
      HFONT m_hFont;

      enum
      {
          HOTKEY_TAB_GROUPS,
          HOTKEY_TAB_MISC
      };
      CTabCtrl m_tab;
      CListBox m_hotkeylist;
      CButton m_sethotkey,m_togglehotkey;

      HWND m_hHotkeyHwnd;  //HWND for the hotkey box.
      BOOL m_bEnable;      //Enable/disable flag for dialog box controls.
      BOOL m_bHotGroupSel; //Group hotkey tab is selected, else the misc. tab has focus.
      BOOL m_bAltered;     //If we have made at least one change we need to save the settings.

      void LoadHotkeyList();
      void UpdateHotkeyBox();
      void RevertChanges();

      virtual void DoDataExchange(CDataExchange*);	// DDX/DDV support

    public:

      enum { ID = IDD_DIALOG_HOTKEYS };

      CHotkeyDlg(CBionicSpiderDlg*);
      ~CHotkeyDlg();

      BOOL OnInitDialog();

      afx_msg void OnSetHotkey();
      afx_msg void OnSelChange();
      afx_msg void OnOK();

      void ToggleHotkeysCtrls(BOOL);
      void EnableCtrls(BOOL);
      int GetHotkeyData(CHotKeyMan::Keydata&);
      int InstallNewHotkey(int,CHotKeyMan::Keydata&);
      LRESULT SetHotkeyStr(const CHotKeyMan::Keydata&);
      HWND WINAPI InitializeHotkey(HWND,RECT*);
      BOOL OnHelpInfo(HELPINFO*);
      BOOL PreTranslateMessage(MSG*);
      afx_msg void OnBnClickedHotkeyToggle();
      afx_msg void OnBnClickedHotkeyDefault();
      afx_msg void OnBnClickedHotkeyRemoveAll();
      afx_msg void OnBnClickedCheckHotkeys();
      afx_msg void OnTabChange(NMHDR*,LRESULT*);
      afx_msg void OnClose();
      afx_msg void OnCancel();

      DECLARE_MESSAGE_MAP()
  };
#endif
