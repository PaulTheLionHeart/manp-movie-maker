/*-----------------------------------------
	ManpMovieMaker.CPP 
	ManpWIN Movie File Sequence Generation Program Written by Paul de Leeuw
  -----------------------------------------*/

#ifdef UNICODE
#pragma message("ManpMovieMaker.cpp compiled as UNICODE")
#else
#pragma message("ManpMovieMaker.cpp compiled as ANSI")
#endif

#include <windows.h>
#include <Winuser.h>
#include <commdlg.h>
#include <commctrl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <setjmp.h>
#include <string.h>
#include <stdarg.h>
#include <shlobj.h>
#include "resource.h"
#include "Dib.h"
#include "View.h"
#include "Screen.h"
#include "DibMacro.h"

#define	 MAX_INSERTED_FRAMES	10

extern	void	InitializeScrollBars(HWND, RECT *, RECT *);
extern	int	decode_png_header(HWND hwnd, char *infile, char *szAppName);
extern	int	png_decoder(HWND hwnd, char *szAppName, char *infile, double  *Zoom, bool UseComment, CDib *TargetDib);
extern "C" void	HandleJPEGError(void);
extern "C" int	SaveJPEG(HWND, FILE *, BYTE *, BOOL, int, WORD *, WORD *, WORD *);
extern "C"	jmp_buf	mark;				// Address for long jump to jump to when an error occurs

extern	void	InitFileList(void);
extern	int	LoadFileList(HWND hwnd, char *Filename);
extern	void	SortFileList(void);
extern	int	ResizeDib(CDib *LocalDib, ResizeStruct RESIZE, BOOL InterpFlag);
extern	int	Crop(HWND hwnd, CDib *pDib, RECT & SelectRect);
extern	void	WINAPI	NormalizeRect(RECT &);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK AboutBoxDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SpecifyImageFileDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK CreateFFMPEGCommandDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void	ChangeView(HWND hwnd, int xdest, int ydest, int widthdest, int heightdest, int xsrc, int ysrc, int widthsrc, int heightsrc, char stretchflag);
char 	szAppName [100] = "ManpMovieMaker";
static	HANDLE	hBitmap = NULL;					// opening bitmap
static	HDC	hdcMem;
static	BITMAP	bm;
static	LOGFONT	lf;

int	GenerateFileSequence(HWND hwnd, char *Filename, int NumInsertedFrames);

enum	JustifyKind	{LEFT, CENTRE, RIGHT, JUSTIFY};
static	JustifyKind	Justification;

int	NumInsertedFrames = 1;
int	FramesPerSecond = 25;
int	StartSeconds = 2;
int	EndSeconds = 2;
int	TotalFrames = 0;
int	filecount;

double	StartZoom;						// this is used to calculate the current zoom level for display in ffmpeg 
double  ratio;							// ration between subsequent PNG files
double  FrameRatio;						// ration between subsequent frames

#define RWIDTH(rect) (rect.right - rect.left)	// IB 2009-04-14
#define RHEIGHT(rect) (rect.bottom - rect.top)

char		szFileName[MAX_PATH] = " ";
char		Name[160];
char		Time[160];

PAINTSTRUCT 	ps;
RECT 		r;
HPALETTE 	hpal = NULL;
StateType	State = NORMAL;

// Styles of app. window 
DWORD		dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;

int		AppIndex;
char		Filename[MAX_PATH]{};
char		OutputPath[MAX_PATH]{};
char		OutputFilename[MAX_PATH]{};
char		MovieFilename[MAX_PATH]{};
char		FullMovieFilename[MAX_PATH]{};
char		InputPath[MAX_PATH]{};
char		ffmpegCommand[1600]{};
char		AudioPath[MAX_PATH]{};
TCHAR 		MovieTitle[512]{};
TCHAR 		MovieSubTitle[512]{};

BOOL		LoopAudio = FALSE;
BOOL		UserDisplayZoom = FALSE;

int		FadeInSeconds = 0;
int		FadeOutSeconds = 5;

int		xdots, ydots, height, width, bits_per_pixel;
int		caption, scroll_width, display_width, display_height, HorOffset, VertOffset, screenx, screeny;

CDib		Dib;				// Device Independent Bitmap class instance
HWND		hwnd;

extern	struct FileListStuff FileList[MAXFILES];	// info for sorting file list

/*-----------------------------------------
	Main Windows Entry Point
  -----------------------------------------*/

int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
		    LPSTR lpszCmdLine, int nCmdShow)
     {
     MSG      msg;
     WNDCLASS wndclass;

     if (!hPrevInstance)
	  {
	  wndclass.style	 = CS_HREDRAW | CS_VREDRAW;
//	  wndclass.style	 = CS_HREDRAW | CS_VREDRAW | CS_SAVEBITS;
	  wndclass.lpfnWndProc	 = WndProc;
	  wndclass.cbClsExtra	 = 0;
	  wndclass.cbWndExtra	 = 0;
	  wndclass.hInstance	 = hInstance;
//	  wndclass.hIcon	 = LoadIcon (NULL, IDI_APPLICATION);
	  wndclass.hIcon	 = LoadIcon (hInstance, szAppName);
	  wndclass.hCursor	 = LoadCursor (NULL, IDC_ARROW);
	  wndclass.hbrBackground = (HBRUSH)GetStockObject (WHITE_BRUSH);
	  wndclass.lpszMenuName  = szAppName;
	  wndclass.lpszClassName = szAppName;

	  RegisterClass (&wndclass);
	  }
     
     hwnd = CreateWindow (szAppName, "ManpWIN Frame Generator",
     			  dwStyle, 
			  160, 120, 				// nice initial location
			  500, 300, 				// nice initial size
			  NULL, NULL, hInstance, NULL);

     ShowWindow (hwnd, nCmdShow);
     UpdateWindow (hwnd);

     while (GetMessage (&msg, NULL, 0, 0))
	  {
	  TranslateMessage (&msg);
	  DispatchMessage (&msg);
	  }

     return (int)msg.wParam;
     }

/*-----------------------------------------
	Draw Opening Bitmap
  -----------------------------------------*/

