/*
  bionicspider LITE
  Mike LeonGuerrero
 */
#include "stdafx.h"
#include "bionicspider.h"
#include "bionicspiderDlg.h"
#include "InputTextDlg.h"
#include "HotkeyDlg.h"
#include "RegistryEd.h"
#include "HyperLink.h"
#include "SunkenButton.h"
#include "WireFrame.h"
#include "StringToken.h"
#include "UpdateDlg.h"
#include "bionicspiderdlg.h"
#include <afxinet.h>

#ifdef _AFXDLL
#define COMPILE_MULTIMON_STUBS
#endif
#include <multimon.h>
#include ".\bionicspiderdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define _EASTER_EGGS

//++ for each new Group that we add to the Group menu list.
#define BS_MENU_GROUP_LIST_ID 50000

//Pixel offset so the wire frame doesn't leave traces on the sys. tray.
//#define WIRE_FRAME_OFFSET 30

//Disable various options/settings for the FREE release version of BS LITE.
//#define LITE_UPGRADE

const int CBionicSpiderDlg::WM_NOTIFYICON = ::RegisterWindowMessage(_T("WM_NOTIFYICON"));
const int CBionicSpiderDlg::WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

//START ABOUT DLG.
class CAboutDlg : public CDialog
{
public:

    CAboutDlg();
    BOOL OnInitDialog();
    enum { IDD = IDD_ABOUTBOX };

private:

    CSunkenButton m_OKSunken;
    CHyperLink m_webaddy;
    CHyperLink m_emailaddy;

    virtual void DoDataExchange(CDataExchange*);    // DDX/DDV support
    HBRUSH OnCtlColor(CDC*,CWnd*,UINT);

    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {}

BOOL CAboutDlg::OnInitDialog()
  {
      CDialog::OnInitDialog();

      CString buffer,em;
      buffer.LoadString(IDS_WEB_URL);

      m_webaddy.SetWindowText(buffer);
      m_webaddy.SetURL(buffer);

      em = "mailto:";
      
      buffer.LoadString(IDS_EMAIL);
      m_emailaddy.SetWindowText(buffer);
      em += buffer;
      
      m_emailaddy.SetURL(em);

      CStatic *pTitleText = (CStatic*)GetDlgItem(IDC_BS_TITLE);
      ASSERT(pTitleText);

      CString title;
      CString ver;
      
      title.LoadString(IDS_APP_NAME);
      ver.LoadString(IDS_BIONICSPIDER_VERSION_NUM);
      title += " v";
      title += ver;

      pTitleText->SetWindowText(title);
      m_OKSunken.SubclassDlgItem(IDC_STATIC_OK,this);
      m_OKSunken.ModifyStyle(0,SS_NOTIFY);
      
      return TRUE;
  }

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
  {
      CDialog::DoDataExchange(pDX);

      DDX_Control(pDX,IDC_WEB_ADDRESS,m_webaddy);
      DDX_Control(pDX,IDC_EMAIL,m_emailaddy);
  }

BEGIN_MESSAGE_MAP(CAboutDlg,CDialog)
ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

//Paint background and child controls of DLG.
HBRUSH CAboutDlg::OnCtlColor(CDC *pDC,
                             CWnd *pWnd,
                             UINT nCtlColor)
  {
      HBRUSH hbBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

      if(nCtlColor == CTLCOLOR_DLG)                 
          return hbBackground;

      DWORD dwCtrlID = pWnd->GetDlgCtrlID();
      if(dwCtrlID == IDC_BS_ICON   ||
         dwCtrlID == IDC_BS_TITLE  ||
         dwCtrlID == IDC_AUTHOR    ||
         dwCtrlID == IDC_COPYRIGHT ||
         dwCtrlID == IDC_EXPIRE    ||
         dwCtrlID == IDC_USER)
      {
          return hbBackground;
      }
      return CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
  }
//END ABOUT DLG

//START UPGRADE MSG DLG
#ifdef LITE_UPGRADE
class CUpgradeMsgDlg : public CDialog
{
public:
    CUpgradeMsgDlg();
    BOOL OnInitDialog();

    enum { IDD = IDD_UPGRADE_MSG };

private:
    CHyperLink m_webaddy;

    virtual void DoDataExchange(CDataExchange*);    // DDX/DDV support
    HBRUSH OnCtlColor(CDC*,CWnd*,UINT);

    DECLARE_MESSAGE_MAP()
};

CUpgradeMsgDlg::CUpgradeMsgDlg() : CDialog(CUpgradeMsgDlg::IDD) {}

BOOL CUpgradeMsgDlg::OnInitDialog()
  {
      CDialog::OnInitDialog();

      CString buffer;
      buffer.LoadString(IDS_WEB_URL_UPDATE);

      m_webaddy.SetWindowText(buffer);
      m_webaddy.SetURL(buffer);

      CStatic *pTitleText = (CStatic*)GetDlgItem(IDC_BS_TITLE);
      ASSERT(pTitleText);

      CStatic *pUpgradeText = (CStatic*)GetDlgItem(IDC_UPGRADE_TEXT);
      ASSERT(pUpgradeText);

      buffer.LoadString(IDS_UPGRADE);
      pUpgradeText->SetWindowText(buffer);

      return TRUE;
  }

void CUpgradeMsgDlg::DoDataExchange(CDataExchange *pDX)
  {
      CDialog::DoDataExchange(pDX);
      DDX_Control(pDX,IDC_UPGRADE_URL,m_webaddy);
  }

BEGIN_MESSAGE_MAP(CUpgradeMsgDlg,CDialog)
ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

HBRUSH CUpgradeMsgDlg::OnCtlColor(CDC *pDC,
                                  CWnd *pWnd,
                                  UINT nCtlColor)
  {
      HBRUSH hbBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

      if(nCtlColor == CTLCOLOR_DLG)                 
          return hbBackground;

      DWORD dwCtrlID = pWnd->GetDlgCtrlID();
      if(dwCtrlID == IDC_UPGRADE_TEXT)
          return hbBackground;

      return CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
  }
//END UPGRADE MSG DLG
#endif

//CBionicSpiderDlg CONSTRUCTOR
CBionicSpiderDlg::CBionicSpiderDlg(CWnd *pParent /*=NULL*/)
	                          : CDialog(CBionicSpiderDlg::IDD, pParent)
  {
      m_hMainMenu = NULL;
      m_hTrayHandle = NULL;

          //Set MENU BOOL array to default values.
      m_bMenuSettings[MENU_ALWAYS_ON_TOP] = TRUE;
      m_bMenuSettings[MENU_WARNINGS] = TRUE;
      m_bMenuSettings[MENU_TRAY_POPUP] = FALSE;
      m_bMenuSettings[MENU_FULLROWSEL] = FALSE;
      m_bMenuSettings[MENU_ANIMATE] = TRUE;
      m_bMenuSettings[MENU_AUTOGROUP] = FALSE;
      m_bMenuSettings[MENU_DOCKING_REGISTRED] = FALSE;
      m_bMenuSettings[MENU_DOCKED_LEFT] = TRUE;
      m_bMenuSettings[MENU_SWITCH_GROUP] = TRUE;
      m_bMenuSettings[MENU_SWITCH_WINDOW] = TRUE;
      m_bMenuSettings[MENU_SWITCH_CURSOR] = TRUE;
      m_bMenuSettings[MENU_HOTKEYS] = TRUE;

          //Set default hotkey toggle to hide, next round will show etc.
      m_bHotkeyToggle[HOTKEY_TOGGLE_HIDE_ALL] = TRUE;
      m_bHotkeyToggle[HOTKEY_TOGGLE_HIDE_GROUP] = TRUE;

      m_bHotkeyDlgInit = FALSE;

      m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
      m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(),
                                       MAKEINTRESOURCE(IDR_ACCEL));
  }

CBionicSpiderDlg::~CBionicSpiderDlg()
  {
          //Free up the menu image obj.
      for(int i = 0;i<BS_MENUMAP_IMAGES;i++)
          m_menuMaps[i].DeleteObject();
  }

void CBionicSpiderDlg::DoDataExchange(CDataExchange* pDX)
  {
      CDialog::DoDataExchange(pDX);
  }

BEGIN_MESSAGE_MAP(CBionicSpiderDlg,CDialog)
ON_REGISTERED_MESSAGE(CBionicTree::WM_TREE_RIGHT_CLICK,OnTreeRightClick)
ON_REGISTERED_MESSAGE(CBionicTree::WM_TREE_CHECK_POPUP,OnTreeCheckPopup)
ON_REGISTERED_MESSAGE(CBionicTree::WM_TREE_CHECK_AUTOGROUP,OnTreeCheckAutoGroup)
ON_REGISTERED_MESSAGE(CBionicTree::WM_TREE_MOUSE_LEAVE,OnTreeMouseLeave)
ON_REGISTERED_MESSAGE(CBionicSpiderDlg::WM_TASKBARCREATED,OnTaskBarCreated)
ON_REGISTERED_MESSAGE(CBionicSpiderDlg::WM_NOTIFYICON,OnNotifyIcon)
ON_REGISTERED_MESSAGE(CBionicTree::WM_TREE_UPDATE,OnUpdateInfo)

