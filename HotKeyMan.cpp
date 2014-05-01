#include "stdafx.h"
#include "resource.h"
#include "HotKeyMan.h"
#include "RegistryEd.h"
#include "StringToken.h"

CHotKeyMan::CHotKeyMan()
  {
      m_hParent = NULL;
      m_bRegistered = FALSE;
  }

CHotKeyMan::~CHotKeyMan()
  {
      UnregKeys();
  }

void CHotKeyMan::Load(HWND hWnd,
                      LPCTSTR lpszRegPath,
                      LPCTSTR lpszRegKey,
                      LPCTSTR lpszRegDefault)
  {
      m_hParent = hWnd;
      m_regPath = lpszRegPath;
      m_regKey = lpszRegKey;
      m_regDefault = lpszRegDefault;
  }

CHotKeyMan::HotKeyID CHotKeyMan::RegHotkey(const Keydata &kdata)
  {
      HotKeyID hotID = 0;
      Keydata data;

          //Check if this hotkeys ID has already been registered, if so, unreg. it.
      POSITION pos = m_keymap.GetStartPosition();
      while(pos)
      {
          m_keymap.GetNextAssoc(pos,hotID,data);
          if(data.id == kdata.id)
              UnregHotkey(hotID);
      }
      hotID = GlobalAddAtom(kdata.keys); //Get an ATOM for this hotkey combo.
      m_keymap[hotID] = kdata;

      RegisterHotKey(m_hParent,
                     hotID,
                     kdata.modKeys,
                     kdata.vk); //register this hotkey with windows.

      TRACE("HOTKEY MAP size: %d\n",m_keymap.GetSize());
      return hotID;
  }

BOOL CHotKeyMan::UnregHotkey(HotKeyID id)
  {
      Keydata data;

      if((m_keymap.Lookup(id,data)))
      {
          m_keymap.RemoveKey(id);
          UnregisterHotKey(m_hParent,id);
          DeleteAtom(id);

          return TRUE;
      }
      return FALSE;
  }

void CHotKeyMan::UnregKeys(BOOL bRemove /* = TRUE */)
  {
      POSITION pos;
      Keydata data;
      HotKeyID id;

      m_bRegistered = FALSE;

      pos = m_keymap.GetStartPosition();
      while(pos)
      {
          m_keymap.GetNextAssoc(pos,id,data);
          UnregisterHotKey(m_hParent,id);
      }
      if(bRemove)
          m_keymap.RemoveAll();
  }

BOOL CHotKeyMan::SetDataID(HotKeyID hotID,
                           int id)
  {
      Keydata kd;

      if(m_keymap.Lookup(hotID,kd))
      {
          m_keymap[hotID].id = id;
          return TRUE;
      }
      return FALSE;
  }

//Get hotkey data via internal ID.
BOOL CHotKeyMan::GetDataByID(int id,
                             Keydata &kdata,
                             HotKeyID &hotkeyID)
  {
      POSITION pos;
      Keydata data;
      HotKeyID hotID;

      pos = m_keymap.GetStartPosition();
      while(pos)
      {
          m_keymap.GetNextAssoc(pos,hotID,data);
          if(id == data.id)
          {
              kdata.keys = data.keys;
              kdata.modKeys = data.modKeys;
              kdata.id = data.id;
              kdata.vk = data.vk;
              hotkeyID = hotID;

              return TRUE;
          }
      }
      return FALSE;
  }

//Get hotkey data via ATOM id.
BOOL CHotKeyMan::GetDataByHotKeyID(HotKeyID hotID,
                                   Keydata &kdata)
  {
      Keydata data;

      if(m_keymap.Lookup(hotID,data))
      {
          kdata.keys = data.keys;
          kdata.modKeys = data.modKeys;
          kdata.id = data.id;
          kdata.vk = data.vk;

          return TRUE;
      }
      return FALSE;
  }