void	DrawBitmap(HWND hwnd, HDC hdc, HANDLE hBitmap)

    {
    DWORD	ErrorCode;
    char	s[180];

    hdcMem = CreateCompatibleDC(hdc);
    if (SelectObject(hdcMem, hBitmap) == NULL)
	{
	ErrorCode = GetLastError();
	sprintf(s, "SelectObject Error type = %ld", ErrorCode);
	MessageBox (hwnd, s, szAppName, MB_ICONEXCLAMATION | MB_OK);
	}
    SetMapMode(hdcMem, GetMapMode(hdc));
    if (GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm) == 0)
	{
	ErrorCode = GetLastError();
	sprintf(s, "GetObject Error type = %ld", ErrorCode);
	MessageBox (hwnd, s, szAppName, MB_ICONEXCLAMATION | MB_OK);
	}
    if (hBitmap)
	{
	DeleteObject(hBitmap);
	hBitmap = NULL;
	}
    MoveWindow (hwnd, 160, 120, bm.bmWidth, bm.bmHeight, TRUE);
    ShowWindow (hwnd, SW_SHOWNORMAL);
    InvalidateRect(hwnd, NULL, FALSE);      
    }

/*-----------------------------------------
	Let's Process a few messages
  -----------------------------------------*/

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
    static HINSTANCE	hInst;
    static HCURSOR	hCursor;
    static HBRUSH	hBrush;
    HDC			hdc;
    DWORD		ErrorCode;
    static HWND		hScroll ;
    PAINTSTRUCT		ps ;
    char		s[180];

    switch (message)
	{
	case WM_CREATE:
	    hInst = ((LPCREATESTRUCT)(_int64)lParam)->hInstance;
	    hdc = GetDC(hwnd);
	    hBitmap = LoadImage(hInst, MAKEINTRESOURCE(MANPMOVIEMAKER_BMP), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR/*LR_LOADFROMFILE*/);
	    DrawBitmap(hwnd, hdc, hBitmap);		// cute opening bitmap
	    BeginPaint(hwnd, &ps);
	    if (BitBlt(ps.hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY) == 0)
		{
		ErrorCode = GetLastError();
		sprintf(s, "Error type = %ld", ErrorCode);
		MessageBox(hwnd, s, szAppName, MB_ICONEXCLAMATION | MB_OK);
		}
	    MoveWindow(hwnd, 160, 120, bm.bmWidth, bm.bmHeight, TRUE);
	    ShowWindow(hwnd, SW_SHOWNORMAL);
	    EndPaint(hwnd, &ps);
	    return 0;

	case WM_INITMENUPOPUP:
	    switch (lParam)
		{
		case 0:	   // File menu
		    EnableMenuItem((HMENU)(_int64)wParam, IDM_SETUPFILEINFO, MF_ENABLED);
		    EnableMenuItem((HMENU)(_int64)wParam, IDM_FFMPEG, MF_ENABLED);
		    EnableMenuItem ((HMENU)(_int64)wParam, IDM_EXIT, MF_ENABLED);
		    break;

		case 1:	   // Help menu
		    EnableMenuItem ((HMENU)(_int64)wParam, IDM_ABOUT, MF_ENABLED);
		    break;
		}
	    return 0;

	  case WM_COMMAND:
	       switch (wParam)
		    {
		    case IDM_EXIT:
			SendMessage (hwnd, WM_CLOSE, 0, 0L);
			return 0;

		    case IDM_SETUPFILEINFO:
			if (DialogBox (hInst, "SpecifyImageFileDlg", hwnd, SpecifyImageFileDlg) != IDOK)
			    return 0;
			else
			    GenerateFileSequence(hwnd, Filename, NumInsertedFrames);
			return 0;

		    case IDM_FFMPEG:
			DialogBox(hInst, "CreateFFMPEGCommandDlg", hwnd, CreateFFMPEGCommandDlg);
			return 0;

			// Messages from Help menu
		    case IDM_ABOUT:
			 DialogBox (hInst, "AboutBoxDlg", hwnd, AboutBoxDlg);
			 return 0;
		    }
	       break;

	  case WM_SIZE:
		InvalidateRect(hwnd, NULL, FALSE);      
		return 0;

	  case WM_PAINT:
		ChangeView(hwnd, -GetScrollPos(hwnd, SB_HORZ), -GetScrollPos(hwnd, SB_VERT), width, height, 0, 0, width, height, TRUE);
		return 0;

        case WM_KEYDOWN:					// Handle any keyboard messages
            switch (wParam) 
                {
                case VK_F1:						// Help    
		    PostMessage (hwnd, WM_COMMAND, IDM_ABOUT,   0L);
                    break;
                case 'f':						// Select Constant to display
                case 'F': 
		     SendMessage (hwnd, WM_COMMAND, IDM_FFMPEG, 0L);
                     break;
                case 's':						// Select Constant to display
                case 'S': 
		     SendMessage (hwnd, WM_COMMAND, IDM_SETUPFILEINFO, 0L);
                     break;
                case VK_RETURN:						// Let's get out of here
                case VK_ESCAPE: 
		    State = NORMAL;
		    SendMessage (hwnd, WM_CLOSE, 0, 0L);
                    break;
                }
            break;

	  case WM_DESTROY :
		PostQuitMessage (0);
	       	return 0;
	  }
     return (long)DefWindowProc (hwnd, message, wParam, lParam);
     }

/**************************************************************************
    Create output movie filename from first PNG filename

    InputFile       Full path of first PNG file
    OutputFile      Generated movie filename only
**************************************************************************/

void CreateOutputFilename(char *InputFile, char *MovieFilename)
    {
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char name[_MAX_FNAME];
    char ext[_MAX_EXT];
    char *ptr;

    if (MovieFilename)
	MovieFilename[0] = '\0';

    if (InputFile == NULL)
	return;

    _splitpath(InputFile, drive, dir, name, ext);

    ptr = name + strlen(name);

    while (ptr > name && isdigit((unsigned char)*(ptr - 1)))
	ptr--;

    *ptr = '\0';

    if (name[0] == '\0')
	strcpy(name, "Movie");

    sprintf(MovieFilename, "%s.mp4", name);
    }

/**************************************************************************
	Update estimates
**************************************************************************/

