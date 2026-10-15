/* =====================================================================
 * MicroWin - 16-bit MS-DOS Windowing Engine (Mode 12h Accelerated)
 * Author: Hugo
 * Date: October 12, 2026
 * Released to the Public Domain
 *
 * COMPILATION INSTRUCTIONS (OpenWatcom):
 *   remove all #include <windows.h>, and simular .h, replace with #include "microwin.h"
 *   compile for example
 *   wcl -bcl=dos -ml -za99 -os hello.c -fe=hello.exe
 * ===================================================================== */
#ifndef MICROWIN_H
#define MICROWIN_H
#include <i86.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <stdarg.h>
#include <ctype.h>
#define PASCAL   __pascal
#define WINAPI   PASCAL
#define CALLBACK PASCAL
#define CDECL    __cdecl
#define FAR      __far
#define TRUE     1
typedef short    SHORT;
#define FALSE    0
#ifndef __export
#define __export
#endif
unsigned _stklen = 16384;

#undef NULL
#define NULL 0
static void explorer_ffree(void FAR* ptr);
#define _ffree explorer_ffree
#define UNREFERENCED_PARAMETER(P)
#define stbtt_uint8 unsigned char
#define stbtt_int8 signed char
#define stbtt_uint16 unsigned short
#define stbtt_int16 signed short
#define stbtt_uint32 unsigned long
#define stbtt_int32 signed long
#define STBTT_malloc(x,u)  ((void)(u),halloc((x),1))
#define STBTT_free(x,u)    ((void)(u),hfree(x))
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#pragma code_seg("STB_TEXT")
#include "stb_truetype.h"
#pragma code_seg("WIN_TEXT")
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef int            INT;
typedef unsigned int   UINT;
typedef int            BOOL;
typedef long           LRESULT;
typedef WORD           WPARAM;
typedef long           LPARAM;
typedef long           LONG;
typedef DWORD          COLORREF;
typedef DWORD FAR*     LPDWORD;
typedef const char FAR* LPCSTR;
typedef char FAR*      LPSTR;
typedef void FAR*      LPVOID;
typedef WORD           ATOM;

typedef WORD HWND, HDC, HINSTANCE, HICON, HCURSOR, HBRUSH, HPEN, HFONT, HMENU, HGDIOBJ, HBITMAP, HGLOBAL, HDWP, HDROP;
typedef void FAR* HANDLE;
typedef HANDLE HRGN;
typedef LRESULT (CALLBACK* HOOKPROC)(int, WPARAM, LPARAM);
typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL (CALLBACK* WNDENUMPROC)(HWND, LPARAM);
typedef void (CALLBACK* TIMERPROC)(HWND, UINT, UINT, DWORD);
typedef long (CALLBACK* FARPROC)();

typedef struct tagRECT { int left, top, right, bottom; } RECT, FAR* LPRECT;
typedef struct tagPOINT { int x, y; } POINT, FAR* LPPOINT;
typedef struct tagSIZE { int cx, cy; } SIZE, FAR* LPSIZE;
typedef struct tagTEXTMETRIC { int tmHeight; int tmAscent; int tmDescent; int tmInternalLeading; int tmExternalLeading; int tmAveCharWidth; int tmMaxCharWidth; } TEXTMETRIC, FAR* LPTEXTMETRIC;
typedef struct tagPAINTSTRUCT { HDC hdc; BOOL fErase; RECT rcPaint; } PAINTSTRUCT, FAR* LPPAINTSTRUCT;

typedef struct tagMSG { HWND hwnd; UINT message; WPARAM wParam; LPARAM lParam; DWORD time; POINT pt; } MSG, FAR* LPMSG;
typedef struct tagWNDCLASS { UINT style; WNDPROC lpfnWndProc; int cbClsExtra; int cbWndExtra; HINSTANCE hInstance; HICON hIcon; HCURSOR hCursor; HBRUSH hbrBackground; LPCSTR lpszMenuName; LPCSTR lpszClassName; } WNDCLASS, FAR* LPWNDCLASS;
typedef struct tagWINDOWPLACEMENT { UINT length; UINT flags; UINT showCmd; POINT ptMinPosition; POINT ptMaxPosition; RECT rcNormalPosition; } WINDOWPLACEMENT, FAR* LPWINDOWPLACEMENT;

typedef struct _BakedFont { char face[32]; int height; BYTE __huge* atlas; stbtt_bakedchar cdata[96]; } BakedFont;

typedef struct _INTERNAL_WND {
    WNDPROC wndProc; char className[32]; char text[256]; DWORD style;
    RECT rect, client; HBRUSH hbrBg; HWND parent; HMENU hMenu;
    HWND child, next; int isVisible;
    int cbCount, cbCurSel;
    char cbItems[16][32];
    int lbCount, lbCurSel;
    char lbItems[20][48];
    DWORD lbData[20];
    RECT restoreRect;
    int isMaximized;
    int scrollVMin, scrollVMax, scrollVPos;
    int scrollHMin, scrollHMax, scrollHPos;
    char _pad[142]; /* Pad 1906 -> 2048 bytes (power of 2 for >64KB halloc) */
} INTERNAL_WND;

typedef struct _INTERNAL_DC {
    HWND hwnd; RECT clip; int penX, penY; HPEN hPen; HBRUSH hBrush; HFONT hFont; int fontScale;
    COLORREF textColor, bkColor; int bkMode;
    BYTE __huge* pSurface; int w, h;
} INTERNAL_DC;

typedef struct _BITMAP_OBJ {
    int w, h;
    BYTE __huge* pixels;
} BITMAP_OBJ;

#define MAX_TTF 8
#define MAX_BAKED 8
#define MAX_WINDOWS 32
#define MAX_MSG 128
#define MAX_DCS 8
#define MAX_BITMAPS 8
#define MAX_MENUS 24
#define MAX_MENU_ITEMS 16
#define MAX_TIMERS 8
#define MENU_HANDLE_BASE 0x5000
#define IS_MENU_HANDLE(h) (((WORD)(h) & 0xFF00) == MENU_HANDLE_BASE && (((WORD)(h) & 0xFF) < MAX_MENUS))
typedef struct _INTERNAL_MENU_ITEM {
    UINT flags;
    UINT id;
    char text[40];
    DWORD itemData;
} INTERNAL_MENU_ITEM;

typedef struct _INTERNAL_MENU {
    int used;
    int count;
    INTERNAL_MENU_ITEM items[MAX_MENU_ITEMS];
} INTERNAL_MENU;

typedef struct _INTERNAL_TIMER {
    int active;
    HWND hwnd;
    UINT id;
    UINT elapse;
    TIMERPROC proc;
    clock_t lastFire;
} INTERNAL_TIMER;

#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define MAKELONG(low, high) ((LPARAM)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#define MAKELPARAM(low, high) MAKELONG(low, high)
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb)      (LOBYTE(rgb))
#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
#define MoveTo(hdc, x, y)   MoveToEx(hdc, x, y, NULL)
#define MakeProcInstance(p,i) (p)
#define FreeProcInstance(p)
#define lstrlen strlen
#define lstrcpy strcpy
#define lstrcat strcat
#define lstrcmp strcmp
#define OFN_OVERWRITEPROMPT 0x00000002L
#define OFN_PATHMUSTEXIST   0x00000800L
#define OFN_FILEMUSTEXIST   0x00001000L
#define OFN_HIDEREADONLY    0x00000004L
#define PD_RETURNDC         0x00000100L
#define PD_NOPAGENUMS       0x00000200L
#define LOGPIXELSX          88
#define LOGPIXELSY          90
#define HORZRES             8
#define VERTRES             10
#define CC_RGBINIT          0x00000001L
#define CC_FULLOPEN         0x00000002L
typedef struct tagOFN {
    DWORD lStructSize; HWND hwndOwner; HINSTANCE hInstance; LPCSTR lpstrFilter; LPSTR lpstrCustomFilter;
    DWORD nMaxCustFilter; DWORD nFilterIndex; LPSTR lpstrFile; DWORD nMaxFile; LPSTR lpstrFileTitle;
    DWORD nMaxFileTitle; LPCSTR lpstrInitialDir; LPCSTR lpstrTitle; DWORD Flags; WORD nFileOffset;
    WORD nFileExtension; LPCSTR lpstrDefExt; DWORD lCustData; UINT (CALLBACK *lpfnHook)(HWND, UINT, WPARAM, LPARAM);
    LPCSTR lpTemplateName;
} OPENFILENAME, FAR* LPOPENFILENAME;