ON_MESSAGE(WM_HOTKEY,OnHotKey)
ON_WM_HELPINFO()
ON_WM_INITMENUPOPUP()
ON_WM_SIZE()
ON_WM_SIZING()
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_CLOSE()
ON_WM_TIMER()
ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_BIONICSPIDER_EXIT, OnBionicSpiderExit)
	ON_UPDATE_COMMAND_UI(ID_TILEWINDOWS_1X1HORT, OnUpdateTileWindows1x1hort)
	ON_UPDATE_COMMAND_UI(ID_TILEWINDOWS_1X1VERT, OnUpdateTileWindows1x1vert)
	ON_UPDATE_COMMAND_UI(ID_TILEWINDOWS_2X2, OnUpdateTileWindows2x2)
	ON_COMMAND(ID_DOCKING_ENABLEDOCKING, OnDockingToggle)
	ON_UPDATE_COMMAND_UI(ID_DOCKING_ENABLEDOCKING, OnUpdateDockingEnableDocking)
	ON_UPDATE_COMMAND_UI(ID_DOCKING_RIGHT, OnUpdateDockingRight)
	ON_UPDATE_COMMAND_UI(ID_DOCKING_LEFT, OnUpdateDockingLeft)
	ON_COMMAND(ID_DOCKING_RIGHT, OnDockingRight)
	ON_COMMAND(ID_DOCKING_LEFT, OnDockingLeft)
	ON_COMMAND(ID_ROOT_ADDGROUP, OnRootAddGroup)
	ON_COMMAND(ID_BIONICSPIDER_ALWAYSONTOP, OnBionicspiderAlwaysOnTop)
	ON_UPDATE_COMMAND_UI(ID_BIONICSPIDER_ALWAYSONTOP, OnUpdateBionicspiderAlwaysOnTop)
	ON_COMMAND(ID_BIONICSPIDER_WARNINGS, OnBionicspiderWarnings)
	ON_UPDATE_COMMAND_UI(ID_BIONICSPIDER_WARNINGS, OnUpdateBionicspiderWarnings)
	ON_COMMAND(ID_ROOT_HIDEALLGROUPS, OnRootHideAllGroups)
	ON_COMMAND(ID_ROOT_SHOWALLGROUPS, OnRootShowAllGroups)
	ON_COMMAND(ID_ROOT_MAXIMIZEALLGROUPS, OnRootMaximizeAllGroups)
	ON_COMMAND(ID_ROOT_MINIMIZEALLGROUPS, OnRootMinimizeAllGroups)
	ON_COMMAND(ID_ROOT_RESTOREALLGROUPS, OnRootRestoreAllGroups)
	ON_COMMAND(ID_ROOT_SORTALLGROUPS, OnRootSortAllGroups)
	ON_COMMAND(ID_ROOT_CLOSEALLGROUPS, OnRootCloseAllGroups)
	ON_COMMAND(ID_ROOT_REMOVEWINDOWSINALLGROUPS, OnRootRemoveWindowsInAllGroups)
	ON_COMMAND(ID_ROOT_REMOVEALLGROUPS, OnRootRemoveAllGroups)
        ON_COMMAND(ID_GROUP_HIDEALLWINDOWS, OnGroupHideAllWindows)
	ON_COMMAND(ID_GROUP_SHOWALLWINDOWS, OnGroupShowAllWindows)
	ON_COMMAND(ID_GROUP_MAXIMIZEALLWINDOWS, OnGroupMaximizeAllWindows)
	ON_COMMAND(ID_GROUP_MINIMIZEALLWINDOWS, OnGroupMinimizeAllWindows)
	ON_COMMAND(ID_GROUP_RESTOREALLWINDOWS, OnGroupRestoreAllWindows)
	ON_COMMAND(ID_TILEWINDOWS_1X1HORT, OnTileWindows1x1Hort)
	ON_COMMAND(ID_TILEWINDOWS_1X1VERT, OnTileWindows1x1Vert)
	ON_COMMAND(ID_TILEWINDOWS_2X2, OnTileWindows2x2)
	ON_COMMAND(ID_GROUPPOSITIONS_GROUPMEMORY, OnGroupPositionsGroupMemory)
	ON_COMMAND(ID_GROUPPOSITIONS_RECALLALL, OnGroupPositionsRecallAll)
	ON_COMMAND(ID_GROUP_ADDWINDOWBYTITLE, OnGroupAddWindowByTitle)
	ON_COMMAND(ID_GROUP_SORTALLWINDOWSINGROUP, OnGroupSortAllWindowsInGroup)
	ON_COMMAND(ID_GROUP_CLOSEALLWINDOWSINGROUP, OnGroupCloseAllWindowsInGroup)
	ON_COMMAND(ID_GROUP_REMOVEALLWINDOWSINGROUP, OnGroupRemoveAllWindowsInGroup)
	ON_COMMAND(ID_GROUP_REMOVETHISGROUP, OnGroupRemoveThisGroup)
	ON_COMMAND(ID_GROUP_RENAMETHISGROUP, OnGroupRenameThisGroup)
	ON_COMMAND(ID_WINDOW_HIDEWINDOW, OnWindowHideWindow)
	ON_COMMAND(ID_WINDOW_SHOWWINDOW, OnWindowShowWindow)
	ON_COMMAND(ID_WINDOW_FOREGROUNDWINDOW, OnWindowForegroundWindow)
	ON_COMMAND(ID_WINDOW_MAXIMIZE, OnWindowMaximize)
	ON_COMMAND(ID_WINDOW_MINIMIZE, OnWindowMinimize)
	ON_COMMAND(ID_WINDOW_RESTORE, OnWindowRestore)
	ON_COMMAND(ID_WINDOW_ALWAYSONTOP, OnWindowAlwaysOnTop)
	ON_COMMAND(ID_WINDOW_CLOSE, OnWindowClose)
	ON_COMMAND(ID_EXPLORER_CONSOLE, OnExplorerConsole)
	ON_COMMAND(ID_EXPLORER_CLONE, OnExplorerClone)
	ON_COMMAND(ID_WINDOW_RENAME, OnWindowRename)
	ON_COMMAND(ID_WINDOW_REMOVE, OnWindowRemove)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_BIONICSPIDER_SWITCHGROUP, OnBionicspiderSwitchGroup)
	ON_UPDATE_COMMAND_UI(ID_BIONICSPIDER_SWITCHGROUP, OnUpdateBionicspiderSwitchGroup)
	ON_COMMAND(ID_BIONICSPIDER_SWITCHWINDOWS, OnBionicspiderSwitchWindows)
	ON_UPDATE_COMMAND_UI(ID_BIONICSPIDER_SWITCHWINDOWS, OnUpdateBionicspiderSwitchWindows)
	ON_COMMAND(ID_BIONICSPIDER_TRAYPOPUP, OnBionicspiderTrayPopUp)
	ON_UPDATE_COMMAND_UI(ID_BIONICSPIDER_TRAYPOPUP, OnUpdateBionicspiderTrayPopUp)
	ON_COMMAND(ID_WINDOW_MINIMIZETOTRAY, OnWindowMinimizeToTray)
	ON_COMMAND(ID_BIONICSPIDER_HOTKEYS, OnBionicspiderHotkeys)
	ON_COMMAND(ID_BIONICSPIDER_AUTOGROUP, OnBionicspiderAutogroup)
	ON_UPDATE_COMMAND_UI(ID_BIONICSPIDER_AUTOGROUP, OnUpdateBionicspiderAutogroup)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_ALWAYSONTOP, OnUpdateWindowAlwaysOnTop)
	ON_COMMAND(ID_HELP_CHECKFORUPDATE, OnHelpCheckForUpdate)
	ON_COMMAND(ID_POSITIONING_MEMORY, OnPositioningWindowMemory)
	ON_COMMAND(ID_POSITIONING_RECALL, OnPositioningWindowRecall)
	ON_UPDATE_COMMAND_UI(ID_EXPLORER_CONSOLE, OnUpdateExplorerConsole)
	ON_UPDATE_COMMAND_UI(ID_EXPLORER_CLONE, OnUpdateExplorerClone)
	ON_COMMAND(ID_BIONICSPIDER_SWITCHCURSOR, OnBionicspiderSwitchCursor)
	ON_UPDATE_COMMAND_UI(ID_BIONICSPIDER_SWITCHCURSOR, OnUpdateBionicspiderSwitchCursor)
	ON_COMMAND(ID_BIONICSPIDER_HIDE, OnBionicspiderHide)
	ON_COMMAND(ID_BIONICSPIDER_FONTS, OnBionicspiderFonts)
	ON_COMMAND(ID_BIONICSPIDER_FULLROW, OnBionicspiderFullrow)
	ON_UPDATE_COMMAND_UI(ID_BIONICSPIDER_FULLROW, OnUpdateBionicspiderFullRow)
        ON_COMMAND(ID_BIONICSPIDER_ANIMATE,OnBionicspiderAnimate)
        ON_UPDATE_COMMAND_UI(ID_BIONICSPIDER_ANIMATE,OnUpdateBionicspiderAnimate)
	ON_COMMAND(ID_ROOT_COLLAPSE, OnRootCollapse)
	ON_COMMAND(ID_ROOT_EXPAND, OnRootExpand)
	ON_COMMAND(ID_GROUP_ONE, OnMenuGroupOne)
	END_MESSAGE_MAP()

// CBionicSpiderDlg message handlers
BOOL CBionicSpiderDlg::OnInitDialog()
  {
      CDialog::OnInitDialog();

      TRACE("MENU ARRAY SIZE: %d.\n",MENU_SETTINGS_SIZE);

          // Add "About..." menu item to system menu.

          // IDM_ABOUTBOX must be in the system command range.
      ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
      ASSERT(IDM_ABOUTBOX < 0xF000);

      CMenu* pSysMenu = GetSystemMenu(FALSE);
      if (pSysMenu != NULL)
      {
          CString strAboutMenu;
          strAboutMenu.LoadString(IDS_ABOUTBOX);
          if (!strAboutMenu.IsEmpty())
          {
              pSysMenu->AppendMenu(MF_SEPARATOR);
              pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
          }
      }

      CString buffer;
      buffer.LoadString(IDS_APP_TITLEBAR);
      SetWindowText(buffer);

          // Set the icon for this dialog.  The framework does this automatically
          //  when the application's main window is not a dialog
      SetIcon(m_hIcon, TRUE);			// Set big icon
      SetIcon(m_hIcon, FALSE);		// Set small icon

          // TODO: Add extra initialization here
      m_hMainMenu = ::GetMenu(m_hWnd);
      ASSERT(m_hMainMenu);

          //Load up the tile bitmaps into the tile submenu under "Group".
      CMenu *subMenu = NULL;
      subMenu = GetMenu()->GetSubMenu(2);
      ASSERT(subMenu);

      if(subMenu)
      {
          m_menuMaps[0].LoadBitmap(IDB_BITMAP_1x1_HORT);
          m_menuMaps[1].LoadBitmap(IDB_BITMAP_1x1_VERT);
          m_menuMaps[2].LoadBitmap(IDB_BITMAP_2x2);

          subMenu->SetMenuItemBitmaps(ID_TILEWINDOWS_1X1HORT,MF_BYCOMMAND,
                                      &m_menuMaps[0],&m_menuMaps[0]);

          subMenu->SetMenuItemBitmaps(ID_TILEWINDOWS_1X1VERT,MF_BYCOMMAND,
                                      &m_menuMaps[1],&m_menuMaps[1]);

          subMenu->SetMenuItemBitmaps(ID_TILEWINDOWS_2X2,MF_BYCOMMAND,
                                      &m_menuMaps[2],&m_menuMaps[2]);
      }

          //Load default menu settings from registry.
      LoadMenuSettings();

          //Are we always topmost?
      SetAlwaysOnTop(m_bMenuSettings[MENU_ALWAYS_ON_TOP]);

          //Subclass our CTreeCtrl.
      m_bionicTree.SubclassDlgItem(CBionicTree::ID,this);

          //Load up tree images and install root node.
      m_bionicTree.LoadTreeImages();
      m_bionicTree.InstallRootNode();

//STATUS STATUSBAR CONFIG
      UINT indi[1] = {IDS_INFO_SELECTION_GROUP};
      m_status.CreateEx(this,WS_VISIBLE | WS_CHILD | CCS_BOTTOM);//CBRS_BOTTOM);
      m_status.SetIndicators(indi,1);
      m_status.SetPaneInfo(0,0,SBPS_STRETCH,0);
      RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
//END STATUSBAR CONFIG

      CString traytext;
      traytext.LoadString(IDS_APP_TITLEBAR_DOCKED);
      TrayIcon(GetSafeHwnd(),traytext,0,m_hIcon,BS_TRAY_ICON_ID);

      m_bionicTree.SetSwitchGroup(m_bMenuSettings[MENU_SWITCH_GROUP]);
      m_bionicTree.SetSwitchWindow(m_bMenuSettings[MENU_SWITCH_WINDOW]);

      if(m_bMenuSettings[MENU_FULLROWSEL])
          m_bionicTree.SetFullRowSelect();

      if(m_bMenuSettings[MENU_SWITCH_CURSOR])
          m_bionicTree.SetSwitchCursor(TRUE);

      if(m_bMenuSettings[MENU_DOCKING_REGISTRED])
          Docking();

      if(m_bMenuSettings[MENU_TRAY_POPUP])
          InitPopupTray(TRUE);

#ifdef LITE_UPGRADE
          //No hotkeys and limited to just 2 Groups...
      m_bionicTree.InstallGroupFolder(_T("Group 1"));
      m_bionicTree.InstallGroupFolder(_T("Group 2"));
#else
          //Install saved groups from registry - do this after we have loaded saved group hotkeys
          //so that we can load them up while we install them under the root node.
      if(m_bMenuSettings[MENU_HOTKEYS])
      {
          LoadHotKeys();              //Load up hotkeys for main window.
          m_bionicTree.LoadHotKeys(); //Load up hotkeys for our CBionicTree.
      }
      m_bionicTree.InstallGroups();
#endif      

          //Install font for the grouping tree.
      m_bionicTree.SetTreeFont();

          //Lastly, auto group windows if set.
      if(m_bMenuSettings[MENU_AUTOGROUP])
          m_bionicTree.AutoGroup(m_bMenuSettings[MENU_AUTOGROUP]);

      UpdateGroupListMenu();

      return TRUE;  //return TRUE unless you set the focus to a control
  }

