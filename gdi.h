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
#ifndef GDI_H
#define GDI_H
#define COLOR_WINDOW        5
#define WHITE_BRUSH         0
#define LTGRAY_BRUSH        1
#define GRAY_BRUSH          2
#define BLACK_BRUSH         4
#define NULL_BRUSH          5
#define WHITE_PEN           6
#define BLACK_PEN           7
#define NULL_PEN            8
#define TRANSPARENT         1
#define OPAQUE              2
#define SYSTEM_FONT         13
#define DEFAULT_GUI_FONT    17
#define SRCCOPY             0x00CC0020
#define PS_SOLID            0
#define PS_DOT              2
#define ALTERNATE           1
#define WINDING             2
#define DT_TOP              0x0000
#define DT_LEFT             0x0000
#define DT_CENTER           0x0001
#define DT_RIGHT            0x0002
#define DT_VCENTER          0x0004
#define DT_WORDBREAK        0x0010
#define DT_SINGLELINE       0x0020
#define DT_CALCRECT         0x0400
#define DT_NOPREFIX         0x0800
#define ANSI_VAR_FONT       12
#define ANSI_CHARSET        0
#define OUT_DEFAULT_PRECIS  0
#define CLIP_DEFAULT_PRECIS 0
#define DEFAULT_QUALITY     0
#define DEFAULT_PITCH       0
#define FF_SWISS            32
#define ETO_OPAQUE          0x0002
#define ETO_CLIPPED         0x0004
#define ATLAS_DIM           256
#define ATLAS_FLOAT         128.0f
static BYTE _RGBtoVGA(COLORREF cr) {
    BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
    if (r > 240 && g > 240 && b > 240) return 15;
    if (r < 15  && g < 15  && b < 15)  return 0;
    if (r > 190 && g > 190 && b > 190) return 7;
    if (r > 100 && g > 100 && b > 100 && r < 150 && g < 150 && b < 150) return 8;
    if (r > 150 && g < 100 && b < 100) return 12;
    if (r < 100 && g > 150 && b < 100) return 10;
    if (r < 100 && g < 150 && b > 150) return 9;
    if (r > 150 && g > 150 && b < 100) return 14;
    if (r > 128 && g < 100 && b > 128) return 13;
    if (r < 100 && g > 128 && b > 128) return 11;
    if (r > 150 && g > 80  && b < 50)  return 6;
    if (r > 100 && g < 50  && b < 50)  return 4;
    if (r < 50  && g > 100 && b < 50)  return 2;
    if (r < 50  && g < 50  && b > 100) return 1;
    if (b > r && b > g) return 9;
    if (r > g && r > b) return 12;
    if (g > r && g > b) return 10;
    return 7;
}

static void _FillRect(INTERNAL_DC* dc, RECT* r, BYTE c) {
    int y, cx;
    int startX = r->left, endX = r->right, startY = r->top, endY = r->bottom;

    if (dc->pSurface) {
        if (startX < 0) startX = 0; if (endX > dc->w) endX = dc->w;
        if (startY < 0) startY = 0; if (endY > dc->h) endY = dc->h;
        if (startX >= endX || startY >= endY) return;
        for (y = startY; y < endY; y++) {
            for (cx = startX; cx < endX; cx++) dc->pSurface[y * dc->w + cx] = c;
        }
        return;
    }

    if (startX < dc->clip.left) startX = dc->clip.left;
    if (endX > dc->clip.right) endX = dc->clip.right;
    if (startY < dc->clip.top) startY = dc->clip.top;
    if (endY > dc->clip.bottom) endY = dc->clip.bottom;
    if (startX >= endX || startY >= endY) return;

    _HideMouse();
    outpw(0x3CE, 0x0205);
    for (y = startY; y < endY; y++) {
        cx = startX;
        volatile BYTE far* vga = (volatile BYTE far*)0xA0000000L + (y * 80) + (cx / 8);
        if (cx < endX && (cx & 7) != 0) {
            int bits = 8 - (cx & 7); BYTE mask;
            if (cx + bits > endX) bits = endX - cx;
            mask = (0xFF >> (cx & 7)) & (0xFF << (8 - ((cx & 7) + bits)));
            outp(0x3CE, 8); outp(0x3CF, mask);
            g_vga_dummy = *vga; *vga = c;
            vga++; cx += bits;
        }
        if (cx + 8 <= endX) {
            outp(0x3CE, 8); outp(0x3CF, 0xFF);
            while (cx + 8 <= endX) { g_vga_dummy = *vga; *vga = c; vga++; cx += 8; }
        }
        if (cx < endX) {
            BYTE mask = 0xFF << (8 - (endX - cx));
            outp(0x3CE, 8); outp(0x3CF, mask);
            g_vga_dummy = *vga; *vga = c;
        }
    }
    outpw(0x3CE, 0x0005); outpw(0x3CE, 0xFF08);
    _ShowMouse();
}