typedef struct tagCHOOSECOLOR {
    DWORD lStructSize;
    HWND hwndOwner;
    HWND hInstance;
    COLORREF rgbResult;
    LPDWORD lpCustColors;
    DWORD Flags;
    LPARAM lCustData;
    UINT (CALLBACK* lpfnHook)(HWND, UINT, WPARAM, LPARAM);
    LPCSTR lpTemplateName;
} CHOOSECOLOR, FAR* LPCHOOSECOLOR;

typedef struct tagPD {
    DWORD lStructSize; HWND hwndOwner; HGLOBAL hDevMode; HGLOBAL hDevNames; HDC hDC; DWORD Flags;
    WORD nFromPage; WORD nToPage; WORD nMinPage; WORD nMaxPage; WORD nCopies; HINSTANCE hInstance;
    DWORD lCustData; UINT (CALLBACK *lpfnPrintHook)(HWND, UINT, WPARAM, LPARAM);
    UINT (CALLBACK *lpfnSetupHook)(HWND, UINT, WPARAM, LPARAM); LPCSTR lpPrintTemplateName;
    LPCSTR lpSetupTemplateName; HGLOBAL hPrintTemplate; HGLOBAL hSetupTemplate;
} PRINTDLG, FAR* LPPRINTDLG;

typedef struct { int cbSize; LPCSTR lpszDocName; LPCSTR lpszOutput; } DOCINFO, FAR* LPDOCINFO;
typedef struct { char name[32]; char path[64]; } TtfRegEntry;

typedef struct {
    char path[64];
    BYTE __huge* ttf;
    long size;
} TtfCacheEntry;

typedef struct tagDRAWITEMSTRUCT {
    UINT CtlType; UINT CtlID; UINT itemID; UINT itemAction; UINT itemState;
    HWND hwndItem; HDC hDC; RECT rcItem; DWORD itemData;
} DRAWITEMSTRUCT, FAR* LPDRAWITEMSTRUCT;

typedef struct tagMEASUREITEMSTRUCT {
    UINT CtlType; UINT CtlID; UINT itemID; UINT itemWidth; UINT itemHeight; DWORD itemData;
} MEASUREITEMSTRUCT, FAR* LPMEASUREITEMSTRUCT;

static TtfRegEntry FAR* g_ttfReg = NULL;
static int g_numTtfReg = 0;
static BakedFont FAR* g_baked = NULL;
static int g_numBaked = 0;
static TtfCacheEntry FAR* g_loadedTtf = NULL;
static int g_numLoadedTtf = 0;

static WNDCLASS FAR* g_classes = NULL;
static int g_classCount = 0;
static INTERNAL_WND __huge* g_windows = NULL;
static int g_windowCount = 0;
static INTERNAL_DC FAR* g_dcs = NULL;
static BITMAP_OBJ FAR* g_bitmaps = NULL;
static MSG FAR* g_msgQueue = NULL;
static int g_msgHead = 0, g_msgTail = 0;
static INTERNAL_MENU FAR* g_menus = NULL;
static INTERNAL_TIMER FAR* g_timers = NULL;

static BYTE FAR* rom_font = NULL;
static volatile BYTE g_vga_dummy = 0;

typedef HANDLE HHOOK;
typedef WORD HTASK;

void FAR* _GetROMFont(void);
#pragma aux _GetROMFont = \
    "push bp"           \
    "mov ax, 01130h"    \
    "mov bh, 06h"       \
    "int 010h"          \
    "mov dx, es"        \
    "mov ax, bp"        \
    "pop bp"            \
    value [dx ax]       \
    modify [ax bx cx dx es];
static int CDECL wsprintf(LPSTR lpOut, LPCSTR lpFmt, ...);
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int show);
static UINT WINAPI WinExec(LPCSTR lpCmdLine, UINT uCmdShow);
static DWORD GetPrivateProfileString(LPCSTR app, LPCSTR key, LPCSTR def, LPSTR ret, DWORD sz, LPCSTR file);
static void _HideMouse(void);
static void _ShowMouse(void);
static void _RepaintAll(void);
static void _StripAmpersand(LPCSTR src, char* dst, int maxLen);

/* Forward prototypes for icoedtv.c functions called prior to definition */
void LoadCFile(const char* path, HWND hwnd);
void ShowRefListDialog(const char* filePath, HWND hwnd);

#pragma code_seg("GDI_TEXT")
#include "gdi.h"
#pragma code_seg("USER_TEXT")
#include "user.h"
#pragma code_seg()
#ifndef CS_VREDRAW
#define CS_VREDRAW 0x0001
#endif
#ifndef CS_HREDRAW
#define CS_HREDRAW 0x0002
#endif
#ifndef IDI_APPLICATION
#define IDI_APPLICATION ((LPCSTR)32512)
#endif
static HICON LoadIcon(HINSTANCE h, LPCSTR name) { (void)h; (void)name; return (HICON)1; }

static void explorer_ffree(void FAR* ptr) {
    if (ptr) hfree(ptr);
}

static void _StripAmpersand(LPCSTR src, char* dst, int maxLen) {
    int i = 0, j = 0;
    if (!src) { dst[0] = '\0'; return; }
    while (src[i] && j < maxLen - 1) {
        if (src[i] == '&' && src[i + 1] != '&') { i++; continue; }
        if (src[i] == '&' && src[i + 1] == '&') i++;
        dst[j++] = src[i++];
    }
    dst[j] = '\0';
}

static BYTE _BrushToVGA(HBRUSH hb) {
    WORD w = (WORD)hb;
    if ((w & 0xFF00) == 0x0200) return (BYTE)(w & 0xFF);
    if (w == 6) return 15; // COLOR_WINDOW + 1 overrides
    if (w == 2) return 3;  // COLOR_BACKGROUND + 1
    if (w > 0 && w <= 32) return 7;
    return _RGBtoVGA((COLORREF)hb);
}

static void _LoadTTFRegistry(void) {
    FILE* f = fopen("ttf.ini", "r");
    if (f && g_ttfReg) {
        char line[128];
        g_numTtfReg = 0;
        while (fgets(line, sizeof(line), f) && g_numTtfReg < MAX_TTF) {
            char* eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                {
                    char* name = line;
                    char* path = eq + 1;
                    int len;
                    char* nl = strchr(path, '\r'); if (nl) *nl = '\0';
                    nl = strchr(path, '\n'); if (nl) *nl = '\0';
                    while (*name == ' ' || *name == '\t') name++;
                    while (*path == ' ' || *path == '\t') path++;
                    len = strlen(name);
                    while (len > 0 && (name[len - 1] == ' ' || name[len - 1] == '\t')) name[--len] = '\0';
                    len = strlen(path);
                    while (len > 0 && (path[len - 1] == ' ' || path[len - 1] == '\t')) path[--len] = '\0';
                    strncpy(g_ttfReg[g_numTtfReg].name, name, 31);
                    g_ttfReg[g_numTtfReg].name[31] = '\0';
                    strncpy(g_ttfReg[g_numTtfReg].path, path, 63);
                    g_ttfReg[g_numTtfReg].path[63] = '\0';
                    g_numTtfReg++;
                }
            }
        }
        fclose(f);
    }
}

