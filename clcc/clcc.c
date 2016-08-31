//Clipboard char counter



//#define UNICODE  
#define _UNICODE

#include <tchar.h> 
#include <windows.h>
#include <shellapi.h>

#define ID_BUTTON1 3001
#define ID_BUTTON2 3002
#define ID_TRAY_APP_ICON                5000
#define ID_TRAY_EXIT_CONTEXT_MENU_ITEM  3000
#define WM_TRAYICON ( WM_USER + 1 )

#define TRAYSIZE  16
#define BUFLEN 110
#define LIMIT 99
#define SZTIP_LEN 64
#define IND_LEN 2

HICON CreateSmallIcon(HWND hwnd, TCHAR * str)
{
    TCHAR *szText = str;
    HDC hdc, hdcMem;
    HBITMAP hBitmap = NULL;
    HBITMAP hOldBitMap = NULL;
    HBITMAP hBitmapMask = NULL;
    ICONINFO iconInfo;
    HFONT hFont;
    HICON hIcon;

    hdc = GetDC (hwnd);
    hdcMem = CreateCompatibleDC ( hdc );
    hBitmap = CreateCompatibleBitmap ( hdc, TRAYSIZE, TRAYSIZE );
    hBitmapMask = CreateCompatibleBitmap ( hdc, TRAYSIZE, TRAYSIZE   );
    ReleaseDC (hwnd, hdc);
    hOldBitMap = (HBITMAP) SelectObject ( hdcMem, hBitmap );
    //PatBlt ( hdcMem, 0, 0, 16, 16, WHITENESS );
    PatBlt ( hdcMem, 0, 0, TRAYSIZE, TRAYSIZE, BLACKNESS );

    hFont = CreateFont (17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT ("Arial"));

    hFont = (HFONT) SelectObject ( hdcMem, hFont );
    TextOut ( hdcMem, 0, 0, szText, lstrlen (szText) );

    SelectObject ( hdc, hOldBitMap );
    hOldBitMap = NULL;

    iconInfo.fIcon = TRUE;
    iconInfo.xHotspot = 0;
    iconInfo.yHotspot = 0;
    iconInfo.hbmMask = hBitmapMask;
    iconInfo.hbmColor = hBitmap;

    hIcon = CreateIconIndirect ( &iconInfo );

    DeleteObject ( SelectObject ( hdcMem, hFont ) );
    DeleteDC ( hdcMem );
    DeleteDC ( hdc );
    DeleteObject ( hBitmap );
    DeleteObject ( hBitmapMask );

    return hIcon;
}

 
void InitNotifyIconData(NOTIFYICONDATA * notifyIconData, HWND hwnd,  TCHAR * s1, TCHAR * s2)
{
    memset( notifyIconData, 0, sizeof( NOTIFYICONDATA ) ) ;

    notifyIconData->cbSize = sizeof(NOTIFYICONDATA);
    notifyIconData->hWnd = hwnd;
    notifyIconData->uID = ID_TRAY_APP_ICON;
    notifyIconData->uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; 
    notifyIconData->uCallbackMessage = WM_TRAYICON;

    notifyIconData->hIcon = CreateSmallIcon(hwnd, s1);
    wcsncpy(notifyIconData->szTip,  s2, SZTIP_LEN);  //strlen of szTip is 64 according to MSDN

    Shell_NotifyIcon(NIM_ADD, notifyIconData);
    DestroyIcon(notifyIconData->hIcon );
}

void Redraw(NOTIFYICONDATA * notifyIconData, HWND hwnd, TCHAR * s1,  TCHAR * s2)
{
    if (notifyIconData->hWnd == hwnd)
    {
        notifyIconData->hIcon = CreateSmallIcon(hwnd, s1 );
        wcsncpy(notifyIconData->szTip,  s2, SZTIP_LEN);
        Shell_NotifyIcon(NIM_MODIFY, notifyIconData);
        DestroyIcon(notifyIconData->hIcon );
    }
}


