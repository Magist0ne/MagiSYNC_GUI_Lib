#	include	"MagiSYNC_Config.h"

//Config  参数检查
#if	(GUIRefresh_Freq	>	50)
#error GUIRefresh_Freq Too High , Don't Higher Than 30
#endif

#if	(GUI_MessProBuffLength	<	32)
#error GUI_MessProBuffLength Too Few
#endif

#if	(TransferMaxLimit				>	124)
#error TransferMaxLimit Too More
#endif

typedef struct//串口环形缓冲区数据结构
{
    uint16_t Head;           
    uint16_t Tail;
    uint16_t Lenght;
    uint8_t  Ring_data[UsartRing_Length];
}RingBuff_t;

/*
函数接口设计：

//基本接口函数
syncStatus_t	MagiSYNC_Init();//初始化GUI库，创建相关任务
syncStatus_t	MagiSYNC_DisInit();//反初始化GUI库，删除相关任务

syncStatus_t	MagiSYNC_Reg_Init();//注册变量，用于页面刷新，需要传入页面编号
syncStatus_t	MagiSYNC_Reg_DisInit();//反注册变量

syncStatus_t	MagiSYNC_GlobalReg_Init();//注册全局变量，全局变量会实时更新，分为单向更新和双向更新，一个地址不可以注册成两种变量
syncStatus_t	MagiSYNC_GlobalReg_DisInit();//反注册全局变量


syncStatus_t	MagiSYNC_Reg_Read();//寄存器读写
syncStatus_t	MagiSYNC_Reg_Write();


syncStatus_t	MagiSYNC_Data_Read();//数据读写，通过调用寄存器读写函数实现，需要传入数据类型
syncStatus_t	MagiSYNC_Data_Write();


syncStatus_t	MagiSYNC_Char_Read();//字符串读写，通过调用寄存器读写函数实现，需要传入字符串长度
syncStatus_t	MagiSYNC_Char_Write();


//通讯层接口函数
syncStatus_t	MagiSYNC_GetUartData();//获取一些数据
syncStatus_t	MagiSYNC_SendUartData();//发送一些数据





//二次封装功能接口





*/



void MessageProcess(void *argument);

syncStatus_t	MagiSYNC_Init(void);//初始化GUI库，创建相关任务
syncStatus_t	MagiSYNC_DisInit(void);