static void _DrawPixel(INTERNAL_DC* dc, int x, int y, BYTE c) {
    if (x >= dc->clip.left && x < dc->clip.right && y >= dc->clip.top && y < dc->clip.bottom) {
        volatile BYTE far* vga = (volatile BYTE far*)0xA0000000L + (y * 80) + (x / 8);
        _HideMouse(); outpw(0x3CE, 0x0205); outp(0x3CE, 8); outp(0x3CF, 0x80 >> (x & 7));
        g_vga_dummy = *vga; *vga = c;
        outpw(0x3CE, 0x0005); outpw(0x3CE, 0xFF08); _ShowMouse();
    }
}

static void _Draw3DBorder(INTERNAL_DC* dc, RECT* r, BYTE tl, BYTE br) {
    RECT top, bot, lft, rgt;
    top.left = r->left; top.top = r->top; top.right = r->right; top.bottom = r->top+1;
    bot.left = r->left; bot.top = r->bottom-1; bot.right = r->right; bot.bottom = r->bottom;
    lft.left = r->left; lft.top = r->top; lft.right = r->left+1; lft.bottom = r->bottom;
    rgt.left = r->right-1; rgt.top = r->top; rgt.right = r->right; rgt.bottom = r->bottom;
    _FillRect(dc, &top, tl); _FillRect(dc, &lft, tl); _FillRect(dc, &bot, br); _FillRect(dc, &rgt, br);
}

static HDC GetDC(HWND hw) {
    INTERNAL_DC* dc = &g_dcs[1];
    if (hw != 0 && hw <= g_windowCount && g_windows) {
        INTERNAL_WND* w = &g_windows[hw];
        dc->hwnd = hw; dc->w = 640; dc->h = 480;
        dc->clip.left = w->rect.left + w->client.left;
        dc->clip.top = w->rect.top + w->client.top;
        dc->clip.right = w->rect.left + w->client.right;
        dc->clip.bottom = w->rect.top + w->client.bottom;
        if (dc->clip.left < 0) dc->clip.left = 0; if (dc->clip.top < 0) dc->clip.top = 0;
        if (dc->clip.right > 640) dc->clip.right = 640; if (dc->clip.bottom > 480) dc->clip.bottom = 480;
    } else {
        dc->hwnd = 0; dc->w = 640; dc->h = 480;
        dc->clip.left = 0; dc->clip.top = 0; dc->clip.right = 640; dc->clip.bottom = 480;
    }
    dc->pSurface = NULL;
    dc->penX = 0; dc->penY = 0; dc->hPen = (HPEN)0x0100; dc->hBrush = (HBRUSH)0x020F;
    dc->hFont = (HFONT)0x0301;
    dc->fontScale = 1;
    dc->bkColor = 15;
    dc->textColor = 0;
    dc->bkMode = 2;
    return (HDC)1;
}

static HDC GetDC(HWND hw);
static int ReleaseDC(HWND hw, HDC hdc) { (void)hw; (void)hdc; return 1; }
static HDC BeginPaint(HWND hw, LPPAINTSTRUCT lp) {
    lp->hdc = GetDC(hw);
    lp->fErase = TRUE;
    lp->rcPaint.left = 0; lp->rcPaint.top = 0;
    lp->rcPaint.right = 640; lp->rcPaint.bottom = 480;
    _HideMouse();
    return lp->hdc;
}
static BOOL EndPaint(HWND hw, const PAINTSTRUCT FAR* lp) { (void)hw; (void)lp; _ShowMouse(); return TRUE; }

static HDC CreateCompatibleDC(HDC h) { 
    int i;
    (void)h;
    for (i = 2; i < MAX_DCS; i++) {
        if (g_dcs[i].hwnd == 0 && g_dcs[i].pSurface == NULL) {
            g_dcs[i] = g_dcs[1];
            g_dcs[i].hwnd = 0;
            return (HDC)i;
        }
    }
    return (HDC)1; 
}

