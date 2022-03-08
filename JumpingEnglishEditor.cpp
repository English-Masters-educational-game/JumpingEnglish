// JumpingEnglishEditor.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include <windows.h>
#include <sstream>
#include <string> 
using namespace std;

#include <gdiplus.h>
using namespace Gdiplus;

#pragma comment(lib, "gdiplus")

#include "framework.h"
#include "JumpingEnglishEditor.h"
#include "character.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

#define ID_VSCROLL 100
#define IDM_OPEN 300
#define IDM_SAVE 400

HWND hVScroll;
int iVScrollPos;

int max_tile_x = 10;
int max_tile_y = 20;

int tile_pos_x = 0;
int tile_pos_y = 0;

int map_block[20][10] = {0};
int map_word[20][10] = {0};

Character char1;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
    ULONG_PTR gpToken;
    GdiplusStartupInput gpsi;

    if (GdiplusStartup(&gpToken, &gpsi, NULL) != Ok)
    {
        MessageBox(NULL, L"GDI+ 라이브러리를 초기화할 수 없습니다.", L"알림", MB_OK);
        return 0;
    }

    //is_block = new int* [max_width];
    //for (int j = 0; j < max_width; j++)
    //{
    //    is_block[j] = new int[max_height];
    //}

    for (int i = 0; i < max_tile_y; i++)
    {
        for (int j = 0; j < max_tile_x; j++)
        {
            map_block[j][i] = 0;
        }
    }

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_JUMPINGENGLISHEDITOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_JUMPINGENGLISHEDITOR));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    GdiplusShutdown(gpToken);

    //for (int j = 0; j < max_width; j++)
    //{
    //    delete [] is_block[j];
    //}
    //delete [] is_block;

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JUMPINGENGLISHEDITOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_JUMPINGENGLISHEDITOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


