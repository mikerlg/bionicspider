#include "stdafx.h"
#include "SunkenButton.h"

#define BKG_COLOR 0xffffff

CSunkenButton::CSunkenButton()
  {
      m_bOverCtrl = FALSE;
      m_pBkBrush = new CBrush(BKG_COLOR);
  }

CSunkenButton::~CSunkenButton()
  {
      delete m_pBkBrush;
  }

BEGIN_MESSAGE_MAP(CSunkenButton, CStatic)
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_TIMER()
ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

afx_msg HBRUSH CSunkenButton::CtlColor(CDC* pDC,
                                       UINT nCtlColor) 
  {
      switch(nCtlColor)
      {
          case CTLCOLOR_STATIC:
                  // Change background color and return the background brush.
              pDC->SetBkColor(BKG_COLOR);
              return (HBRUSH)(m_pBkBrush->GetSafeHandle());

          default:
              return CStatic::OnCtlColor(pDC,this,nCtlColor);
      }
  }

afx_msg void CSunkenButton::OnLButtonUp(UINT flags,
                                        CPoint loc)
  {
      ModifyStyleEx(WS_EX_CLIENTEDGE,0,SWP_FRAMECHANGED);
      GetParent()->SendMessage(WM_COMMAND,IDOK);
  }

afx_msg void CSunkenButton::OnLButtonDown(UINT flags,
                                          CPoint loc)
  {
      ModifyStyleEx(0,WS_EX_CLIENTEDGE,SWP_FRAMECHANGED);
  }

afx_msg void CSunkenButton::OnMouseMove(UINT nFlags,
                                        CPoint point)
  {
      CStatic::OnMouseMove(nFlags, point);
      if(!m_bOverCtrl)
      {
          m_bOverCtrl = TRUE;
          SetTimer(1,100,0);
      }
  }

afx_msg void CSunkenButton::OnTimer(UINT nIDEvent)
  {
      CStatic::OnTimer(nIDEvent);
      CPoint p(GetMessagePos());
      CRect r;

      ScreenToClient(&p);        // Get the bounds of the control (just the client area)
      GetClientRect(r);          // Check the mouse is inside the control

      if(FALSE == r.PtInRect(p))
      {
              // if not then stop looking...
          m_bOverCtrl=FALSE;
          KillTimer(1);
          ModifyStyleEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE,0,SWP_FRAMECHANGED);
      }
      else
          ModifyStyleEx(0,WS_EX_STATICEDGE,SWP_FRAMECHANGED);
  }