static HBITMAP CreateCompatibleBitmap(HDC hdc, int w, int h) { 
    int i;
    (void)hdc;
    for (i = 1; i < MAX_BITMAPS; i++) {
        if (g_bitmaps[i].pixels == NULL) {
            g_bitmaps[i].w = w; g_bitmaps[i].h = h;
            g_bitmaps[i].pixels = (BYTE __huge*)halloc((long)w * h, 1);
            return (HBITMAP)i;
        }
    }
    return (HBITMAP)1; 
}

static BOOL DeleteDC(HDC hdc) { (void)hdc; return TRUE; }
static BOOL BitBlt(HDC dst, int dx, int dy, int w, int h, HDC src, int sx, int sy, DWORD rop) {
    INTERNAL_DC* dDC = &g_dcs[(int)dst];
    INTERNAL_DC* sDC = &g_dcs[(int)src];
    int r, c;
    (void)rop;
    if (sDC->pSurface && !dDC->pSurface) {
        _HideMouse(); outpw(0x3CE, 0x0205);
        for (r = 0; r < h; r++) {
            for (c = 0; c < w; c++) {
                int srcX = sx + c, srcY = sy + r;
                int dstX = dx + c, dstY = dy + r;
                if (dDC->hwnd) {
                    INTERNAL_WND* wnd = &g_windows[dDC->hwnd];
                    dstX += wnd->rect.left + wnd->client.left;
                    dstY += wnd->rect.top + wnd->client.top;
                }
                if (dstX >= dDC->clip.left && dstX < dDC->clip.right && dstY >= dDC->clip.top && dstY < dDC->clip.bottom) {
                    if (srcX >= 0 && srcX < sDC->w && srcY >= 0 && srcY < sDC->h) {
                        BYTE color = sDC->pSurface[srcY * sDC->w + srcX];
                        volatile BYTE far* vga = (volatile BYTE far*)0xA0000000L + (dstY * 80) + (dstX / 8);
                        outp(0x3CE, 8); outp(0x3CF, 0x80 >> (dstX & 7));
                        g_vga_dummy = *vga; *vga = color;
                    }
                }
            }
        }
        outpw(0x3CE, 0x0005); outpw(0x3CE, 0xFF08); _ShowMouse();
    }
    return TRUE;
}
static HFONT CreateFont(int h, int w, int e, int or, int wt, DWORD i, DWORD u, DWORD s, DWORD cs, DWORD op, DWORD cp, DWORD q, DWORD pf, LPCSTR face) {
    int j;
    if (h < 0) h = -h;
    if (h == 0) h = 12;
    (void)w; (void)e; (void)or; (void)wt; (void)i; (void)u; (void)s; (void)cs; (void)op; (void)cp; (void)q; (void)pf;

    for (j = 0; j < g_numTtfReg; j++) {
        if (g_ttfReg && face && stricmp(g_ttfReg[j].name, face) == 0) {
            int k, l;
            BYTE __huge* ttf = NULL;
            long fileSize = 0;

            for (k = 0; k < g_numBaked; k++) {
                if (stricmp(g_baked[k].face, face) == 0 && g_baked[k].height == h) {
                    return (HFONT)(0x4000 | k);
                }
            }

            for (l = 0; l < g_numLoadedTtf; l++) {
                if (stricmp(g_loadedTtf[l].path, g_ttfReg[j].path) == 0) {
                    ttf = g_loadedTtf[l].ttf;
                    fileSize = g_loadedTtf[l].size;
                    break;
                }
            }

            if (!ttf) {
                FILE* filePtr = fopen(g_ttfReg[j].path, "rb");
                if (filePtr) {
                    fseek(filePtr, 0, SEEK_END);
                    fileSize = ftell(filePtr);
                    fseek(filePtr, 0, SEEK_SET);
                    if (fileSize > 0 && fileSize < 524288L) {
                        ttf = (BYTE __huge*)halloc(fileSize, 1);
                        if (ttf) {
                            long bytesRead = 0;
                            char temp[1024];
                            while (bytesRead < fileSize) {
                                size_t toRead = (fileSize - bytesRead) > 1024 ? 1024 : (size_t)(fileSize - bytesRead);
                                size_t n = fread(temp, 1, toRead, filePtr);
                                if (n == 0) break;
                                {
                                    long idx;
                                    for (idx = 0; idx < (long)n; idx++) ttf[bytesRead + idx] = temp[idx];
                                }
                                bytesRead += (long)n;
                            }
                            fclose(filePtr);
                            if (g_loadedTtf && g_numLoadedTtf < MAX_TTF) {
                                strncpy(g_loadedTtf[g_numLoadedTtf].path, g_ttfReg[j].path, 63);
                                g_loadedTtf[g_numLoadedTtf].path[63] = '\0';
                                g_loadedTtf[g_numLoadedTtf].ttf = ttf;
                                g_loadedTtf[g_numLoadedTtf].size = fileSize;
                                g_numLoadedTtf++;
                            }
                        } else fclose(filePtr);
                    } else fclose(filePtr);
                }
            }

            if (ttf && fileSize > 0 && g_baked && g_numBaked < MAX_BAKED) {
                BYTE __huge* atlas = (BYTE __huge*)halloc(256L * 128L, 1);
                if (atlas) {
                    int font_offset = stbtt_GetFontOffsetForIndex((const unsigned char __huge*)ttf, 0);
                    int bake_res = stbtt_BakeFontBitmap((const unsigned char __huge*)ttf, font_offset, (float)h, (unsigned char __huge*)atlas, 256, 128, 32, 96, g_baked[g_numBaked].cdata);
                    if (bake_res > 0) {
                        strncpy(g_baked[g_numBaked].face, face, 31);
                        g_baked[g_numBaked].face[31] = '\0';
                        g_baked[g_numBaked].atlas = atlas;
                        g_baked[g_numBaked].height = h;
                        g_numBaked++;
                        return (HFONT)(0x4000 | (g_numBaked - 1));
                    }
                    hfree(atlas);
                }
            }
        }
    }
    return (HFONT)(0x0300 | (h >= 20 ? 2 : 1));
}