void  UpdateMovieMakerEstimates(HWND hDlg)
    {
    BOOL bTrans;

    int StartSeconds = GetDlgItemInt(hDlg, IDC_STARTSECONDS, &bTrans, TRUE);
    int EndSeconds = GetDlgItemInt(hDlg, IDC_ENDSECONDS, &bTrans, TRUE);
    int FPS = GetDlgItemInt(hDlg, IDC_FRAMES_PER_SEC, &bTrans, TRUE);
    int InsertedFrames = GetDlgItemInt(hDlg, IDC_NUMBER_INSERTED_FRAMES, &bTrans, TRUE);

    int StartFrames = StartSeconds * FPS;
    int EndFrames   = EndSeconds * FPS;

    int FramesPerImage = (InsertedFrames > 0) ? InsertedFrames : 1;
    int TotalFrames = StartFrames + (filecount * FramesPerImage) + EndFrames;
    double Duration = (FPS > 0) ? (double)TotalFrames / FPS : 0.0;

    SetDlgItemInt(hDlg, IDC_SOURCE_FILE_COUNT, filecount, FALSE);
    SetDlgItemInt(hDlg, IDC_TOTAL_OUTPUT_FRAMES, TotalFrames, FALSE);

    char s[64];
    sprintf(s, "%.1f", Duration);
    SetDlgItemText(hDlg, IDC_ESTIMATED_DURATION, s);
    }

/**************************************************************************
	Image files
**************************************************************************/

INT_PTR CALLBACK SpecifyImageFileDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
     BOOL		bTrans;
     char		*ptr;
     HWND		hCtrl;
     char		drive0[_MAX_DRIVE], dir0[_MAX_DIR], name0[_MAX_FNAME], ext0[_MAX_EXT];
     OPENFILENAME	ofn{};

     switch (message)
	  {
	  case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_FIRSTFILE, Filename);
		SetDlgItemText(hDlg, IDC_OUTPUTPATH, OutputPath);
		SetDlgItemText(hDlg, IDC_OUTPUTFILENAME, OutputFilename);
		SetDlgItemInt(hDlg, IDC_NUMBER_INSERTED_FRAMES, NumInsertedFrames, TRUE);
		SetDlgItemInt(hDlg, IDC_STARTSECONDS, StartSeconds, TRUE);
		SetDlgItemInt(hDlg, IDC_ENDSECONDS, EndSeconds, TRUE);
		SetDlgItemInt(hDlg, IDC_FRAMES_PER_SEC, FramesPerSecond, TRUE);
		SetDlgItemText(hDlg, IDC_SOURCE_FILE_COUNT, "?");
		SetDlgItemText(hDlg, IDC_TOTAL_OUTPUT_FRAMES, "?");
		SetDlgItemText(hDlg, IDC_ESTIMATED_DURATION, "?");
	        return FALSE ;

	  case WM_COMMAND:
	        switch ((int) LOWORD(wParam))
		    {
		    case IDC_BROWSE_FIRSTFILE:
			ZeroMemory(&ofn, sizeof(ofn));

			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = Filename;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = "PNG Files (*.png)\0*.png\0" "All Files (*.*)\0*.*\0";
			ofn.nFilterIndex = 1;

			ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			if (GetOpenFileName(&ofn))
			    {
			    char PathName[MAX_PATH];
			    char drive[_MAX_DRIVE];
			    char dir[_MAX_DIR];
			    char name[_MAX_FNAME];
			    char ext[_MAX_EXT];
			    SetDlgItemText(hDlg, IDC_FIRSTFILE, Filename);
			    if (GetDlgItemText(hDlg, IDC_OUTPUTPATH, PathName, MAX_PATH) == 0)
				{
				_splitpath(Filename, drive, dir, name, ext);
				_makepath(PathName, drive, dir, "", "");
				SetDlgItemText(hDlg, IDC_OUTPUTPATH, PathName);
				}
			    CreateOutputFilename(Filename, MovieFilename);
			    LoadFileList(hDlg, Filename);
			    SortFileList();
			    UpdateMovieMakerEstimates(hDlg);
			    }
			break;

		    case IDC_NUMBER_INSERTED_FRAMES:
		    case IDC_FRAMES_PER_SEC:
		    case IDC_STARTSECONDS:
		    case IDC_ENDSECONDS:
			if (HIWORD(wParam) == EN_CHANGE)
			    UpdateMovieMakerEstimates(hDlg);
			break;

		    case IDC_DISPLAY_ZOOM_LEVEL:
			hCtrl = GetDlgItem(hDlg, IDC_DISPLAY_ZOOM_LEVEL);
			UserDisplayZoom = (BOOL)SendMessage(hCtrl, BM_GETCHECK, 0, 0L);
			break;

		    case IDC_BROWSE_OUTPUTPATH:
			{
			BROWSEINFO bi{};
			LPITEMIDLIST pidl;
			char PathName[MAX_PATH]{};

			bi.hwndOwner = hDlg;
			bi.lpszTitle = "Select output folder for JPG frames";
			pidl = SHBrowseForFolder(&bi);

			if (pidl != NULL)
			    {
			    if (SHGetPathFromIDList(pidl, PathName))
				SetDlgItemText(hDlg, IDC_OUTPUTPATH, PathName);
			    CoTaskMemFree(pidl);
			    }
			break;
			}

		    case IDOK:
			StartSeconds = GetDlgItemInt(hDlg, IDC_STARTSECONDS, &bTrans, TRUE);
			if (!bTrans || StartSeconds < 0)
			    StartSeconds = 0;
			EndSeconds = GetDlgItemInt(hDlg, IDC_ENDSECONDS, &bTrans, TRUE);
			if (!bTrans || EndSeconds < 0)
			    EndSeconds = 0;
			FramesPerSecond = GetDlgItemInt(hDlg, IDC_FRAMES_PER_SEC, &bTrans, TRUE);
			if (!bTrans || FramesPerSecond < 10)
			    FramesPerSecond = 10;
			if (FramesPerSecond > 120)
			    FramesPerSecond = 120;
			NumInsertedFrames = GetDlgItemInt(hDlg, IDC_NUMBER_INSERTED_FRAMES, &bTrans, TRUE);
			if (!bTrans || NumInsertedFrames < 1)
			    NumInsertedFrames = 1;
			if (NumInsertedFrames > MAX_INSERTED_FRAMES)
			    NumInsertedFrames = MAX_INSERTED_FRAMES;
			GetDlgItemText(hDlg, IDC_FIRSTFILE, Filename, MAX_PATH);
			GetDlgItemText(hDlg, IDC_OUTPUTPATH, OutputPath, MAX_PATH);
			GetDlgItemText(hDlg, IDC_OUTPUTFILENAME, OutputFilename, MAX_PATH);
			GetDlgItemText(hDlg, IDC_OPENING_TEXT, MovieTitle, 1024);
			GetDlgItemText(hDlg, IDC_OPENING_SUBTEXT, MovieSubTitle, 1024);

			_splitpath(Filename, drive0, dir0, name0, ext0);
			_makepath(InputPath, drive0, dir0, "", "");

			if (!*OutputPath)
			    strcpy(OutputPath, InputPath);

			ptr = OutputPath + strlen(OutputPath) - 1;		// remove final backslash if it exists so we can add it later
			if (*ptr == '\\')
			    *ptr = '\0';
			ptr = InputPath + strlen(InputPath) - 1;		// remove final backslash if it exists so we can add it later
			if (*ptr == '\\')
			    *ptr = '\0';
//			hCtrl = GetDlgItem(hDlg, IDC_USESCALING);
//			UseScaling = (BOOL)SendMessage(hCtrl, BM_GETCHECK, 0, 0L);
			EndDialog (hDlg, TRUE);
			return TRUE;

		    case IDCANCEL:
			EndDialog (hDlg, FALSE);
			return FALSE;
		   }
		   break;
	    }
      return FALSE ;
      }

