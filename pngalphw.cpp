#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

void puts(LPTSTR text) {
  HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
  WriteConsole(cmd, text, lstrlen(text), NULL, NULL);
}

void convert(LPWSTR pathname) {
  Bitmap bmp(pathname);
  Bitmap &bm = bmp;
  UINT width = bm.GetWidth(), height = bm.GetHeight();
  if (!width || !height) {
    puts(TEXT("ERROR: invalid picture file name.\n"));
    return;
  }
  puts(TEXT("OK: processing..."));
  // check if picture has alpha pixel
  for (UINT y = 0; y < height; y++) {
    for (UINT x = 0; x < width; x++) {
      Color color;
      bm.GetPixel(x, y, &color);
      if (color.GetAlpha() != 0xFF) { // has alpha pixel to nop
        puts(TEXT("has already alpha pixel.\n"));
        return;
      }
      // modify picture
      ARGB pxcol = color.GetValue();
      pxcol &= 0xFEFFFFFF;
      color.SetValue(pxcol);
      bm.SetPixel(x, y, color);
    }
  }
  // PNG {557CF406-1A04-11D3-9A73-0000F81EF32E}
  CLSID clsid = {0x557CF406, 0x1A04, 0x11D3,
    {0x9A, 0x73, 0x00, 0x00, 0xF8, 0x1E, 0xF3, 0x2E}};
  TCHAR newpath[0x1000];
  wsprintf(newpath, TEXT("%s-alpha.png"), pathname);
  bm.Save(newpath, &clsid, NULL);
  puts(TEXT("\ndone.\n"));
}

int main(HINSTANCE hi, HINSTANCE hp, LPSTR cl, int cs) {
  // GDI+
  ULONG_PTR token;
  GdiplusStartupInput input;
  GdiplusStartup(&token, &input, NULL);

  int argc;
  LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (argc < 2) {
    puts(TEXT("ERROR: no input.\n"));
    return 0;
  }

  // main
  for (int i = 1; i < argc; i++) {
    TCHAR str[0x400];
    wsprintf(str, TEXT("> %s\n"), argv[i]);
    puts(str);
    convert(argv[i]);
    puts(TEXT("\n"));
  }

  // finish
  GdiplusShutdown(token);
  return 0;
}

void __start__() {
  ExitProcess(main(GetModuleHandle(NULL), 0, NULL, 0));
}
