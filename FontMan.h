#ifndef _FONTMAN_H_
#define _FONTMAN_H_
#include <afx.h>

class CFontMan
  {
      CString m_regPath;     //Path to registry info.
      CString m_regKey;      //Save font information under this registry key.
      CString m_regKeyColor; //Save font color data under this key.
      CString m_regDefault;  //Default font if there is not reg. key to read from.
      COLORREF m_fontColor;

      enum
      {
          LOGFONT_HEIGHT,
          LOGFONT_WIDTH,
          LOGFONT_ESCAPEMENT,
          LOGFONT_ORIENTATION,
          LOGFONT_WEIGHT,
          LOGFONT_ITALIC,
          LOGFONT_UNDERLINE,
          LOGFONT_STRIKEOUT,
          LOGFONT_CHARSET,
          LOGFONT_OUTPRECISION,
          LOGFONT_CLIPPRECISION,
          LOGFONT_QUALITY,
          LOGFONT_PITCHANDFAMILY,
          LOGFONT_SIZE = 14
      };
      void LoadFontData(CList<CString>&,LOGFONT*);

    public:

      CFontMan();
      void Load(LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR);
      BOOL LoadFontFromReg(LOGFONT*);
      BOOL SaveFontInReg(const LOGFONT*);
      COLORREF GetFontColor();
      void SetFontColor(COLORREF);
  };
#endif
