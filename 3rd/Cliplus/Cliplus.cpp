#include "Cliplus.h"
#include <stdio.h>
#include <process.h>
#include <windows.h>
#pragma warning(disable:4996)

/*****************************************************************************
* [windows 控制台程序 响应 Ctrl+C](http://blog.csdn.net/shuiba1122/article/details/40950671)
* [控制台程序输入CTRL+C后弹出异常的原因](http://blog.csdn.net/oilcode/article/details/8886806)
*****************************************************************************/

BOOL CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.   
    case CTRL_C_EVENT:
        fputs("Ctrl-C event\n\n", stdout);
        //Beep( 750, 300 );   
        return(TRUE);
        // CTRL-CLOSE: confirm that the user wants to exit.   
    case CTRL_CLOSE_EVENT:
        //Beep( 600, 200 );   
        fputs("Ctrl-Close event\n\n", stdout);
        return(TRUE);
        // Pass other signals to the next handler.   
    case CTRL_BREAK_EVENT:
        //Beep( 900, 200 );   
        fputs("Ctrl-Break event\n\n", stdout);
        return FALSE;
    case CTRL_LOGOFF_EVENT:
        //Beep( 1000, 200 );   
        fputs("Ctrl-Logoff event\n\n", stdout);
        return FALSE;
    case CTRL_SHUTDOWN_EVENT:
        //Beep( 750, 500 );   
        fputs("Ctrl-Shutdown event\n\n", stdout);
        return FALSE;
    default:
        return FALSE;
    }
}

namespace CliplusNsp {
    /** 
    * @brief: 获得控制台窗口句柄
    * @return HWND CLI窗口句柄
    * @note   
    * 
    */
    HWND GetConsoleWnd()
    {
        char szWndTitle[256] = { 0 };
        GetConsoleTitleA(szWndTitle, 256);
        return FindWindowA("ConsoleWindowClass", szWndTitle);
    }

    /**
    * @brief: 设置窗口置顶
    * @param[in] bool  bIsAlwaysTop  如果为true则置顶，反之，取消置顶
    * @return 无
    * @note
    *
    */
    void SetAlwaysOnTop(bool bIsAlwaysTop = false)
    {
        HWND hWndInsertAfter = bIsAlwaysTop ? HWND_TOPMOST : HWND_NOTOPMOST;
        SetWindowPos(GetConsoleWnd(), hWndInsertAfter, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
    }

    unsigned int __stdcall ThreadCliLoopFunc(void* lpParam)
    {
        CCliplus *pThis = (CCliplus*)lpParam;
        if (NULL != pThis) {
            pThis->OnMessageLoop();
        }
        return 0;
    }
}

CCliplus::CCliplus() :m_emSubSys(EM_CONSOLE), m_bLoop(false), m_bAcceptCmd(true)
{
}

void CCliplus::ShowHelpTips()
{
    fputs("Cliplus used for GUI/CLI Application on Windows\n", stdout);
}

void CCliplus::ShowQuitTip()
{
    fputs("Press Any Key To Quit...\n", stdout);
}

void CCliplus::SetSubSysType(EM_SUBSYSTEM subSysType)
{
    m_emSubSys = subSysType;
}

void CCliplus::AcceptCommand(char *szCmd)
{
    fputs(szCmd, stdout);
}

void CCliplus::Close()
{
    m_bLoop = false;

    //在控制台上模拟按下Enter键
    ::PostMessage(CliplusNsp::GetConsoleWnd(), WM_KEYDOWN, VK_RETURN, NULL);
}

bool CCliplus::OnQuit()
{
    m_bLoop = false;
    ShowQuitTip();
    fgetchar();
    return true;
}

void CCliplus::Open()
{
    if (m_bLoop)
        return;
    switch (m_emSubSys)
    {
    case EM_WINDOWS:
    {
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        _beginthreadex(NULL, 0, CliplusNsp::ThreadCliLoopFunc, this, 0, NULL);
    }; break;
    case EM_CONSOLE:
    {
        OnMessageLoop();
    }; break;
    default:
        break;
    }
}

unsigned int CCliplus::OnMessageLoop()
{
    // 重置Ctrl相关的信号作用
    if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
    {
        const int nCmdLen = 512;
        m_bLoop = true;
        while (m_bLoop)
        {
            if (!m_bAcceptCmd) {
                Sleep(10);
                continue;
            }
            char szCmdBuff[nCmdLen] = { 0 };
            fgets(szCmdBuff, sizeof(char) * nCmdLen, stdin);

            if (*szCmdBuff == 0) {
                Sleep(500);
                if (OnQuit())
                    break;
            }

            szCmdBuff[strlen(szCmdBuff) - 1] = '\0'; //按下Enter之后多出来的"\n"

            if (strncmp(szCmdBuff, "\\q", 2) == 0 ||
                strncmp(szCmdBuff, "exit", 4) == 0 ||
                strncmp(szCmdBuff, "quit", 4) == 0) {
                OnQuit();
            }
            else if (strncmp(szCmdBuff, "settop", 6) == 0) {
                CliplusNsp::SetAlwaysOnTop(true);
            }
            else if (strncmp(szCmdBuff, "notop", 5) == 0) {
                CliplusNsp::SetAlwaysOnTop(false);
            }
            else if (strncmp(szCmdBuff, "-h", 2) == 0 ||
                strncmp(szCmdBuff, "/?", 2) == 0) {
                ShowHelpTips();
            }
            else {
                AcceptCommand(szCmdBuff);
            }
        }
    }

    if (m_emSubSys == EM_WINDOWS)
        FreeConsole();
    return 0;
}