// 렌더링
HBITMAP hBit;
void OnPaint(HWND hWnd, HDC hdc, PAINTSTRUCT& ps)
{
    HDC hMemDC;
    HBITMAP OldBit;

    RECT crt;
    GetClientRect(hWnd, &crt);
    
    if (hBit == NULL) 
    {
        hBit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
    }
    
    hMemDC = CreateCompatibleDC(hdc);
    OldBit = (HBITMAP)SelectObject(hMemDC, hBit);

    Graphics G(hMemDC);
    Pen P(Color(255, 255, 0, 0), 2);

    Image* image = Image::FromFile(L"blocks.png");
    Image* word_box = Image::FromFile(L"word_box.png");

    G.Clear(Color(255, 255, 255, 255));

    // 블럭 라인
    for (int i = 0; i <= max_tile_x; i++)
    {
        G.DrawLine(&P, 32 * i, 0, 32 * i, 32 * max_tile_y);
        for (int j = 0; j <= max_tile_y; j++)
        {
            G.DrawLine(&P, 0, 32*j, 32 * max_tile_x, 32*j);
        }
    }

    for (int j = 0; j < max_tile_y; j++)
    {
        for (int i = 0; i < max_tile_x; i++)
        {
            if (map_block[j][i] > 0)
            {
                Rect rect;
                rect.X = 0 + 32 * i;
                rect.Y = 0 + 32 * j;
                rect.Width = 32;
                rect.Height = 32;

                G.DrawImage(image, rect, 0, 0, 32, 32, UnitPixel);
            }
        }
    }


    // 캐릭터
    Image* character1 = Image::FromFile(L"character.png");

    Rect rect;
    rect.X = char1.x;
    rect.Y = max_tile_y * 32 - char1.y;
    rect.Width = 32;
    rect.Height = 48;

    G.DrawImage(character1, rect, 0, 0, 32, 48, UnitPixel);

    // image.png 파일을 이용하여 Image 객체를 생성합니다.

    // 블럭 이미지
    G.DrawImage(image, 800, 100, 256, 192);

    //for (int i = 0; i <= 8; i++)
    //{
    //    G.DrawLine(&P, 800 + 32 * i, 100, 800 + 32 * i, 100 + 32 * 6);
    //    for (int j = 0; j <= 6; j++)
    //    {
    //        G.DrawLine(&P, 800, 100 + 32 * j, 800 + 32 * 8, 100 + 32 * j);
    //    }
    //}

    // 선택 블럭 선택
    G.DrawRectangle(&P, Rect(800 + 32 * tile_pos_x, 100 + 32 * tile_pos_y, 32, 32));


    // 박스 이미지
    G.DrawImage(word_box, 800+8, 300, 16, 16);

    // 메모리비트를 hdc비트로 블리트
    BitBlt(hdc, 0, 0, crt.right, crt.bottom, hMemDC, 0, 0, SRCCOPY);

    // 데이터 메모리 해제
    delete image;
    delete word_box;
    delete character1;

    SelectObject(hMemDC, OldBit);
    DeleteDC(hMemDC);
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

#define ID_EDIT 100
#define ID_EDIT1 101
HWND hEdit;
_TCHAR str[128] = L"0";

HWND hEdit1;
_TCHAR str1[128] = L"0";

#define ID_BUTTON 0

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    int TempPos;
    stringstream output;
    string text;

    OPENFILENAME OFN;
    //TCHAR str[300];
    TCHAR lpstrFile[MAX_PATH] = L"";

    //HDC hdc = GetDC(hWnd);
    //ReleaseDC(hWnd, hdc);

    time_t mytime;
    static HANDLE hTimer;

    switch (message)
    {
    case WM_TIMER:
            //time(&mytime);
            //str = ctime(&mytime);
        //char1.down();
        //InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_LEFT:
            char1.left();
            break;
        case VK_RIGHT:
            char1.right();
            break;
        case VK_UP:
            char1.jump();
            break;
        case VK_DOWN:
            char1.down();
            break;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_CREATE:
        // 스크롤
        hVScroll = CreateWindow(L"scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_VERT,
            400, 10, 20, 200, hWnd, (HMENU)ID_VSCROLL, hInst, NULL);
        SetScrollRange(hVScroll, SB_CTL, 0, 255, TRUE);
        SetScrollPos(hVScroll, SB_CTL, 0, TRUE);

        hTimer = (HANDLE)SetTimer(hWnd, 1, 1000, NULL);

        //// 에디트
        //hEdit = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 
        //    500, 100, 100, 50, hWnd, (HMENU)ID_EDIT, hInst, NULL);

        //hEdit1 = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        //    650, 100, 100, 50, hWnd, (HMENU)ID_EDIT1, hInst, NULL);

        //// 버튼
        //CreateWindow(TEXT("button"), TEXT("Click Me"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        //    500, 200, 100, 50, hWnd, (HMENU)ID_BUTTON, hInst, NULL);

        // 파일 열기
        CreateWindow(TEXT("button"), TEXT("Open"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            500, 0, 100, 50, hWnd, (HMENU)IDM_OPEN, hInst, NULL);

        // 파일 저장
        CreateWindow(TEXT("button"), TEXT("Save"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            600, 0, 100, 50, hWnd, (HMENU)IDM_SAVE, hInst, NULL);

        return 0;

    case WM_HSCROLL:
        if ((HWND)lParam == hVScroll) TempPos = iVScrollPos;
        switch (LOWORD(wParam)) {
        case SB_LINELEFT:
            TempPos = max(0, TempPos - 1);
            break;
        case SB_LINERIGHT:
            TempPos = min(255, TempPos + 1);
            break;
        case SB_PAGELEFT:
            TempPos = max(0, TempPos - 10);
            break;
        case SB_PAGERIGHT:
            TempPos = min(255, TempPos + 10);
            break;
        case SB_THUMBTRACK:
            TempPos = HIWORD(wParam);
            break;
        }
        if ((HWND)lParam == hVScroll) iVScrollPos = TempPos;
        SetScrollPos((HWND)lParam, SB_CTL, TempPos, TRUE);
        InvalidateRect(hWnd, NULL, FALSE);
        break;

    case WM_MOUSEMOVE:
        //output << LOWORD(lParam) << ", " << HIWORD(lParam);
        //text = output.str();
        //InvalidateRect(hWnd, NULL, false);
        //UpdateWindow(hWnd);
        break;
    case WM_LBUTTONDOWN:
        //text = "Left button down!";
        //output << LOWORD(lParam) << ", " << HIWORD(lParam);
        //text = output.str();
        
    {
        TCHAR buf[80] = { 0 };
        int mouse_x = LOWORD(lParam);
        int mouse_y = HIWORD(lParam);
        wsprintf(buf, L"x: %d, y: %d \n", mouse_x, mouse_y);
        OutputDebugString(buf);


        // 온보드

        if ((mouse_y > 0) && (mouse_y < (max_tile_y * 32)))
        {
            if ((mouse_x > 0) && (mouse_x < (max_tile_x * 32)))
            {
                
                int x = mouse_x / 32;
                int y = mouse_y / 32;
                map_block[y][x] = 1; // 런타임 예외 발생
            }
        }

    }   
        InvalidateRect(hWnd, NULL, FALSE);
        UpdateWindow(hWnd);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        OnPaint(hWnd, hdc, ps);

        EndPaint(hWnd, &ps);
        break;

    //case WM_KEYDOWN:
    //    if (wParam == VK_SPACE)
    //    {
    //        char1.jump();
    //    }

    //    InvalidateRect(hWnd, NULL, FALSE);
    //    break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            //case ID_EDIT:
            //    switch (HIWORD(wParam))
            //    {
            //    case EN_CHANGE:
            //        GetWindowText(hEdit, str, 128);
            //        SetWindowText(hWnd, str);
            //    }
            //    break;

            //case ID_EDIT1:
            //    switch (HIWORD(wParam))
            //    {
            //    case EN_CHANGE:
            //        GetWindowText(hEdit1, str1, 128);
            //        SetWindowText(hWnd, str1);
            //    }
            //    break;

            //case ID_BUTTON:                
            //    max_width = stoi(str);
            //    max_height = stoi(str1);
            //    
            //    InvalidateRect(hWnd, NULL, true);

            //    break;

            case IDM_OPEN:
                memset(&OFN, 0, sizeof(OPENFILENAME));
                OFN.lStructSize = sizeof(OPENFILENAME);
                OFN.hwndOwner = hWnd;
                OFN.lpstrFilter = L"Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0";
                OFN.lpstrFile = lpstrFile;
                OFN.nMaxFile = 256;
                OFN.lpstrInitialDir = L".";
                if (GetOpenFileName(&OFN) != 0) {
                    wsprintf(str, L"%s 파일을 선택했습니다.", OFN.lpstrFile);
                    MessageBox(hWnd, str, L"파일 열기 성공", MB_OK);
                }
                break;

            case IDM_SAVE:
                memset(&OFN, 0, sizeof(OPENFILENAME));
                OFN.lStructSize = sizeof(OPENFILENAME);
                OFN.hwndOwner = hWnd;
                OFN.lpstrFilter = L"Every file(*.*) \0*.*\0TextFile\0*.txt;*.doc\0";
                OFN.lpstrFile = lpstrFile;
                OFN.nMaxFile = 256;
                OFN.lpstrInitialDir = L".";

                if (GetSaveFileName(&OFN) != 0)
                {
                    wsprintf(str, L"%s 파일 저장하시겠습니까?", OFN.lpstrFile);
                    MessageBox(hWnd, str, L"저장하기 선택", MB_OK);
                }

                break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_DESTROY:

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