void CBionicSpiderDlg::OnSysCommand(UINT nID, LPARAM lParam)
  {
      if ((nID & 0xFFF0) == IDM_ABOUTBOX)
      {
          CAboutDlg dlgAbout;
          dlgAbout.DoModal();
      }
      else if(SC_MINIMIZE == nID)
      {
          if(m_bMenuSettings[MENU_ANIMATE])
          {
              CWireFrame wireframe;
              wireframe.WireFrameThis(GetSafeHwnd(),GetTrayHwnd());
          }
          ShowWindow(SW_HIDE);
      }
      else
      {
          CDialog::OnSysCommand(nID, lParam);
      }
  }

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBionicSpiderDlg::OnPaint() 
  {

//       DRAW A STATUS BAR ON A DIALOG...
//       if(!IsZoomed())
//       {
//           CRect rc;
//           GetClientRect(rc);
//           rc.left = rc.right-GetSystemMetrics(SM_CXHSCROLL);
//           rc.top = rc.bottom-GetSystemMetrics(SM_CYVSCROLL);
//           CClientDC dc(this);
//           dc.DrawFrameControl(rc,DFC_SCROLL,DFCS_SCROLLSIZEGRIP);
//       }

      if (IsIconic())
      {
          CPaintDC dc(this); // device context for painting

          SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

              // Center icon in client rectangle
          int cxIcon = GetSystemMetrics(SM_CXICON);
          int cyIcon = GetSystemMetrics(SM_CYICON);
          CRect rect;
          GetClientRect(&rect);
          int x = (rect.Width() - cxIcon + 1) / 2;
          int y = (rect.Height() - cyIcon + 1) / 2;

              // Draw the icon
          dc.DrawIcon(x, y, m_hIcon);
      }
      else
      {
          CDialog::OnPaint();
      }
  }

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBionicSpiderDlg::OnQueryDragIcon()
  {
      return static_cast<HCURSOR>(m_hIcon);
  }

//Prevent the ENTER key etc. from killing main Dialog
BOOL CBionicSpiderDlg::PreTranslateMessage(MSG *pMsg)
  {
      if (m_ttip.GetSafeHwnd())
          m_ttip.RelayEvent(pMsg);

      if(pMsg->message == WM_KEYDOWN)
      {
          if(pMsg->wParam == VK_RETURN ||
             pMsg->wParam == VK_ESCAPE)
          {
              ::TranslateMessage(pMsg);
              ::DispatchMessage(pMsg);
              return TRUE;                    
          }

#ifdef _EASTER_EGGS
          short HIGH_BIT = 1<<15;
          if(GetKeyState('M') & HIGH_BIT &&
             GetKeyState('L') & HIGH_BIT &&
             GetKeyState('G') & HIGH_BIT &&
             GetKeyState('K') & HIGH_BIT &&
             GetKeyState('E') & HIGH_BIT)
              MessageBox(_T("7/18/2004"),_T("Perfect Match!"));
#endif

      }
      if(m_hAccelTable &&
         ::TranslateAccelerator(m_hWnd,m_hAccelTable,pMsg))
          return TRUE;

      return CDialog::PreTranslateMessage(pMsg);
  }

afx_msg void CBionicSpiderDlg::OnSizing(UINT nSide,
                                        LPRECT lpRect)
  {
      CDialog::OnSizing(nSide,lpRect);

      if(m_bMenuSettings[MENU_DOCKING_REGISTRED])// && nSide!=3 && nSide!=6)
      {
          AppBar_Size();

          if(nSide==WMSZ_TOP        ||
             nSide==WMSZ_TOPLEFT    ||
             nSide==WMSZ_TOPRIGHT   ||
             nSide==WMSZ_BOTTOM     ||
             nSide==WMSZ_BOTTOMLEFT ||
             nSide==WMSZ_BOTTOMRIGHT)
          {
              RECT rcWin;
              GetWindowRect(&rcWin);

              lpRect->top = rcWin.top;
              lpRect->bottom = rcWin.bottom;
          }
      }
  }

//Resize all children of Dlg.
afx_msg void CBionicSpiderDlg::OnSize(UINT how,
                                      int cx,
                                      int cy)
  {
      CDialog::OnSize(how,cx,cy);

      if(m_bionicTree.GetSafeHwnd()) //Make sure we have sub-classed our TREE first.
      {
              //Adjust the dimenions of the surrounding windows when docked and sized.
          if(m_bMenuSettings[MENU_DOCKING_REGISTRED])
          {
              RECT rcWindow;

              AppBar_Size();
              GetWindowRect(&rcWindow);

              if (m_dockingOpt.uSide==ABE_TOP || m_dockingOpt.uSide==ABE_BOTTOM)
                  m_dockingOpt.cyHeight=rcWindow.bottom-rcWindow.top;
              else
                  m_dockingOpt.cxWidth=rcWindow.right-rcWindow.left;
          }
          CRect rStatus;
          CRect r;

          RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
          m_status.GetClientRect(&rStatus);
          m_bionicTree.GetWindowRect(&r);
          ScreenToClient(&r);
          r.right = cx - 3;
          r.bottom = cy - rStatus.bottom; //originally cy - 3
          m_bionicTree.MoveWindow(&r);
      }
  }

//START CLEAN UP CODE.
afx_msg void CBionicSpiderDlg::OnClose()
  {
      if(CleanUp())
          CDialog::OnClose();
  }

void CBionicSpiderDlg::OnBionicSpiderExit()
  {
      CleanUp();
  }

BOOL CBionicSpiderDlg::CleanUp()
  {
      TRACE("Starting CLEAN UP code...\n");

      if(m_bMenuSettings[MENU_WARNINGS])
      {
          CString s;
          s.LoadString(IDS_EXIT_WARNING);

          if(!ConfirmMsgBox(GetSafeHwnd(),s))
              return FALSE;
      }

      m_bionicTree.FreeAllGroupWindows();
      m_bionicTree.DumpGroups();
      m_bionicTree.SaveTreeFont();

          //Dump menu settings into the registry.
      DumpMenuSettings();

          //Dump hotkey settings into the reg.
      m_bionicTree.m_hotkeyGroups.SetRegistryString();
      m_hotkey.SetRegistryString();

      Undock();
      TrayIcon(GetSafeHwnd(),"",2,m_hIcon,0);

      PostQuitMessage(0);

      return TRUE;
  }

//END CLEAN UP CODE.

//START MAIN MENU CODE.

//Hack to make update UI handler's work for CDialog's etc.
afx_msg void CBionicSpiderDlg::OnInitMenuPopup(CMenu* pPopupMenu,
                                               UINT nIndex,
                                               BOOL bSysMenu)
  {
      CDialog::OnInitMenuPopup(pPopupMenu,nIndex,bSysMenu);
      CCmdUI cmdUI;

      cmdUI.m_nIndexMax = pPopupMenu->GetMenuItemCount();
      for(UINT n=0;n<cmdUI.m_nIndexMax;++n)
      {
          cmdUI.m_nIndex = n;
          cmdUI.m_nID = pPopupMenu->GetMenuItemID(cmdUI.m_nIndex);
          cmdUI.m_pMenu = pPopupMenu;
          CCmdTarget* pTarget = this;

              // Undocumented MFC cmd calls the ON_UPDATE_COMMAND_UI funcs.
          cmdUI.DoUpdate(pTarget, FALSE);
      }
  }

void CBionicSpiderDlg::SetAlwaysOnTop(BOOL bAlwaysOnTop)
  {
      SetWindowPos((bAlwaysOnTop) ? &wndTopMost : &wndNoTopMost,
                   0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);      
  }


void CBionicSpiderDlg::OnBionicspiderAlwaysOnTop()
  {
      SetAlwaysOnTop((m_bMenuSettings[MENU_ALWAYS_ON_TOP] = (!m_bMenuSettings[MENU_ALWAYS_ON_TOP])));
  }

void CBionicSpiderDlg::OnBionicspiderWarnings()
  {
      m_bMenuSettings[MENU_WARNINGS] = (!m_bMenuSettings[MENU_WARNINGS]);
  }

void CBionicSpiderDlg::OnBionicspiderTrayPopUp()
  {
      m_bMenuSettings[MENU_TRAY_POPUP] = (!m_bMenuSettings[MENU_TRAY_POPUP]);

      if(m_bMenuSettings[MENU_DOCKING_REGISTRED])
      {
          Undock();
          m_bMenuSettings[MENU_DOCKING_REGISTRED] = FALSE;
      }
      InitPopupTray(m_bMenuSettings[MENU_TRAY_POPUP]);
  }

void CBionicSpiderDlg::OnBionicspiderFullrow()
  {
      m_bMenuSettings[MENU_FULLROWSEL] = !m_bMenuSettings[MENU_FULLROWSEL];
      m_bionicTree.SetFullRowSelect(m_bMenuSettings[MENU_FULLROWSEL]);
  }

void CBionicSpiderDlg::OnBionicspiderAnimate()
  {
      m_bMenuSettings[MENU_ANIMATE] = !m_bMenuSettings[MENU_ANIMATE];
  }

void CBionicSpiderDlg::OnBionicspiderAutogroup()
  {
      m_bMenuSettings[MENU_AUTOGROUP] = (!m_bMenuSettings[MENU_AUTOGROUP]);
      m_bionicTree.AutoGroup(m_bMenuSettings[MENU_AUTOGROUP]);
  }

void CBionicSpiderDlg::Docking()
  {
      if(m_bMenuSettings[MENU_TRAY_POPUP])
      {
          m_bMenuSettings[MENU_TRAY_POPUP] = FALSE;
          InitPopupTray(m_bMenuSettings[MENU_TRAY_POPUP]);
      }
      AppBar_Register();

      ShowWindow(SW_HIDE);
      ModifyStyleEx(0,WS_EX_TOOLWINDOW);

//    if(NULL == m_dockingOpt)
//       m_dockingOpt = (DOCKING_OPTIONS*)HeapAlloc(GetProcessHeap(),
//                                                  HEAP_ZERO_MEMORY,sizeof(DOCKING_OPTIONS));

      m_dockingOpt.fAutoHide = FALSE;
      m_dockingOpt.fOnTop = FALSE;
      m_dockingOpt.uSide = 0;
      m_dockingOpt.cxWidth = 120;//235
      m_dockingOpt.cyHeight = 100;

      AppBar_SetSide((m_bMenuSettings[MENU_DOCKED_LEFT]) ? ABE_LEFT : ABE_RIGHT);
          //Refresh frame to reflect WS_EX_TOOLWINDOW style.
      SetWindowPos(&wndTop,0,0,0,0,SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE);

      CString titlebar;
      titlebar.LoadString(IDS_APP_TITLEBAR_DOCKED);
      SetWindowText(titlebar);
      ShowWindow(SW_SHOW);

      SetMenu(NULL); //Hide main menu from window.      
  }