/**************************************************************************
	Copy text to clipboard
**************************************************************************/

int	CopyTextToClipboard(HWND hwnd, char *text)
    {
    HGLOBAL hMem;

    if (text == NULL)
	return FALSE;
    //    if (!(hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, lstrlenA(text)+1)))
    if (!(hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (strlen(text) + 1))))
	return -1;
    //    strcpy((char *)GlobalLock(hMem), text);
    strcpy((char *)GlobalLock(hMem), text);
    GlobalUnlock(hMem);
    OpenClipboard(hwnd);
    EmptyClipboard();
#ifdef UNICODE
    SetClipboardData(CF_UNICODETEXT, hMem);
#else
    SetClipboardData(CF_TEXT, hMem);
#endif

    CloseClipboard();
    return 0;
    }

/**************************************************************************
	Update the ffmpeg Command when a user updates one of the parameters
**************************************************************************/

void UpdateFFmpegCommand(HWND hDlg)
    {
    char    temp[1024];
    char    drive0[_MAX_DRIVE];
    char    dir0[_MAX_DIR];
    char    name0[_MAX_FNAME];
    char    ext0[_MAX_EXT];

    double  MovieDuration;
    double  FadeOutStart;

    ffmpegCommand[0] = '\0';

    GetDlgItemText(hDlg, IDC_AUDIOPATH, AudioPath, MAX_PATH);
    GetDlgItemText(hDlg, IDC_OUTPUTFILENAME, MovieFilename, MAX_PATH);

    sprintf(FullMovieFilename, "%s\\%s", OutputPath, MovieFilename);

    _splitpath(FullMovieFilename, drive0, dir0, name0, ext0);
    _makepath(FullMovieFilename, drive0, dir0, name0, "mp4");

    MovieDuration = (FramesPerSecond > 0) ? (double)TotalFrames / FramesPerSecond : 0.0;

    FadeOutStart = MovieDuration - FadeOutSeconds;

    if (FadeOutStart < 0.0)
	FadeOutStart = 0.0;

    // FFmpeg executable
    strcat(ffmpegCommand, "ffmpeg ");

    // Video input
    sprintf(temp, "-framerate %d " "-i \"%s\\Frame%%05d.jpg\" ", FramesPerSecond, OutputPath);
    strcat(ffmpegCommand, temp);

    // Audio input
    if (*AudioPath)
	{
	if (LoopAudio)
	    strcat(ffmpegCommand, "-stream_loop -1 ");
	sprintf(temp, "-i \"%s\" ",  AudioPath);
	strcat(ffmpegCommand, temp);
	}

    // Video codec
    strcat(ffmpegCommand, "-c:v libx264 " "-crf 18 " "-pix_fmt yuv420p ");

    // Audio codec
    if (*AudioPath)
	strcat(ffmpegCommand, "-c:a aac ");

    // Audio fade
    if (*AudioPath && (FadeInSeconds > 0 || FadeOutSeconds > 0))
	{
	sprintf(temp, "-af \"afade=t=in:st=0:d=%d," "afade=t=out:st=%.1f:d=%d\" ", FadeInSeconds, FadeOutStart, FadeOutSeconds);
	strcat(ffmpegCommand, temp);
	}

    // Movie duration
    sprintf(temp, "-t %.1f ", MovieDuration);
    strcat(ffmpegCommand, temp);

    // Output filename
    sprintf(temp, "\"%s\"", FullMovieFilename);
    strcat(ffmpegCommand, temp);
    SetDlgItemText(hDlg, IDC_FFMPEG, ffmpegCommand);
    }

/**************************************************************************
	ffmpeg Commands Dialog Box
**************************************************************************/

// Zoom values are derived from the PNG corner data
// (mandel_width) exactly as MovieMaker has always done.
// This is intentionally approximate and sufficient for
// display overlays.

