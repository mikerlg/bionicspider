#include "stdafx.h"
#include "resource.h"
#include "RegistryEd.h"
#include "StringToken.h"
#include "FontMan.h"

#define CFONTMAN_REG_DELIM _T(":")

CFontMan::CFontMan()
  {
      m_fontColor = RGB(0,0,0);
  }

//This method needs to be called before all other methods to set default paths.
void CFontMan::Load(LPCTSTR lpszRegPath,
                    LPCTSTR lpszRegKey,
                    LPCTSTR lpszRegKeyColor,
                    LPCTSTR lpszDefault)
  {
      m_regPath = lpszRegPath;
      m_regKey = lpszRegKey;
      m_regKeyColor = lpszRegKeyColor;
      m_regDefault = lpszDefault;
  }

COLORREF CFontMan::GetFontColor()
  {
      return m_fontColor;
  }

void CFontMan::SetFontColor(COLORREF c)
  {
      m_fontColor = c;
  }

//Save the font structure into the regsitry.
BOOL CFontMan::SaveFontInReg(const LOGFONT *plf)
  {
      CRegistryEd reg;
        
      if(reg.SetPathKey(m_regPath) == ERROR_SUCCESS)
      {
          CString buffer;
          buffer.Format("%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%s%s",
                        plf->lfHeight,
                        CFONTMAN_REG_DELIM,
                        plf->lfWidth,
                        CFONTMAN_REG_DELIM,
                        plf->lfEscapement,
                        CFONTMAN_REG_DELIM,
                        plf->lfOrientation,
                        CFONTMAN_REG_DELIM,
                        plf->lfWeight,
                        CFONTMAN_REG_DELIM,
                        plf->lfItalic,
                        CFONTMAN_REG_DELIM,
                        plf->lfUnderline,
                        CFONTMAN_REG_DELIM,
                        plf->lfStrikeOut,
                        CFONTMAN_REG_DELIM,
                        plf->lfCharSet,
                        CFONTMAN_REG_DELIM,
                        plf->lfOutPrecision,
                        CFONTMAN_REG_DELIM,
                        plf->lfClipPrecision,
                        CFONTMAN_REG_DELIM,
                        plf->lfQuality,
                        CFONTMAN_REG_DELIM,
                        plf->lfPitchAndFamily,
                        CFONTMAN_REG_DELIM,
                        plf->lfFaceName,
                        CFONTMAN_REG_DELIM);
          
          reg.SetRegStr(buffer,m_regKey,buffer.GetLength()+1);
          reg.SetRegValue(m_regKeyColor,m_fontColor);
      }
      return TRUE;
  }

//Load up the saved font config. from the registry.
BOOL CFontMan::LoadFontFromReg(LOGFONT *plf)
  {
      CRegistryEd reg;
      CList<CString> fontSettings;
      DWORD colorRef = 0;

      if(reg.SetPathKey(m_regPath) == ERROR_SUCCESS)
      {
          TRACE("Created CFontMan reg key.\n");
          
          CString buffer;
              //If we dont already have this reg key, then create it.
          if(reg.GetRegStr(m_regKey,buffer) != ERROR_SUCCESS)
          {
              TRACE("Creating DEFAULT CFontMan reg key.\n");

              reg.SetRegStr(m_regDefault,m_regKey,m_regDefault.GetLength()+1);
              buffer = m_regDefault;
          }
          if(reg.GetRegValue(m_regKeyColor,colorRef) != ERROR_SUCCESS)
          {
              TRACE("Creating DEFAULT CFontMan COLOR reg. key.\n");

              colorRef = RGB(0,0,0);
              reg.SetRegValue(m_regKeyColor,colorRef);
          }
          m_fontColor = colorRef;

          CStringToken::TokenizeStr(buffer,fontSettings,CFONTMAN_REG_DELIM);
          LoadFontData(fontSettings,plf);
      }
      return TRUE;
  }

//With a list of string font attributes, load up the font structure.
void CFontMan::LoadFontData(CList<CString> &fontData,
                            LOGFONT *plf)
  {
      ASSERT(LOGFONT_SIZE == fontData.GetSize());

      POSITION pos;
      CString s;
      int index = 0;

      pos = fontData.GetHeadPosition();
      while(pos)
      {
          int data = _ttoi((LPCTSTR)fontData.GetAt(pos));
          switch(index)
          {
              case LOGFONT_HEIGHT:
                  plf->lfHeight = data;
                  break;
              case LOGFONT_WIDTH:
                  plf->lfWidth = data;
                  break;
              case LOGFONT_ESCAPEMENT:
                  plf->lfEscapement = data;
                  break;
              case LOGFONT_ORIENTATION:
                  plf->lfOrientation = data;
                  break;
              case LOGFONT_WEIGHT:
                  plf->lfWeight = data;
                  break;
              case LOGFONT_ITALIC:
                  plf->lfItalic = data;
                  break;
              case LOGFONT_UNDERLINE:
                  plf->lfUnderline = data;
                  break;
              case LOGFONT_STRIKEOUT:
                  plf->lfStrikeOut = data;
                  break;
              case LOGFONT_CHARSET:
                  plf->lfCharSet = data;
                  break;
              case LOGFONT_OUTPRECISION:
                  plf->lfOutPrecision = data;
                  break;
              case LOGFONT_CLIPPRECISION:
                  plf->lfClipPrecision = data;
                  break;
              case LOGFONT_QUALITY:
                  plf->lfQuality = data;
                  break;
              case LOGFONT_PITCHANDFAMILY:
                  plf->lfPitchAndFamily = data;
          }

          fontData.GetNext(pos);
          if(pos == fontData.GetTailPosition())
          {
              s = fontData.GetAt(pos);
              strcpy(plf->lfFaceName,s);
              break;
          }
          index++;
      }
  }