void CBionicSpiderDlg::OnDockingToggle()
  {
      if(FALSE == m_bMenuSettings[MENU_DOCKING_REGISTRED])
          Docking();
      else
          Undock();

          //Update the status bar.
//       HTREEITEM hNode = m_bionicTree.GetSelectedItem();
//       ASSERT(hNode);
  }

void CBionicSpiderDlg::OnDockingRight()
  {
      m_bMenuSettings[MENU_DOCKED_LEFT] = FALSE;
      if(m_bMenuSettings[MENU_DOCKING_REGISTRED])
          AppBar_SetSide(ABE_RIGHT);
      else
          OnDockingToggle();
  }

void CBionicSpiderDlg::OnDockingLeft()
  {
      m_bMenuSettings[MENU_DOCKED_LEFT] = TRUE;
      if(m_bMenuSettings[MENU_DOCKING_REGISTRED])
          AppBar_SetSide(ABE_LEFT);
      else
          OnDockingToggle();
  }

void CBionicSpiderDlg::OnBionicspiderSwitchGroup()
  {
      m_bMenuSettings[MENU_SWITCH_GROUP] = !m_bMenuSettings[MENU_SWITCH_GROUP];
      m_bionicTree.SetSwitchGroup(m_bMenuSettings[MENU_SWITCH_GROUP]);
  }

void CBionicSpiderDlg::OnBionicspiderSwitchWindows()
  {
      m_bMenuSettings[MENU_SWITCH_WINDOW] = !m_bMenuSettings[MENU_SWITCH_WINDOW];
      m_bionicTree.SetSwitchWindow(m_bMenuSettings[MENU_SWITCH_WINDOW]);
  }

void CBionicSpiderDlg::OnBionicspiderSwitchCursor()
  {
      m_bMenuSettings[MENU_SWITCH_CURSOR] = !m_bMenuSettings[MENU_SWITCH_CURSOR];
      m_bionicTree.SetSwitchCursor(m_bMenuSettings[MENU_SWITCH_CURSOR]);
  }

void CBionicSpiderDlg::OnBionicspiderHotkeys()
  {
#ifdef LITE_UPGRADE
      upgrade_msg();
#else      
      CList<CString> groups;
      
      m_bionicTree.GetGroupNames(groups);
      CHotkeyDlg hotkeyDlg(this);

          //Temporarily disable hotkeys while hotkey dialog is running.
      m_bHotkeyDlgInit = TRUE;
      m_bionicTree.EnableHotkeys(FALSE);

      hotkeyDlg.DoModal();

      m_bHotkeyDlgInit = FALSE;
      m_bionicTree.EnableHotkeys(TRUE);
#endif      
  }

void CBionicSpiderDlg::OnBionicspiderHide()
  {
      ShowHideSpider();
  }
//END MAIN MENU CODE.

//START ROOT MENU
//Input Dlg
BOOL CBionicSpiderDlg::GetInputText(LPCTSTR lpszAppName,
                                    LPCTSTR lpszInfo,
                                    LPCTSTR lpszDefault,
                                    CString &cstrTextBuffer)
  {
      CInputTextDlg InputDlg(this);

      CString cstrBuffer(lpszAppName);
      CString cstrDefault,cstrFeedback;

      InputDlg.SetCaption(cstrBuffer,lpszDefault,lpszInfo);
      InputDlg.SetBuffer(&cstrFeedback);
      InputDlg.DoModal();

      if(cstrFeedback.IsEmpty())
          return FALSE;
      else
          cstrTextBuffer = cstrFeedback;

      return TRUE;
  }

//Add a new window group under the root.
void CBionicSpiderDlg::OnRootAddGroup()
  {
#ifdef LITE_UPGRADE
      upgrade_msg();
#else
      CString buffer;
      CString appName;
      CString groupText;
      CString groupName;

      appName.LoadString(IDS_APP_NAME);
      groupText.LoadString(IDS_GROUP_NEW);

      groupName.Format(_T("Group %d"),m_bionicTree.GetNumOfGroups()+1);

      if(GetInputText(appName,groupText,groupName,buffer))
          m_bionicTree.InstallGroupFolder(buffer);

      UpdateGroupListMenu();
#endif      
  }

void CBionicSpiderDlg::OnRootHideAllGroups()
  {
      m_bionicTree.HideAllGroupWindows();
  }

void CBionicSpiderDlg::OnRootShowAllGroups()
  {
      m_bionicTree.ShowAllGroupWindows();
  }

void CBionicSpiderDlg::OnRootMaximizeAllGroups()
  {
      m_bionicTree.MaxAllGroupWindows();
  }

void CBionicSpiderDlg::OnRootMinimizeAllGroups()
  {
      m_bionicTree.MinAllGroupWindows();
  }

void CBionicSpiderDlg::OnRootRestoreAllGroups()
  {
      m_bionicTree.RestAllGroupWindows();
  }

void CBionicSpiderDlg::OnRootCollapse()
  {
      m_bionicTree.CollapseAllGroups();
  }

void CBionicSpiderDlg::OnRootExpand()
  {
      m_bionicTree.CollapseAllGroups(FALSE);
  }

void CBionicSpiderDlg::OnRootSortAllGroups()
  {
      m_bionicTree.SortAllGroups();
  }

void CBionicSpiderDlg::OnRootCloseAllGroups()
  {
      if(m_bMenuSettings[MENU_WARNINGS])
      {
          CString msg;
          msg.LoadString(IDS_WARNING_ROOT_CLOSE);

          if(FALSE == ConfirmMsgBox(GetSafeHwnd(),msg))
              return;
      }
      m_bionicTree.CloseAllGroupWindows();
  }

void CBionicSpiderDlg::OnRootRemoveWindowsInAllGroups()
  {
      if(m_bMenuSettings[MENU_WARNINGS])
      {
          CString msg;
          msg.LoadString(IDS_WARNING_ROOT_REMOVE);

          if(FALSE == ConfirmMsgBox(GetSafeHwnd(),msg))
              return;
      }
      m_bionicTree.RemoveAllGroupWindows();
  }

void CBionicSpiderDlg::OnRootRemoveAllGroups()
  {
      if(m_bMenuSettings[MENU_WARNINGS])
      {
          CString cstrRemoveMsg;
          cstrRemoveMsg.LoadString(IDS_WARNING_REMOVE_ALL_GROUPS);

          if(FALSE == ConfirmMsgBox(GetSafeHwnd(),cstrRemoveMsg))
              return;

          if(m_bionicTree.VerifyGroupRoot())
          {
              cstrRemoveMsg.LoadString(IDS_WARNING_SINGLE_GROUP);
              AlertMsgBox(GetSafeHwnd(),cstrRemoveMsg);
              return;
          }
      }
      m_bionicTree.RemoveAllGroups();
      UpdateGroupListMenu();
  }
//END ROOT MENU

//START GROUP MENU
void CBionicSpiderDlg::OnGroupHideAllWindows()
  {
      m_bionicTree.HideGroupWindows();
  }

void CBionicSpiderDlg::OnGroupShowAllWindows()
  {
      m_bionicTree.ShowGroupWindows();
  }

void CBionicSpiderDlg::OnGroupMaximizeAllWindows()
  {
      m_bionicTree.MaxGroupWindows();
  }

void CBionicSpiderDlg::OnGroupMinimizeAllWindows()
  {
      m_bionicTree.MinGroupWindows();
  }

void CBionicSpiderDlg::OnGroupRestoreAllWindows()
  {
      m_bionicTree.RestGroupWindows();
  }

void CBionicSpiderDlg::OnTileWindows1x1Hort()
  {
      m_bionicTree.TileGroupWindows(CBionicTree::BT_TILE_1X1H);
  }

void CBionicSpiderDlg::OnTileWindows1x1Vert()
  {
      m_bionicTree.TileGroupWindows(CBionicTree::BT_TILE_1X1V);
  }

void CBionicSpiderDlg::OnTileWindows2x2()
  {
      m_bionicTree.TileGroupWindows(CBionicTree::BT_TILE_2X2);
  }

void CBionicSpiderDlg::OnGroupPositionsGroupMemory()
  {
      m_bionicTree.GroupMemory();

      CString msg;
      msg.LoadString(IDS_GROUPMEMORY);

      AlertMsgBox(GetSafeHwnd(),msg);
  }

void CBionicSpiderDlg::OnGroupPositionsRecallAll()
  {
      m_bionicTree.GroupRecall();
  }

void CBionicSpiderDlg::OnGroupAddWindowByTitle()
  {
      CString cstrBuffer;
      CString cstrAppName;
      CString cstrDefault;
      CString cstrInfo;

      cstrAppName.LoadString(IDS_APP_NAME);
      cstrInfo.LoadString(IDS_ADDWINCAP);
      cstrDefault.LoadString(IDS_ADDWINCAP_DEFAULT);

      if(GetInputText(cstrAppName,
                      cstrInfo,
                      cstrDefault,
                      cstrBuffer))
      {
          HWND hWnd;

          if((hWnd = ::FindWindow(NULL,cstrBuffer)))
          {
              m_bionicTree.AddWindowToTree(hWnd);
          }
          else
          {
              CString cstrError;
              
              cstrError.LoadString(IDS_ADDWINCAP_ERROR);
              AlertMsgBox(GetSafeHwnd(),cstrError);
          }
      }
  }

void CBionicSpiderDlg::OnGroupSortAllWindowsInGroup()
  {
      m_bionicTree.GroupSort();
  }

void CBionicSpiderDlg::OnGroupCloseAllWindowsInGroup()
  {
      if(m_bMenuSettings[MENU_WARNINGS])
      {
          CString msg;
          msg.LoadString(IDS_WARNING_CLOSE_GROUP);

          if(FALSE == ConfirmMsgBox(GetSafeHwnd(),msg))
              return;
      }
      m_bionicTree.CloseGroupWindows();
  }

void CBionicSpiderDlg::OnGroupRemoveAllWindowsInGroup()
  {
      if(m_bMenuSettings[MENU_WARNINGS])
      {
          CString msg;
          msg.LoadString(IDS_WARNING_REMOVE_WINGROUP);

          if(FALSE == ConfirmMsgBox(GetSafeHwnd(),msg))
              return;
      }
      m_bionicTree.RemoveGroupWindows();
  }

