//
//  ptypes.h
//  Azaria
//
//  Created by guy on 11/11/17.
//  Copyright © 2017 guy. All rights reserved.
//

#ifndef ptypes_h
#define ptypes_h

#include <stdint.h>
#include "assert.h"

void OutputDebugString(const char * buff);

enum {
    WS_POPUP,
    WS_SYSMENU,
    WS_POPUPWINDOW,
};

#define WS_CAPTION 0

#define FAR
#define NEAR
#define PASCAL
#define WINAPI

#if defined(_WIN64)
typedef unsigned __int64 ULONG_PTR;
#else
typedef uint32_t ULONG_PTR;
#endif

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef uint32_t DWORD;
typedef long LONG;
typedef LONG * LPLONG;
typedef void* LPVOID;
typedef const void * LPCVOID;
typedef BYTE *LPBYTE;
typedef uint32_t ULONG;
typedef DWORD *LPDWORD;
typedef void *LPUNKNOWN;
typedef LONG HRESULT;
typedef DWORD DPID;
typedef DPID * LPDPID;
typedef ULONG_PTR DWORD_PTR;

typedef wchar_t WCHAR;
typedef WCHAR *LPWSTR;

#define BOOL bool
#define TRUE true
#define FALSE false
#define UINT unsigned int
#define LPSTR char *
#define HWND HANDLE
#define UCHAR unsigned char
#define VOID void

#define E_FAIL 0x80004005L
#define ERROR_FILE_NOT_FOUND 0x02
#define ERROR_OUT_OF_STRUCTURES 0x54

#define LOWORD(a) (size_t)((WORD)(a))
#define HIWORD(a) ((WORD)(((DWORD)(a) >> 16) & 0xFFFF))

typedef UINT * UINT_PTR;
typedef UINT_PTR WPARAM;
typedef long  * LONG_PTR;
typedef LONG_PTR LPARAM;

typedef struct tagSIZE {
    int32_t cx;
    int32_t cy;
} SIZE, *PSIZE;

typedef struct _GUID {
    DWORD Data1;
    WORD  Data2;
    WORD  Data3;
    BYTE  Data4[8];
} GUID;

typedef GUID * LPGUID;

class IUnknown {
public:
    ULONG Release(){return 0;};
};

//===============================================
// DIRECT PLAY ... TODO: MOVE TO DP HEADER
#define DPLONGNAMELEN           52
#define DPSHORTNAMELEN          20
#define DPSESSIONNAMELEN        32
#define DPPASSWORDLEN           16
#define DPUSERRESERVED          16

