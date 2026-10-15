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
#ifndef USER_H
#define USER_H
#define WM_CREATE           0x0001
#define WM_DESTROY          0x0002
#define WM_SIZE             0x0005
#define WM_SETFOCUS         0x0007
#define WM_KILLFOCUS        0x0008
#define WM_SETREDRAW        0x000B
#define WM_SETTEXT          0x000C
#define WM_GETTEXT          0x000D
#define WM_PAINT            0x000F
#define WM_CLOSE            0x0010
#define WM_QUIT             0x0012
#define WM_ERASEBKGND       0x0014
#define WM_CANCELMODE       0x001F
#define WM_SETCURSOR        0x0020
#define WM_DRAWITEM         0x002B
#define WM_MEASUREITEM      0x002C
#define WM_DELETEITEM       0x002D
#define WM_SETFONT          0x0030
#define WM_WINDOWPOSCHANGING 0x0046
#define WM_NCHITTEST        0x0084
#define WM_GETDLGCODE       0x0087
#define WM_KEYDOWN          0x0100
#define WM_KEYUP            0x0101
#define WM_CHAR             0x0102
#define WM_SYSKEYDOWN       0x0104
#define WM_SYSKEYUP         0x0105
#define WM_SYSCHAR          0x0106
#define WM_COMMAND          0x0111
#define WM_SYSCOMMAND       0x0112
#define WM_TIMER            0x0113
#define WM_HSCROLL          0x0114
#define WM_VSCROLL          0x0115
#define WM_INITMENUPOPUP    0x0117
#define WM_MENUSELECT       0x011F
#define WM_MOUSEMOVE        0x0200
#define WM_LBUTTONDOWN      0x0201
#define WM_LBUTTONUP        0x0202
#define WM_LBUTTONDBLCLK    0x0203
#define WM_RBUTTONDOWN      0x0204
#define WM_RBUTTONUP        0x0205
#define WM_DROPFILES        0x0233
#define WM_USER             0x0400
#define DLGC_WANTARROWS     0x0001
#define DLGC_WANTTAB        0x0002
#define DLGC_WANTALLKEYS    0x0004
#define DLGC_WANTMESSAGE    0x0004
#define DLGC_HASSETSEL      0x0008
#define DLGC_DEFPUSHBUTTON  0x0010
#define DLGC_UNDEFPUSHBUTTON 0x0020
#define DLGC_RADIOBUTTON    0x0040
#define DLGC_WANTCHARS      0x0080
#define DLGC_STATIC         0x0100
#define DLGC_BUTTON         0x2000
#define VK_BACK             0x08
#define VK_TAB              0x09
#define VK_RETURN           0x0D
#define VK_SHIFT            0x10
#define VK_CONTROL          0x11
#define VK_MENU             0x12
#define VK_ESCAPE           0x1B
#define VK_SPACE            0x20
#define VK_PRIOR            0x21
#define VK_NEXT             0x22
#define VK_END              0x23
#define VK_HOME             0x24
#define VK_LEFT             0x25
#define VK_UP               0x26
#define VK_RIGHT            0x27
#define VK_DOWN             0x28
#define VK_INSERT           0x2D
#define VK_DELETE           0x2E
#define VK_F1               0x70
#define VK_F2               0x71
#define VK_F3               0x72
#define VK_F4               0x73
#define VK_F5               0x74
#define VK_F6               0x75
#define VK_F7               0x76
#define VK_F8               0x77
#define VK_F9               0x78
#define VK_F10              0x79
#define WS_OVERLAPPED       0x00000000L
#define WS_OVERLAPPEDWINDOW 0x00CF0000L
#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
#define WS_MAXIMIZEBOX      0x00010000L
#define WS_MINIMIZEBOX      0x00020000L
#define WS_CAPTION          0x00C00000L
#define WS_BORDER           0x00800000L
#define WS_THICKFRAME       0x00040000L
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, FAR* LPSYSTEMTIME;

#define WS_SYSMENU          0x00080000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L
#define WS_EX_DLGMODALFRAME 0x0001L
#define WS_EX_ACCEPTFILES   0x00000010L
#define CW_USEDEFAULT       0x8000
#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_RESTORE          9
#define SIZE_RESTORED       0
#define SIZE_MINIMIZED      1
#define SIZE_MAXIMIZED      2
#define CS_DBLCLKS          0x0008
#define ES_CENTER           0x0001
#define ES_MULTILINE        0x0004
#define ES_WANTRETURN       0x1000
#define ES_AUTOVSCROLL      0x0040
#define ES_AUTOHSCROLL      0x0080
#define EN_SETFOCUS         0x0100
#define EN_KILLFOCUS        0x0200
#define EN_CHANGE           0x0300
#define EN_UPDATE           0x0400
#define SS_LEFT             0x0000L
#define SS_CENTER           0x0001L
#define SS_LEFTNOWORDWRAP   0x000CL
#define SS_NOPREFIX         0x0080L
#define CBS_DROPDOWN        0x0002
#define CBS_DROPDOWNLIST    0x0003
#define CBN_SELCHANGE       1
#define CBN_DBLCLK          2
#define CBN_SETFOCUS        3
#define CBN_KILLFOCUS       4
#define CBN_EDITCHANGE      5
#define CBN_EDITUPDATE      6
#define CBN_DROPDOWN        7
#define CBN_CLOSEUP         8
#define CB_ADDSTRING        0x0143
#define CB_SETCURSEL        0x014E
#define CB_GETCURSEL        0x0147
#define CB_GETCOUNT         0x0146
#define CB_GETLBTEXT        0x0148
#define CB_ERR              (-1)
#define LBN_SELCHANGE       1
#define LBN_DBLCLK          2
#define LBN_SETFOCUS        4
#define LBN_KILLFOCUS       5
#define LB_GETTOPINDEX      0x018E
#define LB_GETITEMRECT      0x0198
#define LB_SETSEL           0x0185
#define LB_SETCARETINDEX    0x019E
#define LB_GETCARETINDEX    0x019F
#define LB_GETSEL           0x0187
#define LB_ADDSTRING        0x0181
#define LB_RESETCONTENT     0x0184
#define LB_GETCOUNT         0x018B
#define LB_GETITEMDATA      0x0199
#define LB_SETCURSEL        0x0186
#define LB_SETCOLUMNWIDTH   0x0196
#define LB_GETCURSEL        0x0188
#define LB_SETITEMDATA      0x019A
#define LB_ERR              (-1)
#define LBS_NOTIFY          0x0001L
#define LBS_OWNERDRAWFIXED  0x0010L
#define LBS_HASSTRINGS      0x0040L
#define LBS_MULTICOLUMN     0x0200L
#define LBS_EXTENDEDSEL     0x0800L
#define EM_SETSEL           0x00B1
#define MF_ENABLED          0x0000L
#define MF_BYCOMMAND        0x0000L
#define MF_BYPOSITION       0x0400L
#define MF_STRING           0x0000L
#define MF_GRAYED           0x0001L
#define MF_DISABLED         0x0002L
#define MF_CHECKED          0x0008L
#define MF_UNCHECKED        0x0000L
#define MF_POPUP            0x0010L
#define MF_MENUBARBREAK     0x0020L
#define MF_OWNERDRAW        0x0100L
#define MF_SEPARATOR        0x0800L
#define MF_SYSMENU          0x2000L
#define TPM_LEFTALIGN       0x0000L
#define TPM_RIGHTBUTTON     0x0002L
#define TPM_RETURNCMD       0x0100L
#define SC_MINIMIZE         0xF020
#define SC_MAXIMIZE         0xF030
#define SC_CLOSE            0xF060
#define SC_RESTORE          0xF120
#define BS_PUSHBUTTON       0x0000
#define BS_DEFPUSHBUTTON    0x0001
#define BS_CHECKBOX         0x0002
#define BS_AUTOCHECKBOX     0x0003
#define BS_RADIOBUTTON      0x0004
#define BS_AUTORADIOBUTTON  0x0009
#define BS_OWNERDRAW        0x000B
#define BM_GETCHECK         0x00F0
#define BM_SETCHECK         0x00F1
#define BM_GETSTATE         0x00F2
#define BM_SETSTATE         0x00F3
#define BN_CLICKED          0
#define SBS_HORZ            0x0000L
#define SBS_VERT            0x0001L
#define ODS_SELECTED        0x0001
#define ODT_MENU            1
#define ODT_BUTTON          4
#define COLOR_BACKGROUND    1
#define COLOR_MENU          4
#define COLOR_MENUTEXT      7
#define COLOR_WINDOWTEXT    8
#define COLOR_HIGHLIGHT     13
#define COLOR_HIGHLIGHTTEXT 14
#define COLOR_BTNFACE       15
#define COLOR_BTNSHADOW     16
#define COLOR_BTNTEXT       18
#define COLOR_BTNHIGHLIGHT  20
#define FW_NORMAL           400
#define FW_SEMIBOLD         600
#define FW_BOLD             700
#define DEFAULT_CHARSET     1
#define IDC_ARROW           ((LPCSTR)32512)
#define IDC_CROSS           ((LPCSTR)32515)
#define IDC_SIZEWE          ((LPCSTR)32644)
#define IDC_SIZENS          ((LPCSTR)32645)
#define MB_OK               0x0000
#define MB_OKCANCEL         0x0001
#define MB_YESNOCANCEL      0x0003
#define MB_YESNO            0x0004
#define MB_ICONHAND         0x0010
#define MB_ICONQUESTION     0x0020
#define MB_ICONEXCLAMATION  0x0030
#define MB_ICONWARNING      0x0030
#define MB_ICONINFORMATION  0x0040
#define MB_ICONASTERISK     0x0040
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#define GW_HWNDFIRST        0
#define GW_HWNDLAST         1
#define GW_HWNDNEXT         2
#define GW_HWNDPREV         3
#define GW_OWNER            4
#define GW_CHILD            5
#define GWW_ID              0
#define GWW_HINSTANCE       2
#define GWL_WNDPROC         0
#define GWL_ID              2
#define GWL_STYLE           4
#define SM_CXSCREEN         0
#define SM_CYSCREEN         1
#define SWP_NOSIZE          0x0001
#define SWP_NOMOVE          0x0002
#define SWP_NOZORDER        0x0004
#define SWP_NOACTIVATE      0x0010
#define SWP_SHOWWINDOW      0x0040
#define SWP_HIDEWINDOW      0x0080
#define HWND_TOPMOST        ((HWND)-1)
#define HWND_BOTTOM         ((HWND)1)
#define SPI_SETWORKAREA     47
#define SB_HORZ             0
#define SB_VERT             1
#define SB_CTL              2
#define SB_BOTH             3
#define SB_LINEUP           0
#define SB_LINELEFT         0
#define SB_LINEDOWN         1
#define SB_LINERIGHT        1
#define SB_PAGEUP           2
#define SB_PAGELEFT         2
#define SB_PAGEDOWN         3
#define SB_PAGERIGHT        3
#define SB_THUMBPOSITION    4
#define SB_THUMBTRACK       5
#define SB_TOP              6
#define SB_LEFT             6
#define SB_BOTTOM           7
#define SB_RIGHT            7
#define SB_ENDSCROLL        8
#define DRIVE_REMOVABLE     2
#define DRIVE_FIXED         3
#define DRIVE_REMOTE        4
#define MK_LBUTTON          0x0001
#define HTCLIENT            1
#define MSGF_MENU           2
#define WH_MSGFILTER        (-1)
#define PM_REMOVE           0x0001
#define GMEM_FIXED          0x0000
#define GMEM_MOVEABLE       0x0002
#define GMEM_ZEROINIT       0x0040
#define GHND                (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR                (GMEM_FIXED | GMEM_ZEROINIT)
#ifndef _export
#define _export __export
#endif
#define GlobalAllocPtr(flags, cb)   ((void FAR*)GlobalAlloc((flags), (cb)))
#define GlobalFreePtr(lp)           ((BOOL)(GlobalFree((HGLOBAL)(lp)) == NULL))
#define DT_TOP              0x0000
#define DT_LEFT             0x0000
#define DT_CENTER           0x0001
#define DT_RIGHT            0x0002
#define DT_VCENTER          0x0004
#define DT_WORDBREAK        0x0010
#define DT_SINGLELINE       0x0020
typedef struct tagWINDOWPOS {
    HWND hwnd;
    HWND hwndInsertAfter;
    int x;
    int y;
    int cx;
    int cy;
    UINT flags;
} WINDOWPOS, FAR* LPWINDOWPOS;

