#ifndef __EXBIONICWINDOW_H__
#define __EXBIONICWINDOW_H__
#include "BionicWindow.h"

class CExBionicWindow : public CBionicWindow
  {
    protected:
      RECT m_rectExClass;

    public:
      CExBionicWindow();
      ~CExBionicWindow();

      virtual BOOL SetState(int);
      virtual void SetHandle(HWND);
      virtual void SetMemory();
  };
#endif