static HGDIOBJ GetStockObject(int fn) {
    switch (fn) {
        case WHITE_BRUSH:      return (HGDIOBJ)0x020F;
        case LTGRAY_BRUSH:     return (HGDIOBJ)0x0207;
        case GRAY_BRUSH:       return (HGDIOBJ)0x0208;
        case BLACK_BRUSH:      return (HGDIOBJ)0x0200;
        case NULL_BRUSH:       return (HGDIOBJ)0x02FF;
        case WHITE_PEN:        return (HGDIOBJ)0x010F;
        case BLACK_PEN:        return (HGDIOBJ)0x0100;
        case NULL_PEN:         return (HGDIOBJ)0x01FF;
        case SYSTEM_FONT:
        case DEFAULT_GUI_FONT: return (HGDIOBJ)0x0301;
    }
    return (HGDIOBJ)0;
}

static HBRUSH CreateSolidBrush(COLORREF cr) { return (HBRUSH)(0x0200 | _RGBtoVGA(cr)); }
static HPEN CreatePen(int s, int w, COLORREF cr) { (void)s; (void)w; return (HPEN)(0x0100 | _RGBtoVGA(cr)); }

static BOOL _DeleteObject(HGDIOBJ h) { (void)h; return TRUE; }
#define DeleteObject(h) _DeleteObject((HGDIOBJ)(DWORD)(h))
static HGDIOBJ SelectObject(HDC hdc, HGDIOBJ h) {
    INTERNAL_DC* dc = &g_dcs[(int)hdc]; HGDIOBJ old = 0; int type = ((WORD)h) >> 8;
    if (type == 1) { old = dc->hPen; dc->hPen = (HPEN)h; }
    else if (type == 2) { old = dc->hBrush; dc->hBrush = (HBRUSH)h; }
    else if (type == 3 || type == 0x40) { old = dc->hFont; dc->fontScale = ((WORD)h) & 0xFF; dc->hFont = (HFONT)h; }
    else if (type == 0 && (int)h > 0 && (int)h < MAX_BITMAPS) {
        dc->pSurface = g_bitmaps[(int)h].pixels;
        dc->w = g_bitmaps[(int)h].w;
        dc->h = g_bitmaps[(int)h].h;
    }
    return old;
}

static COLORREF SetTextColor(HDC h, COLORREF c) { COLORREF o = g_dcs[(int)h].textColor; g_dcs[(int)h].textColor = _RGBtoVGA(c); return o; }
static COLORREF SetBkColor(HDC h, COLORREF c) { COLORREF o = g_dcs[(int)h].bkColor; g_dcs[(int)h].bkColor = _RGBtoVGA(c); return o; }
static int SetBkMode(HDC h, int m) { int o = g_dcs[(int)h].bkMode; g_dcs[(int)h].bkMode = m; return o; }
static int SetPolyFillMode(HDC h, int m) { (void)h; return m; }

