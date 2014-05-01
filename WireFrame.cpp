#include "stdafx.h"
#include "WireFrame.h"
#include <tchar.h>

BOOL CWireFrame::IsDeskTopWindow(HWND hWnd)
  {
      TCHAR cBuffer[512];
      GetClassName(hWnd,cBuffer,512);
      if(lstrcmp(cBuffer,_T("Progman") )==0 ||
         lstrcmp(cBuffer,_T("Shell_TrayWnd"))==0)
          return FALSE;
      
      return TRUE;
  }

HWND CWireFrame::TraceToParent(HWND hChild)
  {
      HWND hWnd,hParent;

      if(!( hWnd=GetParent(hChild)))
          return hChild;

      do { hParent=hWnd; } while(hWnd=GetParent(hWnd));
      return hParent;
  }

void CWireFrame::WireFrameAnim(const LPRECT lprcFrom,
                               const LPRECT lprcTo,
                               UINT uiFrameWidth,
                               UINT nMilliSecSpeed)
  {
      POINT pt[CWIREFRAME_NUM_OF_POINTS];
      HPEN hPen,hOldPen;
      RECT recTrans;
      HDC hDC;
      int nMode;
      
      GdiFlush();
      hDC=GetDC(NULL);
      nMode=SetROP2(hDC,R2_NOT);
      hPen=CreatePen(PS_SOLID,uiFrameWidth,RGB(0,0,0));
      hOldPen=(HPEN)SelectObject(hDC,hPen);

      for(int i=0;i<CWIREFRAME_NUM_OF_ITERATIONS;i++)
      {
          recTrans.left=lprcFrom->left+((lprcTo->left-lprcFrom->left)*i/CWIREFRAME_NUM_OF_ITERATIONS);
          recTrans.right=lprcFrom->right+(int)((lprcTo->right-lprcFrom->right)*i/CWIREFRAME_NUM_OF_ITERATIONS);
          recTrans.top=lprcFrom->top+(int)((lprcTo->top-lprcFrom->top )*i/CWIREFRAME_NUM_OF_ITERATIONS );
          recTrans.bottom=lprcFrom->bottom+(int)((lprcTo->bottom-lprcFrom->bottom)*i/CWIREFRAME_NUM_OF_ITERATIONS);

          pt[0].x=recTrans.left;       
          pt[0].y=recTrans.top; 
          pt[1].x=recTrans.right;
          pt[1].y=recTrans.top;
          pt[2].x=recTrans.right;
          pt[2].y=recTrans.bottom;
          pt[3].x=recTrans.left;
          pt[3].y=recTrans.bottom;
          pt[4].x=recTrans.left;
          pt[4].y=recTrans.top;
          
          Polyline(hDC,pt,CWIREFRAME_NUM_OF_POINTS);
          GdiFlush();
          Sleep( nMilliSecSpeed );
          Polyline( hDC,pt,CWIREFRAME_NUM_OF_POINTS);
          GdiFlush();
      }
      SetROP2(hDC,nMode);
      SelectObject(hDC,hOldPen);
      ReleaseDC(NULL,hDC);
  }

BOOL CWireFrame::WireFrameWindow(HWND hDrag,
                                 HWND* hWnd)
  {
      RECT rClient,rOther;
      HWND hTemp;
      POINT pt;
      
      GetCursorPos(&pt);
      hTemp=WindowFromPoint(pt);
      hTemp=TraceToParent(hTemp);
      if( !IsDeskTopWindow(hTemp))
          return FALSE;

      GetWindowRect(hDrag,&rClient);
      GetWindowRect(hTemp,&rOther);
      WireFrameAnim(&rOther,&rClient,
                    CWIREFRAME_FRAME_WIDTH,
                    CWIREFRAME_FRAME_DELAY);

      *hWnd=hTemp;
      return TRUE;
  }

BOOL CWireFrame::WireFrameRects(RECT &rcDrag,
                                RECT &rcTo)
  {
      WireFrameAnim(&rcDrag,&rcTo,
                    CWIREFRAME_FRAME_WIDTH,
                    CWIREFRAME_FRAME_DELAY);

      return TRUE;
  }

BOOL CWireFrame::WireFrameThis(HWND hDrag,
                               HWND hTo)
  {
      RECT rDrag,rTo;

      GetWindowRect(hDrag,&rDrag);
      GetWindowRect(hTo,&rTo);

      WireFrameAnim(&rDrag,&rTo,
                    CWIREFRAME_FRAME_WIDTH,
                    CWIREFRAME_FRAME_DELAY);

      return TRUE;
  }