static void _HideMouse(void) { if (g_mouseState == 0) { union REGS r; r.w.ax = 0x02; int86(0x33, &r, &r); } g_mouseState++; }
static void _ShowMouse(void) { if (g_mouseState > 0) { g_mouseState--; if (g_mouseState == 0) { union REGS r; r.w.ax = 0x01; int86(0x33, &r, &r); } } }

static void _DrawFocusFrame(INTERNAL_DC* dc, RECT* r) {
    int x, y;
    for (x = r->left + 2; x < r->right - 2; x += 2) {
        _DrawPixel(dc, x, r->top + 2, 0);
        _DrawPixel(dc, x, r->bottom - 3, 0);
    }
    for (y = r->top + 2; y < r->bottom - 2; y += 2) {
        _DrawPixel(dc, r->left + 2, y, 0);
        _DrawPixel(dc, r->right - 3, y, 0);
    }
}

static void _PaintTree(HWND hw) {
    INTERNAL_WND* w;
    HWND child;
    BYTE bgCol;
    HFONT defFont = (HFONT)0x0301;
    if (!g_windows || hw == 0 || hw > g_windowCount) return;
    w = &g_windows[hw];
    if (!w->isVisible) return;
    
    bgCol = _BrushToVGA(w->hbrBg);

    if (((w->style & WS_CAPTION) == WS_CAPTION) || (!w->parent && (w->style & WS_CAPTION))) {
        RECT nc = w->rect; RECT btnX, btnM, grip;
        _FillRect(&g_dcs[1], &nc, bgCol);
        _Draw3DBorder(&g_dcs[1], &nc, 15, 0);
        nc.left += 2; nc.top += 2; nc.right -= 2; nc.bottom = nc.top + 16;
        _FillRect(&g_dcs[1], &nc, (_GetTopLevelWindow(hwndFocus) == hw) ? 1 : 8);

        g_dcs[1].hwnd = 0; g_dcs[1].textColor = 15; g_dcs[1].bkMode = 1; g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
        g_dcs[1].clip.left = nc.left; g_dcs[1].clip.top = nc.top; g_dcs[1].clip.right = nc.right - 35; g_dcs[1].clip.bottom = nc.bottom;
        TextOut((HDC)1, nc.left + 4, nc.top + 2, w->text, strlen(w->text));
        g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;

        btnX.left = nc.right - 15; btnX.top = nc.top + 2; btnX.right = nc.right - 2; btnX.bottom = nc.bottom - 2;
        _FillRect(&g_dcs[1], &btnX, 7); _Draw3DBorder(&g_dcs[1], &btnX, 15, 0);
        RECT btnXInner = btnX; btnXInner.left++; btnXInner.top++; btnXInner.right--; btnXInner.bottom--;
        _Draw3DBorder(&g_dcs[1], &btnXInner, 7, 8);
        g_dcs[1].textColor = 0; TextOut((HDC)1, btnX.left + 4, btnX.top + 1, "X", 1);

        if (w->style & WS_MAXIMIZEBOX) {
            btnM.left = btnX.left - 15; btnM.top = btnX.top; btnM.right = btnX.left - 2; btnM.bottom = btnX.bottom;
            _FillRect(&g_dcs[1], &btnM, 7); _Draw3DBorder(&g_dcs[1], &btnM, 15, 0);
            RECT btnMInner = btnM; btnMInner.left++; btnMInner.top++; btnMInner.right--; btnMInner.bottom--;
            _Draw3DBorder(&g_dcs[1], &btnMInner, 7, 8);
            TextOut((HDC)1, btnM.left + 4, btnM.top + 1, w->isMaximized ? "R" : "M", 1);
        }

        if (IS_MENU_HANDLE(w->hMenu) && g_menus) {
            int mIdx = (WORD)w->hMenu & 0xFF;
            if (g_menus[mIdx].used) {
                RECT mb; int i, mx = w->rect.left + 6;
                mb.left = w->rect.left + 2; mb.top = w->rect.top + 18;
                mb.right = w->rect.right - 2; mb.bottom = mb.top + 18;
                _FillRect(&g_dcs[1], &mb, 7);
                g_dcs[1].hwnd = 0; g_dcs[1].textColor = 0; g_dcs[1].bkMode = 1; g_dcs[1].hFont = defFont;
                for (i = 0; i < g_menus[mIdx].count; i++) {
                    char clean[40]; SIZE sz;
                    _StripAmpersand(g_menus[mIdx].items[i].text, clean, sizeof(clean));
                    GetTextExtentPoint((HDC)1, clean, strlen(clean), &sz);
                    TextOut((HDC)1, mx, mb.top + 2, clean, strlen(clean));
                    mx += sz.cx + 14;
                }
            }
        }

        if (w->style & WS_THICKFRAME) {
            grip.left = w->rect.right - 14; grip.top = w->rect.bottom - 14; grip.right = w->rect.right; grip.bottom = w->rect.bottom;
            _FillRect(&g_dcs[1], &grip, 7);
            _DrawPixel(&g_dcs[1], grip.right-4, grip.bottom-4, 8); _DrawPixel(&g_dcs[1], grip.right-6, grip.bottom-4, 8);
            _DrawPixel(&g_dcs[1], grip.right-4, grip.bottom-6, 8);
        }
    } else if (!w->parent) {
        RECT cb = w->rect;
        if (w->hbrBg) _FillRect(&g_dcs[1], &cb, bgCol);
        if (w->style & WS_BORDER) _Draw3DBorder(&g_dcs[1], &cb, 15, 8);
    } else if (w->hbrBg && stricmp(w->className, "BUTTON") != 0 && stricmp(w->className, "COMBOBOX") != 0 &&
               stricmp(w->className, "EDIT") != 0 && stricmp(w->className, "STATIC") != 0 &&
               stricmp(w->className, "LISTBOX") != 0 && stricmp(w->className, "SCROLLBAR") != 0) {
        RECT cb = w->rect;
        _FillRect(&g_dcs[1], &cb, bgCol);
    }

    SendMessage(hw, WM_PAINT, 0, 0);

    if (stricmp(w->className, "BUTTON") == 0) {
        if ((w->style & 0x0F) == BS_OWNERDRAW && w->parent) {
            DRAWITEMSTRUCT dis;
            dis.CtlType = ODT_BUTTON;
            dis.CtlID = (UINT)w->hMenu;
            dis.itemID = 0;
            dis.itemAction = 1;
            dis.itemState = (hw == hwndFocus) ? ODS_SELECTED : 0;
            dis.hwndItem = hw;
            dis.hDC = GetDC(hw);
            dis.rcItem.left = 0; dis.rcItem.top = 0;
            dis.rcItem.right = w->rect.right - w->rect.left;
            dis.rcItem.bottom = w->rect.bottom - w->rect.top;
            dis.itemData = 0;
            SendMessage(w->parent, WM_DRAWITEM, (WPARAM)dis.CtlID, (LPARAM)(LPDRAWITEMSTRUCT)&dis);
            g_dcs[1].hwnd = 0;
            g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
        } else {
            RECT cb = w->rect; char clean[64]; SIZE sz; int tx, ty;
            _FillRect(&g_dcs[1], &cb, 7); 
            _Draw3DBorder(&g_dcs[1], &cb, 15, 0);
            RECT inner = cb; inner.left++; inner.top++; inner.right--; inner.bottom--;
            _Draw3DBorder(&g_dcs[1], &inner, 7, 8);
            
            g_dcs[1].hwnd = 0; g_dcs[1].textColor = 0; g_dcs[1].bkMode = 1; g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
            _StripAmpersand(w->text, clean, sizeof(clean));
            GetTextExtentPoint((HDC)1, clean, strlen(clean), &sz);
            tx = cb.left + ((cb.right - cb.left) - sz.cx) / 2;
            ty = cb.top + ((cb.bottom - cb.top) - sz.cy) / 2;
            if (tx < cb.left + 2) tx = cb.left + 2;
            g_dcs[1].clip.left = cb.left + 2; g_dcs[1].clip.top = cb.top + 2; g_dcs[1].clip.right = cb.right - 2; g_dcs[1].clip.bottom = cb.bottom - 2;
            TextOut((HDC)1, tx, ty, clean, strlen(clean));
            g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
        }
    } else if (stricmp(w->className, "COMBOBOX") == 0) {
        RECT cb = w->rect; RECT btn;
        _FillRect(&g_dcs[1], &cb, 15); _Draw3DBorder(&g_dcs[1], &cb, 8, 15);
        btn.left = cb.right - 16; btn.top = cb.top + 1; btn.right = cb.right - 1; btn.bottom = cb.bottom - 1;
        _FillRect(&g_dcs[1], &btn, 7); 
        _Draw3DBorder(&g_dcs[1], &btn, 15, 0);
        RECT btnInner = btn; btnInner.left++; btnInner.top++; btnInner.right--; btnInner.bottom--;
        _Draw3DBorder(&g_dcs[1], &btnInner, 7, 8);
        
        g_dcs[1].hwnd = 0; g_dcs[1].textColor = 0; g_dcs[1].bkMode = 1; g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
        g_dcs[1].clip.left = cb.left + 2; g_dcs[1].clip.top = cb.top + 2; g_dcs[1].clip.right = btn.left - 2; g_dcs[1].clip.bottom = cb.bottom - 2;
        TextOut((HDC)1, w->rect.left + 4, w->rect.top + 3, w->text, strlen(w->text));
        g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
        TextOut((HDC)1, btn.left + 4, btn.top + 2, "v", 1);
    } else if (stricmp(w->className, "LISTBOX") == 0) {
        RECT cb = w->rect; int i;
        _FillRect(&g_dcs[1], &cb, 15); _Draw3DBorder(&g_dcs[1], &cb, 8, 15);
        g_dcs[1].hwnd = 0; g_dcs[1].bkMode = 1; g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
        g_dcs[1].clip.left = cb.left + 2; g_dcs[1].clip.top = cb.top + 2; g_dcs[1].clip.right = cb.right - 2; g_dcs[1].clip.bottom = cb.bottom - 2;
        for (i = 0; i < w->lbCount; i++) {
            int iy = cb.top + 2 + i * 16;
            if (iy + 16 > cb.bottom) break;
            if (i == w->lbCurSel) {
                RECT sr; sr.left = cb.left + 2; sr.top = iy; sr.right = cb.right - 2; sr.bottom = iy + 16;
                _FillRect(&g_dcs[1], &sr, 1);
                g_dcs[1].textColor = 15;
            } else {
                g_dcs[1].textColor = 0;
            }
            TextOut((HDC)1, cb.left + 4, iy + 1, w->lbItems[i], strlen(w->lbItems[i]));
        }
        g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
    } else if (stricmp(w->className, "SCROLLBAR") == 0) {
        RECT sb = w->rect; RECT btn;
        _FillRect(&g_dcs[1], &sb, 7); _Draw3DBorder(&g_dcs[1], &sb, 8, 15);
        
        btn = sb; btn.right = btn.left + 14;
        _FillRect(&g_dcs[1], &btn, 7); 
        _Draw3DBorder(&g_dcs[1], &btn, 15, 0);
        RECT btnInner1 = btn; btnInner1.left++; btnInner1.top++; btnInner1.right--; btnInner1.bottom--;
        _Draw3DBorder(&g_dcs[1], &btnInner1, 7, 8);
        
        g_dcs[1].hwnd = 0; g_dcs[1].textColor = 0; g_dcs[1].bkMode = 1; g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
        TextOut((HDC)1, btn.left + 3, btn.top + 1, "<", 1);
        
        btn.left = sb.right - 14; btn.right = sb.right;
        _FillRect(&g_dcs[1], &btn, 7); 
        _Draw3DBorder(&g_dcs[1], &btn, 15, 0);
        RECT btnInner2 = btn; btnInner2.left++; btnInner2.top++; btnInner2.right--; btnInner2.bottom--;
        _Draw3DBorder(&g_dcs[1], &btnInner2, 7, 8);
        
        TextOut((HDC)1, btn.left + 3, btn.top + 1, ">", 1);
        if (w->scrollHMax > w->scrollHMin) {
            int range = (sb.right - 14) - (sb.left + 14) - 14;
            int denom = w->scrollHMax - w->scrollHMin; if (denom <= 0) denom = 1;
            if (range > 0) {
                int tx = sb.left + 14 + ((w->scrollHPos - w->scrollHMin) * range / denom);
                RECT thumb; thumb.left = tx; thumb.top = sb.top; thumb.right = tx + 14; thumb.bottom = sb.bottom;
                _FillRect(&g_dcs[1], &thumb, 7); 
                _Draw3DBorder(&g_dcs[1], &thumb, 15, 0);
                RECT thumbInner = thumb; thumbInner.left++; thumbInner.top++; thumbInner.right--; thumbInner.bottom--;
                _Draw3DBorder(&g_dcs[1], &thumbInner, 7, 8);
            }
        }
    } else if (stricmp(w->className, "STATIC") == 0) {
        RECT cb = w->rect; SIZE sz; int tx = cb.left + 2, ty;
        _FillRect(&g_dcs[1], &cb, 7);
        if (w->style & WS_BORDER) _Draw3DBorder(&g_dcs[1], &cb, 8, 15);
        g_dcs[1].hwnd = 0; g_dcs[1].textColor = 0; g_dcs[1].bkMode = 1; g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
        GetTextExtentPoint((HDC)1, w->text, strlen(w->text), &sz);
        if (w->style & SS_CENTER) tx = cb.left + ((cb.right - cb.left) - sz.cx) / 2;
        ty = cb.top + ((cb.bottom - cb.top) - sz.cy) / 2;
        g_dcs[1].clip.left = cb.left + 1; g_dcs[1].clip.top = cb.top + 1; g_dcs[1].clip.right = cb.right - 1; g_dcs[1].clip.bottom = cb.bottom - 1;
        TextOut((HDC)1, tx, ty, w->text, strlen(w->text));
        g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
    } else if (stricmp(w->className, "EDIT") == 0) {
        RECT cb = w->rect;
        _FillRect(&g_dcs[1], &cb, 15);
        _Draw3DBorder(&g_dcs[1], &cb, (hw == hwndFocus) ? 1 : 8, (hw == hwndFocus) ? 1 : 15);
        g_dcs[1].hwnd = 0; g_dcs[1].textColor = 0; g_dcs[1].bkMode = 1; g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
        g_dcs[1].clip.left = cb.left + 2; g_dcs[1].clip.top = cb.top + 2; g_dcs[1].clip.right = cb.right - 2; g_dcs[1].clip.bottom = cb.bottom - 2;
        TextOut((HDC)1, w->rect.left + 3, w->rect.top + 2, w->text, strlen(w->text));
        if (hw == hwndFocus) {
            SIZE sz;
            GetTextExtentPoint((HDC)1, w->text, strlen(w->text), &sz);
            TextOut((HDC)1, w->rect.left + 3 + sz.cx, w->rect.top + 2, "_", 1);
        }
        g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
    }

    if (w->style & WS_VSCROLL && stricmp(w->className, "COMBOBOX") != 0 && stricmp(w->className, "LISTBOX") != 0) {
        RECT sb; RECT btn;
        sb.left = w->rect.right - 14; sb.top = w->rect.top + w->client.top;
        sb.right = w->rect.right; sb.bottom = w->rect.top + w->client.bottom;
        if (w->style & WS_HSCROLL) sb.bottom -= 14;
        _FillRect(&g_dcs[1], &sb, 7); _Draw3DBorder(&g_dcs[1], &sb, 8, 15);
        
        btn = sb; btn.bottom = btn.top + 14;
        _FillRect(&g_dcs[1], &btn, 7); 
        _Draw3DBorder(&g_dcs[1], &btn, 15, 0);
        RECT btnInner1 = btn; btnInner1.left++; btnInner1.top++; btnInner1.right--; btnInner1.bottom--;
        _Draw3DBorder(&g_dcs[1], &btnInner1, 7, 8);
        
        g_dcs[1].hwnd = 0; g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
        TextOut((HDC)1, btn.left + 3, btn.top + 1, "^", 1);
        
        btn.top = sb.bottom - 14; btn.bottom = sb.bottom;
        _FillRect(&g_dcs[1], &btn, 7); 
        _Draw3DBorder(&g_dcs[1], &btn, 15, 0);
        RECT btnInner2 = btn; btnInner2.left++; btnInner2.top++; btnInner2.right--; btnInner2.bottom--;
        _Draw3DBorder(&g_dcs[1], &btnInner2, 7, 8);
        
        TextOut((HDC)1, btn.left + 4, btn.top + 1, "v", 1);
        if (w->scrollVMax > w->scrollVMin) {
            int range = (sb.bottom - 14) - (sb.top + 14) - 14;
            int denom = w->scrollVMax - w->scrollVMin; if (denom <= 0) denom = 1;
            int ty = sb.top + 14 + ((w->scrollVPos - w->scrollVMin) * range / denom);
            RECT thumb; thumb.left = sb.left; thumb.top = ty; thumb.right = sb.right; thumb.bottom = ty + 14;
            _FillRect(&g_dcs[1], &thumb, 7); 
            _Draw3DBorder(&g_dcs[1], &thumb, 15, 0);
            RECT thumbInner = thumb; thumbInner.left++; thumbInner.top++; thumbInner.right--; thumbInner.bottom--;
            _Draw3DBorder(&g_dcs[1], &thumbInner, 7, 8);
        }
    }
    if (w->style & WS_HSCROLL && stricmp(w->className, "SCROLLBAR") != 0) {
        RECT sb; RECT btn;
        sb.left = w->rect.left + w->client.left; sb.top = w->rect.bottom - 14;
        sb.right = w->rect.right; sb.bottom = w->rect.bottom;
        if (w->style & WS_VSCROLL) sb.right -= 14;
        _FillRect(&g_dcs[1], &sb, 7); _Draw3DBorder(&g_dcs[1], &sb, 8, 15);
        
        btn = sb; btn.right = btn.left + 14;
        _FillRect(&g_dcs[1], &btn, 7); 
        _Draw3DBorder(&g_dcs[1], &btn, 15, 0);
        RECT btnInner1 = btn; btnInner1.left++; btnInner1.top++; btnInner1.right--; btnInner1.bottom--;
        _Draw3DBorder(&g_dcs[1], &btnInner1, 7, 8);
        
        g_dcs[1].hwnd = 0; g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
        TextOut((HDC)1, btn.left + 4, btn.top + 1, "<", 1);
        
        btn.left = sb.right - 14; btn.right = sb.right;
        _FillRect(&g_dcs[1], &btn, 7); 
        _Draw3DBorder(&g_dcs[1], &btn, 15, 0);
        RECT btnInner2 = btn; btnInner2.left++; btnInner2.top++; btnInner2.right--; btnInner2.bottom--;
        _Draw3DBorder(&g_dcs[1], &btnInner2, 7, 8);
        
        TextOut((HDC)1, btn.left + 4, btn.top + 1, ">", 1);
        if (w->scrollHMax > w->scrollHMin) {
            int range = (sb.right - 14) - (sb.left + 14) - 14;
            int denom = w->scrollHMax - w->scrollHMin; if (denom <= 0) denom = 1;
            int tx = sb.left + 14 + ((w->scrollHPos - w->scrollHMin) * range / denom);
            RECT thumb; thumb.left = tx; thumb.top = sb.top; thumb.right = tx + 14; thumb.bottom = sb.bottom;
            _FillRect(&g_dcs[1], &thumb, 7); 
            _Draw3DBorder(&g_dcs[1], &thumb, 15, 0);
            RECT thumbInner = thumb; thumbInner.left++; thumbInner.top++; thumbInner.right--; thumbInner.bottom--;
            _Draw3DBorder(&g_dcs[1], &thumbInner, 7, 8);
        }
    }

    child = w->child;
    while (child) { _PaintTree(child); child = g_windows[child].next; }
}