//Load up hotkeys from the registry.
BOOL CHotKeyMan::LoadHotKeyData()
  {
      CRegistryEd reg;
      Hotkeys keys;

      if(reg.SetPathKey(m_regPath) == ERROR_SUCCESS)
      {
          TRACE("Created hotkey registry key.\n");

          CString buffer;
              //If we dont already have this reg key, then create it.
          if(reg.GetRegStr(m_regKey,buffer) != ERROR_SUCCESS)
          {
              TRACE("Creating DEFAULT HOTKEY registry key.\n");
                  //Load up default hotkey config. and store in registry.
              reg.SetRegStr(m_regDefault,m_regKey,m_regDefault.GetLength()+1);
              buffer = m_regDefault;
          }
          CStringToken::TokenizeStr(buffer,keys,HOTKEYMAN_REGDELIM);
          LoadMap(keys);

          m_bRegistered = TRUE;
          return TRUE;
      }
      return FALSE;
  }

//Load up the hotkey map from a list of hotkey strings.
void CHotKeyMan::LoadMap(const Hotkeys &hotkeys)
  {
      Hotkeys keys;
      POSITION pos;

      pos = hotkeys.GetHeadPosition();
      while(pos)
      {
          Keydata tdata;
          CString buffer; //Use this buffer to construct the hotkey string.

          CStringToken::TokenizeStr(hotkeys.GetAt(pos),keys,HOTKEYMAN_DELIM); //split this hotkey string via tokens '+'

          POSITION currPos;
          currPos = keys.GetHeadPosition();

          while(currPos)
          {
              CString strKey = keys.GetAt(currPos);

              if(strKey == HOTKEYMAN_CTRL)
              {
                  tdata.modKeys |= MOD_CONTROL;
                  buffer += strKey;
                  buffer += HOTKEYMAN_DELIM;
              }
              else if(strKey == HOTKEYMAN_ALT)
              {
                  tdata.modKeys |= MOD_ALT;
                  buffer += strKey;
                  buffer += HOTKEYMAN_DELIM;
              }
              else if(strKey == HOTKEYMAN_SHIFT)
              {
                  tdata.modKeys |= MOD_SHIFT;
                  buffer += strKey;
                  buffer += HOTKEYMAN_DELIM;
              }
              else //else we either have the LETTER key or the message ID
              {
                  if(currPos != keys.GetTailPosition() && //if this is not the last element, then we have our key
                     keys.GetCount()>0)
                  {
                      CString s;
                      s = keys.GetAt(currPos);

                      buffer += s;

                      tdata.vk = _ttoi(s);
                  }
                  else                                     //else we have our message id that we will send to m_hParent
                      tdata.id = _ttoi(strKey);
              }
              keys.GetNext(currPos);
          }
          HotKeyID keyatom = GlobalAddAtom(hotkeys.GetAt(pos)); //get ba unique ID for this hotkey combo.
          if(keyatom)
          {
              if(RegisterHotKey(m_hParent,keyatom,tdata.modKeys,tdata.vk)) //register this hotkey with windows.
              {
                  tdata.keys = buffer;

                  m_keymap[keyatom] = tdata;

                  TRACE("Loading HOTKEY: %s\n",tdata.keys);
              }
              else //Error: this hotkey has already been installed by another process.
              {
                  buffer = "";
                  GetKeyStrDisplay(tdata,buffer);
                  MessageBox(m_hParent,buffer,_T("This hotkey is already in use."),MB_OK);
              }
          }
          tdata.modKeys = 0;
          tdata.id = 0;
          tdata.vk = 0;
          buffer = "";

          keys.RemoveAll();
          hotkeys.GetNext(pos);
      }      
  }

//Install default registry hotkey string into the registry and load it up.
void CHotKeyMan::LoadDefaultData()
  {
      CRegistryEd reg;
      Hotkeys keys;

      UnregKeys(TRUE);

//       if(reg.SetPathKey(m_regPath) == ERROR_SUCCESS)
//       {
//           TRACE("Creating DEFAULT HOTKEY registry key.\n");
//           reg.SetRegStr(m_regDefault,m_regKey,m_regDefault.GetLength()+1);
//       }

      CStringToken::TokenizeStr(m_regDefault,keys,HOTKEYMAN_REGDELIM);
      LoadMap(keys);
  }

//Gather all hotkey data from the map and store it into the registry.
BOOL CHotKeyMan::SetRegistryString()
  {
      CString regStr;
      CRegistryEd reg;
        
      if(GetRegistryString(regStr))
      {
          if(reg.SetPathKey(m_regPath) == ERROR_SUCCESS)
          {
              reg.SetRegStr(regStr,m_regKey,regStr.GetLength()+1);
              return TRUE;
          }
      }
      else //We have no hotkeys to store so just blank out the hotkey reg. string.
      {
          if(reg.SetPathKey(m_regPath) == ERROR_SUCCESS)
          {
              regStr = "";
              reg.SetRegStr(regStr,m_regKey,regStr.GetLength()+1);
          }
      }
      return FALSE;
  }

