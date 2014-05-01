#ifndef _UPDATEDLG_H_
#define _UPDATEDLG_H_
#include <afxinet.h>
#include "HyperLink.h"
#include "resource.h"

class CUpdateDlg : public CDialog
  {
    public:

      enum
      {
          ID = IDD_UPDATE_CHECK,
          BS_UPDATECHECK_NOUPDATE = 0,
          BS_UPDATECHECK_UPDATE,
          BS_UPDATECHECK_NONET
      };
      CUpdateDlg(CWnd *pParent = NULL);

      BOOL OnInitDialog();
      void SetUpdateText(LPCTSTR);
      int UpdateCheck(LPCTSTR);

      BOOL PreTranslateMessage(MSG*);
      static UINT CheckUpdateThread(LPVOID);

    private:
      
      CHyperLink m_webaddy;
      CString m_text;

      virtual void DoDataExchange(CDataExchange*);
  };
#endif