static void _RepaintAll(void) {
    int i;
    g_dcs[1].hwnd = 0;
    g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
    _FillRect(&g_dcs[1], &g_dcs[1].clip, 3);
    for (i = 1; i <= g_windowCount; i++) {
        if (!g_windows[i].parent && g_windows[i].isVisible) {
            _PaintTree((HWND)i);
        }
    }
}

static void _HardwarePoll(void) {
    union REGS r;
    static int last_b = 0, last_rb = 0, last_alt_tab = 0;
    static int last_mx = -1, last_my = -1;
    int mx, my, b, rb, i;
    HWND target;
    BYTE biosFlags = *((volatile BYTE far*)0x00400017L);

    if (g_timers) {
        DWORD now = GetTickCount();
        for (i = 0; i < MAX_TIMERS; i++) {
            if (g_timers[i].active) {
                DWORD elapsedMs = now - (DWORD)g_timers[i].lastFire;
                if (g_timers[i].lastFire == 0 || elapsedMs >= g_timers[i].elapse) {
                    g_timers[i].lastFire = now ? now : 1;
                    if (g_timers[i].proc) g_timers[i].proc(g_timers[i].hwnd, WM_TIMER, g_timers[i].id, now);
                    else if (g_timers[i].hwnd) PostMessage(g_timers[i].hwnd, WM_TIMER, g_timers[i].id, 0);
                }
            }
        }
    }

    if ((biosFlags & 0x08) && (inp(0x60) == 0x0F)) {
        if (!last_alt_tab) {
            last_alt_tab = 1;
            if (kbhit()) getch();
            _CycleTopLevelWindow();
        }
    } else {
        last_alt_tab = 0;
    }

    if (kbhit()) {
        int ch = getch();
        int scan = 0;
        if (ch == 0 || ch == 0xE0) scan = getch();
        _ProcessKeyboardInput(ch, scan, biosFlags);
    }

    r.w.ax = 0x03; int86(0x33, &r, &r);
    mx = r.w.cx; my = r.w.dx;
    b = r.w.bx & 1;
    rb = (r.w.bx & 2) ? 1 : 0;

    if (rb && !last_rb) {
        target = _WindowFromPoint(0, mx, my);
        if (target && g_windows) {
            int cx = mx - (g_windows[target].rect.left + g_windows[target].client.left);
            int cy = my - (g_windows[target].rect.top + g_windows[target].client.top);
            PostMessage(target, WM_RBUTTONDOWN, 0, MAKELONG(cx, cy));
        }
    } else if (!rb && last_rb) {
        target = _WindowFromPoint(0, mx, my);
        if (target && g_windows) {
            int cx = mx - (g_windows[target].rect.left + g_windows[target].client.left);
            int cy = my - (g_windows[target].rect.top + g_windows[target].client.top);
            PostMessage(target, WM_RBUTTONUP, 0, MAKELONG(cx, cy));
        }
    }
    last_rb = rb;

    if (b && !last_b) {
        if (g_dropdownWnd && g_windows) {
            INTERNAL_WND* tw = &g_windows[g_dropdownWnd];
            int listTop = tw->rect.bottom;
            int listBot = listTop + (tw->cbCount * 14) + 2;
            int listLeft = tw->rect.left + tw->client.left;
            int listRight = tw->rect.right;
            if (mx >= listLeft && mx <= listRight && my >= listTop && my <= listBot) {
                int sel = (my - listTop - 1) / 14;
                if (sel >= 0 && sel < tw->cbCount) {
                    tw->cbCurSel = sel; strcpy(tw->text, tw->cbItems[sel]);
                    if (tw->parent) PostMessage(tw->parent, WM_COMMAND, (WPARAM)tw->hMenu, MAKELONG(g_dropdownWnd, CBN_SELCHANGE));
                }
            }
            g_dropdownWnd = 0; g_needsPaint = 1; last_b = b; return;
        }

        target = _WindowFromPoint(0, mx, my);
        if (!target) { last_b = b; return; }

        if (g_windows && (((g_windows[target].style & WS_CAPTION) == WS_CAPTION) || (!g_windows[target].parent && (g_windows[target].style & WS_CAPTION)))) {
            INTERNAL_WND* tw = &g_windows[target];
            if (mx >= tw->rect.right - 18 && mx <= tw->rect.right - 2 && my >= tw->rect.top && my <= tw->rect.top + 18) {
                PostMessage(target, WM_CLOSE, 0, 0);
                DestroyWindow(target);
                last_b = b; return;
            }
            if ((tw->style & WS_MAXIMIZEBOX) && mx >= tw->rect.right - 33 && mx <= tw->rect.right - 19 && my >= tw->rect.top && my <= tw->rect.top + 18) {
                if (tw->isMaximized) { MoveWindow(target, tw->restoreRect.left, tw->restoreRect.top, tw->restoreRect.right - tw->restoreRect.left, tw->restoreRect.bottom - tw->restoreRect.top, TRUE); tw->isMaximized = 0; }
                else { tw->restoreRect = tw->rect; MoveWindow(target, 0, 0, 640, 480, TRUE); tw->isMaximized = 1; }
                PostMessage(target, WM_SIZE, 0, MAKELONG(tw->client.right - tw->client.left, tw->client.bottom - tw->client.top));
                last_b = b; return;
            }
            if (IS_MENU_HANDLE(tw->hMenu) && g_menus && my >= tw->rect.top + 18 && my < tw->rect.top + 36) {
                int mIdx = (WORD)tw->hMenu & 0xFF;
                if (g_menus[mIdx].used) {
                    int itemX = tw->rect.left + 6;
                    HFONT defFont = (HFONT)0x0301;
                    g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
                    for (i = 0; i < g_menus[mIdx].count; i++) {
                        char clean[40]; SIZE sz;
                        _StripAmpersand(g_menus[mIdx].items[i].text, clean, sizeof(clean));
                        GetTextExtentPoint((HDC)1, clean, strlen(clean), &sz);
                        if (mx >= itemX && mx < itemX + sz.cx + 14) {
                            if (g_menus[mIdx].items[i].flags & MF_POPUP) {
                                TrackPopupMenu((HMENU)g_menus[mIdx].items[i].id, 0, itemX, tw->rect.top + 36, 0, target, NULL);
                            } else {
                                PostMessage(target, WM_COMMAND, g_menus[mIdx].items[i].id, 0);
                            }
                            break;
                        }
                        itemX += sz.cx + 14;
                    }
                }
                last_b = b; return;
            }
            if ((tw->style & WS_THICKFRAME) && !tw->isMaximized && mx >= tw->rect.right - 14 && my >= tw->rect.bottom - 14) {
                g_dragWnd = target; g_dragMode = 2; g_dragX = mx; g_dragY = my; last_b = b; return;
            }
            if (!tw->isMaximized && my >= tw->rect.top && my <= tw->rect.top + 18) {
                g_dragWnd = target; g_dragMode = 1; g_dragX = mx - tw->rect.left; g_dragY = my - tw->rect.top; last_b = b; return;
            }
        }

        if (g_windows && stricmp(g_windows[target].className, "SCROLLBAR") == 0) {
            INTERNAL_WND* sw = &g_windows[target];
            SetFocus(target);
            if (mx < sw->rect.left + 14) {
                if (sw->parent) PostMessage(sw->parent, WM_HSCROLL, SB_LINELEFT, MAKELONG(sw->scrollHPos, target));
            } else if (mx > sw->rect.right - 14) {
                if (sw->parent) PostMessage(sw->parent, WM_HSCROLL, SB_LINERIGHT, MAKELONG(sw->scrollHPos, target));
            } else {
                int range = (sw->rect.right - 14) - (sw->rect.left + 14) - 14;
                int denom = sw->scrollHMax - sw->scrollHMin; if (denom <= 0) denom = 1;
                int tx = sw->rect.left + 14 + ((sw->scrollHPos - sw->scrollHMin) * range / denom);
                if (mx < tx) {
                    if (sw->parent) PostMessage(sw->parent, WM_HSCROLL, SB_PAGELEFT, MAKELONG(sw->scrollHPos, target));
                } else {
                    if (sw->parent) PostMessage(sw->parent, WM_HSCROLL, SB_PAGERIGHT, MAKELONG(sw->scrollHPos, target));
                }
            }
            last_b = b; return;
        }

        if (g_windows && stricmp(g_windows[target].className, "LISTBOX") == 0) {
            INTERNAL_WND* lw = &g_windows[target];
            int idx = (my - (lw->rect.top + 2)) / 16;
            SetFocus(target);
            if (idx >= 0 && idx < lw->lbCount) {
                lw->lbCurSel = idx;
                g_needsPaint = 1;
                if (lw->parent) PostMessage(lw->parent, WM_COMMAND, (WPARAM)lw->hMenu, MAKELONG(target, LBN_SELCHANGE));
            }
            last_b = b; return;
        }

        if (g_windows && (g_windows[target].style & WS_VSCROLL)) {
            if (mx >= g_windows[target].rect.right - 14) {
                int sbTop = g_windows[target].rect.top + g_windows[target].client.top;
                int sbBot = g_windows[target].rect.top + g_windows[target].client.bottom;
                if (g_windows[target].style & WS_HSCROLL) sbBot -= 14;
                if (my < sbTop + 14) PostMessage(target, WM_VSCROLL, SB_LINEUP, 0);
                else if (my > sbBot - 14) PostMessage(target, WM_VSCROLL, SB_LINEDOWN, 0);
                else {
                    int range = (sbBot - 14) - (sbTop + 14) - 14;
                    int denom = g_windows[target].scrollVMax - g_windows[target].scrollVMin; if (denom <= 0) denom = 1;
                    int ty = sbTop + 14 + ((g_windows[target].scrollVPos - g_windows[target].scrollVMin) * range / denom);
                    if (my < ty) PostMessage(target, WM_VSCROLL, SB_PAGEUP, 0);
                    else if (my > ty + 14) PostMessage(target, WM_VSCROLL, SB_PAGEDOWN, 0);
                    else { g_dragWnd = target; g_dragMode = 3; g_dragY = my - ty; }
                }
                last_b = b; return;
            }
        }

        if (g_windows && (g_windows[target].style & WS_HSCROLL)) {
            if (my >= g_windows[target].rect.bottom - 14) {
                int sbLeft = g_windows[target].rect.left + g_windows[target].client.left;
                int sbRight = g_windows[target].rect.left + g_windows[target].client.right;
                if (g_windows[target].style & WS_VSCROLL) sbRight -= 14;
                if (mx < sbLeft + 14) PostMessage(target, WM_HSCROLL, SB_LINEUP, 0);
                else if (mx > sbRight - 14) PostMessage(target, WM_HSCROLL, SB_LINEDOWN, 0);
                else {
                    int range = (sbRight - 14) - (sbLeft + 14) - 14;
                    int denom = g_windows[target].scrollHMax - g_windows[target].scrollHMin; if (denom <= 0) denom = 1;
                    int tx = sbLeft + 14 + ((g_windows[target].scrollHPos - g_windows[target].scrollHMin) * range / denom);
                    if (mx < tx) PostMessage(target, WM_HSCROLL, SB_PAGEUP, 0);
                    else if (mx > tx + 14) PostMessage(target, WM_HSCROLL, SB_PAGEDOWN, 0);
                    else { g_dragWnd = target; g_dragMode = 4; g_dragX = mx - tx; }
                }
                last_b = b; return;
            }
        }

        if (g_windows && stricmp(g_windows[target].className, "COMBOBOX") == 0) {
            SetFocus(target);
            g_dropdownWnd = target; g_needsPaint = 1; last_b = b; return;
        }

        if (g_windows && stricmp(g_windows[target].className, "EDIT") == 0) {
            SetFocus(target);
            last_b = b; return;
        }

        if (g_windows && g_windows[target].wndProc == DefWindowProc && g_windows[target].parent) {
            SetFocus(target);
            PostMessage(g_windows[target].parent, WM_COMMAND, (WPARAM)g_windows[target].hMenu, (LPARAM)MAKELONG(target, 0));
        } else if (g_windows) {
            int cx = mx - (g_windows[target].rect.left + g_windows[target].client.left);
            int cy = my - (g_windows[target].rect.top + g_windows[target].client.top);
            PostMessage(target, WM_LBUTTONDOWN, 0, MAKELONG(cx, cy));
        }
    }
    else if (b && g_dragWnd && g_windows) {
        if (g_dragMode == 1) {
            INTERNAL_WND* w = &g_windows[g_dragWnd];
            MoveWindow(g_dragWnd, mx - g_dragX, my - g_dragY, w->rect.right - w->rect.left, w->rect.bottom - w->rect.top, FALSE);
        } else if (g_dragMode == 2) {
            INTERNAL_WND* w = &g_windows[g_dragWnd];
            int nw = (w->rect.right - w->rect.left) + (mx - g_dragX);
            int nh = (w->rect.bottom - w->rect.top) + (my - g_dragY);
            if (nw < 100) nw = 100; if (nh < 50) nh = 50;
            if (nw > 640) nw = 640; if (nh > 480) nh = 480;
            MoveWindow(g_dragWnd, w->rect.left, w->rect.top, nw, nh, FALSE);
            g_dragX = mx; g_dragY = my;
            PostMessage(g_dragWnd, WM_SIZE, 0, MAKELONG(w->client.right - w->client.left, w->client.bottom - w->client.top));
        } else if (g_dragMode == 3) {
            INTERNAL_WND* w = &g_windows[g_dragWnd];
            int sbTop = w->rect.top + w->client.top;
            int sbBot = w->rect.top + w->client.bottom;
            if (w->style & WS_HSCROLL) sbBot -= 14;
            int range = (sbBot - 14) - (sbTop + 14) - 14;
            int denom = w->scrollVMax - w->scrollVMin; if (denom <= 0) denom = 1;
            if (range > 0) {
                int ty = my - g_dragY;
                if (ty < sbTop + 14) ty = sbTop + 14;
                if (ty > sbBot - 14 - 14) ty = sbBot - 14 - 14;
                {
                    int newPos = w->scrollVMin + ((ty - (sbTop + 14)) * denom / range);
                    if (w->scrollVPos != newPos) {
                        w->scrollVPos = newPos;
                        PostMessage(g_dragWnd, WM_VSCROLL, SB_THUMBTRACK, MAKELONG(newPos, 0));
                    }
                }
            }
        } else if (g_dragMode == 4) {
            INTERNAL_WND* w = &g_windows[g_dragWnd];
            int sbLeft = w->rect.left + w->client.left;
            int sbRight = w->rect.left + w->client.right;
            if (w->style & WS_VSCROLL) sbRight -= 14;
            int range = (sbRight - 14) - (sbLeft + 14) - 14;
            int denom = w->scrollHMax - w->scrollHMin; if (denom <= 0) denom = 1;
            if (range > 0) {
                int tx = mx - g_dragX;
                if (tx < sbLeft + 14) tx = sbLeft + 14;
                if (tx > sbRight - 14 - 14) tx = sbRight - 14 - 14;
                {
                    int newPos = w->scrollHMin + ((tx - (sbLeft + 14)) * denom / range);
                    if (w->scrollHPos != newPos) {
                        w->scrollHPos = newPos;
                        PostMessage(g_dragWnd, WM_HSCROLL, SB_THUMBTRACK, MAKELONG(newPos, 0));
                    }
                }
            }
        }
    }
    else if (!b && last_b) {
        if (g_dropdownWnd) {
        } else if (g_dragWnd) {
            if (g_dragMode == 3) PostMessage(g_dragWnd, WM_VSCROLL, SB_THUMBPOSITION, MAKELONG(g_windows[g_dragWnd].scrollVPos, 0));
            else if (g_dragMode == 4) PostMessage(g_dragWnd, WM_HSCROLL, SB_THUMBPOSITION, MAKELONG(g_windows[g_dragWnd].scrollHPos, 0));
            g_needsPaint = 1; g_dragWnd = 0; g_dragMode = 0;
        } else if (g_windows) {
            target = g_capturedWnd ? g_capturedWnd : _WindowFromPoint(0, mx, my);
            if (target) {
                int cx = mx - (g_windows[target].rect.left + g_windows[target].client.left);
                int cy = my - (g_windows[target].rect.top + g_windows[target].client.top);
                PostMessage(target, WM_LBUTTONUP, 0, MAKELONG(cx, cy));
            }
        }
    }

    if (mx != last_mx || my != last_my) {
        if (!g_dragWnd && g_windows) {
            target = g_capturedWnd ? g_capturedWnd : _WindowFromPoint(0, mx, my);
            if (target) {
                int cx = mx - (g_windows[target].rect.left + g_windows[target].client.left);
                int cy = my - (g_windows[target].rect.top + g_windows[target].client.top);
                PostMessage(target, WM_MOUSEMOVE, 0, MAKELONG(cx, cy));
            }
        }
        last_mx = mx; last_my = my;
    }
    last_b = b;
}
static BOOL GetMessage(LPMSG m, HWND hw, UINT fMin, UINT fMax) {
    (void)hw; (void)fMin; (void)fMax;
    while (g_msgHead == g_msgTail) {
        if (g_needsPaint && g_windowCount > 0) {
            _RepaintAll();
            if (g_dropdownWnd && g_windows) {
                INTERNAL_WND* tw = &g_windows[g_dropdownWnd]; int i;
                RECT listRect;
                listRect.left = tw->rect.left + tw->client.left; listRect.right = tw->rect.right;
                listRect.top = tw->rect.bottom; listRect.bottom = listRect.top + (tw->cbCount * 14) + 2;
                _FillRect(&g_dcs[1], &listRect, 15); _Draw3DBorder(&g_dcs[1], &listRect, 8, 0);
                g_dcs[1].hwnd = 0; g_dcs[1].textColor = 0; g_dcs[1].bkMode = 1;
                g_dcs[1].hFont = (HFONT)(g_numBaked > 0 ? 0x4000 : 0x0301); g_dcs[1].fontScale = 1;
                g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
                for (i = 0; i < tw->cbCount; i++) {
                    if (i == tw->cbCurSel) {
                        RECT sel; sel.left = listRect.left + 1; sel.top = listRect.top + 1 + (i * 14); sel.right = listRect.right - 1; sel.bottom = sel.top + 14;
                        _FillRect(&g_dcs[1], &sel, 1);
                        g_dcs[1].textColor = 15; g_dcs[1].bkColor = 1;
                    } else {
                        g_dcs[1].textColor = 0; g_dcs[1].bkColor = 15;
                    }
                    TextOut((HDC)1, listRect.left + 4, listRect.top + 2 + (i * 14), tw->cbItems[i], strlen(tw->cbItems[i]));
                }
            }
            g_needsPaint = 0;
        }
        _HardwarePoll();
    }
    if (g_msgQueue) {
        *m = g_msgQueue[g_msgHead];
        g_msgHead = (g_msgHead + 1) % MAX_MSG;
    }
    return (m->message != WM_QUIT);
}

