/*
    wireframe.h
    wireframe.cpp
    Create a "wire frame" animation when
    the user drags the web seeker over a valid window.
*/
#ifndef _WIREFRAME_H_
#define _WIREFRAME_H_
#include <windows.h>

/* #define FRAME_WIDTH 10 */
/* #define FRAME_DELAY 5 */
/* #define NUM_OF_POINTS 5 */
/* #define NUM_OF_ITERATIONS 10 */

class CWireFrame
  {
      enum
      {
          CWIREFRAME_FRAME_WIDTH = 10,
          CWIREFRAME_FRAME_DELAY = 5,
          CWIREFRAME_NUM_OF_POINTS = 5,
          CWIREFRAME_NUM_OF_ITERATIONS = 10
      };

    public:

      void WireFrameAnim(const LPRECT,const LPRECT,UINT,UINT);
      BOOL WireFrameWindow(HWND,HWND*);
      BOOL WireFrameThis(HWND,HWND);
      BOOL WireFrameRects(RECT&,RECT&);
      BOOL IsDeskTopWindow(HWND);
      HWND TraceToParent(HWND);
  };
#endif