INT_PTR CALLBACK CreateFFMPEGCommandDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
    OPENFILENAME	ofn{};
    HWND		hCtrl;
    BOOL		bTrans;

    switch (message)
	{
	case WM_INITDIALOG:
	    {
	    double Duration = (FramesPerSecond > 0) ? (double)TotalFrames / FramesPerSecond : 0.0;
	    char s[32];
	    sprintf(s, "%.1f", Duration);
	    SetDlgItemText(hDlg, IDC_ESTIMATED_DURATION, s);
	    SetDlgItemText(hDlg, IDC_AUDIOPATH, AudioPath);
	    SetDlgItemText(hDlg, IDC_OUTPUTFILENAME, MovieFilename);
	    SetDlgItemText(hDlg, IDC_FFMPEG, ffmpegCommand);
	    FadeInSeconds = 0;
	    FadeOutSeconds = 2;
	    LoopAudio = FALSE;
	    UserDisplayZoom = FALSE;

	    SetDlgItemInt(hDlg, IDC_FADEIN, FadeInSeconds, TRUE);
	    SetDlgItemInt(hDlg, IDC_FADEOUT, FadeOutSeconds, TRUE);

	    hCtrl = GetDlgItem(hDlg, IDC_LOOPAUDIO);
	    SendMessage(hCtrl, BM_SETCHECK, LoopAudio, 0L);

	    hCtrl = GetDlgItem(hDlg, IDC_DISPLAY_ZOOM_LEVEL);
	    SendMessage(hCtrl, BM_SETCHECK, UserDisplayZoom, 0L);

	    UpdateFFmpegCommand(hDlg);
	    return FALSE;
	    }

	case WM_COMMAND:
	    switch ((int)LOWORD(wParam))
		{
		case IDC_BROWSE_AUDIO:
		    ZeroMemory(&ofn, sizeof(ofn));

		    ofn.lStructSize = sizeof(ofn);
		    ofn.hwndOwner = hDlg;
		    ofn.lpstrFile = AudioPath;
		    ofn.nMaxFile = MAX_PATH;
		    ofn.lpstrFilter = "Audio Files (*.mp3;*.wav;*.flac)\0*.mp3;*.wav;*.flac\0" "All Files (*.*)\0*.*\0";;
		    ofn.nFilterIndex = 1;

		    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		    if (GetOpenFileName(&ofn))
			{
			SetDlgItemText(hDlg, IDC_AUDIOPATH, AudioPath);
//			if (HIWORD(wParam) == EN_CHANGE)
			    UpdateFFmpegCommand(hDlg);
			}
		    break;

		case IDC_FADEIN:
		    FadeInSeconds = GetDlgItemInt(hDlg, IDC_FADEIN, &bTrans, TRUE);
		    if (HIWORD(wParam) == EN_CHANGE)
			UpdateFFmpegCommand(hDlg);
		    break;

		case IDC_FADEOUT:
		    FadeOutSeconds = GetDlgItemInt(hDlg, IDC_FADEOUT, &bTrans, TRUE);
		    if (HIWORD(wParam) == EN_CHANGE)
			UpdateFFmpegCommand(hDlg);
		    break;

		case IDC_LOOPAUDIO:
		    hCtrl = GetDlgItem(hDlg, IDC_LOOPAUDIO);
		    LoopAudio = (BOOL)SendMessage(hCtrl, BM_GETCHECK, 0, 0L);

		    UpdateFFmpegCommand(hDlg);
		    break;

		case IDC_COPY_FFMPEG:
		    CopyTextToClipboard(hDlg, ffmpegCommand);
		    return FALSE;

		case IDCANCEL:
		case IDOK:
		    EndDialog(hDlg, TRUE);
		    return TRUE;
		}
	    break;
	}
    return FALSE;
    }

/**************************************************************************
	About Constant Dialog Box
**************************************************************************/

INT_PTR CALLBACK AboutBoxDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
     switch (message)
	  {
	  case WM_COMMAND:
	        switch ((int) LOWORD(wParam))
		    {
		    case IDOK:
			EndDialog (hDlg, TRUE);
			return TRUE;
		   }
		   break;
	    }
      return FALSE ;
      }

/*-----------------------------------------
	Handle View Options
  -----------------------------------------*/

void	ChangeView(HWND hwnd, int xdest, int ydest, int widthdest, int heightdest, int xsrc, int ysrc, int widthsrc, int heightsrc, char stretchflag)
    { 
    HPALETTE hpalT;

    BeginPaint(hwnd, &ps);
    hpalT = SelectPalette (ps.hdc, hpal, FALSE);
    RealizePalette(ps.hdc);
    SetMapMode(ps.hdc,MM_TEXT);
    SetBkMode(ps.hdc,TRANSPARENT);
    SetStretchBltMode (ps.hdc, STRETCH_DELETESCANS) ;	// correct stretch mode for zoom etc
    StretchDIBits (ps.hdc, xdest, ydest, widthdest, heightdest, xsrc, ysrc, widthsrc, heightsrc,
			       (LPSTR)Dib.DibPixels, (LPBITMAPINFO)Dib.pDibInf, DIB_PAL_COLORS, SRCCOPY);
    SelectPalette(ps.hdc,hpalT,FALSE);
    EndPaint(hwnd, &ps);
    }

/*-----------------------------------------
    Setup View Options
-----------------------------------------*/

void	SetupView(HWND hwnd)
    {
//    static	BOOL	first = TRUE;				// start window maximised
    int		xFrame, yFrame;
    RECT 	WARect;						// this is the usable screen taking taskbar into account

    SystemParametersInfo(SPI_GETWORKAREA, 0, &WARect, 0);	// let's get usable area including task bar

    screenx = WARect.right - WARect.left;
    screeny = WARect.bottom - WARect.top;

    //#ifdef _WIN64
    xFrame = GetSystemMetrics(SM_CXFRAME) * 2;			// 2017 version of development environment reports only half the size of 2008 version. Not sure why
    yFrame = GetSystemMetrics(SM_CYFRAME) * 2;
    InvalidateRect(hwnd, &r, FALSE);
    caption = GetSystemMetrics(SM_CYCAPTION) + xFrame * 2 + GetSystemMetrics(SM_CYMENU);
    scroll_width = yFrame * 2;
    display_width = width + scroll_width;
    display_height = height + caption;					// include caption
    display_width = (display_width < screenx) ? display_width : screenx;	// large pictures
    display_height = (display_height < screeny) ? display_height : screeny;
    MoveWindow(hwnd, (screenx - display_width) / 2, (screeny - display_height) / 2, display_width, display_height, TRUE);

    HorOffset = (screenx - display_width) / 2 + yFrame;
    VertOffset = (screeny - display_height) / 2 + GetSystemMetrics(SM_CYCAPTION) + yFrame + GetSystemMetrics(SM_CYMENU);
    }

/**************************************************************************
    Normalise DIB dimensions for video encoding

    H.264 / yuv420p requires even width and height.
    Crop one column from the right and/or one row from the bottom if needed.
**************************************************************************/

int NormaliseDib(HWND hwnd, CDib *Dib)
    {
    if ((Dib->DibWidth & 1) == 0 && (Dib->DibHeight & 1) == 0)
	return 0;

    RECT SelectRect;

    SelectRect.left = 0;
    SelectRect.top = 0;
    SelectRect.right = Dib->DibWidth & ~1;
    SelectRect.bottom = Dib->DibHeight & ~1;

    if (SelectRect.right <= 0 || SelectRect.bottom <= 0)
	{
	MessageBox(hwnd,
	    "Invalid DIB dimensions while normalising image for JPEG output",
	    szAppName,
	    MB_ICONEXCLAMATION | MB_OK);
	MessageBeep(0);
	return -1;
	}

    return Crop(hwnd, Dib, SelectRect);
    }