static BOOL TranslateMessage(const MSG FAR* m) { (void)m; return TRUE; }
static LRESULT DispatchMessage(const MSG FAR* m) { return SendMessage(m->hwnd, m->message, m->wParam, m->lParam); }

static DWORD GetPrivateProfileString(LPCSTR app, LPCSTR key, LPCSTR def, LPSTR ret, DWORD sz, LPCSTR file) {
    (void)app; (void)key; (void)file;
    strncpy(ret, def, (size_t)sz);
    return strlen(ret);
}

static UINT WINAPI WinExec(LPCSTR lpCmdLine, UINT uCmdShow) {
    int res;
    (void)uCmdShow;
    if (!lpCmdLine || lpCmdLine[0] == '\0') return 0;
    res = system(lpCmdLine);
    return (res == -1) ? 0 : 33;
}

static int CDECL wsprintf(LPSTR lpOut, LPCSTR lpFmt, ...) {
    va_list args;
    int len;
    va_start(args, lpFmt);
    len = vsprintf(lpOut, lpFmt, args);
    va_end(args);
    return len;
}

static BOOL WritePrivateProfileString(LPCSTR app, LPCSTR key, LPCSTR str, LPCSTR file) { (void)app; (void)key; (void)str; (void)file; return TRUE; }
static DWORD GetModuleFileName(HINSTANCE h, LPSTR file, DWORD sz) { (void)h; (void)sz; strcpy(file, "C:\\TASKBAR.EXE"); return 14; }
static BOOL GetSaveFileName(OPENFILENAME FAR* ofn) { (void)ofn; return FALSE; }
static BOOL PrintDlg(PRINTDLG FAR* pd) { (void)pd; return FALSE; }
static BOOL ChooseColor(LPCHOOSECOLOR lpcc) { (void)lpcc; return FALSE; }