void CBionicSpiderDlg::OnGroupRemoveThisGroup()
  {
      if(m_bMenuSettings[MENU_WARNINGS])
      {
          CString cstrRemoveMsg;
          cstrRemoveMsg.LoadString(IDS_WARNING_REMOVE_GROUP);

              //Confirm that the user wants to remove this group.
          if(FALSE == ConfirmMsgBox(GetSafeHwnd(),cstrRemoveMsg))
              return;

              //We need at least 1 group under the root (2 nodes)
          if(m_bionicTree.VerifyGroupRoot())
          {
              cstrRemoveMsg.LoadString(IDS_WARNING_SINGLE_GROUP);
              AlertMsgBox(GetSafeHwnd(),cstrRemoveMsg);
              return;
          }
              //If no group is selected, then pop up defaults MsgBox.
          if(m_bionicTree.GetTreeNodeType()!=CBionicTree::BT_NODE_GROUP)
          {
              cstrRemoveMsg.LoadString(IDS_WARNING_GROUP_NOT_SELECTED);
              AlertMsgBox(GetSafeHwnd(),cstrRemoveMsg);
          }
      }
      m_bionicTree.RemoveSelectedGroup();
          //Update the Groups in menu list.
      UpdateGroupListMenu();
  }

void CBionicSpiderDlg::OnGroupRenameThisGroup()
  {
      CString cstrBuffer;
      CString cstrAppName;
      CString cstrDefault;
      CString cstrNewGroupText;

      cstrAppName.LoadString(IDS_APP_NAME);
      cstrNewGroupText.LoadString(IDS_GROUP_NEW);
      m_bionicTree.GetNodeText(cstrDefault,CBionicTree::BT_NODE_GROUP);

      if(GetInputText(cstrAppName,cstrNewGroupText,
                      cstrDefault,cstrBuffer))
      {
          m_bionicTree.RenameSelectedGroup(cstrBuffer);
          UpdateGroupListMenu();
      }
  }
//END GROUP MENU

//START WINDOW MENU

void CBionicSpiderDlg::OnWindowHideWindow()
  {
      m_bionicTree.HideWindow();
  }

void CBionicSpiderDlg::OnWindowShowWindow()
  {
      m_bionicTree.ShowWindow();
  }

void CBionicSpiderDlg::OnWindowForegroundWindow()
  {
      m_bionicTree.ForegroundWindow();
  }

void CBionicSpiderDlg::OnWindowMaximize()
  {
      m_bionicTree.MaxWindow();
  }

void CBionicSpiderDlg::OnWindowMinimize()
  {
      m_bionicTree.MinWindow();
  }

void CBionicSpiderDlg::OnWindowRestore()
  {
      m_bionicTree.RestWindow();
  }

void CBionicSpiderDlg::OnWindowMinimizeToTray()
  {
      CBionicTree::SBT_TrayData trayData;

      if(m_bionicTree.GetTrayIconData(&trayData) &&
         BS_TRAY_ICON_ID != trayData.id) //Make sure we have a valid ID. (Not the main tray ID)
      {
          if(m_bMenuSettings[MENU_ANIMATE])
          {
              CWireFrame wireframe;
              wireframe.WireFrameThis(trayData.hWnd,GetTrayHwnd());

                  //Refresh desktop to erase frame trace.
              SystemParametersInfo(SPI_SETDESKWALLPAPER,0,NULL,SPIF_SENDCHANGE);
          }
          TrayIcon(GetSafeHwnd(),
                   trayData.cstrBuffer,0,
                   trayData.hIcon,
                   trayData.id);
      }
  }

void CBionicSpiderDlg::OnWindowAlwaysOnTop()
  {
      m_bionicTree.ToggleAlwaysOnTop();
  }

void CBionicSpiderDlg::OnPositioningWindowMemory()
  {
      if(m_bionicTree.WindowPositionMemory())
      {
          CString msg;
          
          msg.LoadString(IDS_WINMEMORY);
          AlertMsgBox(GetSafeHwnd(),msg);
      }
  }

void CBionicSpiderDlg::OnPositioningWindowRecall()
  {
      m_bionicTree.WindowPositionRecall();
  }

void CBionicSpiderDlg::OnWindowClose()
  {
      if(m_bMenuSettings[MENU_WARNINGS])
      {
          CString cstrRemoveMsg;
          cstrRemoveMsg.LoadString(IDS_WARNING_CLOSE_WINDOW);

          if(FALSE == ConfirmMsgBox(GetSafeHwnd(),cstrRemoveMsg))
              return;
      }
      m_bionicTree.RemoveWindow(TRUE); //Remove window and close it.
  }

void CBionicSpiderDlg::OnExplorerConsole()
  {
      m_bionicTree.WindowExConsole();
  }

void CBionicSpiderDlg::OnExplorerClone()
  {
      m_bionicTree.WindowExClone();
  }

void CBionicSpiderDlg::OnWindowRename()
  {
      CString cstrBuffer;
      CString cstrAppName;
      CString cstrDefault;
      CString cstrNewGroupText;

      cstrAppName.LoadString(IDS_APP_NAME);
      cstrNewGroupText.LoadString(IDS_WINDOW_RENAME);
      m_bionicTree.GetNodeText(cstrDefault,CBionicTree::BT_NODE_WINDOW);

      if(GetInputText(cstrAppName,cstrNewGroupText,
                      cstrDefault,cstrBuffer))
      {
          m_bionicTree.RenameWindow(cstrBuffer);
      }

  }

void CBionicSpiderDlg::OnWindowRemove()
  {
      if(m_bMenuSettings[MENU_WARNINGS])
      {
          CString cstrRemoveMsg;
          cstrRemoveMsg.LoadString(IDS_WARNING_REMOVE_WINDOW);

          if(FALSE == ConfirmMsgBox(GetSafeHwnd(),cstrRemoveMsg))
              return;
      }
      m_bionicTree.RemoveWindow();
  }

//END WINDOW MENU

//START HELP MENU

void CBionicSpiderDlg::OnHelpContents()
  {
      TCHAR path[MAX_PATH];
      TCHAR *p = NULL;
      CString helpURL;
      CString buffer;

      if(GetModuleFileName(NULL,path,MAX_PATH))
      {
          for(p = path;*p!=_T('\0');p++);
          for(;*p != '\\';p--);
          *p = '\0';

          helpURL.LoadString(IDS_HELP_PATH);
          buffer.Format("%s\\%s",path,helpURL);
          ShellExecute(NULL,"open",buffer,NULL,NULL,SW_SHOWNORMAL);
      }
  }

void CBionicSpiderDlg::OnHelpAbout()
  {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
  }
//END HELP MENU

//START MENU UPDATE
void CBionicSpiderDlg::OnUpdateTileWindows1x1hort(CCmdUI *pCmdUI)
  {
//      pCmdUI->Enable(FALSE);
  }

void CBionicSpiderDlg::OnUpdateTileWindows1x1vert(CCmdUI *pCmdUI)
  {
//      pCmdUI->Enable(FALSE);
  }

void CBionicSpiderDlg::OnUpdateTileWindows2x2(CCmdUI *pCmdUI)
  {
//    pCmdUI->Enable(FALSE);
  }

void CBionicSpiderDlg::OnUpdateBionicspiderAlwaysOnTop(CCmdUI *pCmdUI)
  {
//    pCmdUI->SetCheck(m_bAlwaysOnTop);
      pCmdUI->SetCheck(m_bMenuSettings[MENU_ALWAYS_ON_TOP]);
  }

void CBionicSpiderDlg::OnUpdateBionicspiderWarnings(CCmdUI *pCmdUI)
  {
      pCmdUI->SetCheck(m_bMenuSettings[MENU_WARNINGS]);
  }

void CBionicSpiderDlg::OnUpdateBionicspiderTrayPopUp(CCmdUI *pCmdUI)
  {
      BOOL bEnable = TRUE;

      if(m_bMenuSettings[MENU_DOCKING_REGISTRED])
          bEnable = FALSE;
      else
          pCmdUI->SetCheck(m_bMenuSettings[MENU_TRAY_POPUP]);

      pCmdUI->Enable(bEnable);
  }

void CBionicSpiderDlg::OnUpdateBionicspiderFullRow(CCmdUI *pCmdUI)
  {
      pCmdUI->SetCheck(m_bMenuSettings[MENU_FULLROWSEL]);
  }

void CBionicSpiderDlg::OnUpdateBionicspiderAnimate(CCmdUI *pCmdUI)
  {
      pCmdUI->SetCheck(m_bMenuSettings[MENU_ANIMATE]);
  }

void CBionicSpiderDlg::OnUpdateDockingEnableDocking(CCmdUI *pCmdUI)
  {
      BOOL bEnable = TRUE;

      if(m_bMenuSettings[MENU_TRAY_POPUP])
          bEnable = FALSE;
      else
          pCmdUI->SetCheck(m_bMenuSettings[MENU_DOCKING_REGISTRED]);

      pCmdUI->Enable(bEnable);
  }

void CBionicSpiderDlg::OnUpdateDockingRight(CCmdUI *pCmdUI)
  {
      BOOL bEnable = TRUE;

      if(m_bMenuSettings[MENU_TRAY_POPUP])
          bEnable = FALSE;
      else
          pCmdUI->SetRadio((!m_bMenuSettings[MENU_DOCKED_LEFT]));

      pCmdUI->Enable(bEnable);
  }

void CBionicSpiderDlg::OnUpdateDockingLeft(CCmdUI *pCmdUI)
  {
      BOOL bEnable = TRUE;

      if(m_bMenuSettings[MENU_TRAY_POPUP])
          bEnable = FALSE;
      else
          pCmdUI->SetRadio(m_bMenuSettings[MENU_DOCKED_LEFT]);

      pCmdUI->Enable(bEnable);
  }

void CBionicSpiderDlg::OnUpdateBionicspiderSwitchGroup(CCmdUI *pCmdUI)
  {
      pCmdUI->SetCheck(m_bMenuSettings[MENU_SWITCH_GROUP]);
  }

void CBionicSpiderDlg::OnUpdateBionicspiderSwitchWindows(CCmdUI *pCmdUI)
  {
      pCmdUI->SetCheck(m_bMenuSettings[MENU_SWITCH_WINDOW]);
  }

void CBionicSpiderDlg::OnUpdateBionicspiderSwitchCursor(CCmdUI *pCmdUI)
  {
      pCmdUI->SetCheck(m_bMenuSettings[MENU_SWITCH_CURSOR]);
  }

void CBionicSpiderDlg::OnUpdateBionicspiderAutogroup(CCmdUI *pCmdUI)
  {
      pCmdUI->SetCheck(m_bMenuSettings[MENU_AUTOGROUP]);
  }

void CBionicSpiderDlg::OnUpdateWindowAlwaysOnTop(CCmdUI *pCmdUI)
  {
      pCmdUI->SetCheck(m_bionicTree.GetAlwaysOnTop());
  }

void CBionicSpiderDlg::OnUpdateExplorerConsole(CCmdUI *pCmdUI)
  {
      pCmdUI->Enable(m_bionicTree.IsExplorerCheck());
  }

void CBionicSpiderDlg::OnUpdateExplorerClone(CCmdUI *pCmdUI)
  {
      pCmdUI->Enable(m_bionicTree.IsExplorerCheck());
  }

//END MENU UPDATE

