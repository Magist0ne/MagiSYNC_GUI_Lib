#include "cmsis_os2.h"

/*
//抛弃该分类机制
#define	RegSYNC_Mode								1		//变量刷新策略，刷新页面对应的变量，需要注册函数，变量可以零散分布
#define	PageSYNC_Mode								2		//按页刷新策略，只刷新显示的页面，变量分配时需要按页面分块好
#define	GolbalSYNC_Mode							3		//全局刷新策略，完全实现本地BUFF和屏幕的同步，同步数据基于时间戳机制（消耗资源大，刷新慢）
#define	PageGolbalMixedSYN_CMode		4		//混合刷新策略，只刷新显示的页面，同时同步更新部分变量区域
*/
#define	ShadowReg_Mode							1		//全局变量刷新策略：影子寄存器机制
#define	Preemptive_Mode							2		//全局变量刷新策略：影子寄存器机制

typedef	enum	{
	SimplexData	=	1,
	DuplexData	=	2
}	Data_Dir_Def;

typedef	enum	{
	Register		=	1,
	DisRegister	=	2
}	PackReg_Def;

#define	TransferMaxLimitDefault			64						



/// Status code values returned by CMSIS-RTOS functions.
typedef enum {
  syncOK                  		=  0,         ///< Success
  syncError                   = -1,         ///< ERROR
  syncErrorTimeout            = -2,         ///< Timeout
  syncErrorResource           = -3,         ///< Resource Not Enough
  syncErrorParameter          = -4,         ///< Parameter ERROR
} syncStatus_t;

typedef enum {
  listOK                  		=  0,         ///< Success
  listError                   = -1,         ///< ERROR
  listErrorFull           		= -2,         ///< List Full
} ListStatus_t;


typedef enum {
	DataType_char			=  0,
  DataType_uint8_t	=  1,
	DataType_uint16_t	=  2,
	DataType_uint32_t	=  3,
	DataType_float		=  4,
	DataType_double		=  5,
} DataType_t;




typedef struct
{
	void*				pData;
	uint8_t*		osDataOK;
	DataType_t	DataType;
}GlobalRegHandle_Def;
/*
//定义实例：
struct
{
	uint32_t			UserData;//数据内容
	uint8_t				UserDataOK;
}Example_Data;
GlobalRegHandle	Example_Data_Handle	=	{	.pData		=	&(Example_Data.UserData),
																				.osDataOK	=	&(Example_Data.UserData),
																				.DataType	=		DataType_uint32_t
																			};
*/










typedef struct
{
	uint32_t		Data_Size;
	uint32_t		ListLength;
	uint32_t		ListCounter;
	void*				pListData;
}ListHandle_Def;

typedef struct
{
	uint32_t		ListCellEnable;
	uint32_t		ListLastNum;
	uint32_t		ListNextNum;
}ListCtrl_Def;

/*
//定义List实例
typedef struct
{
	ListCtrl_Def	ListCtrl;
	DataTypedef		Data;//链表内容,用户自定义数据
}Example_List_Def;
Example_List_Def	Example_List[32];
ListHandle_Def		Example_ListHandle	=	{.ListLength	=	32,.pListData	=	Example_List,.Data_Size	=	sizeof(Example_List_Def),.ListCounter	=	0}

//或者
struct
{
	ListCtrl_Def	ListCtrl;
	DataTypedef		Data;//链表内容,用户自定义数据
}Example_List[32];
ListHandle_Def		Example_ListHandle	=	{.ListLength	=	32,.pListData	=	Example_List,.Data_Size	=	sizeof(Example_List[0]),.ListCounter	=	0}

//后续所有对List的操作都通过Example_ListHandle进行
//List操作函数
ListStatus_t	List_Init();						//定义完List需要先初始化
ListStatus_t	List_Length();					//返回链表长度
ListStatus_t	List_Add();						//插入链表一个内容
ListStatus_t	List_Del();						//删除链表一个内容
ListStatus_t	List_Take();						//从链表取出一个内容，并从链表删除,拷贝的方式取出
ListStatus_t	List_Look();						//从链表取出一个内容，不从链表删除,拷贝的方式取出
*/










typedef struct//和DW屏标准交互格式
{
    uint8_t*    Pro_OK; 
    uint8_t     length;
    uint16_t    ADDR;
    void*       TargetADDR;		
}DW_Data_RW_Def;


typedef struct//实时变量注册、反注册数据包
{
		PackReg_Def			RegOrDisReg;		//Register、DisRegister，注册反注册
		Data_Dir_Def		Data_Direct;		//SimplexData、Simplex，单向传输、双向传输
		DataType_t			DataType;				//数据类型
		void*						pMcuData;				//MCU内部变量地址
		uint16_t				GUI_ADDR;				//对应绑定的GUI地址
		osStatus_t*			osStatusReturn;	//返回值地址，操作成功返回osOK
}GolbalData_Process_Def;