static LRESULT WINAPI CallNextHookEx(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam) {
    if (hhk) {
        HOOKPROC fn = (HOOKPROC)hhk;
        return fn(nCode, wParam, lParam);
    }
    return 0;
}

static HINSTANCE WINAPI ShellExecute(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, int nShowCmd) {
    char fullCmd[256];
    UINT res;
    (void)hwnd; (void)lpOperation; (void)lpDirectory;
    if (lpParameters && lpParameters[0] != '\0') {
        wsprintf(fullCmd, "%s %s", lpFile, lpParameters);
        res = WinExec(fullCmd, (UINT)nShowCmd);
    } else {
        res = WinExec(lpFile, (UINT)nShowCmd);
    }
    return (HINSTANCE)(res > 32 ? res : 32);
}

static BOOL PeekMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
    (void)hWnd; (void)wMsgFilterMin; (void)wMsgFilterMax;
    if (g_msgHead == g_msgTail) {
        if (g_needsPaint && g_windowCount > 0) {
            _RepaintAll();
            g_needsPaint = 0;
        }
        _HardwarePoll();
        if (g_msgHead == g_msgTail) return FALSE;
    }
    if (g_msgQueue) {
        *lpMsg = g_msgQueue[g_msgHead];
        if (wRemoveMsg & PM_REMOVE) {
            g_msgHead = (g_msgHead + 1) % MAX_MSG;
        }
        return TRUE;
    }
    return FALSE;
}