typedef struct tagDELETEITEMSTRUCT {
    UINT CtlType;
    UINT CtlID;
    UINT itemID;
    HWND hwndItem;
    DWORD itemData;
} DELETEITEMSTRUCT, FAR* LPDELETEITEMSTRUCT;

typedef struct tagCREATESTRUCT {
    LPVOID lpCreateParams;
    HINSTANCE hInstance;
    HMENU hMenu;
    HWND hwndParent;
    int cy;
    int cx;
    int y;
    int x;
    LONG style;
    LPCSTR lpszName;
    LPCSTR lpszClass;
    DWORD dwExStyle;
} CREATESTRUCT, FAR* LPCREATESTRUCT;

#ifndef MAX_MENUS
#define MAX_MENUS 24
#define MAX_MENU_ITEMS 16
#define MAX_TIMERS 8
#define MENU_HANDLE_BASE 0x5000
#define IS_MENU_HANDLE(h) (((WORD)(h) & 0xFF00) == MENU_HANDLE_BASE && (((WORD)(h) & 0xFF) < MAX_MENUS))
typedef void (CALLBACK* TIMERPROC)(HWND, UINT, UINT, DWORD);

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

static INTERNAL_MENU FAR* g_menus = NULL;
static INTERNAL_TIMER FAR* g_timers = NULL;

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

static void _PaintTree(HWND hw);
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
#endif
static RECT g_microwinWorkArea = {0, 0, 640, 480};
static HWND hwndRoot = NULL, hwndFocus = NULL, g_dropdownWnd = 0;
static int g_needsPaint = 1;

static int g_mouseState = 0;
static HWND g_dragWnd = 0;
static int g_dragMode = 0;
static int g_dragX = 0, g_dragY = 0;
static HWND g_capturedWnd = NULL;

static LRESULT SendMessage(HWND hw, UINT m, WPARAM w, LPARAM l);
static void PostMessage(HWND hw, UINT m, WPARAM w, LPARAM l);
static BOOL ShowWindow(HWND hw, int s);
static HWND SetFocus(HWND hw);
static BOOL DestroyWindow(HWND hw);
static BOOL MoveWindow(HWND hw, int x, int y, int w, int h, BOOL rpt);
static BOOL SetWindowPos(HWND hw, HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags);
static BOOL TrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT FAR* prcRect);

static void WINAPI GetLocalTime(LPSYSTEMTIME lpSystemTime) {
    time_t t;
    struct tm* tm_info;
    time(&t);
    tm_info = localtime(&t);
    if (tm_info && lpSystemTime) {
        lpSystemTime->wYear = tm_info->tm_year + 1900;
        lpSystemTime->wMonth = tm_info->tm_mon + 1;
        lpSystemTime->wDayOfWeek = tm_info->tm_wday;
        lpSystemTime->wDay = tm_info->tm_mday;
        lpSystemTime->wHour = tm_info->tm_hour;
        lpSystemTime->wMinute = tm_info->tm_min;
        lpSystemTime->wSecond = tm_info->tm_sec;
        lpSystemTime->wMilliseconds = 0;
    }
}

static void WINAPI GetSystemTime(LPSYSTEMTIME lpSystemTime) {
    GetLocalTime(lpSystemTime);
}
static HWND _GetTopLevelWindow(HWND hw) {
    if (!g_windows || hw == 0 || hw > g_windowCount) return hw;
    while (g_windows[hw].parent && g_windows[hw].parent <= g_windowCount) {
        hw = g_windows[hw].parent;
    }
    return hw;
}

static int _IsFocusableControl(HWND hw) {
    INTERNAL_WND* w;
    if (!g_windows || hw == 0 || hw > g_windowCount) return 0;
    w = &g_windows[hw];
    if (!w->isVisible || (w->style & WS_DISABLED)) return 0;
    if (stricmp(w->className, "STATIC") == 0) return 0;
    if (stricmp(w->className, "BUTTON") == 0 ||
        stricmp(w->className, "EDIT") == 0 ||
        stricmp(w->className, "COMBOBOX") == 0 ||
        stricmp(w->className, "LISTBOX") == 0 ||
        stricmp(w->className, "SCROLLBAR") == 0 ||
        (w->style & WS_TABSTOP)) {
        return 1;
    }
    return 0;
}

static void _CollectControls(HWND parent, HWND* list, int* count, int maxCount) {
    HWND child;
    if (!g_windows || parent == 0 || parent > g_windowCount) return;
    child = g_windows[parent].child;
    while (child) {
        if (g_windows[child].isVisible && !(g_windows[child].style & WS_DISABLED)) {
            if (_IsFocusableControl(child) && *count < maxCount) {
                list[(*count)++] = child;
            }
            if (g_windows[child].child) {
                _CollectControls(child, list, count, maxCount);
            }
        }
        child = g_windows[child].next;
    }
}

static void _CycleFocus(HWND topWnd, int dir) {
    HWND list[MAX_WINDOWS];
    int count = 0, i, curIdx = -1;
    if (!topWnd) topWnd = _GetTopLevelWindow(hwndFocus ? hwndFocus : hwndRoot);
    if (!topWnd) return;
    _CollectControls(topWnd, list, &count, MAX_WINDOWS);
    if (count == 0) {
        SetFocus(topWnd);
        return;
    }
    for (i = 0; i < count; i++) {
        if (list[i] == hwndFocus) { curIdx = i; break; }
    }
    if (curIdx == -1) {
        curIdx = (dir > 0) ? 0 : (count - 1);
    } else {
        curIdx = (curIdx + dir + count) % count;
    }
    SetFocus(list[curIdx]);
}

static void _CycleTopLevelWindow(void) {
    HWND topList[MAX_WINDOWS];
    int count = 0, i, curIdx = -1;
    HWND curTop = _GetTopLevelWindow(hwndFocus ? hwndFocus : hwndRoot);
    if (!g_windows) return;
    for (i = 1; i <= g_windowCount; i++) {
        if (!g_windows[i].parent && g_windows[i].isVisible && !(g_windows[i].style & WS_DISABLED)) {
            topList[count++] = (HWND)i;
        }
    }
    if (count <= 0) return;
    for (i = 0; i < count; i++) {
        if (topList[i] == curTop) { curIdx = i; break; }
    }
    curIdx = (curIdx + 1) % count;
    SetFocus(topList[curIdx]);
}

static char _GetMnemonicChar(LPCSTR text) {
    if (!text) return 0;
    while (*text) {
        if (*text == '&' && *(text + 1) != '&' && *(text + 1) != '\0') {
            return (char)toupper((unsigned char)*(text + 1));
        }
        if (*text == '&' && *(text + 1) == '&') text++;
        text++;
    }
    return 0;
}

static char _ScanToAltChar(int scan) {
    static const char row1[] = "QWERTYUIOP";
    static const char row2[] = "ASDFGHJKL";
    static const char row3[] = "ZXCVBNM";
    if (scan >= 0x10 && scan <= 0x19) return row1[scan - 0x10];
    if (scan >= 0x1E && scan <= 0x26) return row2[scan - 0x1E];
    if (scan >= 0x2C && scan <= 0x32) return row3[scan - 0x2C];
    if (scan >= 0x02 && scan <= 0x0A) return (char)('1' + (scan - 0x02));
    if (scan == 0x0B) return '0';
    return 0;
}

static int _HandleAltMnemonic(HWND topWnd, char ch) {
    HWND list[MAX_WINDOWS];
    int count = 0, i;
    char target = (char)toupper((unsigned char)ch);
    if (!topWnd || !target || !g_windows) return 0;

    if (IS_MENU_HANDLE(g_windows[topWnd].hMenu) && g_menus) {
        int mIdx = (WORD)g_windows[topWnd].hMenu & 0xFF;
        if (g_menus[mIdx].used) {
            int itemX = g_windows[topWnd].rect.left + 6;
            HFONT defFont = (HFONT)0x0301;
            g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
            for (i = 0; i < g_menus[mIdx].count; i++) {
                char clean[40]; SIZE sz;
                char mch = _GetMnemonicChar(g_menus[mIdx].items[i].text);
                _StripAmpersand(g_menus[mIdx].items[i].text, clean, sizeof(clean));
                GetTextExtentPoint((HDC)1, clean, strlen(clean), &sz);
                if (mch == target) {
                    if (g_menus[mIdx].items[i].flags & MF_POPUP) {
                        TrackPopupMenu((HMENU)g_menus[mIdx].items[i].id, 0, itemX, g_windows[topWnd].rect.top + 36, 0, topWnd, NULL);
                    } else {
                        PostMessage(topWnd, WM_COMMAND, g_menus[mIdx].items[i].id, 0);
                    }
                    return 1;
                }
                itemX += sz.cx + 14;
            }
        }
    }

    _CollectControls(topWnd, list, &count, MAX_WINDOWS);
    for (i = 0; i < count; i++) {
        HWND ctrl = list[i];
        char mch = _GetMnemonicChar(g_windows[ctrl].text);
        if (mch == target) {
            SetFocus(ctrl);
            if (stricmp(g_windows[ctrl].className, "BUTTON") == 0 && g_windows[ctrl].parent) {
                PostMessage(g_windows[ctrl].parent, WM_COMMAND, (WPARAM)g_windows[ctrl].hMenu, MAKELONG(ctrl, BN_CLICKED));
            }
            return 1;
        }
    }
    return 0;
}
static HWND _FindDialogActionBtn(HWND topWnd, int isCancel) {
    HWND list[MAX_WINDOWS];
    int count = 0, i;
    if (!topWnd || !g_windows) return 0;
    _CollectControls(topWnd, list, &count, MAX_WINDOWS);
    for (i = 0; i < count; i++) {
        HWND c = list[i];
        if (stricmp(g_windows[c].className, "BUTTON") == 0) {
            int id = (int)g_windows[c].hMenu;
            if (!isCancel && (id == IDOK || (g_windows[c].style & BS_DEFPUSHBUTTON) || stricmp(g_windows[c].text, "OK") == 0 || stricmp(g_windows[c].text, "Apply") == 0 || stricmp(g_windows[c].text, "Select") == 0 || stricmp(g_windows[c].text, "Update Tags") == 0))
                return c;
            if (isCancel && (id == IDCANCEL || stricmp(g_windows[c].text, "Cancel") == 0))
                return c;
        }
    }
    return 0;
}