//////////////////////////////////////////////////////////////////////
//	Save JPEG File
//////////////////////////////////////////////////////////////////////

int write_jpg_file(HWND hwnd, TCHAR *szSaveFileName, CDib *pDib, BOOL WriteThumbFile)
    {
    char	t[200];
    FILE	*fp;
    int		quality = 90;

     if (setjmp(mark))				// PNG and JPEG error handling
	{
	HandleJPEGError();
	return -1;
	}

     if (NormaliseDib(hwnd, pDib) < 0)
	 return -1;

    if ((fp = fopen(szSaveFileName, "wb")) == NULL)
	{
	wsprintf(t, "Unable to open file: %s", szSaveFileName);
	MessageBox(hwnd, t, szAppName, MB_ICONEXCLAMATION | MB_OK);
	MessageBeep(0);
	return -1;
	}

    WORD    ImageWidth = pDib->DibWidth;
    WORD    ImageHeight = pDib->DibHeight;
    WORD    Bits = pDib->BitsPerPixel;

    if (SaveJPEG(hwnd, fp, pDib->DibPixels, WriteThumbFile, quality, &ImageHeight, &ImageWidth, &Bits) < 0)
	{
	sprintf(t, TEXT("Error in Opening %s"), szSaveFileName);
	MessageBox(hwnd, t, "Write JPEG", MB_ICONEXCLAMATION | MB_OK);
	if (fp)
	    fclose(fp);
	return -1;
	}
    if (fp)
	fclose(fp);
    return 0;
    }

/**************************************************************************
	Take user keyboard input
**************************************************************************/

void user_data(void)

    {
    MSG msg;

    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	}
    }

/**************************************************************************
	Report status
**************************************************************************/

void ShowStatus(HWND hwnd, const char *fmt, ...)
    {
    char status[240];

    va_list args;
    va_start(args, fmt);
    vsnprintf(status, sizeof(status), fmt, args);
    va_end(args);

    status[sizeof(status) - 1] = '\0';
    SetWindowText(hwnd, status);
    }

/**************************************************************************
	Make sure we start with a fresh folder
**************************************************************************/

int DeleteExistingFrames(HWND hwnd, const char *OutputPath)
    {
    char SearchSpec[MAX_PATH];
    char FullName[MAX_PATH];
    char Message[512];
    WIN32_FIND_DATA FindData;
    int FrameCount = 0;

    sprintf(SearchSpec, "%s\\Frame*.jpg", OutputPath);

    HANDLE hFind = FindFirstFile(SearchSpec, &FindData);

    if (hFind == INVALID_HANDLE_VALUE)
	return 0;

    do
	{
	FrameCount++;
	} while (FindNextFile(hFind, &FindData));

    FindClose(hFind);

    if (FrameCount == 0)
	return 0;

    sprintf(Message, "Found %d existing frame files in:\n\n%s\n\nDelete them before creating a new sequence?", FrameCount, OutputPath);
    if (MessageBox(hwnd, Message, "Delete Existing Frames?", MB_ICONQUESTION | MB_YESNO) != IDYES)
	return -1;

    hFind = FindFirstFile(SearchSpec, &FindData);

    if (hFind == INVALID_HANDLE_VALUE)
	return 0;

    do
	{
	sprintf(FullName, "%s\\%s", OutputPath, FindData.cFileName);
	DeleteFile(FullName);
	} while (FindNextFile(hFind, &FindData));

    FindClose(hFind);
    return 0;
    }

/**************************************************************************
	Add text to a frame
**************************************************************************/

void OverlayFrameText(HWND hwnd, int FrameNumber, int TotalFrames)
    {
    HDC hDC = GetDC(hwnd);

    // Title
    if (MovieTitle[0] != '\0')
	{
	RECT rectTitle;
	rectTitle.left = Dib.DibWidth / 15;
	rectTitle.top = Dib.DibHeight * 3 / 10;
	rectTitle.right = Dib.DibWidth / 2;
	rectTitle.bottom = Dib.DibHeight * 6 / 10;

	LOGFONT lfTitle{};
	lfTitle.lfHeight = -(Dib.DibHeight / 12);
	lfTitle.lfWeight = FW_BOLD;
	strcpy(lfTitle.lfFaceName, "Arial");

	Dib.ShadowText2Dib(hDC, &rectTitle, &lfTitle, 2, MovieTitle);
	}

    // Subtitle
    if (MovieSubTitle[0] != '\0')
	{
	RECT rectSubtitle;
	rectSubtitle.left = Dib.DibWidth / 15;
	rectSubtitle.top = Dib.DibHeight * 15 / 20;
	rectSubtitle.right = Dib.DibWidth / 2;
	rectSubtitle.bottom = Dib.DibHeight * 17 / 20;

	LOGFONT lfSubtitle{};
	lfSubtitle.lfHeight = -(Dib.DibHeight / 26);
	lfSubtitle.lfWeight = FW_NORMAL;
	strcpy(lfSubtitle.lfFaceName, "Arial");

	Dib.ShadowText2Dib(hDC, &rectSubtitle, &lfSubtitle, 1, MovieSubTitle);
	}
    ReleaseDC(hwnd, hDC);
    }

/**************************************************************************
	Format the zoom level for display
**************************************************************************/

void FormatZoomString(double Zoom, char *buffer, int size)
    {
    if (Zoom < 1000.0)
	{
	_snprintf_s(buffer, size, _TRUNCATE, "Zoom: %.2f", Zoom);
	}
    else
	{
	double exponent;
	double logZoom;
	double mantissa;

	logZoom = log10(Zoom);
	exponent = floor(logZoom);
	mantissa = pow(10.0, logZoom - exponent);

	_snprintf_s(buffer, size, _TRUNCATE, "Zoom: %.2f x 10^%.0f", mantissa, exponent);
	}
    }

/**************************************************************************
	Calculate the zoom level to be displayed
**************************************************************************/

double CalculateRelativeZoom(int FrameNumber)
    {
    int ZoomFrame = FrameNumber - StartSeconds * FramesPerSecond;

    if (ZoomFrame <= 0)
	return 1.0;

    return pow(FrameRatio, (double)ZoomFrame);
    }

/**************************************************************************
	Display zoom text
**************************************************************************/