#define MAKE_HRESULT(sev,fac,code) \
((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#define _FACDP  0x877
#define MAKE_DPHRESULT( code )    MAKE_HRESULT( 1, _FACDP, code )

#define E_INVALIDARG 0x80070057L

#define DP_OK                           0
#define DPSEND_GUARANTEE            0x00000001
#define DPERR_INVALIDOBJECT             MAKE_DPHRESULT( 130 )
#define DPERR_INVALIDPLAYER             MAKE_DPHRESULT( 150 )
#define DPERR_NOCONNECTION              MAKE_DPHRESULT( 170 )
#define DPERR_NOMESSAGES                MAKE_DPHRESULT( 190 )
#define DPERR_INVALIDPARAM              E_INVALIDARG
#define DPERR_INVALIDPARAMS             DPERR_INVALIDPARAM
#define DPERR_UNAVAILABLE               MAKE_DPHRESULT( 250 )
#define DPERR_BUSY                      MAKE_DPHRESULT( 270 )
#define DPERR_UNAVAILABLE               MAKE_DPHRESULT( 250 )

//
// System message structures and types.
//
// System messages have a leading 4 byte type code to identify the message.
// an app knows it is a system message because it is addressed 'To' player 0.
//


#define DPSYS_ADDPLAYER               0x0003  // DPMSG_ADDPLAYER
#define DPSYS_DELETEPLAYER            0x0005  // DPMSG_DELETEPLAYER

#define DPSYS_ADDPLAYERTOGROUP        0x0007  // DPMSG_GROUPADD

#define DPSYS_INVITE                  0x000e  // DPMSG_INVITE, Net only.

#define DPSYS_DELETEGROUP             0x0020  // DPMSG_DELETEPLAYER
#define DPSYS_DELETEPLAYERFROMGRP     0x0021  // DPMSG_GROUPDELETE
#define DPSYS_SESSIONLOST             0x0031

#define DPSYS_CONNECT                 0x484b  // DPMSG_GENERIC

#define DPRECEIVE_ALL               0x00000001

#define DPESC_TIMEDOUT 1

#define DPOPEN_OPENSESSION          0x00000001
#define DPOPEN_CREATESESSION        0x00000002

typedef struct
{
    DWORD   dwSize;
    GUID    guidSession;
    DWORD_PTR dwSession;
    DWORD   dwMaxPlayers;
    DWORD   dwCurrentPlayers;
    DWORD   dwFlags;
    char    szSessionName[DPSESSIONNAMELEN];
    char    szUserField[DPUSERRESERVED];
    DWORD_PTR   dwReserved1;
    char    szPassword[DPPASSWORDLEN];
    DWORD_PTR   dwReserved2;
    DWORD_PTR   dwUser1;
    DWORD_PTR   dwUser2;
    DWORD_PTR   dwUser3;
    DWORD_PTR   dwUser4;
} DPSESSIONDESC,*LPDPSESSIONDESC;

typedef struct
{
    DWORD       dwType;
    DPID        dpId;
} DPMSG_DELETEPLAYER;

typedef struct
{
    DWORD       dwType;
    DWORD       dwPlayerType;
    DPID        dpId;
    char        szLongName[DPLONGNAMELEN];
    char        szShortName[DPSHORTNAMELEN];
    DWORD       dwCurrentPlayers;
}   DPMSG_ADDPLAYER;

typedef BOOL (*  LPDPENUMPLAYERSCALLBACK)(
                                                  DPID   dpId,
                                                  LPSTR  lpFriendlyName,
                                                  LPSTR  lpFormalName,
                                                  DWORD  dwFlags,
                                                  LPVOID          lpContext );

typedef struct tagDPNAME
{
    DWORD   dwSize;
    DWORD   dwFlags;            /* Not used must be 0 */
    
    union /*playerShortName */      /* Player's Handle? */
    {
        LPWSTR  lpszShortName;
        LPSTR   lpszShortNameA;
    } DUMMYUNIONNAME1;
    
    union /*playerLongName */       /* Player's formal/real name */
    {
        LPWSTR  lpszLongName;
        LPSTR   lpszLongNameA;
    } DUMMYUNIONNAME2;
    
} DPNAME, *LPDPNAME;

typedef const DPNAME *LPCDPNAME;

typedef BOOL (*  LPDPENUMPLAYERSCALLBACK2)(
                                                   DPID            dpId,
                                                   DWORD           dwPlayerType,
                                                   LPCDPNAME       lpName,
                                                   DWORD           dwFlags,
                                                   LPVOID          lpContext );

typedef BOOL (*  LPDPENUMSESSIONSCALLBACK)(
                                                   LPDPSESSIONDESC lpDPSessionDesc,
                                                   LPVOID      lpContext,
                                                   LPDWORD     lpdwTimeOut,
                                                   DWORD       dwFlags);

/*  DPCAPS -  Used to obtain the capabilities of a DirectPlay object */
typedef struct tagDPCAPS
{
    DWORD dwSize;               /* Size of structure in bytes */
    DWORD dwFlags;
    DWORD dwMaxBufferSize;
    DWORD dwMaxQueueSize;       /* Obsolete. */
    DWORD dwMaxPlayers;         /* Maximum players/groups (local + remote) */
    DWORD dwHundredBaud;        /* Bandwidth in 100 bits per second units;
                                 * i.e. 24 is 2400, 96 is 9600, etc.
                                 */
    DWORD dwLatency;            /* Estimated latency; 0 = unknown */
    DWORD dwMaxLocalPlayers;    /* Maximum # of locally created players */
    DWORD dwHeaderLength;       /* Maximum header length in bytes */
    DWORD dwTimeout;            /* Service provider's suggested timeout value
                                 * This is how long DirectPlay will wait for
                                 * responses to system messages
                                 */
} DPCAPS, *LPDPCAPS;

typedef BOOL (* LPDPENUMDPCALLBACKA)(
                                              LPGUID      lpguidSP,
                                              LPSTR       lpSPName,       /* ptr to str w/ driver description */
                                              DWORD       dwMajorVersion, /* Major # of driver spec in lpguidSP */
                                              DWORD       dwMinorVersion, /* Minor # of driver spec in lpguidSP */
                                              LPVOID      lpContext);     /* User given */

class IDirectPlay : public IUnknown {
public:
    void GetMessageCount(DPID pid, DWORD *count)
    {assert(false);};
    HRESULT CreatePlayer(DPID *pid, const char * friendlyName, const char * formalName, int flags)
    {assert(false); return false;};
    HRESULT DestroyPlayer(DPID pid)
    {assert(false); return false;};
    HRESULT EnableNewPlayers(BOOL enable)
    {assert(false); return false;};
    HRESULT EnumPlayers(LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK lpEnumPlayersCallback2, LPVOID lpContext, DWORD dwFlags)
    {assert(false); return false;};
    HRESULT GetPlayerName(DPID pid, const char * friendly, DWORD *a_len, const char * formal,  DWORD *b_len)
    {assert(false); return false;};
    HRESULT SetPlayerName(DPID pidID, const char * lpPlayerFriendlyName, const char * lpPlayerFormalName)
    {assert(false); return false;};
    HRESULT Receive(LPDPID lpidFrom, LPDPID lpidTo, DWORD dwFlags,  LPCVOID lpData, LPDWORD lpdwDataSize)
    {assert(false); return false;};
    HRESULT Send(DPID idFrom, DPID idTo, DWORD dwFlags, LPCVOID lpData, DWORD dwDataSize)
    {assert(false); return false;};
    HRESULT Open(LPDPSESSIONDESC lpsd)
    {assert(false); return false;};
    HRESULT EnumSessions(LPDPSESSIONDESC lpsd, DWORD dwTimeout, LPDPENUMSESSIONSCALLBACK lpEnumSessionsCallback, LPVOID lpContext, DWORD dwFlags)
    {assert(false); return false;};
    HRESULT Close(void)
    {assert(false); return false;};
    HRESULT SaveSession(LPSTR arg1)
    {assert(false); return false;};
    HRESULT GetCaps(LPDPCAPS lpDPCaps)
    {assert(false); return false;};
};
typedef IDirectPlay * LPDIRECTPLAY;

typedef void* HANDLE;

typedef DWORD COLORREF;
typedef DWORD* LPCOLORREF;

typedef DWORD DPID;

typedef void * LPVOID;

typedef void * HBITMAP;

typedef struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
} PALETTEENTRY, *LPPALETTEENTRY;

typedef struct tagLOGPALETTE {
    WORD         palVersion;
    WORD         palNumEntries;
    PALETTEENTRY palPalEntry[1];
} LOGPALETTE;

typedef void * HPALETTE;

typedef struct {
    WORD x;
    WORD y;
} POINT;
typedef POINT * LPPOINT;

// TODO: yep
#define O_BINARY 0
#define O_TEXT 0
#define _MAX_DRIVE 1024
#define _MAX_DIR   1024
#define _MAX_FNAME 1024
#define _MAX_PATH  1024
#define _MAX_EXT   1024

#define MAX_DRIVE 1024
#define MAX_DIR   1024
#define MAX_FNAME 1024
#define MAX_PATH  1024
#define MAX_EXT   1024


#endif /* ptypes_h */