static void _ProcessKeyboardInput(int ch, int scan, BYTE biosFlags) {
    int shiftDown = (biosFlags & 0x03) ? 1 : 0;
    int ctrlDown  = (biosFlags & 0x04) ? 1 : 0;
    int altDown   = (biosFlags & 0x08) ? 1 : 0;
    HWND focus = (hwndFocus && hwndFocus <= g_windowCount && g_windows && g_windows[hwndFocus].isVisible) ? hwndFocus : hwndRoot;
    HWND topWnd = _GetTopLevelWindow(focus);
    INTERNAL_WND* fw = (focus && g_windows) ? &g_windows[focus] : NULL;
    LRESULT dlgCode = 0;

    if (!g_windows || !focus) return;

    /* Alt+F4 closes the active top-level window */
    if (ch == 0 && (scan == 0x6B || (altDown && scan == 0x3E))) {
        if (topWnd) {
            PostMessage(topWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
            PostMessage(topWnd, WM_CLOSE, 0, 0);
            DestroyWindow(topWnd);
        }
        return;
    }

    /* Alt+Tab cycles top-level windows */
    if ((ch == 0 && scan == 0xA5) || (altDown && (ch == 9 || scan == 0x0F))) {
        _CycleTopLevelWindow();
        return;
    }

    /* Alt+Down / Alt+Up or F4 toggles ComboBox dropdown */
    if (fw && stricmp(fw->className, "COMBOBOX") == 0) {
        if ((altDown && (scan == 0x50 || scan == 0x48)) || (ch == 0 && scan == 0x3E)) {
            g_dropdownWnd = (g_dropdownWnd == focus) ? 0 : focus;
            g_needsPaint = 1;
            return;
        }
    }

    /* Alt+Letter mnemonics */
    if (altDown && ch == 0) {
        char altCh = _ScanToAltChar(scan);
        if (altCh && _HandleAltMnemonic(topWnd, altCh)) return;
    } else if (altDown && ch >= 32 && ch < 127) {
        if (_HandleAltMnemonic(topWnd, (char)ch)) return;
    }

    /* F10 activates first menu bar item */
    if (ch == 0 && scan == 0x44 && topWnd && IS_MENU_HANDLE(g_windows[topWnd].hMenu) && g_menus) {
        int mIdx = (WORD)g_windows[topWnd].hMenu & 0xFF;
        if (g_menus[mIdx].used && g_menus[mIdx].count > 0) {
            if (g_menus[mIdx].items[0].flags & MF_POPUP)
                TrackPopupMenu((HMENU)g_menus[mIdx].items[0].id, 0, g_windows[topWnd].rect.left + 6, g_windows[topWnd].rect.top + 36, 0, topWnd, NULL);
            else
                PostMessage(topWnd, WM_COMMAND, g_menus[mIdx].items[0].id, 0);
            return;
        }
    }

    /* Dropdown open handling */
    if (g_dropdownWnd && g_dropdownWnd <= g_windowCount) {
        INTERNAL_WND* dw = &g_windows[g_dropdownWnd];
        if (ch == 27) {
            g_dropdownWnd = 0; g_needsPaint = 1; return;
        }
        if (ch == 13) {
            g_dropdownWnd = 0; g_needsPaint = 1;
            if (dw->parent) PostMessage(dw->parent, WM_COMMAND, (WPARAM)dw->hMenu, MAKELONG(focus, CBN_SELCHANGE));
            return;
        }
        if (ch == 0 && (scan == 0x48 || scan == 0x4B)) {
            if (dw->cbCurSel > 0) {
                dw->cbCurSel--; strcpy(dw->text, dw->cbItems[dw->cbCurSel]);
                g_needsPaint = 1;
                if (dw->parent) PostMessage(dw->parent, WM_COMMAND, (WPARAM)dw->hMenu, MAKELONG(g_dropdownWnd, CBN_SELCHANGE));
            }
            return;
        }
        if (ch == 0 && (scan == 0x50 || scan == 0x4D)) {
            if (dw->cbCurSel < dw->cbCount - 1) {
                dw->cbCurSel++; strcpy(dw->text, dw->cbItems[dw->cbCurSel]);
                g_needsPaint = 1;
                if (dw->parent) PostMessage(dw->parent, WM_COMMAND, (WPARAM)dw->hMenu, MAKELONG(g_dropdownWnd, CBN_SELCHANGE));
            }
            return;
        }
    }

    dlgCode = SendMessage(focus, WM_GETDLGCODE, 0, 0);

    /* Tab / Shift+Tab navigation */
    if (ch == 9 || (ch == 0 && scan == 0x0F)) {
        if (!(dlgCode & (DLGC_WANTTAB | DLGC_WANTALLKEYS))) {
            int dir = (shiftDown || (ch == 0 && scan == 0x0F)) ? -1 : 1;
            _CycleFocus(topWnd, dir);
            return;
        }
    }

    /* Extended keys (Arrows, Home, End, PgUp, PgDn, Delete, F-keys) */
    if (ch == 0 || ch == 0xE0) {
        UINT vk = 0;
        switch (scan) {
            case 0x48: vk = VK_UP; break;
            case 0x50: vk = VK_DOWN; break;
            case 0x4B: vk = VK_LEFT; break;
            case 0x4D: vk = VK_RIGHT; break;
            case 0x47: vk = VK_HOME; break;
            case 0x4F: vk = VK_END; break;
            case 0x49: vk = VK_PRIOR; break;
            case 0x51: vk = VK_NEXT; break;
            case 0x52: vk = VK_INSERT; break;
            case 0x53: vk = VK_DELETE; break;
            case 0x3B: vk = VK_F1; break;
            case 0x3C: vk = VK_F2; break;
            case 0x3D: vk = VK_F3; break;
            case 0x3E: vk = VK_F4; break;
            case 0x3F: vk = VK_F5; break;
        }

        if (fw && stricmp(fw->className, "LISTBOX") == 0) {
            int newSel = fw->lbCurSel;
            if (vk == VK_UP || vk == VK_LEFT) newSel--;
            else if (vk == VK_DOWN || vk == VK_RIGHT) newSel++;
            else if (vk == VK_HOME || vk == VK_PRIOR) newSel = 0;
            else if (vk == VK_END || vk == VK_NEXT) newSel = fw->lbCount - 1;
            if (newSel < 0) newSel = 0;
            if (newSel >= fw->lbCount) newSel = fw->lbCount - 1;
            if (newSel >= 0 && newSel != fw->lbCurSel) {
                fw->lbCurSel = newSel;
                g_needsPaint = 1;
                if (fw->parent) PostMessage(fw->parent, WM_COMMAND, (WPARAM)fw->hMenu, MAKELONG(focus, LBN_SELCHANGE));
            }
            return;
        }

        if (fw && stricmp(fw->className, "COMBOBOX") == 0) {
            int newSel = fw->cbCurSel;
            if (vk == VK_UP || vk == VK_LEFT) newSel--;
            else if (vk == VK_DOWN || vk == VK_RIGHT) newSel++;
            else if (vk == VK_HOME) newSel = 0;
            else if (vk == VK_END) newSel = fw->cbCount - 1;
            if (newSel >= 0 && newSel < fw->cbCount && newSel != fw->cbCurSel) {
                fw->cbCurSel = newSel;
                strcpy(fw->text, fw->cbItems[newSel]);
                g_needsPaint = 1;
                if (fw->parent) PostMessage(fw->parent, WM_COMMAND, (WPARAM)fw->hMenu, MAKELONG(focus, CBN_SELCHANGE));
                return;
            }
        }

        if (fw && stricmp(fw->className, "SCROLLBAR") == 0) {
            UINT sbCode = 0xFFFF;
            if (vk == VK_LEFT || vk == VK_UP) sbCode = SB_LINELEFT;
            else if (vk == VK_RIGHT || vk == VK_DOWN) sbCode = SB_LINERIGHT;
            else if (vk == VK_PRIOR) sbCode = SB_PAGELEFT;
            else if (vk == VK_NEXT) sbCode = SB_PAGERIGHT;
            if (sbCode != 0xFFFF && fw->parent) {
                PostMessage(fw->parent, WM_HSCROLL, sbCode, MAKELONG(fw->scrollHPos, focus));
                return;
            }
        }

        if (fw && stricmp(fw->className, "EDIT") == 0) {
            if (vk == VK_DELETE) {
                int len = strlen(fw->text);
                if (len > 0) {
                    fw->text[len - 1] = '\0';
                    g_needsPaint = 1;
                    if (fw->parent) PostMessage(fw->parent, WM_COMMAND, (WPARAM)fw->hMenu, MAKELONG(focus, EN_CHANGE));
                }
            }
            if (vk) PostMessage(focus, WM_KEYDOWN, vk, 0);
            return;
        }

        if (fw && stricmp(fw->className, "BUTTON") == 0) {
            if (vk == VK_RIGHT || vk == VK_DOWN) { _CycleFocus(topWnd, 1); return; }
            if (vk == VK_LEFT || vk == VK_UP) { _CycleFocus(topWnd, -1); return; }
        }

        if (vk) {
            PostMessage(focus, WM_KEYDOWN, vk, 0);
            if (focus != topWnd && topWnd && vk == VK_DELETE) {
                PostMessage(topWnd, WM_KEYDOWN, vk, 0);
            }
        }
        return;
    }

    /* Ctrl+Z shortcut */
    if (ch == 26 || (ctrlDown && (ch == 'z' || ch == 'Z'))) {
        PostMessage(topWnd ? topWnd : focus, WM_KEYDOWN, 'Z', 0);
        return;
    }

    /* Escape key */
    if (ch == 27) {
        PostMessage(focus, WM_KEYDOWN, VK_ESCAPE, 0);
        if (focus != topWnd && topWnd) {
            HWND cancelBtn = _FindDialogActionBtn(topWnd, 1);
            if (cancelBtn && g_windows[cancelBtn].parent) {
                PostMessage(g_windows[cancelBtn].parent, WM_COMMAND, (WPARAM)g_windows[cancelBtn].hMenu, MAKELONG(cancelBtn, BN_CLICKED));
            } else if (topWnd != hwndRoot) {
                DestroyWindow(topWnd);
            } else {
                PostMessage(topWnd, WM_KEYDOWN, VK_ESCAPE, 0);
            }
        } else if (topWnd && topWnd != hwndRoot) {
            DestroyWindow(topWnd);
        }
        return;
    }

    /* Enter key */
    if (ch == 13) {
        if (fw && stricmp(fw->className, "BUTTON") == 0) {
            if (fw->parent) PostMessage(fw->parent, WM_COMMAND, (WPARAM)fw->hMenu, MAKELONG(focus, BN_CLICKED));
            return;
        }
        if (fw && stricmp(fw->className, "LISTBOX") == 0) {
            if (fw->parent) PostMessage(fw->parent, WM_COMMAND, (WPARAM)fw->hMenu, MAKELONG(focus, LBN_DBLCLK));
            return;
        }
        if (fw && stricmp(fw->className, "EDIT") == 0) {
            if ((dlgCode & DLGC_WANTALLKEYS) || (fw->style & ES_WANTRETURN)) {
                if ((fw->style & ES_MULTILINE) && !(dlgCode & DLGC_WANTALLKEYS)) {
                    int len = strlen(fw->text);
                    if (len < 253) {
                        fw->text[len] = '\r'; fw->text[len + 1] = '\n'; fw->text[len + 2] = '\0';
                        g_needsPaint = 1;
                        if (fw->parent) PostMessage(fw->parent, WM_COMMAND, (WPARAM)fw->hMenu, MAKELONG(focus, EN_CHANGE));
                    }
                    return;
                }
                PostMessage(focus, WM_KEYDOWN, VK_RETURN, 0);
                return;
            }
            {
                HWND defBtn = _FindDialogActionBtn(topWnd, 0);
                if (defBtn && g_windows[defBtn].parent) {
                    PostMessage(g_windows[defBtn].parent, WM_COMMAND, (WPARAM)g_windows[defBtn].hMenu, MAKELONG(defBtn, BN_CLICKED));
                    return;
                }
            }
            PostMessage(focus, WM_KEYDOWN, VK_RETURN, 0);
            return;
        }
        if (topWnd && topWnd != hwndRoot) {
            HWND defBtn = _FindDialogActionBtn(topWnd, 0);
            if (defBtn && g_windows[defBtn].parent) {
                PostMessage(g_windows[defBtn].parent, WM_COMMAND, (WPARAM)g_windows[defBtn].hMenu, MAKELONG(defBtn, BN_CLICKED));
                return;
            }
        }
        PostMessage(focus, WM_KEYDOWN, VK_RETURN, 0);
        return;
    }

    /* Spacebar activates focused button */
    if (ch == ' ' && fw && stricmp(fw->className, "BUTTON") == 0) {
        if (fw->parent) PostMessage(fw->parent, WM_COMMAND, (WPARAM)fw->hMenu, MAKELONG(focus, BN_CLICKED));
        return;
    }

    /* Backspace */
    if (ch == 8) {
        if (fw && (stricmp(fw->className, "EDIT") == 0 || (stricmp(fw->className, "COMBOBOX") == 0 && (fw->style & CBS_DROPDOWN) && (fw->style & 0x0F) != CBS_DROPDOWNLIST))) {
            int len = strlen(fw->text);
            if (len > 0) {
                if (len >= 2 && fw->text[len - 2] == '\r' && fw->text[len - 1] == '\n') fw->text[len - 2] = '\0';
                else fw->text[len - 1] = '\0';
                g_needsPaint = 1;
                if (fw->parent) {
                    UINT notify = (stricmp(fw->className, "COMBOBOX") == 0) ? CBN_EDITCHANGE : EN_CHANGE;
                    PostMessage(fw->parent, WM_COMMAND, (WPARAM)fw->hMenu, MAKELONG(focus, notify));
                }
            }
            return;
        }
        PostMessage(focus, WM_KEYDOWN, VK_BACK, 0);
        return;
    }

    /* Printable characters */
    if (ch >= 32 && ch < 127) {
        if (fw && (stricmp(fw->className, "EDIT") == 0 || (stricmp(fw->className, "COMBOBOX") == 0 && (fw->style & 0x0F) == CBS_DROPDOWN))) {
            int len = strlen(fw->text);
            if (len < 254) {
                fw->text[len] = (char)ch;
                fw->text[len + 1] = '\0';
                g_needsPaint = 1;
                if (fw->parent) {
                    UINT notify = (stricmp(fw->className, "COMBOBOX") == 0) ? CBN_EDITCHANGE : EN_CHANGE;
                    PostMessage(fw->parent, WM_COMMAND, (WPARAM)fw->hMenu, MAKELONG(focus, notify));
                }
            }
            return;
        }
        if (fw && stricmp(fw->className, "LISTBOX") == 0) {
            int i;
            char target = (char)toupper((unsigned char)ch);
            for (i = 0; i < fw->lbCount; i++) {
                int idx = (fw->lbCurSel + 1 + i) % fw->lbCount;
                if (toupper((unsigned char)fw->lbItems[idx][0]) == target) {
                    fw->lbCurSel = idx;
                    g_needsPaint = 1;
                    if (fw->parent) PostMessage(fw->parent, WM_COMMAND, (WPARAM)fw->hMenu, MAKELONG(focus, LBN_SELCHANGE));
                    break;
                }
            }
            return;
        }
        PostMessage(focus, WM_CHAR, (WPARAM)ch, 0);
        PostMessage(focus, WM_KEYDOWN, (WPARAM)toupper(ch), 0);
    }
}

static LRESULT CALLBACK DefWindowProc(HWND hw, UINT m, WPARAM w, LPARAM l) {
    INTERNAL_WND* wnd;
    if (!g_windows || hw == 0 || hw > g_windowCount) return 0;
    wnd = &g_windows[hw];
    if (m == CB_ADDSTRING) {
        if (wnd->cbCount < 16 && l) {
            strncpy(wnd->cbItems[wnd->cbCount], (char FAR*)l, 31);
            wnd->cbItems[wnd->cbCount][31] = '\0';
            wnd->cbCount++;
        }
        return wnd->cbCount - 1;
    }
    if (m == CB_SETCURSEL) {
        wnd->cbCurSel = (int)w;
        if ((int)w >= 0 && (int)w < wnd->cbCount) strcpy(wnd->text, wnd->cbItems[(int)w]);
        if (!g_dragWnd) g_needsPaint = 1;
        return w;
    }
    if (m == CB_GETCURSEL) return wnd->cbCurSel;
    if (m == CB_GETCOUNT) return wnd->cbCount;
    if (m == CB_GETLBTEXT) {
        if ((int)w >= 0 && (int)w < wnd->cbCount && l) {
            strcpy((char FAR*)l, wnd->cbItems[(int)w]);
            return strlen((char FAR*)l);
        }
        return CB_ERR;
    }
    if (m == LB_ADDSTRING) {
        if (wnd->lbCount < 20 && l) {
            strncpy(wnd->lbItems[wnd->lbCount], (char FAR*)l, 47);
            wnd->lbItems[wnd->lbCount][47] = '\0';
            wnd->lbData[wnd->lbCount] = 0;
            wnd->lbCount++;
            if (!g_dragWnd) g_needsPaint = 1;
            return wnd->lbCount - 1;
        }
        return LB_ERR;
    }
    if (m == LB_RESETCONTENT) {
        wnd->lbCount = 0;
        wnd->lbCurSel = -1;
        if (!g_dragWnd) g_needsPaint = 1;
        return 0;
    }
    if (m == LB_GETCOUNT) return wnd->lbCount;
    if (m == LB_SETCURSEL) {
        if ((int)w >= -1 && (int)w < wnd->lbCount) {
            wnd->lbCurSel = (int)w;
            if (!g_dragWnd) g_needsPaint = 1;
            return w;
        }
        return LB_ERR;
    }
    if (m == LB_GETCURSEL) return (wnd->lbCount > 0) ? wnd->lbCurSel : LB_ERR;
    if (m == LB_SETITEMDATA) {
        if ((int)w >= 0 && (int)w < wnd->lbCount) {
            wnd->lbData[(int)w] = (DWORD)l;
            return 0;
        }
        return LB_ERR;
    }
    if (m == LB_GETITEMDATA) {
        if ((int)w >= 0 && (int)w < wnd->lbCount) return (LRESULT)wnd->lbData[(int)w];
        return LB_ERR;
    }
    if (m == LB_GETTOPINDEX) return 0;
    if (m == LB_GETITEMRECT) {
        LPRECT prc = (LPRECT)l;
        if (prc) {
            int idx = (int)w;
            prc->left = 0; prc->top = idx * 16; prc->right = 100; prc->bottom = (idx + 1) * 16;
        }
        return TRUE;
    }
    if (m == LB_SETSEL || m == LB_SETCARETINDEX) return 0;
    if (m == LB_GETCARETINDEX) return 0;
    if (m == LB_GETSEL) return 1;
    if (m == EM_SETSEL) return 0;
    return 0;
}

static LRESULT SendMessage(HWND hw, UINT m, WPARAM w, LPARAM l) {
    if (hw != 0 && hw <= g_windowCount && g_windows && g_windows[hw].wndProc)
        return g_windows[hw].wndProc(hw, m, w, l);
    return 0;
}

static void PostMessage(HWND hw, UINT m, WPARAM w, LPARAM l) {
    if (g_msgQueue) {
        g_msgQueue[g_msgTail].hwnd = hw; g_msgQueue[g_msgTail].message = m;
        g_msgQueue[g_msgTail].wParam = w; g_msgQueue[g_msgTail].lParam = l;
        g_msgTail = (g_msgTail + 1) % MAX_MSG;
    }
}

static void PostQuitMessage(int n) { PostMessage(0, WM_QUIT, n, 0); }

static BOOL GetClientRect(HWND hw, LPRECT r) {
    if (hw != 0 && hw <= g_windowCount && g_windows && r) {
        r->left = 0;
        r->top = 0;
        r->right = g_windows[hw].client.right - g_windows[hw].client.left;
        r->bottom = g_windows[hw].client.bottom - g_windows[hw].client.top;
        return TRUE;
    }
    return FALSE;
}

static BOOL InvalidateRect(HWND hw, const RECT FAR* r, BOOL b) {
    (void)hw; (void)r; (void)b;
    if (!g_dragWnd) g_needsPaint = 1;
    return TRUE;
}

static void _ShiftWindow(HWND hw, int dx, int dy) {
    INTERNAL_WND* w;
    HWND child;
    if (!g_windows || hw == 0 || hw > g_windowCount) return;
    w = &g_windows[hw];
    child = w->child;
    w->rect.left += dx; w->rect.right += dx; w->rect.top += dy; w->rect.bottom += dy;
    while (child) { _ShiftWindow(child, dx, dy); child = g_windows[child].next; }
}

static BOOL MoveWindow(HWND hw, int x, int y, int w, int h, BOOL rpt) {
    INTERNAL_WND* wnd;
    int dx, dy;
    HWND child;
    if (!g_windows || hw == 0 || hw > g_windowCount) return FALSE;
    wnd = &g_windows[hw];
    if (strcmp(wnd->className, "COMBOBOX") == 0) h = 22;
    if (wnd->parent && wnd->parent <= g_windowCount) {
        x += g_windows[wnd->parent].rect.left + g_windows[wnd->parent].client.left;
        y += g_windows[wnd->parent].rect.top + g_windows[wnd->parent].client.top;
    }
    dx = x - wnd->rect.left; dy = y - wnd->rect.top; child = wnd->child;
    wnd->rect.left = x; wnd->rect.top = y; wnd->rect.right = x + w; wnd->rect.bottom = y + h;
    wnd->client.left = 0; wnd->client.top = 0; wnd->client.right = w; wnd->client.bottom = h;
    if (((wnd->style & WS_CAPTION) == WS_CAPTION) || (!wnd->parent && (wnd->style & WS_CAPTION))) {
        wnd->client.top += 18; wnd->client.left += 2; wnd->client.right -= 2; wnd->client.bottom -= 2;
        if (IS_MENU_HANDLE(wnd->hMenu)) wnd->client.top += 18;
    }
    while (child) { _ShiftWindow(child, dx, dy); child = g_windows[child].next; }
    
    SendMessage(hw, WM_SIZE, 0, MAKELONG(wnd->client.right - wnd->client.left, wnd->client.bottom - wnd->client.top));
    
    if (rpt && !g_dragWnd) g_needsPaint = 1;
    return TRUE;
}
static BOOL ShowWindow(HWND hw, int s) {
    if (hw != 0 && hw <= g_windowCount && g_windows) {
        g_windows[hw].isVisible = (s == SW_HIDE) ? 0 : 1;
        if (!g_dragWnd) g_needsPaint = 1;
        return TRUE;
    }
    return FALSE;
}

static BOOL UpdateWindow(HWND hw) { SendMessage(hw, WM_PAINT, 0, 0); g_needsPaint = 1; return TRUE; }

static HWND SetFocus(HWND hw) {
    HWND old = hwndFocus;
    if (old == hw) return old;
    hwndFocus = hw;
    if (old && old <= g_windowCount && g_windows) {
        SendMessage(old, WM_KILLFOCUS, (WPARAM)hw, 0);
        if (g_windows[old].parent) {
            if (stricmp(g_windows[old].className, "EDIT") == 0)
                SendMessage(g_windows[old].parent, WM_COMMAND, (WPARAM)g_windows[old].hMenu, MAKELONG(old, EN_KILLFOCUS));
            else if (stricmp(g_windows[old].className, "COMBOBOX") == 0)
                SendMessage(g_windows[old].parent, WM_COMMAND, (WPARAM)g_windows[old].hMenu, MAKELONG(old, CBN_KILLFOCUS));
            else if (stricmp(g_windows[old].className, "LISTBOX") == 0)
                SendMessage(g_windows[old].parent, WM_COMMAND, (WPARAM)g_windows[old].hMenu, MAKELONG(old, LBN_KILLFOCUS));
        }
    }
    if (hw && hw <= g_windowCount && g_windows) {
        SendMessage(hw, WM_SETFOCUS, (WPARAM)old, 0);
        if (g_windows[hw].parent) {
            if (stricmp(g_windows[hw].className, "EDIT") == 0)
                SendMessage(g_windows[hw].parent, WM_COMMAND, (WPARAM)g_windows[hw].hMenu, MAKELONG(hw, EN_SETFOCUS));
            else if (stricmp(g_windows[hw].className, "COMBOBOX") == 0)
                SendMessage(g_windows[hw].parent, WM_COMMAND, (WPARAM)g_windows[hw].hMenu, MAKELONG(hw, CBN_SETFOCUS));
            else if (stricmp(g_windows[hw].className, "LISTBOX") == 0)
                SendMessage(g_windows[hw].parent, WM_COMMAND, (WPARAM)g_windows[hw].hMenu, MAKELONG(hw, LBN_SETFOCUS));
        }
    }
    g_needsPaint = 1;
    return old;
}

static HWND GetFocus(void) { return hwndFocus; }
static HWND GetActiveWindow(void) { return _GetTopLevelWindow(hwndFocus ? hwndFocus : hwndRoot); }

static BOOL EnumChildWindows(HWND p, WNDENUMPROC proc, LPARAM l) {
    HWND c;
    if (!g_windows || p == 0 || p > g_windowCount) return FALSE;
    c = g_windows[p].child;
    while (c) { proc(c, l); c = g_windows[c].next; }
    return TRUE;
}

static int GetWindowText(HWND hw, LPSTR str, int max) {
    if (hw != 0 && hw <= g_windowCount && g_windows && str) {
        strncpy(str, g_windows[hw].text, max);
        if (max > 0) str[max - 1] = '\0';
        return strlen(str);
    }
    return 0;
}

static BOOL SetWindowText(HWND hw, LPCSTR str) {
    if (hw != 0 && hw <= g_windowCount && g_windows && str) {
        strncpy(g_windows[hw].text, str, 255);
        g_windows[hw].text[255] = '\0';
        if (!g_dragWnd) g_needsPaint = 1;
        return TRUE;
    }
    return FALSE;
}

static BOOL IsRectEmpty(const RECT FAR* r) { return (!r || r->left >= r->right || r->top >= r->bottom); }
static BOOL SetRectEmpty(LPRECT r) { if (r) r->left = r->right = r->top = r->bottom = 0; return TRUE; }
static int MessageBox(HWND hw, LPCSTR t, LPCSTR c, UINT ty) { (void)hw; (void)t; (void)c; (void)ty; return IDOK; }
static void MessageBeep(UINT uType) { (void)uType; }

static void DragAcceptFiles(HWND hWnd, BOOL fAccept) { (void)hWnd; (void)fAccept; }
static UINT DragQueryFile(HDROP hDrop, UINT iFile, LPSTR lpszFile, UINT cch) {
    (void)hDrop; (void)iFile;
    if (lpszFile && cch > 0) lpszFile[0] = '\0';
    return 0;
}
static void DragFinish(HDROP hDrop) { (void)hDrop; }

static HDWP BeginDeferWindowPos(int nNumWindows) { (void)nNumWindows; return (HDWP)1; }
static HDWP DeferWindowPos(HDWP hWinPosInfo, HWND hWnd, HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags) {
    SetWindowPos(hWnd, hWndInsertAfter, x, y, cx, cy, uFlags);
    return hWinPosInfo;
}
static BOOL EndDeferWindowPos(HDWP hWinPosInfo) { (void)hWinPosInfo; return TRUE; }

static int SetScrollRange(HWND hw, int bar, int min, int max, BOOL draw) {
    (void)draw;
    if (hw != 0 && hw <= g_windowCount && g_windows) {
        if (bar == SB_VERT) { g_windows[hw].scrollVMin = min; g_windows[hw].scrollVMax = max; }
        else { g_windows[hw].scrollHMin = min; g_windows[hw].scrollHMax = max; }
    }
    return 1;
}

static int SetScrollPos(HWND hw, int bar, int pos, BOOL draw) {
    if (hw != 0 && hw <= g_windowCount && g_windows) {
        if (bar == SB_VERT) g_windows[hw].scrollVPos = pos;
        else g_windows[hw].scrollHPos = pos;
        if (draw && !g_dragWnd) g_needsPaint = 1;
    }
    return pos;
}

static int GetScrollPos(HWND hw, int bar) {
    if (hw != 0 && hw <= g_windowCount && g_windows) {
        if (bar == SB_VERT) return g_windows[hw].scrollVPos;
        return g_windows[hw].scrollHPos;
    }
    return 0;
}

static ATOM RegisterClass(const WNDCLASS FAR* lpwc) {
    if (g_classes && g_classCount < 16 && lpwc) {
        g_classes[g_classCount] = *lpwc;
        return ++g_classCount;
    }
    return 0;
}

static HCURSOR LoadCursor(HINSTANCE h, LPCSTR name) { (void)h; (void)name; return (HCURSOR)1; }

static HWND CreateWindow(LPCSTR cls, LPCSTR name, DWORD style, int x, int y, int w, int h, HWND p, HMENU m, HINSTANCE inst, LPVOID param) {
    INTERNAL_WND* wnd; int i; WNDPROC proc = DefWindowProc; HBRUSH bg = (HBRUSH)7;
    HWND newHwnd;
    (void)inst; (void)param;
    if (!g_windows || g_windowCount >= MAX_WINDOWS - 1) return 0;

    if (x == CW_USEDEFAULT) { x = 20; y = 20; }
    if (w == CW_USEDEFAULT || w < 0) w = 300;
    if (h == CW_USEDEFAULT || h < 0) h = 200;
    if (w > 640) w = 640; if (h > 480) h = 480;
    if (strcmp(cls, "COMBOBOX") == 0) h = 22;

    if (g_classes) {
        for (i = 0; i < g_classCount; i++) {
            if (g_classes[i].lpszClassName && stricmp(g_classes[i].lpszClassName, cls) == 0) {
                proc = g_classes[i].lpfnWndProc;
                if (g_classes[i].hbrBackground) bg = g_classes[i].hbrBackground;
            }
        }
    }

    newHwnd = (HWND)(++g_windowCount);
    wnd = &g_windows[newHwnd];
    wnd->wndProc = proc; wnd->style = style; wnd->parent = p; wnd->hMenu = m;
    wnd->isVisible = (style & WS_VISIBLE) ? 1 : 0;
    strncpy(wnd->className, cls, 31); wnd->className[31] = '\0';
    if (name) { strncpy(wnd->text, name, 255); wnd->text[255] = '\0'; } else wnd->text[0] = '\0';
    wnd->hbrBg = bg;
    wnd->child = 0; wnd->next = 0;
    wnd->cbCount = 0; wnd->cbCurSel = 0;
    wnd->lbCount = 0; wnd->lbCurSel = 0;
    wnd->isMaximized = 0;
    wnd->scrollVMin = wnd->scrollVMax = wnd->scrollVPos = 0;
    wnd->scrollHMin = wnd->scrollHMax = wnd->scrollHPos = 0;

    if (p && p <= g_windowCount && (style & WS_CHILD)) {
        x += g_windows[p].client.left + g_windows[p].rect.left;
        y += g_windows[p].client.top + g_windows[p].rect.top;
        if (!g_windows[p].child) g_windows[p].child = newHwnd;
        else { HWND sib = g_windows[p].child; while (g_windows[sib].next) sib = g_windows[sib].next; g_windows[sib].next = newHwnd; }
    } else {
        wnd->parent = 0;
        if (!hwndRoot) hwndRoot = newHwnd;
    }

    wnd->rect.left = x; wnd->rect.top = y; wnd->rect.right = x + w; wnd->rect.bottom = y + h;
    wnd->client.left = 0; wnd->client.top = 0; wnd->client.right = w; wnd->client.bottom = h;
    if (((style & WS_CAPTION) == WS_CAPTION) || (!wnd->parent && (style & WS_CAPTION))) {
        wnd->client.top += 18; wnd->client.left += 2; wnd->client.right -= 2; wnd->client.bottom -= 2;
        if (IS_MENU_HANDLE(m)) wnd->client.top += 18;
    }

    SendMessage(newHwnd, WM_CREATE, 0, 0);
    if (!wnd->parent && w > 0 && h > 0) {
        SendMessage(newHwnd, WM_SIZE, 0, MAKELONG(wnd->client.right - wnd->client.left, wnd->client.bottom - wnd->client.top));
        if (wnd->isVisible) {
            HWND list[MAX_WINDOWS];
            int count = 0;
            _CollectControls(newHwnd, list, &count, MAX_WINDOWS);
            if (count > 0 && newHwnd != hwndRoot) hwndFocus = list[0];
            else hwndFocus = newHwnd;
        }
    }
    return newHwnd;
}

static HWND CreateWindowEx(DWORD exStyle, LPCSTR cls, LPCSTR name, DWORD style, int x, int y, int w, int h, HWND p, HMENU m, HINSTANCE inst, LPVOID param) {
    (void)exStyle;
    return CreateWindow(cls, name, style, x, y, w, h, p, m, inst, param);
}

static HMENU _AllocMenu(void) {
    int i;
    if (!g_menus) {
        g_menus = (INTERNAL_MENU FAR*)halloc(MAX_MENUS * sizeof(INTERNAL_MENU), 1);
        if (!g_menus) return 0;
    }
    for (i = 1; i < MAX_MENUS; i++) {
        if (!g_menus[i].used) {
            g_menus[i].used = 1;
            g_menus[i].count = 0;
            return (HMENU)(MENU_HANDLE_BASE | i);
        }
    }
    return 0;
}

static HMENU CreateMenu(void) { return _AllocMenu(); }
static HMENU CreatePopupMenu(void) { return _AllocMenu(); }

static BOOL DestroyMenu(HMENU hMenu) {
    if (IS_MENU_HANDLE(hMenu) && g_menus) {
        int idx = (WORD)hMenu & 0xFF;
        g_menus[idx].used = 0;
        g_menus[idx].count = 0;
        return TRUE;
    }
    return FALSE;
}

static BOOL AppendMenu(HMENU hMenu, UINT uFlags, UINT uIDNewItem, LPCSTR lpNewItem) {
    if (IS_MENU_HANDLE(hMenu) && g_menus) {
        int idx = (WORD)hMenu & 0xFF;
        INTERNAL_MENU* m = &g_menus[idx];
        if (m->used && m->count < MAX_MENU_ITEMS) {
            INTERNAL_MENU_ITEM* it = &m->items[m->count++];
            it->flags = uFlags;
            it->id = uIDNewItem;
            it->itemData = (DWORD)lpNewItem;
            if (!(uFlags & (MF_SEPARATOR | MF_OWNERDRAW)) && lpNewItem) {
                strncpy(it->text, lpNewItem, 39);
                it->text[39] = '\0';
            } else {
                it->text[0] = '\0';
            }
            return TRUE;
        }
    }
    return FALSE;
}

static HMENU GetSubMenu(HMENU hMenu, int nPos) {
    if (IS_MENU_HANDLE(hMenu) && g_menus) {
        int idx = (WORD)hMenu & 0xFF;
        if (g_menus[idx].used && nPos >= 0 && nPos < g_menus[idx].count) {
            return (HMENU)g_menus[idx].items[nPos].id;
        }
    }
    return 0;
}

static DWORD CheckMenuItem(HMENU hMenu, UINT uIDCheckItem, UINT uCheck) {
    if (IS_MENU_HANDLE(hMenu) && g_menus) {
        int idx = (WORD)hMenu & 0xFF, i;
        INTERNAL_MENU* m = &g_menus[idx];
        if (m->used) {
            for (i = 0; i < m->count; i++) {
                if ((uCheck & MF_BYPOSITION) ? ((UINT)i == uIDCheckItem) : (m->items[i].id == uIDCheckItem)) {
                    DWORD prev = m->items[i].flags & MF_CHECKED;
                    if (uCheck & MF_CHECKED) m->items[i].flags |= MF_CHECKED;
                    else m->items[i].flags &= ~MF_CHECKED;
                    return prev;
                }
            }
        }
    }
    return 0;
}

static BOOL EnableMenuItem(HMENU hMenu, UINT uIDEnableItem, UINT uEnable) {
    if (IS_MENU_HANDLE(hMenu) && g_menus) {
        int idx = (WORD)hMenu & 0xFF, i;
        INTERNAL_MENU* m = &g_menus[idx];
        if (m->used) {
            for (i = 0; i < m->count; i++) {
                if ((uEnable & MF_BYPOSITION) ? ((UINT)i == uIDEnableItem) : (m->items[i].id == uIDEnableItem)) {
                    if (uEnable & (MF_GRAYED | MF_DISABLED)) m->items[i].flags |= MF_GRAYED;
                    else m->items[i].flags &= ~MF_GRAYED;
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

static BOOL SetMenu(HWND hWnd, HMENU hMenu) {
    if (hWnd != 0 && hWnd <= g_windowCount && g_windows) {
        INTERNAL_WND* w = &g_windows[hWnd];
        w->hMenu = hMenu;
        if ((w->style & WS_CAPTION) == WS_CAPTION) {
            int oldTop = w->client.top;
            int newTop = 18 + (IS_MENU_HANDLE(hMenu) ? 18 : 0);
            int dy = newTop - oldTop;
            w->client.top = newTop;
            if (dy != 0) {
                HWND c = w->child;
                while (c) { _ShiftWindow(c, 0, dy); c = g_windows[c].next; }
            }
        }
        g_needsPaint = 1;
        return TRUE;
    }
    return FALSE;
}

static BOOL DrawMenuBar(HWND hWnd) { (void)hWnd; g_needsPaint = 1; return TRUE; }

static void _MeasurePopupMenu(INTERNAL_MENU* m, HWND hWnd, int* outW, int* outH, int* itemHeights) {
    int i, maxW = 90, totalH = 4;
    HFONT defFont = (HFONT)0x0301;
    g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
    for (i = 0; i < m->count; i++) {
        INTERNAL_MENU_ITEM* it = &m->items[i];
        if (it->flags & MF_SEPARATOR) {
            itemHeights[i] = 6;
            totalH += 6;
        } else if (it->flags & MF_OWNERDRAW) {
            MEASUREITEMSTRUCT mis;
            mis.CtlType = ODT_MENU; mis.CtlID = 0; mis.itemID = it->id;
            mis.itemWidth = 90; mis.itemHeight = 18; mis.itemData = it->itemData;
            if (hWnd) SendMessage(hWnd, WM_MEASUREITEM, 0, (LPARAM)(LPMEASUREITEMSTRUCT)&mis);
            if ((int)mis.itemWidth + 16 > maxW) maxW = (int)mis.itemWidth + 16;
            itemHeights[i] = (mis.itemHeight > 0) ? (int)mis.itemHeight : 18;
            totalH += itemHeights[i];
        } else {
            char clean[40]; SIZE sz;
            _StripAmpersand(it->text, clean, sizeof(clean));
            GetTextExtentPoint((HDC)1, clean, strlen(clean), &sz);
            if (sz.cx + 32 > maxW) maxW = sz.cx + 32;
            itemHeights[i] = 18;
            totalH += 18;
        }
    }
    *outW = maxW;
    *outH = totalH;
}

static void _DrawPopupMenuBox(INTERNAL_MENU* m, HWND hWnd, int x, int y, int w, int h, int* itemHeights, int hoverIdx) {
    RECT rc; int i, curY = y + 2;
    HFONT defFont = (HFONT)0x0301;
    rc.left = x; rc.top = y; rc.right = x + w; rc.bottom = y + h;
    g_dcs[1].hwnd = 0;
    g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
    _FillRect(&g_dcs[1], &rc, 7);
    _Draw3DBorder(&g_dcs[1], &rc, 15, 0);

    for (i = 0; i < m->count; i++) {
        INTERNAL_MENU_ITEM* it = &m->items[i];
        int ih = itemHeights[i];
        RECT ir;
        ir.left = x + 2; ir.top = curY; ir.right = x + w - 2; ir.bottom = curY + ih;
        if (it->flags & MF_SEPARATOR) {
            RECT sep;
            sep.left = ir.left + 2; sep.right = ir.right - 2;
            sep.top = curY + 2; sep.bottom = curY + 3;
            _FillRect(&g_dcs[1], &sep, 8);
            sep.top = curY + 3; sep.bottom = curY + 4;
            _FillRect(&g_dcs[1], &sep, 15);
        } else if (it->flags & MF_OWNERDRAW) {
            DRAWITEMSTRUCT dis;
            dis.CtlType = ODT_MENU; dis.CtlID = 0; dis.itemID = it->id;
            dis.itemAction = 1;
            dis.itemState = (i == hoverIdx) ? ODS_SELECTED : 0;
            dis.hwndItem = (HWND)0;
            dis.hDC = (HDC)1;
            dis.rcItem = ir;
            dis.itemData = it->itemData;
            g_dcs[1].hwnd = 0;
            g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
            if (hWnd) SendMessage(hWnd, WM_DRAWITEM, 0, (LPARAM)(LPDRAWITEMSTRUCT)&dis);
            g_dcs[1].hwnd = 0;
            g_dcs[1].clip.left = 0; g_dcs[1].clip.top = 0; g_dcs[1].clip.right = 640; g_dcs[1].clip.bottom = 480;
        } else {
            char clean[40];
            BYTE fg = (it->flags & MF_GRAYED) ? 8 : ((i == hoverIdx) ? 15 : 0);
            _FillRect(&g_dcs[1], &ir, (i == hoverIdx) ? 1 : 7);
            g_dcs[1].hwnd = 0; g_dcs[1].hFont = defFont; g_dcs[1].fontScale = 1;
            g_dcs[1].textColor = fg; g_dcs[1].bkMode = 1;
            _StripAmpersand(it->text, clean, sizeof(clean));
            if (it->flags & MF_CHECKED) TextOut((HDC)1, ir.left + 3, curY + 2, "*", 1);
            TextOut((HDC)1, ir.left + 14, curY + 2, clean, strlen(clean));
            if (it->flags & MF_POPUP) TextOut((HDC)1, ir.right - 10, curY + 2, ">", 1);
        }
        curY += ih;
    }
}

static BOOL TrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT FAR* prcRect) {
    INTERNAL_MENU* m;
    int menuW, menuH, itemHeights[MAX_MENU_ITEMS];
    int hover = -1, lastHover = -2, subHover = -1, lastSubHover = -2;
    INTERNAL_MENU* subMenu = NULL;
    int subX = 0, subY = 0, subW = 0, subH = 0, subHeights[MAX_MENU_ITEMS];
    union REGS r;
    int waitRelease = 1;
    (void)nReserved; (void)prcRect;

    if (!IS_MENU_HANDLE(hMenu) || !g_menus) return FALSE;
    m = &g_menus[(WORD)hMenu & 0xFF];
    if (!m->used || m->count <= 0) return FALSE;

    if (hWnd) SendMessage(hWnd, WM_INITMENUPOPUP, (WPARAM)hMenu, 0);

    _MeasurePopupMenu(m, hWnd, &menuW, &menuH, itemHeights);
    if (x + menuW > 640) x = 640 - menuW;
    if (x < 0) x = 0;
    if (y + menuH > 480) y = (y - menuH >= 0) ? (y - menuH) : (480 - menuH);
    if (y < 0) y = 0;

    _RepaintAll();
    _DrawPopupMenuBox(m, hWnd, x, y, menuW, menuH, itemHeights, -1);

    for (;;) {
        int mx, my, btns, i, curY;
        if (kbhit()) {
            int k = getch();
            if (k == 0 || k == 0xE0) {
                int sc = getch();
                if (sc == 0x48) { hover = (hover <= 0) ? (m->count - 1) : (hover - 1); }
                else if (sc == 0x50) { hover = (hover + 1) % m->count; }
            } else if (k == 27) {
                break;
            } else if (k == 13 && hover >= 0) {
                INTERNAL_MENU_ITEM* it = &m->items[hover];
                if (!(it->flags & (MF_SEPARATOR | MF_GRAYED | MF_POPUP))) {
                    g_needsPaint = 1;
                    if (!(uFlags & TPM_RETURNCMD) && hWnd) PostMessage(hWnd, WM_COMMAND, it->id, 0);
                    return (uFlags & TPM_RETURNCMD) ? (BOOL)it->id : TRUE;
                }
            }
        }

        r.w.ax = 0x03; int86(0x33, &r, &r);
        mx = r.w.cx; my = r.w.dx; btns = r.w.bx & 3;
        if (waitRelease) {
            if (btns == 0) waitRelease = 0;
            continue;
        }

        if (mx >= x && mx < x + menuW && my >= y + 2 && my < y + menuH - 2) {
            curY = y + 2;
            for (i = 0; i < m->count; i++) {
                if (my >= curY && my < curY + itemHeights[i]) { hover = i; break; }
                curY += itemHeights[i];
            }
        }

        subHover = -1;
        if (subMenu && mx >= subX && mx < subX + subW && my >= subY + 2 && my < subY + subH - 2) {
            curY = subY + 2;
            for (i = 0; i < subMenu->count; i++) {
                if (my >= curY && my < curY + subHeights[i]) { subHover = i; break; }
                curY += subHeights[i];
            }
        }

        if (hover != lastHover) {
            subMenu = NULL;
            if (hover >= 0 && (m->items[hover].flags & MF_POPUP) && IS_MENU_HANDLE(m->items[hover].id)) {
                subMenu = &g_menus[(WORD)m->items[hover].id & 0xFF];
                if (subMenu->used && subMenu->count > 0) {
                    int itemTop = y + 2;
                    for (i = 0; i < hover; i++) itemTop += itemHeights[i];
                    _MeasurePopupMenu(subMenu, hWnd, &subW, &subH, subHeights);
                    subX = (x + menuW + subW <= 640) ? (x + menuW - 2) : (x - subW + 2);
                    subY = (itemTop + subH <= 480) ? itemTop : (480 - subH);
                } else subMenu = NULL;
            }
            _RepaintAll();
            _DrawPopupMenuBox(m, hWnd, x, y, menuW, menuH, itemHeights, hover);
            if (subMenu) _DrawPopupMenuBox(subMenu, hWnd, subX, subY, subW, subH, subHeights, subHover);
            lastHover = hover;
            lastSubHover = subHover;
        } else if (subMenu && subHover != lastSubHover) {
            _DrawPopupMenuBox(subMenu, hWnd, subX, subY, subW, subH, subHeights, subHover);
            lastSubHover = subHover;
        }

        if (btns) {
            UINT chosenId = 0;
            int validPick = 0;
            while (r.w.bx & 3) { r.w.ax = 0x03; int86(0x33, &r, &r); }
            if (subMenu && subHover >= 0) {
                INTERNAL_MENU_ITEM* sit = &subMenu->items[subHover];
                if (!(sit->flags & (MF_SEPARATOR | MF_GRAYED | MF_POPUP))) {
                    chosenId = sit->id; validPick = 1;
                }
            } else if (hover >= 0) {
                INTERNAL_MENU_ITEM* it = &m->items[hover];
                if (it->flags & MF_POPUP) continue;
                if (!(it->flags & (MF_SEPARATOR | MF_GRAYED))) {
                    chosenId = it->id; validPick = 1;
                }
            }
            g_needsPaint = 1;
            if (validPick) {
                if (!(uFlags & TPM_RETURNCMD) && hWnd) PostMessage(hWnd, WM_COMMAND, chosenId, 0);
                return (uFlags & TPM_RETURNCMD) ? (BOOL)chosenId : TRUE;
            }
            break;
        }
    }
    g_needsPaint = 1;
    return FALSE;
}

static void _HideSubtree(HWND hw) {
    HWND c;
    if (!g_windows || hw == 0 || hw > g_windowCount) return;
    g_windows[hw].isVisible = 0;
    c = g_windows[hw].child;
    while (c) {
        _HideSubtree(c);
        c = g_windows[c].next;
    }
}

static BOOL DestroyWindow(HWND hw) {
    if (hw != 0 && hw <= g_windowCount && g_windows) {
        _HideSubtree(hw);
        if (g_dropdownWnd == hw) g_dropdownWnd = 0;
        if (_GetTopLevelWindow(hwndFocus) == hw) {
            hwndFocus = hwndRoot;
        }
        SendMessage(hw, WM_DESTROY, 0, 0);
        if (hw == hwndRoot) PostQuitMessage(0);
        g_needsPaint = 1;
        return TRUE;
    }
    return FALSE;
}

static BOOL ExitWindows(DWORD dwReserved, UINT uReturnCode) {
    (void)dwReserved; (void)uReturnCode;
    PostQuitMessage(0);
    return TRUE;
}

static BOOL EnableWindow(HWND hw, BOOL bEnable) {
    if (hw != 0 && hw <= g_windowCount && g_windows) {
        if (bEnable) g_windows[hw].style &= ~WS_DISABLED;
        else g_windows[hw].style |= WS_DISABLED;
        return TRUE;
    }
    return FALSE;
}

static BOOL EnumWindows(WNDENUMPROC proc, LPARAM l) {
    int i;
    if (!g_windows) return FALSE;
    for (i = 1; i <= g_windowCount; i++) {
        if (g_windows[i].isVisible && !g_windows[i].parent) {
            if (!proc((HWND)i, l)) break;
        }
    }
    return TRUE;
}

static BOOL FreeLibrary(HINSTANCE h) { (void)h; return TRUE; }

static SHORT GetKeyState(int nVirtKey) {
    BYTE biosFlags = *((volatile BYTE far*)0x00400017L);
    if (nVirtKey == VK_SHIFT)   return (biosFlags & 0x03) ? (SHORT)0x8000 : 0;
    if (nVirtKey == VK_CONTROL) return (biosFlags & 0x04) ? (SHORT)0x8000 : 0;
    if (nVirtKey == VK_MENU)    return (biosFlags & 0x08) ? (SHORT)0x8000 : 0;
    return 0;
}

static SHORT GetAsyncKeyState(int vKey) { return GetKeyState(vKey); }

static BOOL GetCursorPos(LPPOINT lpPoint) {
    union REGS r;
    if (!lpPoint) return FALSE;
    r.w.ax = 0x03; int86(0x33, &r, &r);
    lpPoint->x = r.w.cx; lpPoint->y = r.w.dx;
    return TRUE;
}

static HGLOBAL GlobalFree(HGLOBAL hMem) {
    if (hMem != 0) hfree((void __huge*)((DWORD)(WORD)hMem << 16));
    return 0;
}

static BOOL GlobalUnlock(HGLOBAL hMem) { (void)hMem; return TRUE; }
static LPVOID GlobalLock(HGLOBAL hMem) { return (LPVOID)((DWORD)(WORD)hMem << 16); }

static HGLOBAL GlobalAlloc(UINT uFlags, DWORD dwBytes) {
    DWORD sz = dwBytes ? dwBytes : 1;
    void __huge* p = halloc(sz, 1);
    (void)uFlags;
    if (!p) return 0;
    return (HGLOBAL)HIWORD((DWORD)p);
}

#undef GlobalAllocPtr
#undef GlobalFreePtr
static void FAR* GlobalAllocPtr(UINT flags, DWORD cb) {
    DWORD sz = cb ? cb : 1;
    void __huge* p = halloc(sz, 1);
    if (p && (flags & GMEM_ZEROINIT)) {
        BYTE __huge* bp = (BYTE __huge*)p;
        DWORD i;
        for (i = 0; i < sz; i++) bp[i] = 0;
    }
    return (void FAR*)p;
}

static BOOL GlobalFreePtr(void FAR* lp) {
    if (lp) hfree((void __huge*)lp);
    return TRUE;
}

static HWND GetParent(HWND hw) {
    if (hw != 0 && hw <= g_windowCount && g_windows) return g_windows[hw].parent;
    return NULL;
}

static int GetClassName(HWND hWnd, LPSTR lpClassName, int nMaxCount) {
    if (hWnd != 0 && hWnd <= g_windowCount && lpClassName && g_windows) {
        strncpy(lpClassName, g_windows[hWnd].className, nMaxCount);
        if (nMaxCount > 0) lpClassName[nMaxCount - 1] = '\0';
        return strlen(lpClassName);
    }
    return 0;
}

static HWND GetWindow(HWND hWnd, UINT uCmd) {
    if (hWnd == 0 || hWnd > g_windowCount || !g_windows) return NULL;
    if (uCmd == GW_OWNER) return g_windows[hWnd].parent;
    if (uCmd == GW_CHILD) return g_windows[hWnd].child;
    if (uCmd == GW_HWNDNEXT) return g_windows[hWnd].next;
    return NULL;
}

static WORD GetWindowWord(HWND hWnd, int nIndex) {
    if (hWnd != 0 && hWnd <= g_windowCount && g_windows) {
        if (nIndex == GWW_ID) return (WORD)g_windows[hWnd].hMenu;
        if (nIndex == GWW_HINSTANCE) return 1;
    }
    return 0;
}

static HWND GetCapture(void) { return g_capturedWnd; }

static LONG GetWindowLong(HWND hWnd, int nIndex) {
    if (hWnd != 0 && hWnd <= g_windowCount && g_windows) {
        if (nIndex == GWL_WNDPROC) return (LONG)g_windows[hWnd].wndProc;
        if (nIndex == GWL_ID) return (LONG)g_windows[hWnd].hMenu;
        if (nIndex == GWL_STYLE) return (LONG)g_windows[hWnd].style;
    }
    return 0;
}

static HMENU GetSystemMenu(HWND hWnd, BOOL bRevert) { (void)hWnd; (void)bRevert; return (HMENU)1; }
static DWORD GetSysColor(int nIndex) { (void)nIndex; return RGB(192, 192, 192); }
static HTASK GetCurrentTask(void) { return (HTASK)1; }
static HANDLE GetProp(HWND hWnd, LPCSTR lpString) { (void)hWnd; (void)lpString; return NULL; }
static int GetWindowTextLength(HWND hw) { return (hw != 0 && hw <= g_windowCount && g_windows) ? strlen(g_windows[hw].text) : 0; }

static HWND GetDlgItem(HWND hDlg, int nID) {
    HWND child;
    if (!g_windows || hDlg == 0 || hDlg > g_windowCount) return NULL;
    child = g_windows[hDlg].child;
    while (child) {
        if ((int)g_windows[child].hMenu == nID) return child;
        child = g_windows[child].next;
    }
    return NULL;
}

static UINT GetPrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName) {
    char buf[32];
    DWORD res = GetPrivateProfileString(lpAppName, lpKeyName, "", buf, sizeof(buf), lpFileName);
    if (res == 0 || buf[0] == '\0') return (UINT)nDefault;
    return (UINT)atoi(buf);
}

static int GetKeyNameText(LONG lParam, LPSTR lpString, int nMaxCount) {
    (void)lParam;
    if (nMaxCount > 0 && lpString) lpString[0] = '\0';
    return 0;
}

static int GetSystemMetrics(int nIndex) {
    if (nIndex == SM_CXSCREEN) return 640;
    if (nIndex == SM_CYSCREEN) return 480;
    return 0;
}

static DWORD GetVersion(void) { return 0x0A03; }
static DWORD GetTickCount(void) { return (DWORD)(clock() * 1000L / CLOCKS_PER_SEC); }
#define GetCurrentTime GetTickCount
static BOOL GetWindowRect(HWND hw, LPRECT r) {
    if (hw != 0 && hw <= g_windowCount && g_windows && r) {
        *r = g_windows[hw].rect;
        return TRUE;
    }
    SetRectEmpty(r);
    return FALSE;
}

static FARPROC GetProcAddress(HINSTANCE h, LPCSTR name) { (void)h; (void)name; return NULL; }
static DWORD WINAPI GetWinFlags(void) { return 0x0130; }

static BOOL GetWindowPlacement(HWND hw, LPWINDOWPLACEMENT w) {
    if (hw != 0 && hw <= g_windowCount && g_windows && w) {
        w->showCmd = 0; w->rcNormalPosition = g_windows[hw].rect;
        return TRUE;
    }
    return FALSE;
}

static BOOL IsZoomed(HWND hWnd) { return (hWnd != 0 && hWnd <= g_windowCount && g_windows) ? g_windows[hWnd].isMaximized : FALSE; }
static BOOL IsWindowEnabled(HWND hw) { return (hw != 0 && hw <= g_windowCount && g_windows) ? !(g_windows[hw].style & WS_DISABLED) : FALSE; }
static BOOL IsWindowVisible(HWND hw) { return (hw != 0 && hw <= g_windowCount && g_windows) ? g_windows[hw].isVisible : FALSE; }
static BOOL IsIconic(HWND hw) { (void)hw; return FALSE; }
static BOOL IsWindow(HWND hw) { return (hw != 0 && hw <= g_windowCount && g_windows != NULL && g_windows[hw].wndProc != NULL); }

static UINT SetTimer(HWND hWnd, UINT nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc) {
    int i, slot = -1;
    if (!g_timers) {
        g_timers = (INTERNAL_TIMER FAR*)halloc(MAX_TIMERS * sizeof(INTERNAL_TIMER), 1);
        if (!g_timers) return 0;
        for (i = 0; i < MAX_TIMERS; i++) {
            g_timers[i].active = 0;
            g_timers[i].hwnd = 0;
        }
    }
    for (i = 0; i < MAX_TIMERS; i++) {
        if (g_timers[i].active && g_timers[i].hwnd == hWnd && g_timers[i].id == nIDEvent) {
            slot = i; break;
        }
        if (!g_timers[i].active && slot == -1) slot = i;
    }
    if (slot != -1) {
        g_timers[slot].active = 1;
        g_timers[slot].hwnd = hWnd;
        g_timers[slot].id = nIDEvent ? nIDEvent : 1;
        g_timers[slot].elapse = uElapse ? uElapse : 1000;
        g_timers[slot].proc = lpTimerFunc;
        g_timers[slot].lastFire = 0;
        return g_timers[slot].id;
    }
    return nIDEvent;
}
static BOOL KillTimer(HWND hWnd, UINT nIDEvent) {
    int i;
    if (!g_timers) return FALSE;
    for (i = 0; i < MAX_TIMERS; i++) {
        if (g_timers[i].active && g_timers[i].hwnd == hWnd && g_timers[i].id == nIDEvent) {
            g_timers[i].active = 0;
            return TRUE;
        }
    }
    return TRUE;
}

static LPSTR lstrcpyn(LPSTR lpString1, LPCSTR lpString2, int iMaxLength) {
    strncpy(lpString1, lpString2, iMaxLength);
    if (iMaxLength > 0) lpString1[iMaxLength - 1] = '\0';
    return lpString1;
}

static int lstrcmpi(LPCSTR str1, LPCSTR str2) { return stricmp(str1, str2); }
static HINSTANCE LoadLibrary(LPCSTR name) { (void)name; return (HINSTANCE)1; }
static UINT MapVirtualKey(UINT uCode, UINT uMapType) { (void)uMapType; return uCode; }

static BOOL PtInRect(const RECT FAR* lprc, POINT pt) {
    if (!lprc) return FALSE;
    return (pt.x >= lprc->left && pt.x < lprc->right && pt.y >= lprc->top && pt.y < lprc->bottom);
}

static BOOL Polygon(HDC hdc, const POINT FAR* lpPoints, int nCount) {
    int i;
    if (nCount < 2) return FALSE;
    MoveToEx(hdc, lpPoints[0].x, lpPoints[0].y, NULL);
    for (i = 1; i < nCount; i++) LineTo(hdc, lpPoints[i].x, lpPoints[i].y);
    LineTo(hdc, lpPoints[0].x, lpPoints[0].y);
    return TRUE;
}

static BOOL ReleaseCapture(void) { g_capturedWnd = NULL; return TRUE; }
static HANDLE RemoveProp(HWND hWnd, LPCSTR lpString) { (void)hWnd; (void)lpString; return NULL; }
static HCURSOR SetCursor(HCURSOR hCursor) { return hCursor; }

static BOOL ScreenToClient(HWND hWnd, LPPOINT lpPoint) {
    if (hWnd != 0 && hWnd <= g_windowCount && lpPoint && g_windows) {
        lpPoint->x -= g_windows[hWnd].rect.left + g_windows[hWnd].client.left;
        lpPoint->y -= g_windows[hWnd].rect.top + g_windows[hWnd].client.top;
        return TRUE;
    }
    return FALSE;
}

static WORD SetWindowWord(HWND hWnd, int nIndex, WORD wNewWord) {
    if (hWnd != 0 && hWnd <= g_windowCount && nIndex == GWW_ID && g_windows) {
        WORD old = (WORD)g_windows[hWnd].hMenu;
        g_windows[hWnd].hMenu = (HMENU)wNewWord;
        return old;
    }
    return 0;
}

static HWND SetCapture(HWND hWnd) {
    HWND old = g_capturedWnd;
    g_capturedWnd = hWnd;
    return old;
}

static LONG SetWindowLong(HWND hWnd, int nIndex, LONG dwNewLong) {
    if (hWnd != 0 && hWnd <= g_windowCount && g_windows) {
        if (nIndex == GWL_WNDPROC) {
            LONG old = (LONG)g_windows[hWnd].wndProc;
            g_windows[hWnd].wndProc = (WNDPROC)dwNewLong;
            return old;
        } else if (nIndex == GWL_ID) {
            LONG old = (LONG)g_windows[hWnd].hMenu;
            g_windows[hWnd].hMenu = (HMENU)dwNewLong;
            return old;
        } else if (nIndex == GWL_STYLE) {
            LONG old = (LONG)g_windows[hWnd].style;
            g_windows[hWnd].style = (DWORD)dwNewLong;
            return old;
        }
    }
    return 0;
}

static BOOL SetProp(HWND hWnd, LPCSTR lpString, HANDLE hData) { (void)hWnd; (void)lpString; (void)hData; return TRUE; }

static BOOL SetWindowPos(HWND hw, HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags) {
    (void)hWndInsertAfter;
    if (!g_windows || hw == 0 || hw > g_windowCount) return FALSE;
    {
        INTERNAL_WND* w = &g_windows[hw];
        int curX = w->rect.left;
        int curY = w->rect.top;
        int curW = w->rect.right - w->rect.left;
        int curH = w->rect.bottom - w->rect.top;
        if (w->parent && w->parent <= g_windowCount) {
            curX -= (g_windows[w->parent].rect.left + g_windows[w->parent].client.left);
            curY -= (g_windows[w->parent].rect.top + g_windows[w->parent].client.top);
        }
        if (uFlags & SWP_NOMOVE) { x = curX; y = curY; }
        if (uFlags & SWP_NOSIZE) { cx = curW; cy = curH; }
        MoveWindow(hw, x, y, cx, cy, TRUE);
        if (uFlags & SWP_SHOWWINDOW) ShowWindow(hw, SW_SHOW);
        if (uFlags & SWP_HIDEWINDOW) ShowWindow(hw, SW_HIDE);
    }
    return TRUE;
}

static HWND SetActiveWindow(HWND hw) { return SetFocus(hw); }

static HHOOK WINAPI SetWindowsHookEx(int idHook, HOOKPROC lpfn, HINSTANCE hMod, HTASK hTask) {
    (void)idHook; (void)hMod; (void)hTask;
    return (HHOOK)lpfn;
}

static BOOL WINAPI UnhookWindowsHookEx(HHOOK hhk) { (void)hhk; return TRUE; }

static BOOL WINAPI SystemParametersInfo(UINT uiAction, UINT uiParam, void *pvParam, UINT fWinIni) {
    (void)uiParam; (void)fWinIni;
    if (uiAction == SPI_SETWORKAREA && pvParam) {
        g_microwinWorkArea = *(LPRECT)pvParam;
        return TRUE;
    }
    if (uiAction == 48 && pvParam) {
        *(LPRECT)pvParam = g_microwinWorkArea;
        return TRUE;
    }
    return FALSE;
}

static HWND _WindowFromPointSubtree(HWND parent, int mx, int my) {
    HWND hit = parent;
    HWND child;
    if (!g_windows || parent == 0 || parent > g_windowCount) return parent;
    child = g_windows[parent].child;
    while (child) {
        INTERNAL_WND* cw = &g_windows[child];
        if (cw->isVisible && mx >= cw->rect.left && mx <= cw->rect.right && my >= cw->rect.top && my <= cw->rect.bottom)
            hit = _WindowFromPointSubtree(child, mx, my);
        child = cw->next;
    }
    return hit;
}

static HWND _WindowFromPoint(HWND root, int mx, int my) {
    int i;
    if (!g_windows) return 0;
    if (root != 0) return _WindowFromPointSubtree(root, mx, my);
    for (i = g_windowCount; i >= 1; i--) {
        INTERNAL_WND* w = &g_windows[i];
        if (!w->parent && w->isVisible && mx >= w->rect.left && mx <= w->rect.right && my >= w->rect.top && my <= w->rect.bottom) {
            return _WindowFromPointSubtree((HWND)i, mx, my);
        }
    }
    return hwndRoot;
}

static LPSTR AnsiUpper(LPSTR str) {
    if (str) { char* p = str; while (*p) { *p = (char)toupper(*p); p++; } }
    return str;
}

static LPSTR AnsiLower(LPSTR str) {
    if (str) { char* p = str; while (*p) { *p = (char)tolower(*p); p++; } }
    return str;
}

static BOOL BringWindowToTop(HWND hw) { (void)hw; g_needsPaint = 1; return TRUE; }

static BOOL ClientToScreen(HWND hWnd, LPPOINT lpPoint) {
    if (hWnd != 0 && hWnd <= g_windowCount && lpPoint && g_windows) {
        lpPoint->x += g_windows[hWnd].rect.left + g_windows[hWnd].client.left;
        lpPoint->y += g_windows[hWnd].rect.top + g_windows[hWnd].client.top;
        return TRUE;
    }
    return FALSE;
}

static LRESULT CallWindowProc(FARPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (lpPrevWndFunc) return ((WNDPROC)lpPrevWndFunc)(hWnd, Msg, wParam, lParam);
    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

static UINT GetDriveType(int nDrive) { (void)nDrive; return DRIVE_FIXED; }
static HMENU GetMenu(HWND hWnd) { return (hWnd != 0 && hWnd <= g_windowCount && g_windows) ? g_windows[hWnd].hMenu : NULL; }
static BOOL GetOpenFileName(LPOPENFILENAME lpofn) { (void)lpofn; return FALSE; }
static HWND GetDesktopWindow(void) { return hwndRoot; }
static HWND WindowFromPoint(POINT Point) { return _WindowFromPoint(0, Point.x, Point.y); }

static HWND FindWindow(LPCSTR lpClassName, LPCSTR lpWindowName) {
    int i;
    if (!g_windows) return NULL;
    for (i = 1; i <= g_windowCount; i++) {
        if (g_windows[i].isVisible) {
            BOOL matchClass = TRUE, matchName = TRUE;
            if (lpClassName && stricmp(g_windows[i].className, lpClassName) != 0) matchClass = FALSE;
            if (lpWindowName && strcmp(g_windows[i].text, lpWindowName) != 0) matchName = FALSE;
            if (matchClass && matchName) return (HWND)i;
        }
    }
    return NULL;
}

#endif /* USER_H */
