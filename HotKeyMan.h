#ifndef _HOTKEYMAN_H_
#define _HOTKEYMAN_H_
#include <afxtempl.h>
#include <afx.h>

#define HOTKEYMAN_ALT _T("alt")
#define HOTKEYMAN_CTRL _T("ctrl")
#define HOTKEYMAN_SHIFT _T("shift")
#define HOTKEYMAN_DELIM _T("+")
#define HOTKEYMAN_REGDELIM _T(":")

class CHotKeyMan
{
  public:

    struct Keydata
    {
        CString keys;   //String representation of hotkey.
        UINT modKeys;   //Any modifier keys - ALT,CTRL,SHIFT.
        int id;         //Special id to identify this hotkey.
        int vk;         //The key that will trigger this hotkey.
        Keydata()
            {
                modKeys = 0;
                id = 0;
                vk = 0;
            }
    };
    typedef CMap<ATOM,ATOM,Keydata,Keydata> HotkeyData; //map[ATOM] = hotkeydata
    typedef CList<CString> Hotkeys;                     //Hotkey strings collected from the registry.
    typedef ATOM HotKeyID;

  private:

    HotkeyData m_keymap; //Map will hold a keydata item for each hotkey, ATOM of hotkey is the key.
    HWND m_hParent;      //Send all messages to this HWND
    CString m_regPath;   //Path to our confiuration in registry.
    CString m_regKey;    //Registry key to hotkey settings.
    CString m_regDefault; //Default hotkey data path.
    BOOL m_bRegistered;   //Flag to check if the hotkeys have already been registered.

    void LoadMap(const Hotkeys&); //Load map via hotkey string list.

  public:

    CHotKeyMan();
    ~CHotKeyMan();

    void Load(HWND,LPCTSTR,LPCTSTR,LPCTSTR);               //Set parent HWND and registry paths.
    void UnregKeys(BOOL = TRUE);                   //Unregister everything.
    void GetKeyStrByData(const Keydata&,CString&); //Construct a hotkey string given keydata.
    void GetKeyStrDisplay(const Keydata&,CString&);//Construct a hotkey string that is readable text.
    void LoadDefaultData();                    //Load up default hotkey list.
    BOOL LoadHotKeyData();                               //Load up hotkey list from the registry.
    BOOL GetRegistryString(CString&); //Construct registry string by hotkey data.
    BOOL SetRegistryString();         //Install hotkeys into the registry.
    BOOL GetDataByID(int,Keydata&,HotKeyID&);            //Get hotkey data via internal hotkey id.
    BOOL GetDataByHotKeyID(HotKeyID,Keydata&);

    BOOL UnregHotkey(HotKeyID);         //Unregister a specific hotkey.
    BOOL SetDataID(HotKeyID,int);       //Given an ATOM, change the id of the hotkey.
    HotKeyID RegHotkey(const Keydata&); //Register a hotkey, return hotkey ATOM.
    INT_PTR GetNumOfHotkeys();
    BOOL RegisteredHotkeys();
};
#endif