//Pop up one of the context menus based on nMenuID (Root,Group,Window)
void CBionicSpiderDlg::PopContextMenu(int nMenuID)
  {
      CMenu *pMainMenu = GetMenu();
      CMenu *pSubMenu = NULL;

      if(NULL == pMainMenu)
      {
          CMenu mainMenu;
          mainMenu.Attach(m_hMainMenu);
          pSubMenu = mainMenu.GetSubMenu(nMenuID);
          mainMenu.Detach();
          TRACE("USING MENU HANDLE.\n");
      }
      else
      {
          pSubMenu = pMainMenu->GetSubMenu(nMenuID);
          TRACE("USING GetMenu().\n");
      }
      ASSERT(NULL!=pSubMenu);

      if(pSubMenu)
      {
          POINT pt;
          ::SetForegroundWindow(GetSafeHwnd());
          GetCursorPos(&pt);
          pSubMenu->TrackPopupMenu(0,pt.x,pt.y,this);
          PostMessage(WM_NULL,0,0);
      }
  }

//This msg will be sent by our sub-classed CTreeCtrl object.
afx_msg LRESULT CBionicSpiderDlg::OnTreeRightClick(WPARAM wParam,
                                                   LPARAM lParam)
  {
      OnUpdateInfo(wParam,lParam);
      PopContextMenu((int)lParam);

      return 0;
  }

//END MENU CODE.

//START APPBAR DOCKING CODE.
void CBionicSpiderDlg::Undock()
  {
      if(m_bMenuSettings[MENU_DOCKING_REGISTRED])
      {
          AppBar_UnRegister();

          ModifyStyleEx(WS_EX_TOOLWINDOW,0);
          ::SetMenu(m_hWnd,m_hMainMenu);

          SetWindowPos(&wndTop,0,0,300,450,SWP_FRAMECHANGED);
          
          CString titlebar;
          titlebar.LoadString(IDS_APP_TITLEBAR);
          SetWindowText(titlebar);

          CenterWndOnScreen(GetSafeHwnd());
          m_bMenuSettings[MENU_DOCKING_REGISTRED] = FALSE;

              //We are undocked - we can update the group MENU list now.
          UpdateGroupListMenu();
      }
  }

void CBionicSpiderDlg::AppBar_Register()
  {
      APPBARDATA abd;
	
      abd.cbSize = sizeof(APPBARDATA);
      abd.hWnd = GetSafeHwnd();
      abd.uCallbackMessage = WM_USER+1010;

      SHAppBarMessage(ABM_NEW,&abd);
      m_bMenuSettings[MENU_DOCKING_REGISTRED] = TRUE;
  }

void CBionicSpiderDlg::AppBar_Size()
  {
      RECT rc;
      APPBARDATA abd;

      if(m_bMenuSettings[MENU_DOCKING_REGISTRED])
      {
          abd.cbSize = sizeof(APPBARDATA);
          abd.hWnd = GetSafeHwnd();

          GetWindowRect(&rc);
          AppBar_QuerySetPos(m_dockingOpt.uSide,&rc,&abd,TRUE);
      }
  }

BOOL CBionicSpiderDlg::AppBar_SetSide(UINT uSide)
  {
//       if(!m_dockingOpt)
//           return FALSE;

      APPBARDATA abd;
      RECT rc;
      HWND hwnd = GetSafeHwnd();
      BOOL fAutoHide = FALSE;

          // Calculate the size of the screen so we can occupy the full width or
          // height of the screen on the edge we request.
//      rc.top=0;
//      rc.left=0;
//      rc.right=GetSystemMetrics(SM_CXSCREEN);
//      rc.bottom=GetSystemMetrics(SM_CYSCREEN);

      MONITORINFOEX mi;
      HMONITOR hMultiMon = NULL;

          //the following code is for multiple monitor support
      hMultiMon=MonitorFromWindow(GetSafeHwnd(),MONITOR_DEFAULTTOPRIMARY);
      mi.cbSize = sizeof(MONITORINFOEX);

      if(GetMonitorInfo(hMultiMon,&mi))
      {
          rc.top = mi.rcMonitor.top;
          rc.left = mi.rcMonitor.left;
          rc.right = mi.rcMonitor.right;
          rc.bottom = mi.rcMonitor.bottom;
      }
      else //defaults
      {
          rc.top = 0;
          rc.left = 0;
          rc.right = GetSystemMetrics(SM_CXSCREEN);
          rc.bottom = GetSystemMetrics(SM_CYSCREEN);
      }

          // Fill out the APPBARDATA struct with the basic information
      abd.cbSize = sizeof(APPBARDATA);
      abd.hWnd = hwnd;
          // Adjust the rectangle to set our height or width depending on the
          // side we want.
      switch(uSide)
      {
          case ABE_TOP:
              rc.bottom = rc.top + m_dockingOpt.cyHeight;
              break;
          case ABE_BOTTOM:
              rc.top = rc.bottom - m_dockingOpt.cyHeight;
              break;
          case ABE_LEFT:
              rc.right = rc.left + m_dockingOpt.cxWidth;
              break;
          case ABE_RIGHT:
              rc.left = rc.right - m_dockingOpt.cxWidth;
              break;
      }
          // Move the appbar to the new screen space.
      AppBar_QuerySetPos(uSide,&rc,&abd,TRUE);
      return (TRUE);
  }	

void CBionicSpiderDlg::AppBar_QuerySetPos(UINT uEdge,
                                          LPRECT lprc,
                                          PAPPBARDATA pabd,
                                          BOOL fMove)
  {
//       if(!m_dockingOpt)
//           return;

      int iHeight = 0;
      int iWidth = 0;
          // Fill out the APPBARDATA struct and save the edge we're moving to
          // in the appbar DOCKING_OPTIONS struct.
      pabd->rc = *lprc;
      pabd->uEdge = uEdge;
      m_dockingOpt.uSide = uEdge;

      AppBar_QueryPos(&(pabd->rc));
          // Tell the system we're moving to this new approved position.
      SHAppBarMessage(ABM_SETPOS, pabd);

      if(fMove)
          MoveWindow(pabd->rc.left,pabd->rc.top, 
                     pabd->rc.right-pabd->rc.left,
                     pabd->rc.bottom-pabd->rc.top,TRUE);
  }

void CBionicSpiderDlg::AppBar_QueryPos(LPRECT lprc)
  {
//       if(!m_dockingOpt)
//           return;

      HWND hwnd = GetSafeHwnd();
      APPBARDATA abd;
      int iWidth = 0;
      int iHeight = 0;

          // Fill out the APPBARDATA struct and save the edge we're moving to
          // in the appbar DOCKING_OPTIONS struct.
      abd.hWnd = hwnd;
      abd.cbSize = sizeof(APPBARDATA);
      abd.rc = *lprc;
      abd.uEdge = m_dockingOpt.uSide;

          // Calculate the part we want to occupy.  We only figure out the top
          // and bottom coordinates if we're on the top or bottom of the screen.
          // Likewise for the left and right.  We will always try to occupy the
          // full height or width of the screen edge.
      if((ABE_LEFT==abd.uEdge) || (ABE_RIGHT==abd.uEdge))
      {
          iWidth = abd.rc.right - abd.rc.left;

          abd.rc.top = 0;
          abd.rc.bottom = GetSystemMetrics(SM_CYSCREEN);
      }
      else
      {
          iHeight = abd.rc.bottom - abd.rc.top;
          abd.rc.left = 0;
          abd.rc.right = GetSystemMetrics(SM_CXSCREEN);
      }

          // Ask the system for the screen space
      SHAppBarMessage(ABM_QUERYPOS, &abd);

          // The system will return an approved position along the edge we're asking
          // for.  However, if we can't get the exact position requested, the system
          // only updates the edge that's incorrect.  For example, if we want to 
          // attach to the bottom of the screen and the taskbar is already there, 
          // we'll pass in a rect like 0, 964, 1280, 1024 and the system will return
          // 0, 964, 1280, 996.  Since the appbar has to be above the taskbar, the 
          // bottom of the rect was adjusted to 996.  We need to adjust the opposite
          // edge of the rectangle to preserve the height we want.

      switch (abd.uEdge)
      {
          case ABE_LEFT:
              abd.rc.right = abd.rc.left + iWidth;
              break;

          case ABE_RIGHT:
              abd.rc.left = abd.rc.right - iWidth;
              break;

          case ABE_TOP:
              abd.rc.bottom = abd.rc.top + iHeight;
              break;

          case ABE_BOTTOM:
              abd.rc.top = abd.rc.bottom - iHeight;
              break;
      }
      *lprc = abd.rc;
  }

void CBionicSpiderDlg::AppBar_UnRegister()
  {
      APPBARDATA abd;
	
      abd.cbSize = sizeof(APPBARDATA);
      abd.hWnd = GetSafeHwnd();

//    m_bMenuSettings[MENU_DOCKING_REGISTRED]=(!SHAppBarMessage(ABM_REMOVE,&abd));
//    return (!m_bMenuSettings[MENU_DOCKING_REGISTRED]);

      SHAppBarMessage(ABM_REMOVE,&abd);
      m_bMenuSettings[MENU_DOCKING_REGISTRED] = FALSE;
  }
//END APPBAR CODE

//START MISC. METHODS

//Msg. from the seeker. wParam = HWND of window.
afx_msg LRESULT CBionicSpiderDlg::OnSeekerWindow(WPARAM wParam,
                                                 LPARAM lParam)
  {
      BOOL bValid = m_bionicTree.ValidateWindow((HWND)wParam);
      if(bValid)
      {
              //If in auto group mode, check if this window is blacklisted, if so, remove it from the list.
          if(m_bMenuSettings[MENU_AUTOGROUP])
              m_bionicTree.RemoveBlacklisted((HWND)wParam);

          m_bionicTree.AddWindowToTree((HWND)wParam);
          //UpdateInfo();
      }
      if(FALSE == bValid &&
         m_bMenuSettings[MENU_WARNINGS])
      {
          CString msgWarning;
          msgWarning.LoadString(IDS_SEEKER_WARNING);

          AlertMsgBox(GetSafeHwnd(),msgWarning);
      }
      return 0;
  }

BOOL CBionicSpiderDlg::TrayIcon(HWND hWnd,
                                LPCTSTR lpszTip,
                                DWORD nAction,
                                HICON hIcon,
                                UINT uiID)
  {
      NOTIFYICONDATA ni;

      ni.uID = uiID;

      ni.cbSize = sizeof(ni);
      ni.hWnd = hWnd;
      lstrcpyn(ni.szTip,lpszTip,sizeof(ni.szTip));

      if(nAction == 1)
      {
          ni.uFlags = NIF_TIP;
          return Shell_NotifyIcon(NIM_MODIFY,&ni);
      }
      else if(nAction == 2)
          return Shell_NotifyIcon(NIM_DELETE,&ni);

      ni.hIcon = hIcon;

      ni.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
      ni.uCallbackMessage = CBionicSpiderDlg::WM_NOTIFYICON;

      return Shell_NotifyIcon(NIM_ADD,&ni);
  }

