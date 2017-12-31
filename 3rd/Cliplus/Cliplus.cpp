#include "Cliplus.h"
#include <stdio.h>
#include <process.h>
#include <windows.h>
#pragma warning(disable:4996)

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

        if (strncmp(szCmdBuff, "\\q", 2) == 0 ||
            strncmp(szCmdBuff, "exit", 4) == 0 ||
            strncmp(szCmdBuff, "quit", 4) == 0) {
            m_bLoop = false;
            ShowQuitTip();
            fgetchar();
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

    if (m_emSubSys == EM_WINDOWS)
        FreeConsole();
    return 0;
}