void OverlayZoomText(HWND hwnd, int FrameNumber)
    {
    static char ZoomString[80] = "";
    LOGFONT lf{};
    HDC hDC = GetDC(hwnd);

    lf.lfHeight = -(Dib.DibHeight / 25);
    lf.lfWeight = FW_NORMAL;
    strcpy(lf.lfFaceName, "Arial");

    if ((FrameNumber - StartSeconds * FramesPerSecond) % (FramesPerSecond / 2) == 0)
	{
	double Zoom = CalculateRelativeZoom(FrameNumber);
	FormatZoomString(Zoom, ZoomString, sizeof(ZoomString));
	}

    RECT rect;

    rect.left = Dib.DibWidth / 40;
    rect.right = Dib.DibWidth / 2;
    rect.top = Dib.DibHeight * 17 / 20;
    rect.bottom = Dib.DibHeight * 18 / 20;

    Dib.ShadowText2Dib(hDC, &rect, &lf, 1, ZoomString);
    if (hDC)
	ReleaseDC(hwnd, hDC);
    }

/**************************************************************************
	If zoom = 1, then let's morph frames instead
**************************************************************************/

void MorphDib(CDib *Dib1, CDib *Dib2, CDib *Dst, double BlendFraction)
    {
    long NumBytes;
    BYTE *p1;
    BYTE *p2;
    BYTE *pDst;

    NumBytes = WIDTHBYTES((DWORD)Dst->DibWidth * (DWORD)Dst->BitsPerPixel) * (DWORD)Dst->DibHeight;

    p1 = Dib1->DibPixels;
    p2 = Dib2->DibPixels;
    pDst = Dst->DibPixels;

    for (long i = 0; i < NumBytes; i++)
	{
	pDst[i] = (BYTE)((1.0 - BlendFraction) * p1[i] + BlendFraction * p2[i]);
	}
    }

/**************************************************************************
	Here's where we do all the work
**************************************************************************/

