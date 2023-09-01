#	include	"MagiSYNC_PublicData.h"

//该部分可自由配置

//关键配置
#	define		GUI_SyncUsart								huart3






//常规使能配置
#	define		TransferMaxLimitEnable			1													//是否更改最大通讯长度







//常规参数配置
#	define		GUIRefresh_Freq							20								//刷新率,默认20，即每秒处理20次UI
#	define		MessageProcessStackSize			2048							//串口环形缓冲区长度，数据接收是通过环形缓冲区实现的，不宜太小
#	define		UsartRing_Length						1024							//串口环形缓冲区长度，数据接收是通过环形缓冲区实现的，不宜太小
#	define		UsartSendBuff_Length				512								//串口发送缓冲区长度
#	define		GlobalRegRefreshEnable			1									//使能变量全局刷新，全局刷新的变量需要使用MagiSYNC_GlobalReg_Init注册才能使能

#	define		DuplexDataSYNC_Mode					ShadowReg_Mode		//双向数据数据同步策略


#if(GUI_SYNC_Mode	==	PageSYNC_Mode)||(GUI_SYNC_Mode	==	PageGolbalMixedSYN_CMode)
	#	define		GUI_PageBuffSize						0x100							//页缓冲缓存大小
#endif

#if(GUI_SYNC_Mode	==	GolbalSYNC_Mode)||(GUI_SYNC_Mode	==	PageGolbalMixedSYN_CMode)
	#	define		GUI_GolbalSYNC_StartADDR		0x0000						//同步更新起始地址
	#	define		GUI_GolbalSYNC_Size					0x0000						//同步更新数据区域大小
#endif

#if	TransferMaxLimitEnable
	# define 	TransferMaxLimit						TransferMaxLimitDefault		//默认64字字，即128字节		
#else
	# define 	TransferMaxLimit						120												//最大259字节，这里的单位为字，即240字节
#endif

#	define		GUI_Touch_ADDR							0X5000						//触控变量返回地址，默认0X5000，也可改成其他



//该部分尽量不要改动，可能会大幅度影响运行速度或者引起错误
#	define		GUI_MessProBuffLength			128							//数据解析函数内部缓冲区长度，过小会导致无法解析长的字符串，尽量不要小于32