//Get the hotkeys in string format to store in the registry.
BOOL CHotKeyMan::GetRegistryString(CString &buff)
  {
      POSITION pos;
      Keydata data;
      HotKeyID id;

      if(m_keymap.IsEmpty())
          return FALSE;

      pos = m_keymap.GetStartPosition();
      while(pos)
      {
          m_keymap.GetNextAssoc(pos,id,data);

          buff += data.keys;
          buff += HOTKEYMAN_DELIM;
          
              //Convert ID into a string and concat.
          CString sid;
          sid.Format("%d",data.id);
          
          buff += sid;
          buff += HOTKEYMAN_REGDELIM;
      }
      return TRUE;
  }

//Construct a string representation of the hotkey data. (for registry storage)
void CHotKeyMan::GetKeyStrByData(const Keydata &kd,
                                 CString &buffer)
  {
      if(kd.modKeys & MOD_CONTROL)
      {
          buffer += HOTKEYMAN_CTRL;
          buffer += HOTKEYMAN_DELIM;
      }
      if(kd.modKeys & MOD_ALT)
      {
          buffer += HOTKEYMAN_ALT;
          buffer += HOTKEYMAN_DELIM;
      }
      if(kd.modKeys & MOD_SHIFT)
      {
          buffer += HOTKEYMAN_SHIFT;
          buffer += HOTKEYMAN_DELIM;
      }

      CString s;
      s.Format("%d",kd.vk);
      buffer += s;

      buffer.Format("%s%s%d",buffer,HOTKEYMAN_DELIM,kd.id);
  }

//Readable hotkey text.
void CHotKeyMan::GetKeyStrDisplay(const Keydata &kd,
                                  CString &buffer)
  {
      if(kd.modKeys & MOD_CONTROL)
      {
          buffer += HOTKEYMAN_CTRL;
          buffer += HOTKEYMAN_DELIM;
      }
      if(kd.modKeys & MOD_ALT)
      {
          buffer += HOTKEYMAN_ALT;
          buffer += HOTKEYMAN_DELIM;
      }
      if(kd.modKeys & MOD_SHIFT)
      {
          buffer += HOTKEYMAN_SHIFT;
          buffer += HOTKEYMAN_DELIM;
      }
//    Construct display "hotkey" string.
      CString s;
      switch(kd.vk)
      {
              //VK codes for function keys.
          case VK_F1:
              s = "F1";
              break;
          case VK_F2:
              s = "F2";
              break;
          case VK_F3:
              s = "F3";
              break;
          case VK_F4:
              s = "F4";
              break;
          case VK_F5:
              s = "F5";
              break;
          case VK_F6:
              s = "F6";
              break;
          case VK_F7:
              s = "F7";
              break;
          case VK_F8:
              s = "F8";
              break;
          case VK_F9:
              s = "F9";
              break;
          case VK_F10:
              s = "F10";
              break;
          case VK_F11:
              s = "F11";
              break;
          case VK_F12:
              s = "F12";
              break;
          case VK_UP:
              s = "Up";
              break;
          case VK_DOWN:
              s = "Down";
              break;
          case VK_RIGHT:
              s = "Right";
              break;
          case VK_LEFT:
              s = "Left";
              break;

          default:
          {
                  //No need to keyboard state with GetKeyboardState(),
                  //it fucks up the translation because the hotkey modifiers.
              BYTE state[256] = {0};
              char buff = 0;
                      
                  //Convert the virtual key code into a "char" representation.
              ToAscii(kd.vk,
                      MapVirtualKey(kd.vk,0),
                      (PBYTE)state,
                      (LPWORD)&buff,0);

              s.Format("%c",toupper(buff));
          }
      }
      buffer.Format("%s%s",buffer,s);
  }

INT_PTR CHotKeyMan::GetNumOfHotkeys()
  {
      return m_keymap.GetCount();
  }

BOOL CHotKeyMan::RegisteredHotkeys()
  {
      return m_bRegistered;
  }

