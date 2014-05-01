#ifndef _CSUNKENBUTTON_H_
#define _CSUNKENBUTTON_H_
#include <afxwin.h>
#include <afxcmn.h>

class CSunkenButton : public CStatic
  {
    protected:

      BOOL m_bOverCtrl;
      CBrush *m_pBkBrush;

    public:
      CSunkenButton();
      virtual ~CSunkenButton();
      afx_msg HBRUSH CtlColor(CDC*,UINT);
      afx_msg void OnLButtonUp(UINT,CPoint);
      afx_msg void OnLButtonDown(UINT,CPoint);
      afx_msg void OnMouseMove(UINT,CPoint);
      afx_msg void OnTimer(UINT);

      DECLARE_MESSAGE_MAP()
  };
#endif