UINT WM_TASKBARCREATED = 0 ;
HWND g_hwnd ;
HMENU g_menu ;
NOTIFYICONDATA g_notifyIconData ;

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndNextViewer;
    HGLOBAL hglb; 
    size_t  l1 =0;
    size_t  len=0;   
    TCHAR buf [IND_LEN + 1];
    TCHAR wstr[BUFLEN + 1];
    TCHAR * clpbrd_str;
    UINT uFormat = (UINT)(-1); 
    int i,j;

    if ( message==WM_TASKBARCREATED && !IsWindowVisible( hwnd ) )
    {
        return 0;
    }

    switch (message)
    {
        case WM_CREATE:
            hwndNextViewer = SetClipboardViewer(hwnd); 

            g_menu = CreatePopupMenu();
            AppendMenu(g_menu, MF_STRING, ID_TRAY_EXIT_CONTEXT_MENU_ITEM,  TEXT( "Exit" ) );
            break;

        case WM_CHANGECBCHAIN: 
            //If the next window is closing, repair the chain. 
            if ((HWND) wParam == hwndNextViewer) 
                hwndNextViewer = (HWND) lParam; 

            // Otherwise, pass the message to the next link. 
            else if (hwndNextViewer != NULL) 
                SendMessage(hwndNextViewer, message, wParam, lParam); 
            break;
        case WM_DRAWCLIPBOARD:  // clipboard contents changed. 
            // Update the window by using Auto clipboard format. 
            // Pass the message to the next window in clipboard 
            // viewer chain. 
            SendMessage(hwndNextViewer, message, wParam, lParam); 
            //static UINT auPriorityList[] = { CF_TEXT }; 
            static UINT auPriorityList[] = { CF_UNICODETEXT }; 
            uFormat = GetPriorityClipboardFormat(auPriorityList, 4); 
            if ( uFormat == CF_UNICODETEXT )
            {
                if (OpenClipboard(hwnd)) 
                { 

                    hglb = GetClipboardData(uFormat); 
                    clpbrd_str = GlobalLock(hglb); 
                    //MultiByteToWideChar( CP_ACP, 0, ansi_str, -1, wstr, BUFLEN);
                    //wcsncpy ( wstr, clpbrd_str, BUFLEN);
                    l1 = wcslen (clpbrd_str);

                    j=0;
                    for (i=0; i<=l1; i++)
                    {
                        if (j >= BUFLEN)
                        { 
                            wstr[j]='\0';
                            break;
                        }
                        if ( clpbrd_str[i] != TEXT('\r') && clpbrd_str[i] != TEXT('\n')  )
                        {
                            wstr[j] = clpbrd_str[i];
                            j++;
                        }
                    }

                    len = wcslen (wstr);

                    if (len > LIMIT )
                    {
                        wstr[BUFLEN] = '\0';
                        wsprintf( buf, TEXT(">L") );
                    }
                    else
                    {
                        wsprintf(buf, TEXT("%02d"), len);
                    }

                    Redraw(&g_notifyIconData, hwnd, buf, wstr );
                    //wprintf(TEXT("%s: %s \n"), buf,  wstr );

                    GlobalUnlock(hglb); 
                    CloseClipboard(); 
                } 
            }
            break; 

        case WM_SYSCOMMAND:
            switch( wParam & 0xfff0 ) 
            {
                case SC_MINIMIZE:
                case SC_CLOSE: 
                    ShowWindow(hwnd, SW_HIDE);
                    return 0 ;
                    break;
            }
            break;

        case WM_COMMAND:
            {
                if (LOWORD(wParam) == ID_BUTTON1)
                {
                    DestroyWindow(hwnd);
                }
                else if (LOWORD(wParam) == ID_BUTTON2)
                {
                    ShowWindow(hwnd, SW_HIDE);
                }
                break;
            }


        case WM_TRAYICON:
            {
                if (lParam == WM_LBUTTONUP)
                {
                    if( !IsWindowVisible( hwnd ) )
                    {
                        ShowWindow(hwnd, SW_SHOW);
                    }else{
                        ShowWindow(hwnd, SW_HIDE);
                    }

                }
                else if (lParam == WM_RBUTTONDOWN) 
                {
                    POINT curPoint ;
                    GetCursorPos( &curPoint ) ;

                    SetForegroundWindow(hwnd); 

                    UINT clicked = TrackPopupMenu(
                            g_menu,
                            TPM_RETURNCMD | TPM_NONOTIFY, 
                            curPoint.x, curPoint.y, 0,
                            hwnd, NULL);

                    if (clicked == ID_TRAY_EXIT_CONTEXT_MENU_ITEM)
                    {
                        DestroyWindow(hwnd);
                        //PostQuitMessage( 0 ) ;
                    }
                }
            }
            break;

        case WM_NCHITTEST:
            {
                UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
                if(uHitTest == HTCLIENT)
                    return HTCAPTION;
                else
                    return uHitTest;
            }

        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            return 0;
            break;

        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
            ChangeClipboardChain(hwnd, hwndNextViewer); 
            PostQuitMessage (0);
            break;

    }

    return DefWindowProc( hwnd, message, wParam, lParam ) ;
}