static BOOL GetTextExtentPoint(HDC hdc, LPCSTR str, int cb, LPSIZE lpSize) {
    INTERNAL_DC* dc = &g_dcs[(int)hdc];
    if (((WORD)dc->hFont & 0xFF00) == 0x4000 && g_baked) {
        int bIdx = (WORD)dc->hFont & 0xFF; BakedFont* bf = &g_baked[bIdx];
        float fx = 0, fy = 0; int i;
        for (i = 0; i < cb; i++) {
            int ch = (unsigned char)str[i];
            if (ch >= 32 && ch < 128) { stbtt_aligned_quad q; stbtt_GetBakedQuad(bf->cdata, 256, 128, ch-32, &fx, &fy, &q, 1); }
        }
        lpSize->cx = (int)fx; lpSize->cy = bf->height; return TRUE;
    }
    {
        int s = dc->fontScale; if (s == 0) s = 1; lpSize->cx = cb * 8 * s; lpSize->cy = 16 * s; return TRUE;
    }
}

static DWORD GetTextExtent(HDC hdc, LPCSTR str, int cb) {
    SIZE sz;
    GetTextExtentPoint(hdc, str, cb, &sz);
    return (DWORD)MAKELONG(sz.cx, sz.cy);
}

static BOOL TextOut(HDC hdc, int x, int y, LPCSTR str, int cb) {
    INTERNAL_DC* dc = &g_dcs[(int)hdc];
    int absX = x, absY = y; int i, r, c, s = dc->fontScale; if (s == 0) s = 1;
    if (dc->hwnd != 0 && g_windows) {
        INTERNAL_WND* w = &g_windows[dc->hwnd];
        absX += w->rect.left + w->client.left; absY += w->rect.top + w->client.top;
    }

    if (((WORD)dc->hFont & 0xFF00) == 0x4000 && g_baked) {
        int bIdx = (WORD)dc->hFont & 0xFF;
        BakedFont* bf = &g_baked[bIdx];
        float fx = (float)absX, fy = (float)(absY + (int)(bf->height * 0.8f));

        if (dc->bkMode == 2) {
            float tw = 0, th = 0; stbtt_aligned_quad tq; RECT bg;
            for (i = 0; i < cb; ++i) if (str[i] >= 32 && str[i] < 128) stbtt_GetBakedQuad(bf->cdata, 256, 128, str[i]-32, &tw, &th, &tq, 1);
            bg.left = absX; bg.top = absY; bg.right = absX + (int)tw; bg.bottom = absY + bf->height;
            _FillRect(dc, &bg, (BYTE)dc->bkColor);
        }

        _HideMouse(); outpw(0x3CE, 0x0205);
        for (i = 0; i < cb; i++) {
            int ch = (unsigned char)str[i];
            if (ch >= 32 && ch < 128) {
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(bf->cdata, 256, 128, ch - 32, &fx, &fy, &q, 1);
                for (r = (int)q.y0; r < (int)q.y1; r++) {
                    if (r < dc->clip.top || r >= dc->clip.bottom) continue;
                    for (c = (int)q.x0; c < (int)q.x1; c++) {
                        int sx, sy;
                        if (c < dc->clip.left || c >= dc->clip.right) continue;
                        sx = (int)(q.s0 * 256.0f) + (c - (int)q.x0);
                        sy = (int)(q.t0 * 128.0f) + (r - (int)q.y0);
                        if (sx >= 0 && sx < 256 && sy >= 0 && sy < 128) {
                            if (bf->atlas[(long)sy * 256 + sx] > 96) {
                                volatile BYTE far* vga = (volatile BYTE far*)0xA0000000L + (r * 80) + (c / 8);
                                outp(0x3CE, 8); outp(0x3CF, 0x80 >> (c & 7));
                                g_vga_dummy = *vga; *vga = (BYTE)dc->textColor;
                            }
                        }
                    }
                }
            }
        }
        outpw(0x3CE, 0x0005); outpw(0x3CE, 0xFF08); _ShowMouse();
        return TRUE;
    }

    if (dc->bkMode == 2) { RECT bg; bg.left = absX; bg.top = absY; bg.right = absX + (cb*8*s); bg.bottom = absY + (16*s); _FillRect(dc, &bg, (BYTE)dc->bkColor); }
    _HideMouse(); outpw(0x3CE, 0x0205);
    for (i = 0; i < cb; i++) {
        BYTE ch = str[i];
        for (r = 0; r < 16; r++) {
            BYTE row = rom_font[(ch * 16) + r]; if (!row) continue;
            for (c = 0; c < 8; c++) if (row & (0x80 >> c)) {
                if (s == 1) {
                    int px = absX + (i * 8) + c, py = absY + r;
                    if (px >= dc->clip.left && px < dc->clip.right && py >= dc->clip.top && py < dc->clip.bottom) {
                        volatile BYTE far* vga = (volatile BYTE far*)0xA0000000L + (py * 80) + (px / 8);
                        outp(0x3CE, 8); outp(0x3CF, 0x80 >> (px & 7));
                        g_vga_dummy = *vga; *vga = (BYTE)dc->textColor;
                    }
                } else {
                    int sx, sy;
                    for (sy = 0; sy < s; sy++) {
                        for (sx = 0; sx < s; sx++) {
                            int px = absX + (i * 8 * s) + (c * s) + sx, py = absY + (r * s) + sy;
                            if (px >= dc->clip.left && px < dc->clip.right && py >= dc->clip.top && py < dc->clip.bottom) {
                                volatile BYTE far* vga = (volatile BYTE far*)0xA0000000L + (py * 80) + (px / 8);
                                outp(0x3CE, 8); outp(0x3CF, 0x80 >> (px & 7));
                                g_vga_dummy = *vga; *vga = (BYTE)dc->textColor;
                            }
                        }
                    }
                }
            }
        }
    }
    outpw(0x3CE, 0x0005); outpw(0x3CE, 0xFF08); _ShowMouse();
    return TRUE;
}

