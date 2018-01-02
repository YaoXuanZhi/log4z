/*****************************************************************************
*  @file     Cliplus.h
*  @brief    控制台辅助类
*  @note
*  此类库仅支持Windows，要想让其适配Linux/Unix系统请自行修改><
*
*****************************************************************************/
#ifndef __CCLIPLUS_H
#define __CCLIPLUS_H

/** @brief 子系统类型 */
enum EM_SUBSYSTEM
{
    EM_WINDOWS, /**< 窗口程序 */
    EM_CONSOLE, /**< 控制台程序 */
};

/**
* @class 
* @brief 控制台辅助类库
* @note   
* 支持GUI和CLI程序（Windows）
*/
class CCliplus{
private:
	bool m_bLoop;/**< 循环控制标记 */
	bool m_bAcceptCmd;/**< 是否接受外部指令 */
	EM_SUBSYSTEM m_emSubSys;/**< 子系统类型 */
public:
	CCliplus();

	/** 
	* @brief: 关闭CLI窗口
	* @return 无
	* @note   
	* 
	*/
	void Close();

	/** 
	* @brief: 打开CLI窗口
	* @return 无
	* @note   
	* 
	*/
	void Open();

    /** 
    * @brief: 设置外部输入
    * @param[in] bool  bIsCan  如果为true，则支持输入，反之无法输入
    * @return 无
    * @note   
    * 
    */
    void SetAcceptCmd(bool bIsCanAccept)
    {
        m_bAcceptCmd = bIsCanAccept;
    }

public:
    virtual unsigned int OnMessageLoop();

protected:
    /** 
    * @brief: 设置子系统类型
    * @param[in] EM_SUBSYSTEM  subSysType  子系统类型
    * @return 无
    * @note   
    * 
    */
    void SetSubSysType(EM_SUBSYSTEM subSysType);

	/** 
	* @brief: 接收用户输入指令
	* @param[in] char*  szCmd  字符串指令
	* @return 无
	* @note   
	* 
	*/
	virtual void AcceptCommand(char *szCmd);

	/** 
	* @brief: 打印帮助信息
	* @return 无
	* @note   
	* 
	*/
	virtual void ShowHelpTips();

	/** 
	* @brief: 打印退出信息
	* @return 无
	* @note   
	* 
	*/
	virtual void ShowQuitTip();

	/** 
	* @brief: 退出
	* @return 无
	* @note   
	* 
	*/
    virtual bool OnQuit();
};

#endif
