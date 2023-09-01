#include "cmsis_os2.h"

/*
//�����÷������
#define	RegSYNC_Mode								1		//����ˢ�²��ԣ�ˢ��ҳ���Ӧ�ı�������Ҫע�ắ��������������ɢ�ֲ�
#define	PageSYNC_Mode								2		//��ҳˢ�²��ԣ�ֻˢ����ʾ��ҳ�棬��������ʱ��Ҫ��ҳ��ֿ��
#define	GolbalSYNC_Mode							3		//ȫ��ˢ�²��ԣ���ȫʵ�ֱ���BUFF����Ļ��ͬ����ͬ�����ݻ���ʱ������ƣ�������Դ��ˢ������
#define	PageGolbalMixedSYN_CMode		4		//���ˢ�²��ԣ�ֻˢ����ʾ��ҳ�棬ͬʱͬ�����²��ֱ�������
*/
#define	ShadowReg_Mode							1		//ȫ�ֱ���ˢ�²��ԣ�Ӱ�ӼĴ�������
#define	Preemptive_Mode							2		//ȫ�ֱ���ˢ�²��ԣ�Ӱ�ӼĴ�������

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
//����ʵ����
struct
{
	uint32_t			UserData;//��������
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
//����Listʵ��
typedef struct
{
	ListCtrl_Def	ListCtrl;
	DataTypedef		Data;//��������,�û��Զ�������
}Example_List_Def;
Example_List_Def	Example_List[32];
ListHandle_Def		Example_ListHandle	=	{.ListLength	=	32,.pListData	=	Example_List,.Data_Size	=	sizeof(Example_List_Def),.ListCounter	=	0}

//����
struct
{
	ListCtrl_Def	ListCtrl;
	DataTypedef		Data;//��������,�û��Զ�������
}Example_List[32];
ListHandle_Def		Example_ListHandle	=	{.ListLength	=	32,.pListData	=	Example_List,.Data_Size	=	sizeof(Example_List[0]),.ListCounter	=	0}

//�������ж�List�Ĳ�����ͨ��Example_ListHandle����
//List��������
ListStatus_t	List_Init();						//������List��Ҫ�ȳ�ʼ��
ListStatus_t	List_Length();					//����������
ListStatus_t	List_Add();						//��������һ������
ListStatus_t	List_Del();						//ɾ������һ������
ListStatus_t	List_Take();						//������ȡ��һ�����ݣ���������ɾ��,�����ķ�ʽȡ��
ListStatus_t	List_Look();						//������ȡ��һ�����ݣ���������ɾ��,�����ķ�ʽȡ��
*/










typedef struct//��DW����׼������ʽ
{
    uint8_t*    Pro_OK; 
    uint8_t     length;
    uint16_t    ADDR;
    void*       TargetADDR;		
}DW_Data_RW_Def;


typedef struct//ʵʱ����ע�ᡢ��ע�����ݰ�
{
		PackReg_Def			RegOrDisReg;		//Register��DisRegister��ע�ᷴע��
		Data_Dir_Def		Data_Direct;		//SimplexData��Simplex�������䡢˫����
		DataType_t			DataType;				//��������
		void*						pMcuData;				//MCU�ڲ�������ַ
		uint16_t				GUI_ADDR;				//��Ӧ�󶨵�GUI��ַ
		osStatus_t*			osStatusReturn;	//����ֵ��ַ�������ɹ�����osOK
}GolbalData_Process_Def;