static int DrawText(HDC hdc, LPCSTR str, int cb, LPRECT r, UINT fmt) {
    SIZE sz;
    int len = (cb == -1) ? strlen(str) : cb;
    int x = r->left, y = r->top;
    GetTextExtentPoint(hdc, str, len, &sz);
    if (fmt & DT_CALCRECT) {
        r->right = r->left + sz.cx;
        r->bottom = r->top + sz.cy;
        return sz.cy;
    }
    if (fmt & DT_CENTER) x = r->left + ((r->right - r->left) - sz.cx) / 2;
    else if (fmt & DT_RIGHT) x = r->right - sz.cx;
    if (fmt & DT_VCENTER) y = r->top + ((r->bottom - r->top) - sz.cy) / 2;
    TextOut(hdc, x, y, str, len);
    return sz.cy;
}

static BOOL GetTextMetrics(HDC hdc, LPTEXTMETRIC lptm) {
    INTERNAL_DC* dc = &g_dcs[(int)hdc];
    if (((WORD)dc->hFont & 0xFF00) == 0x4000 && g_baked) {
        int bIdx = (WORD)dc->hFont & 0xFF; BakedFont* bf = &g_baked[bIdx];
        lptm->tmHeight = bf->height; lptm->tmAveCharWidth = bf->height / 2; lptm->tmMaxCharWidth = bf->height; return TRUE;
    }
    {
        int s = dc->fontScale; if (s == 0) s = 1; lptm->tmHeight = 16 * s; lptm->tmAveCharWidth = 8 * s; lptm->tmMaxCharWidth = 8 * s; return TRUE;
    }
}

static BOOL MoveToEx(HDC hdc, int X, int Y, LPPOINT lp) { INTERNAL_DC* dc = &g_dcs[(int)hdc]; if (lp) { lp->x = dc->penX; lp->y = dc->penY; } dc->penX = X; dc->penY = Y; return TRUE; }

static BOOL LineTo(HDC hdc, int X, int Y) {
    INTERNAL_DC* dc = &g_dcs[(int)hdc];
    int offX = 0, offY = 0;
    if (dc->hwnd != 0 && g_windows && !dc->pSurface) {
        INTERNAL_WND* w = &g_windows[dc->hwnd];
        offX = w->rect.left + w->client.left; offY = w->rect.top + w->client.top;
    }
    {
        int x1 = offX + dc->penX, y1 = offY + dc->penY, x2 = offX + X, y2 = offY + Y;
        int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1, dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1, err = (dx > dy ? dx : -dy) / 2, e2;
        BYTE color = (BYTE)(((WORD)dc->hPen) & 0xFF);
        if (((WORD)dc->hPen) == 0x01FF) { dc->penX = X; dc->penY = Y; return TRUE; }
        
        if (!dc->pSurface) { _HideMouse(); outpw(0x3CE, 0x0205); }
        for (;;) {
            if (dc->pSurface) {
                if (x1 >= 0 && x1 < dc->w && y1 >= 0 && y1 < dc->h) dc->pSurface[y1 * dc->w + x1] = color;
            } else if (x1 >= dc->clip.left && x1 < dc->clip.right && y1 >= dc->clip.top && y1 < dc->clip.bottom) {
                volatile BYTE far* vga = (volatile BYTE far*)0xA0000000L + (y1 * 80) + (x1 / 8);
                outp(0x3CE, 8); outp(0x3CF, 0x80 >> (x1 & 7));
                g_vga_dummy = *vga; *vga = color;
            }
            if (x1 == x2 && y1 == y2) break;
            e2 = err; if (e2 > -dx) { err -= dy; x1 += sx; } if (e2 < dy) { err += dx; y1 += sy; }
        }
        if (!dc->pSurface) { outpw(0x3CE, 0x0005); outpw(0x3CE, 0xFF08); _ShowMouse(); }
    }
    dc->penX = X; dc->penY = Y; return TRUE;
}