//Init. CDialog for tray popup state.
void CBionicSpiderDlg::InitPopupTray(BOOL bPopupTray)
  {
      DWORD dwRemove = 0;
      DWORD dwAdd = 0;
      int nShow = 0;

      int sizex = 300;
      int sizey = 400;

      ASSERT(m_hMainMenu);
      if(bPopupTray)
      {
          nShow = SW_HIDE;
          dwRemove = 0;
          dwAdd = WS_EX_TOOLWINDOW;
          SetMenu(NULL);
          
          sizex = 200;
          sizey = 300;
      }
      else
      {
          nShow = SW_SHOW;
          dwRemove = WS_EX_TOOLWINDOW;
          dwAdd = 0;
          ::SetMenu(GetSafeHwnd(),m_hMainMenu);
      }
      ModifyStyleEx(dwRemove,dwAdd);

      SetWindowPos(&wndTop,0,0,sizex,sizey,SWP_FRAMECHANGED | SWP_NOMOVE);

      if(SW_SHOW == nShow)
          ShowWindow(SW_SHOW);

          //Locate the taskbar.
      APPBARDATA task = {0};
      task.cbSize = sizeof(APPBARDATA);

      if(bPopupTray && (task.hWnd = ::FindWindow("Shell_TrayWnd",NULL)))
      {
          RECT rc;

          SHAppBarMessage(ABM_GETTASKBARPOS,&task);
          TRACE2("Taskbar position: right: %d, top: %d\n",task.rc.right,task.rc.top);

              //Get the entire rect, not just the client.
          GetWindowRect(&rc);

              //Convert to client coords.
          rc.right = rc.right - rc.left;
          rc.bottom = rc.bottom - rc.top;

          int x = 0;
          int y = 0;
          switch(task.uEdge)
          {
              case ABE_BOTTOM:
                  x = task.rc.right - rc.right;
                  y = task.rc.top - rc.bottom;
              break;

              case ABE_TOP:
                  x = task.rc.right - rc.right;
                  y = task.rc.bottom;
                  break;

              case ABE_RIGHT:
                  x = task.rc.left - rc.right;
                  y = task.rc.bottom - rc.bottom;
                  break;

              case ABE_LEFT:
                  x = task.rc.right;
                  y = task.rc.bottom - rc.bottom;
                  break;
          }
          SetWindowPos(&wndTop,x,y,0,0,SWP_NOSIZE);
      }
      else
          CenterWndOnScreen(GetSafeHwnd());
  }

//This msg. will be sent from the tree to check if "tray popup" is set.
afx_msg LRESULT CBionicSpiderDlg::OnTreeCheckPopup(WPARAM wParam,
                                                   LPARAM lParam)
  {
      return (LRESULT)m_bMenuSettings[MENU_TRAY_POPUP];
  }

//When removing windows from the tree we need to check the need to blacklist them - let the tree know.
afx_msg LRESULT CBionicSpiderDlg::OnTreeCheckAutoGroup(WPARAM wParam,
                                                       LPARAM lParam)
  {
      return (LRESULT)m_bMenuSettings[MENU_AUTOGROUP];
  }

afx_msg LRESULT CBionicSpiderDlg::OnTreeMouseLeave(WPARAM wParam,
                                                   LPARAM lParam)
  {
      if(m_bMenuSettings[MENU_TRAY_POPUP])
      {
          CRect rc;
          POINT pt;

          GetCursorPos(&pt);
          m_bionicTree.GetWindowRect(rc);
          rc.left += 5;
          rc.top += 5;
          rc.right -= 5;
          rc.bottom -= 5;
          
          if(FALSE == rc.PtInRect(pt))
          {
              if(m_bMenuSettings[MENU_ANIMATE])
              {
                  CWireFrame wireframe;
                  RECT rDrag,rTo;

                  ::GetWindowRect(this->m_hWnd,&rDrag);
                  ::GetWindowRect(GetTrayHwnd(),&rTo);

                  rTo.top = 0; //-= WIRE_FRAME_OFFSET;
                  rTo.bottom = 0; //-= WIRE_FRAME_OFFSET;
                  wireframe.WireFrameRects(rDrag,rTo);
              }
              ShowWindow(SW_HIDE);
          }
      }
      return 0;
  }

LRESULT CBionicSpiderDlg::OnNotifyIcon(WPARAM wParam,
                                       LPARAM lParam)
  {
          //Handle main tray ICON.
      if(BS_TRAY_ICON_ID == wParam)
      {
          switch(lParam)
          {
              case WM_LBUTTONUP:
              {
                  if(m_bMenuSettings[MENU_ANIMATE])
                  {
                      CWireFrame wireframe;
                      RECT rDrag,rTo;

                      ::GetWindowRect(GetTrayHwnd(),&rDrag);
                      ::GetWindowRect(this->m_hWnd,&rTo);

                      rDrag.top = 0;
                      rDrag.bottom = 0;

                          //Offset the drag RECT because the tray won't refresh itself.
//                   rDrag.top -= WIRE_FRAME_OFFSET;
//                   rDrag.bottom -= WIRE_FRAME_OFFSET;

                      wireframe.WireFrameRects(rDrag,rTo);
                  }
                  ShowWindow(SW_SHOW);
              }
              break;

              case WM_RBUTTONUP:
                  PopContextMenu(0); //Pop up the main context menu.
                  break;
          }
      }
      else
      {
              //Handle sys. icon click from a trayed window.
          if(WM_LBUTTONUP == lParam)
          {
                  //wParam = tray icon ID.
              m_bionicTree.SetStateID((UINT)wParam,SW_SHOW);
              TrayIcon(GetSafeHwnd(),"",2,0,(UINT)wParam);  //Remove icon from tray.
          }
      }
      return 0;
  }

BOOL CBionicSpiderDlg::ConfirmMsgBox(HWND hWnd,
                                     LPCTSTR lpszConfirm)
  {
      BOOL bConfirm = FALSE;
      CString cstrAppName;

      cstrAppName.LoadString(IDS_APP_NAME);
      if((::MessageBox(hWnd,lpszConfirm,cstrAppName,
                       MB_YESNO | MB_ICONQUESTION))==IDYES)
          bConfirm = TRUE;

      return bConfirm;
  }

void CBionicSpiderDlg::AlertMsgBox(HWND hWnd,
                                   LPCTSTR lpszAlert)
  {
      CString cstrAppName;
      cstrAppName.LoadString(IDS_APP_NAME);

      ::MessageBox(hWnd,lpszAlert,cstrAppName,MB_OK | MB_ICONINFORMATION);
  }

//Handle all global HOTKEY msgs.
afx_msg LRESULT CBionicSpiderDlg::OnHotKey(WPARAM wParam,
                                           LPARAM lParam)
  {
          //Only handle hotkeys if we do not have the hotkey Dialog initiated.
      if(!m_bHotkeyDlgInit)
      {
          CHotKeyMan::HotKeyID hotkeyID = (CHotKeyMan::HotKeyID)wParam;
          CHotKeyMan::Keydata kdata;

          if(m_hotkey.GetDataByHotKeyID(hotkeyID,kdata))
          {
              TRACE("MISC. HOTKEY:\n");

              switch(kdata.id)
              {
                  case HOTKEY_GROUP_UP:
                      m_bionicTree.MoveGroupUp();
                      break;
                  
                  case HOTKEY_GROUP_DOWN:
                      m_bionicTree.MoveGroupUp(FALSE);
                      break;
                  
                  case HOTKEY_GROUP_HIDE_SHOW:

                      if(m_bHotkeyToggle[HOTKEY_TOGGLE_HIDE_GROUP])
                          m_bionicTree.HideGroupWindows();
                      else
                          m_bionicTree.ShowGroupWindows();

                          //Toggle for the next round.
                      m_bHotkeyToggle[HOTKEY_TOGGLE_HIDE_GROUP] = !m_bHotkeyToggle[HOTKEY_TOGGLE_HIDE_GROUP];

                      break;

                  case HOTKEY_ROOT_HIDE_SHOW_ALL:

                      if(m_bHotkeyToggle[HOTKEY_TOGGLE_HIDE_ALL])
                          m_bionicTree.HideAllGroupWindows();
                      else
                          m_bionicTree.ShowAllGroupWindows();

                      m_bHotkeyToggle[HOTKEY_TOGGLE_HIDE_ALL] = !m_bHotkeyToggle[HOTKEY_TOGGLE_HIDE_ALL];

                      break;

                  case HOTKEY_FOREGROUND_SPIDER:
                      ShowHideSpider();
                  break;
              }
          }
      }
      return 0;
  }

//EXPLORER.EXE has crashed so reinstall main tray icon.
LRESULT CBionicSpiderDlg::OnTaskBarCreated(WPARAM wParam,
                                           LPARAM lParam)
  {
      CString traytext;
      traytext.LoadString(IDS_APP_TITLEBAR_DOCKED);

      TrayIcon(GetSafeHwnd(),traytext,0,m_hIcon,BS_TRAY_ICON_ID);
      return 0;
  }

//Get the system tray HWND.
HWND CBionicSpiderDlg::GetTrayHwnd()
  {
      if(m_hTrayHandle == NULL ||
         !IsWindow(m_hTrayHandle))
      {
          m_hTrayHandle = ::FindWindow(_T("Shell_TrayWnd"),NULL);
          m_hTrayHandle = ::FindWindowEx(m_hTrayHandle,NULL,_T("TrayNotifyWnd"),NULL);
          m_hTrayHandle = ::FindWindowEx(m_hTrayHandle,NULL,_T("SysPager"),NULL);
          m_hTrayHandle = ::FindWindowEx(m_hTrayHandle,NULL,_T("ToolbarWindow32"),NULL);
      }
      return m_hTrayHandle;
  }

//Dump menu settings to registry.
void CBionicSpiderDlg::DumpMenuSettings()
  {
      CRegistryEd reg;
      CString regPath;
      CString regKey;
      CString menuRegStr;
      CString delim;
      CString menuChecked,menuUnChecked;
      
      menuChecked.LoadString(IDS_MENU_CHECKED);
      menuUnChecked.LoadString(IDS_MENU_UNCHECKED);
      delim.LoadString(IDS_DELIMINATOR);

      regPath.LoadString(IDS_REGISTRY_PATH);
      regKey.LoadString(IDS_MENU_REGKEY);

      for(int i = 0;i<MENU_SETTINGS_SIZE;i++)
      {
          menuRegStr += (m_bMenuSettings[i]) ? menuChecked : menuUnChecked;
          menuRegStr += delim;
      }
      if(reg.SetPathKey(regPath) == ERROR_SUCCESS)
          reg.SetRegStr(menuRegStr,regKey,menuRegStr.GetLength()+1);
  }

//Reload the menu settings from the reg.
void CBionicSpiderDlg::LoadMenuSettings()
  {
      CRegistryEd reg;
      CString regPath;
      CString regKey;
      CString delim;
      CString menuChecked,menuUnChecked;

      CList<CString> groups;
      POSITION pos;

      menuChecked.LoadString(IDS_MENU_CHECKED);
      menuUnChecked.LoadString(IDS_MENU_UNCHECKED);
      delim.LoadString(IDS_DELIMINATOR);
      
      regPath.LoadString(IDS_REGISTRY_PATH);
      regKey.LoadString(IDS_MENU_REGKEY);
      
      if(reg.SetPathKey(regPath) == ERROR_SUCCESS)
      {
          CString buffer;
              //If we dont already have this reg key, then create it.
          if(reg.GetRegStr(regKey,buffer) != ERROR_SUCCESS)
          {
              TRACE("Creating DEFAULT MENU SETTINGS registry key.\n");

              buffer.LoadString(IDS_MENU_DEFAUTS);
              reg.SetRegStr(buffer,regKey,buffer.GetLength()+1);
          }
          CStringToken::TokenizeStr(buffer,groups,delim);

          pos = groups.GetHeadPosition();
          int n = 0;

          while(pos)
          {
              if(n<MENU_SETTINGS_SIZE)
              {
                  LPCTSTR menuItem = (LPCTSTR)groups.GetAt(pos);
                  if(menuChecked == menuItem)
                      m_bMenuSettings[n] = TRUE;
                  else if(menuUnChecked == menuItem)
                      m_bMenuSettings[n] = FALSE;
              }
              else
                  break;
              n++;
              groups.GetNext(pos);
          }
      }
  }