int	GenerateFileSequence(HWND hwnd, char *Filename, int NumInsertedFrames)
    {
    char    JPEGFilename[MAX_PATH];
    double  RezizeValue[MAX_INSERTED_FRAMES];
    double  Zoom1 = 1.0, Zoom2 = 1.0;
    ResizeStruct	RESIZE = { 1.0, 1.0, 1, 1, false };
    CDib    CurrentDib, PreviousDib, FinalDib;
    RECT    SelectRect;
    int	    NumEndFrames = 50;		// number of times to repeat last frame. Assume 2 seconds at 25 frames per second
    int	    NumStartFrames = 50;		// number of times to repeat first frame. Assume 2 seconds at 25 frames per second
    int	    FrameNumber = 0;

    NumStartFrames = StartSeconds * FramesPerSecond;
    NumEndFrames = EndSeconds * FramesPerSecond;

    int FramesPerImage = (NumInsertedFrames > 0) ? NumInsertedFrames : 1;
    TotalFrames = NumStartFrames + (filecount * FramesPerImage) + NumEndFrames;

    State = BATCH;

    InitFileList();							// init view next file if directory changes
    LoadFileList(hwnd, Filename);
    SortFileList();
    if (DeleteExistingFrames(hwnd, OutputPath) < 0)
	return -1;
    if (NumInsertedFrames > 0)						// we need to calculate the relative zoom level for each inserted frame
	{
	if (decode_png_header(hwnd, FileList[0].FileName, "ManpWIN Sequence Generator") < 0)		// get zoom for first frame
	    return -1;
	if (Dib.InitDib(width, height, bits_per_pixel) == NULL)
	    {
	    switch (Dib.DibErrorCode)
		{
		case NODIBMEMORY:
		    MessageBox(hwnd, "Can't allocate DIB memory. Using windows default palette", szAppName, MB_ICONEXCLAMATION | MB_OK);
		    break;
		case NOPIXELMEMORY:
		    MessageBox(hwnd, "Can't allocate memory for pixels", szAppName, MB_ICONEXCLAMATION | MB_OK);
		    break;
		}
	    return -1;
	    }
	else
	    memset(Dib.DibPixels, 0, WIDTHBYTES((DWORD)width * (DWORD)bits_per_pixel) * (DWORD)height);
	if (png_decoder(hwnd, FileList[0].FileName, "ManpWIN Sequence Generqator", &Zoom1, true, &Dib) < 0)
	    return -1;

	StartZoom = Zoom1;

	if (decode_png_header(hwnd, FileList[1].FileName, "ManpWIN Sequence Generator") < 0)		// get zoom for second frame
	    return -1;

	if (png_decoder(hwnd, FileList[1].FileName, "ManpWIN Sequence Generqator", &Zoom2, true, &Dib) < 0)
	    return -1;

	if (Zoom1 <= 0.0 || Zoom2 <= 0.0 || Zoom1 == Zoom2)
	    {
	    ratio = 1.0;	    
	    for (int i = 0; i < NumInsertedFrames; i++)
		RezizeValue[i] = 1.0;
	    }
	else
	    {
	    ratio = Zoom1 / Zoom2;
	    for (int i = 0; i < NumInsertedFrames; i++)
		RezizeValue[i] = pow(ratio, (double)i / NumInsertedFrames);
	    }
	}
    
    ratio = Zoom1 / Zoom2;
    FrameRatio = pow(ratio, 1.0 / NumInsertedFrames);
    BOOL DisplayZoom = (Zoom1 != Zoom2 && UserDisplayZoom);

    // Initialise a few extra Dibs
    if (CurrentDib.InitDib(width, height, bits_per_pixel) == NULL)
	{
	switch (Dib.DibErrorCode)
	    {
	    case NODIBMEMORY:
		MessageBox(hwnd, "Can't allocate DIB memory. Using windows default palette", szAppName, MB_ICONEXCLAMATION | MB_OK);
		break;
	    case NOPIXELMEMORY:
		MessageBox(hwnd, "Can't allocate memory for pixels", szAppName, MB_ICONEXCLAMATION | MB_OK);
		break;
	    }
	return -1;
	}
    else
	memset(CurrentDib.DibPixels, 0, WIDTHBYTES((DWORD)width * (DWORD)bits_per_pixel) * (DWORD)height);
    PreviousDib = CurrentDib;		// initialise the rest to CurrentDib by simple copying
    FinalDib = CurrentDib;

    for (int i = 0; i < filecount; ++i)
	{
	bool	IsMorph = false;
	user_data();
	if (State == NORMAL)
	    break;
	strcpy(Filename, FileList[i].FileName);
	if (decode_png_header(hwnd, Filename, "ManpWIN Sequence Generator") < 0)
	    return -1;
	xdots = width;
	ydots = height;

	SetupView(hwnd);
	if (Dib.InitDib(width, height, bits_per_pixel) == NULL)
	    {
	    switch (Dib.DibErrorCode)
		{
		case NODIBMEMORY:
		    MessageBox(hwnd, "Can't allocate DIB memory. Using windows default palette", szAppName, MB_ICONEXCLAMATION | MB_OK);
		    break;
		case NOPIXELMEMORY:
		    MessageBox(hwnd, "Can't allocate memory for pixels", szAppName, MB_ICONEXCLAMATION | MB_OK);
		    break;
		}
	    return -1;
	    }
	else
	    memset(Dib.DibPixels, 0, WIDTHBYTES((DWORD)width * (DWORD)bits_per_pixel) * (DWORD)height);

	double	dummy;
	if (png_decoder(hwnd, Filename, "ManpWIN Sequence Generator", &dummy, false, &Dib) < 0)
	    return -1;

	CurrentDib = Dib;       // clean current PNG
	if (i == 0 && StartSeconds > 0)
	    {
	    OverlayFrameText(hwnd, FrameNumber, TotalFrames);
	    for (int k = 0; k < NumStartFrames; k++)
		{
		ShowStatus(hwnd, "Creating Frame %d of %d", FrameNumber + 1, TotalFrames);
		sprintf(JPEGFilename, "%s\\Frame%05d.jpg", OutputPath, FrameNumber);
		if (write_jpg_file(hwnd, JPEGFilename, &Dib, FALSE) < 0)
		    return -1;
		FrameNumber++;
		}
	    }
	Dib = CurrentDib;

	if (NumInsertedFrames > 1 && ratio != 1.0)
	    {
	    CurrentDib = Dib;
	    // Zoom logic
	    for (int j = 0; j < NumInsertedFrames; j++)
		{
		if (j > 0)
		    {
		    RESIZE.HorResizeValue = RESIZE.VertResizeValue = RezizeValue[j];

		    ResizeDib(&Dib, RESIZE, true);

		    SelectRect.left = (Dib.DibWidth - CurrentDib.DibWidth) / 2;
		    SelectRect.right = SelectRect.left + CurrentDib.DibWidth;
		    SelectRect.top = (Dib.DibHeight - CurrentDib.DibHeight) / 2;
		    SelectRect.bottom = SelectRect.top + CurrentDib.DibHeight;

		    NormalizeRect(SelectRect);
		    Crop(hwnd, &Dib, SelectRect);
		    }

		ShowStatus(hwnd, "Creating Frame %d of %d", FrameNumber + 1, TotalFrames);
		sprintf(JPEGFilename, "%s\\Frame%05d.jpg", OutputPath, FrameNumber);

		if (i == filecount - 1 && j == NumInsertedFrames - 1)
		    FinalDib = Dib;          // save clean final frame before zoom text

		if (DisplayZoom)
		    OverlayZoomText(hwnd, FrameNumber);

		if (write_jpg_file(hwnd, JPEGFilename, &Dib, FALSE) < 0)
		    return -1;

		Dib = CurrentDib;
		FrameNumber++;
		}
	    }
	else if (NumInsertedFrames > 1 && ratio == 1.0)
	    {
	    // Morph logic
	    if (i == 0)
		{
		// First PNG - just write it
		ShowStatus(hwnd, "Creating Frame %d of %d",  FrameNumber + 1, TotalFrames);
		sprintf(JPEGFilename, "%s\\Frame%05d.jpg", OutputPath, FrameNumber);
		if (write_jpg_file(hwnd, JPEGFilename, &Dib, FALSE) < 0)
		    return -1;
		FrameNumber++;
		}
	    else
		{
		// Morph frames first
		for (int j = 1; j < NumInsertedFrames; j++)
		    {
		    double BlendFraction = (double)j / (double)NumInsertedFrames;
		    MorphDib(&PreviousDib, &CurrentDib, &Dib, BlendFraction);
		    ShowStatus(hwnd, "Creating Frame %d of %d", FrameNumber + 1, TotalFrames);
		    sprintf(JPEGFilename, "%s\\Frame%05d.jpg", OutputPath, FrameNumber);
		    if (write_jpg_file(hwnd, JPEGFilename, &Dib, FALSE) < 0)
			return -1;
		    FrameNumber++;
		    }
		// Then write the real PNG
		Dib = CurrentDib;
		ShowStatus(hwnd, "Creating Frame %d of %d", FrameNumber + 1, TotalFrames);
		sprintf(JPEGFilename, "%s\\Frame%05d.jpg", OutputPath, FrameNumber);
		if (write_jpg_file(hwnd, JPEGFilename, &Dib, FALSE) < 0)
		    return -1;
		FrameNumber++;
		}
	    }
	else
	    {
	    ShowStatus(hwnd, "Creating Frame %d of %d", FrameNumber + 1, TotalFrames);
	    sprintf(JPEGFilename, "%s\\Frame%05d.jpg", OutputPath, FrameNumber);
	    if (DisplayZoom)
		OverlayZoomText(hwnd, FrameNumber);
	    if (write_jpg_file(hwnd, JPEGFilename, &Dib, FALSE) < 0)
		return -1;
	    FrameNumber++;
	    }

	InvalidateRect(hwnd, &r, FALSE);
	PreviousDib = CurrentDib;
	}

    if (NumEndFrames > 0)
	{
	int FinalZoomFrameNumber = FrameNumber;
	for (int j = 0; j < NumEndFrames; j++)
	    {
	    ShowStatus(hwnd, "Creating Frame %d of %d", FrameNumber + 1, TotalFrames);
	    if (NumInsertedFrames > 1)
		Dib = FinalDib;              // clean copy every time
	    sprintf(JPEGFilename, "%s\\Frame%05d.jpg", OutputPath, FrameNumber);
	    if (DisplayZoom)
		OverlayZoomText(hwnd, FinalZoomFrameNumber);
	    if (write_jpg_file(hwnd, JPEGFilename, &Dib, FALSE) < 0)
		return -1;
	    FrameNumber++;
	    }
	}

    ShowStatus(hwnd, "Created %d frames", FrameNumber);
    return 0;
    }