static BOOL Polyline(HDC hdc, const POINT FAR* lpPoints, int nCount) {
    int i;
    if (nCount < 2 || !lpPoints) return FALSE;
    MoveToEx(hdc, lpPoints[0].x, lpPoints[0].y, NULL);
    for (i = 1; i < nCount; i++) LineTo(hdc, lpPoints[i].x, lpPoints[i].y);
    return TRUE;
}

static BOOL Rectangle(HDC hdc, int l, int t, int r, int b) {
    INTERNAL_DC* dc = &g_dcs[(int)hdc]; RECT rect; int pX = dc->penX, pY = dc->penY;
    int offX = 0, offY = 0;
    if (dc->hwnd != 0 && g_windows) {
        INTERNAL_WND* w = &g_windows[dc->hwnd];
        offX = w->rect.left + w->client.left; offY = w->rect.top + w->client.top;
    }
    rect.left = offX + l; rect.top = offY + t; rect.right = offX + r; rect.bottom = offY + b;
    if (((WORD)dc->hBrush) != 0x02FF) { RECT f; f.left = rect.left+1; f.top = rect.top+1; f.right = rect.right; f.bottom = rect.bottom; _FillRect(dc, &f, (BYTE)(((WORD)dc->hBrush) & 0xFF)); }
    if (((WORD)dc->hPen) != 0x01FF) { MoveToEx(hdc, l, t, NULL); LineTo(hdc, r, t); LineTo(hdc, r, b); LineTo(hdc, l, b); LineTo(hdc, l, t); }
    dc->penX = pX; dc->penY = pY; return TRUE;
}

static BOOL RoundRect(HDC hdc, int l, int t, int r, int b, int w, int h) {
    INTERNAL_DC* dc = &g_dcs[(int)hdc]; 
    int offX = 0, offY = 0;
    int rx = w / 2, ry = h / 2;
    int cx, cy;
    
    if (dc->hwnd != 0 && g_windows) {
        INTERNAL_WND* wnd = &g_windows[dc->hwnd];
        offX = wnd->rect.left + wnd->client.left; 
        offY = wnd->rect.top + wnd->client.top;
    }
    l += offX; r += offX; t += offY; b += offY;
    
    if (rx <= 0 || ry <= 0) return Rectangle(hdc, l - offX, t - offY, r - offX, b - offY);
    if (rx > (r - l) / 2) rx = (r - l) / 2;
    if (ry > (b - t) / 2) ry = (b - t) / 2;

    if (((WORD)dc->hBrush) != 0x02FF) {
        BYTE cb = (BYTE)(((WORD)dc->hBrush) & 0xFF);
        RECT r1, r2;
        r1.left = l + rx; r1.top = t; r1.right = r - rx; r1.bottom = b;
        r2.left = l; r2.top = t + ry; r2.right = r; r2.bottom = b - ry;
        _FillRect(dc, &r1, cb);
        _FillRect(dc, &r2, cb);
        for (cy = 0; cy < ry; cy++) {
            for (cx = 0; cx < rx; cx++) {
                if ((cx - rx)*(cx - rx)*ry*ry + (cy - ry)*(cy - ry)*rx*rx <= rx*rx*ry*ry) {
                    _DrawPixel(dc, l + rx - 1 - cx, t + ry - 1 - cy, cb); 
                    _DrawPixel(dc, r - rx + cx, t + ry - 1 - cy, cb); 
                    _DrawPixel(dc, l + rx - 1 - cx, b - ry + cy, cb); 
                    _DrawPixel(dc, r - rx + cx, b - ry + cy, cb); 
                }
            }
        }
    }
    if (((WORD)dc->hPen) != 0x01FF) {
        BYTE cp = (BYTE)(((WORD)dc->hPen) & 0xFF);
        for (cx = l + rx; cx < r - rx; cx++) { _DrawPixel(dc, cx, t, cp); _DrawPixel(dc, cx, b - 1, cp); }
        for (cy = t + ry; cy < b - ry; cy++) { _DrawPixel(dc, l, cy, cp); _DrawPixel(dc, r - 1, cy, cp); }
        for (cy = 0; cy < ry; cy++) {
            for (cx = 0; cx < rx; cx++) {
                long d2 = (long)(cx - rx)*(cx - rx)*ry*ry + (long)(cy - ry)*(cy - ry)*rx*rx;
                long inner = (long)(rx - 1)*(rx - 1)*ry*ry;
                if (d2 <= (long)rx*rx*ry*ry && d2 >= inner) {
                    _DrawPixel(dc, l + rx - 1 - cx, t + ry - 1 - cy, cp);
                    _DrawPixel(dc, r - rx + cx, t + ry - 1 - cy, cp);
                    _DrawPixel(dc, l + rx - 1 - cx, b - ry + cy, cp);
                    _DrawPixel(dc, r - rx + cx, b - ry + cy, cp);
                }
            }
        }
    }
    return TRUE;
}
static BOOL Ellipse(HDC hdc, int l, int t, int r, int b) { return Rectangle(hdc, l, t, r, b); }