int WINAPI wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR args, int iCmdShow )
{
    TCHAR className[] = TEXT("tray icon class");

    WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated") ;

    HWND hWndGroupBox;
    HWND hWndGroupBox1;

    WNDCLASSEX wnd = { 0 };

    wnd.hInstance = hInstance;
    wnd.lpszClassName = className;
    wnd.lpfnWndProc = WndProc;
    wnd.style = CS_HREDRAW | CS_VREDRAW ;
    wnd.cbSize = sizeof (WNDCLASSEX);

    wnd.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wnd.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wnd.hCursor = LoadCursor (NULL, IDC_ARROW);
    wnd.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE ;

#ifdef REVISION
    TCHAR rev[] = TEXT ( "Revision: "REVISION );
#elif
    TCHAR rev[] = TEXT ( "Revision: not available" );
#endif

#ifdef OS 
    TCHAR os[] = TEXT ( "OS: "OS );
#elif
    TCHAR os[] = TEXT ( "OS: not available" );
#endif

#ifdef  TODAY
    TCHAR today[] = TEXT ( "Date: "TODAY );
#elif
    TCHAR today[] = TEXT ( "Date: not available" );
#endif

#ifdef  COMPILER
    TCHAR compiler[] = TEXT ( "Compiler: "COMPILER );
#elif
    TCHAR compiler[] = TEXT ( "Compiler: not availabe" );
#endif

    TCHAR desc[] = TEXT("Description: \n  The clcc app was designed in the ATP team situated \nin Petrozavodsk. You can get source code of clcc under \nthe terms of GPLv3. Visit http://github.com/chuharev/ATP/ \nfor more information.");


    FreeConsole();

    if (!RegisterClassEx(&wnd))
    {
        FatalAppExit( 0, TEXT("Couldn't register window class!") );
    }

    g_hwnd = CreateWindowEx ( 0, className, TEXT( "Clipboard Char Counter" ),
            WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 
            415, 370, NULL, NULL, hInstance, NULL); 

    SetWindowLong(g_hwnd, GWL_STYLE, GetWindowLong(g_hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX);


    hWndGroupBox1 = CreateWindow(TEXT("BUTTON"), TEXT("General info"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            5, 5, 400, 140, g_hwnd, 0, hInstance, NULL);


    CreateWindow( TEXT("static"), TEXT("Author: A. Chuharev"), 
            WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 20, 170, 20, hWndGroupBox1, 0, hInstance, NULL ) ;

    CreateWindow( TEXT("static"), desc, 
            WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 40, 385, 80, hWndGroupBox1, 0, hInstance, NULL ) ;


    hWndGroupBox = CreateWindow(TEXT("BUTTON"), TEXT("Build info"), WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            5, 150, 320, 120, g_hwnd, 0, hInstance, NULL);

    CreateWindow( TEXT("static"), rev, 
            WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 20, 170, 20, hWndGroupBox, 0, hInstance, NULL ) ;

    CreateWindow( TEXT("static"), os, 
            WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 40, 170, 20, hWndGroupBox, 0, hInstance, NULL ) ;

    CreateWindow( TEXT("static"), compiler, 
            WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 60, 250, 20, hWndGroupBox, 0, hInstance, NULL ) ;

    CreateWindow( TEXT("static"), today, 
            WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 80, 150, 20, hWndGroupBox, 0, hInstance, NULL ) ;




    CreateWindow(TEXT("BUTTON"), TEXT("Exit"), WS_TABSTOP | BS_DEFPUSHBUTTON|  WS_CHILD | WS_VISIBLE,
     100, 300, 80, 30, g_hwnd,(HMENU)ID_BUTTON1, hInstance, NULL);

    CreateWindow(TEXT("BUTTON"), TEXT("Minimize"), WS_TABSTOP | BS_DEFPUSHBUTTON|  WS_CHILD | WS_VISIBLE,
            200, 300, 80, 30, g_hwnd,(HMENU)ID_BUTTON2, hInstance, NULL);


    ShowWindow(g_hwnd, SW_HIDE);
    //ShowWindow(g_hwnd, SW_SHOW);

    InitNotifyIconData(&g_notifyIconData, g_hwnd, TEXT("HI!"), TEXT("Clipboard Char Counter") );

    MSG msg ;
    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);

    return msg.wParam;
}