//Contact mother website and check for new updates...
void CBionicSpiderDlg::OnHelpCheckForUpdate()
  {
      CUpdateDlg updateDlg(this);
      updateDlg.DoModal();
  }

BOOL CBionicSpiderDlg::CenterWndOnScreen(HWND hWnd)
  {
      HWND hwndParent = NULL;
      RECT rect,rectP;
      int width,height;      
      int screenwidth,screenheight;
      int x,y;

      hwndParent=::GetDesktopWindow();
      ::GetWindowRect(hWnd,&rect);
      ::GetWindowRect(hwndParent,&rectP);
        
      width=rect.right-rect.left;
      height=rect.bottom-rect.top;

      x=((rectP.right-rectP.left)-width)/2+rectP.left;
      y=((rectP.bottom-rectP.top)-height)/2+rectP.top;

      screenwidth=GetSystemMetrics(SM_CXSCREEN);
      screenheight=GetSystemMetrics(SM_CYSCREEN);
    
//Make sure that the dialog box never moves outside of the screen
      if(x<0)x=0;
      if(y<0) y=0;
      if(x+width> screenwidth) x=screenwidth-width;
      if(y+height>screenheight)y=screenheight-height;

      ::ShowWindow(hWnd,FALSE);
      ::MoveWindow(hWnd,x,y,width,height,FALSE);
      ::ShowWindow(hWnd,TRUE);

      return TRUE;
  }

//Update the info. on seeker side.
// void CBionicSpiderDlg::UpdateInfo()
//   {
// //       if(!m_bMenuSettings[MENU_COMPACT])
// //       {
// //           CBionicTree::SBT_TreeData data;
// //           CString info,buffer;

// //           m_bionicTree.GetTreeData(&data);
// //           info.LoadString(IDS_INFO);

// //           buffer.Format(info,
// //                         data.numOfWindows,
// //                         data.numOfWindows - data.numOfHidden,
// //                         data.numOfHidden,
// //                         data.numOfGroups);

// //           m_info.SetWindowText(buffer);
// //       }
//   }

//Update the info. on the selection side.
void CBionicSpiderDlg::UpdateSelInfo(HTREEITEM hItem,
                                     CBionicTree::BT_TREE_NODE_TYPE type)
  {
      CString info,buffer;
      CString on,off;

      on.LoadString(IDS_YES);
      off.LoadString(IDS_NO);

      switch(type)
      {
          case CBionicTree::BT_NODE_ROOT:
              buffer.Format("[Groups: %d, Windows: %d]",m_bionicTree.GetGroupsCount(),CBionicWindow::GetNumOfWindows());
              break;

          case CBionicTree::BT_NODE_GROUP:
          {
              CBionicTree::SBT_GroupInfo info;
              CString groupname, msgformat;

                  //get the Group index to set the cursor sel. for the group list box.
              int id = m_bionicTree.GetGroupNum(hItem);
              //m_listGroups.SetCurSel(id);

              msgformat.LoadString(IDS_INFO_SELECTION_GROUP);
              groupname = m_bionicTree.GetItemText(hItem);

              m_bionicTree.GetGroupInfo(info,hItem);

              buffer.Format(msgformat,
                            info.ncount,
                            info.nvisible,
                            info.ncount-info.nvisible,
                            info.hotkey);
          }
          break;

          case CBionicTree::BT_NODE_WINDOW:
          {
              CBionicTree::SBT_WindowInfo wininfo;
              CBionicWindow *pWin = 0;
                  
              pWin = (CBionicWindow*)m_bionicTree.GetItemData(hItem);
              ASSERT(pWin);

              m_bionicTree.GetWindowInfo(wininfo);
              
              info.LoadString(IDS_INFO_WINDOW);
              buffer.Format(info,
                            (wininfo.bVisible) ? on : off,
                            (wininfo.bOnTop) ? on : off,
                            (wininfo.bTray) ? on : off);
          }
          break;
      }
      if(!m_bMenuSettings[MENU_DOCKING_REGISTRED])
          m_status.SetPaneText(0,buffer,TRUE);
      else
          m_status.SetPaneText(0,"",TRUE);//when docked, we don't want a long string in the statusbar.
  }

//Message sent from the tree to update info.
afx_msg LRESULT CBionicSpiderDlg::OnUpdateInfo(WPARAM wParam,
                                               LPARAM lParam)
  {
      UpdateSelInfo((HTREEITEM)wParam,(CBionicTree::BT_TREE_NODE_TYPE)lParam);
      m_bionicTree.SelectItem((HTREEITEM)wParam);
      return 0;
  }

BOOL CBionicSpiderDlg::GetHotkeyState()
  {
      return m_bMenuSettings[MENU_HOTKEYS];
  }

void CBionicSpiderDlg::SetHotkeyState(BOOL bEnable)
  {
      m_bMenuSettings[MENU_HOTKEYS] = bEnable;
  }

void CBionicSpiderDlg::LoadHotKeys()
  {
      CString regPath,regKey,regDefault;

      regPath.LoadString(IDS_REGISTRY_PATH);
      regKey.LoadString(IDS_HOTKEY_REGKEY_MISC);
      regDefault.LoadString(IDS_HOTKEY_DEFAULT_MISC);

      m_hotkey.Load(GetSafeHwnd(),regPath,regKey,regDefault);
      m_hotkey.LoadHotKeyData();
  }

void CBionicSpiderDlg::OnBionicspiderFonts()
  {
      CFont *pfont = 0;
      LOGFONT lf;
          
      pfont = m_bionicTree.GetFont();
      ASSERT(pfont);

      if(pfont)
      {
          pfont->GetLogFont(&lf);

          CFontDialog fontDlg(&lf,CF_EFFECTS | CF_SCREENFONTS,NULL,this);
          fontDlg.m_cf.rgbColors = m_bionicTree.GetFontColor();

          if(fontDlg.DoModal() == IDOK)
          {
              fontDlg.GetCurrentFont(&lf);
              m_bionicTree.SetTreeFont(&lf,fontDlg.GetColor());
          }
      }
  }

//Just return, don't handle help (F1)
BOOL CBionicSpiderDlg::OnHelpInfo(HELPINFO *pHelpInfo)
  {
      return TRUE;
  }

//Toggle spider's visibility.
void CBionicSpiderDlg::ShowHideSpider()
  {
      CWireFrame wf;
      HWND hTo,hFro,hTray,hWnd;
      int state;
                      
      hTray = GetTrayHwnd();
      hWnd = GetSafeHwnd();

      if(IsWindowVisible())
      {
          hTo = hTray;
          hFro = hWnd;
          state = SW_HIDE;
      }
      else
      {
          hTo = hWnd;
          hFro = hTray;
          state = SW_SHOW;
          SetForegroundWindow();
      }
      if(m_bMenuSettings[MENU_ANIMATE])
          wf.WireFrameThis(hFro,hTo);

      ShowWindow(state);
  }
//END MISC. METHOD

//START GROUP LIST FROM MENU.
BOOL CBionicSpiderDlg::OnCommand(WPARAM wParam,
                                 LPARAM lParam)
  {
          //Only handle MENU messages.
      if(HIWORD(wParam) == 0)
      {
          int id = LOWORD(wParam);

          if(id >= BS_MENU_GROUP_LIST_ID)
          {
                  //Get the Group ID.
              id -= BS_MENU_GROUP_LIST_ID;
              id += 1;
                  
              TRACE("::OnCommand: Group: %d\n",id);

              HTREEITEM hGroup = m_bionicTree.GetGroupByNum(id);
              ASSERT(hGroup);

              m_bionicTree.HideAllGroupWindows();
              m_bionicTree.ShowGroupWindows(hGroup);

              return TRUE;
          }
      }
      return CDialog::OnCommand(wParam,lParam);
  }

//Refresh the Group list in the main menu.
void CBionicSpiderDlg::UpdateGroupListMenu()
  {
      CMenu *pSubMenu = NULL;
      CMenu *pGroupSubMenu = NULL;

          //Check if the menu is attached, if not (docked,tray pop etc.) use the saved menu handle.
      if(NULL == (pSubMenu = GetMenu()))
      {
          TRACE("UpdateGroupListMenu(): USING MENU HANDLE.\n");
          CMenu mainMenu;
          mainMenu.Attach(m_hMainMenu);
          pSubMenu = mainMenu.GetSubMenu(0);
          mainMenu.Detach();
      }
      else //The menu is attached, so just get the Group submenu.
          pSubMenu = pSubMenu->GetSubMenu(0);

      CList<CString> groupNames;
      POSITION pos;
      int index = 0;

          //Grab all the Group names into a list.
      m_bionicTree.GetGroupNames(groupNames);
      pos = groupNames.GetHeadPosition();

          //Update the very fist Group name in menu list.
          //The menu list will ALWAYS have at LEAST ONE Group.
      pSubMenu->ModifyMenu(ID_GROUP_ONE,
                           MF_BYCOMMAND,
                           ID_GROUP_ONE,
                           (LPCTSTR)groupNames.GetAt(pos));

          //Set to the next Group name.
      groupNames.GetNext(pos);

          //Grab the submenu for the Group list.
      pGroupSubMenu = pSubMenu->GetSubMenu(0);
      ASSERT(pGroupSubMenu);

          //Delete Groups under the menu list.
      index = 0;
      while(pGroupSubMenu->DeleteMenu(BS_MENU_GROUP_LIST_ID+index,
                                      MF_BYCOMMAND))
      {
          TRACE("Deleted Group List: %d\n",BS_MENU_GROUP_LIST_ID+index);
          index++;
      }
      index = 0;
      while(pos) //Update the Group list menu IDs.
      {
          TRACE("Adding Group List ID: %d\n",BS_MENU_GROUP_LIST_ID+index);

          pGroupSubMenu->InsertMenu(-1,MF_BYPOSITION,
                                    BS_MENU_GROUP_LIST_ID+index,
                                    (LPCTSTR)groupNames.GetAt(pos));
          ++index;
          groupNames.GetNext(pos);
      }
  }

//Will always jump to the very first Group.
//The tree will always have AT LEAST one Group.
void CBionicSpiderDlg::OnMenuGroupOne()
  {
      HTREEITEM hGroup = m_bionicTree.GetGroupByNum(0);
      ASSERT(hGroup);

      m_bionicTree.HideAllGroupWindows();
      m_bionicTree.ShowGroupWindows(hGroup);
  }

//END GROUP LIST FROM MENU.

//A pro. feature was accessed; tell 'em to upgrade.
#ifdef LITE_UPGRADE
void CBionicSpiderDlg::upgrade_msg()
  {
      CUpgradeMsgDlg dlgUpgrade;
      dlgUpgrade.DoModal();
  }
#endif