static BOOL FillRect(HDC hdc, const RECT FAR* rt, HBRUSH hb) {
    INTERNAL_DC* dc = &g_dcs[(int)hdc]; RECT rect;
    int offX = 0, offY = 0;
    if (dc->hwnd != 0 && g_windows) {
        INTERNAL_WND* w = &g_windows[dc->hwnd];
        offX = w->rect.left + w->client.left; offY = w->rect.top + w->client.top;
    }
    rect.left = offX + rt->left; rect.top = offY + rt->top; rect.right = offX + rt->right; rect.bottom = offY + rt->bottom;
    _FillRect(dc, &rect, (BYTE)(((WORD)hb) & 0xFF));
    return TRUE;
}

static int FrameRect(HDC hdc, const RECT FAR* lprc, HBRUSH hbr) {
    INTERNAL_DC* dc = &g_dcs[(int)hdc]; RECT rect;
    int offX = 0, offY = 0;
    BYTE c = (BYTE)(((WORD)hbr) & 0xFF);
    if (!lprc) return 0;
    if (dc->hwnd != 0 && g_windows) {
        INTERNAL_WND* w = &g_windows[dc->hwnd];
        offX = w->rect.left + w->client.left; offY = w->rect.top + w->client.top;
    }
    rect.left = offX + lprc->left; rect.top = offY + lprc->top;
    rect.right = offX + lprc->right; rect.bottom = offY + lprc->bottom;
    _Draw3DBorder(dc, &rect, c, c);
    return 1;
}

static int GetDeviceCaps(HDC h, int i) { (void)h; (void)i; return 100; }
static int StartDoc(HDC h, const DOCINFO FAR* di) { (void)h; (void)di; return 1; }
static int StartPage(HDC h) { (void)h; return 1; }
static int EndPage(HDC h) { (void)h; return 1; }
static int EndDoc(HDC h) { (void)h; return 1; }
static DWORD SetWindowOrg(HDC hdc, int x, int y) { (void)hdc; (void)x; (void)y; return 0; }
static HRGN CreateRectRgnIndirect(const RECT FAR* lprc) { (void)lprc; return (HRGN)1; }
static int SelectClipRgn(HDC hdc, HRGN hrgn) { (void)hdc; (void)hrgn; return 1; }

static BOOL ExtTextOut(HDC hdc, int X, int Y, UINT fuOptions, const RECT FAR* lprc, LPCSTR lpString, UINT cbCount, const INT FAR* lpDx) {
    (void)lpDx;
    if ((fuOptions & ETO_OPAQUE) && lprc) {
        INTERNAL_DC* dc = &g_dcs[(int)hdc];
        RECT bg = *lprc;
        int offX = 0, offY = 0;
        if (dc->hwnd != 0 && g_windows) {
            INTERNAL_WND* w = &g_windows[dc->hwnd];
            offX = w->rect.left + w->client.left; offY = w->rect.top + w->client.top;
        }
        bg.left += offX; bg.top += offY; bg.right += offX; bg.bottom += offY;
        _FillRect(dc, &bg, (BYTE)dc->bkColor);
    }
    if (cbCount > 0 && lpString) {
        return TextOut(hdc, X, Y, lpString, cbCount);
    }
    return TRUE;
}

#endif /* GDI_H */