int main(int argc, char** argv) {
    union REGS r; int ret; int i;
    (void)argc; (void)argv;

    g_ttfReg = (TtfRegEntry FAR*)halloc(MAX_TTF, sizeof(TtfRegEntry));
    g_baked = (BakedFont FAR*)halloc(MAX_BAKED, sizeof(BakedFont));
    g_loadedTtf = (TtfCacheEntry FAR*)halloc(MAX_TTF, sizeof(TtfCacheEntry));

    g_windows = (INTERNAL_WND FAR*)halloc(MAX_WINDOWS, sizeof(INTERNAL_WND));
    g_classes = (WNDCLASS FAR*)halloc(16, sizeof(WNDCLASS));
    g_dcs = (INTERNAL_DC FAR*)halloc(MAX_DCS, sizeof(INTERNAL_DC));
    g_bitmaps = (BITMAP_OBJ FAR*)halloc(MAX_BITMAPS, sizeof(BITMAP_OBJ));
    g_msgQueue = (MSG FAR*)halloc(MAX_MSG, sizeof(MSG));
    g_menus = (INTERNAL_MENU FAR*)halloc(MAX_MENUS, sizeof(INTERNAL_MENU));
    g_timers = (INTERNAL_TIMER FAR*)halloc(MAX_TIMERS, sizeof(INTERNAL_TIMER));

    if (g_ttfReg) memset(g_ttfReg, 0, MAX_TTF * sizeof(TtfRegEntry));
    if (g_baked) memset(g_baked, 0, MAX_BAKED * sizeof(BakedFont));
    if (g_loadedTtf) memset(g_loadedTtf, 0, MAX_TTF * sizeof(TtfCacheEntry));
    if (g_windows) memset(g_windows, 0, MAX_WINDOWS * sizeof(INTERNAL_WND));
    if (g_classes) memset(g_classes, 0, 16 * sizeof(WNDCLASS));
    if (g_dcs) memset(g_dcs, 0, MAX_DCS * sizeof(INTERNAL_DC));
    if (g_bitmaps) memset(g_bitmaps, 0, MAX_BITMAPS * sizeof(BITMAP_OBJ));
    if (g_msgQueue) memset(g_msgQueue, 0, MAX_MSG * sizeof(MSG));
    if (g_menus) memset(g_menus, 0, MAX_MENUS * sizeof(INTERNAL_MENU));
    if (g_timers) memset(g_timers, 0, MAX_TIMERS * sizeof(INTERNAL_TIMER));

    rom_font = (BYTE FAR*)_GetROMFont();
    _LoadTTFRegistry();

    r.w.ax = 0x0012; int86(0x10, &r, &r);
    r.w.ax = 0x0000; int86(0x33, &r, &r);
    r.w.ax = 0x0001; int86(0x33, &r, &r);

    ret = WinMain(1, 0, "", 1);

    r.w.ax = 0x0003; int86(0x10, &r, &r);

    if (g_baked) {
        for (i = 0; i < g_numBaked; i++) {
            if (g_baked[i].atlas) hfree(g_baked[i].atlas);
        }
        hfree(g_baked);
    }
    if (g_ttfReg) hfree(g_ttfReg);
    if (g_loadedTtf) {
        for (i = 0; i < g_numLoadedTtf; i++) {
            if (g_loadedTtf[i].ttf) hfree(g_loadedTtf[i].ttf);
        }
        hfree(g_loadedTtf);
    }
    if (g_windows) hfree(g_windows);
    if (g_classes) hfree(g_classes);
    if (g_dcs) hfree(g_dcs);
    if (g_bitmaps) hfree(g_bitmaps);
    if (g_msgQueue) hfree(g_msgQueue);
    if (g_menus) hfree(g_menus);
    if (g_timers) hfree(g_timers);

    return ret;
}
#endif /* MICROWIN_H */