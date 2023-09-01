#	include "FreeRTOS.h"
#	include "task.h"
#	include "main.h"
#	include "cmsis_os.h"
#	include "MagiSYNC.h"




//函数声明区
static	ListStatus_t	List_Init(ListHandle_Def*	ListHandle);							//定义完List需要先初始化
static	uint32_t			List_GetLength(ListHandle_Def*	ListHandle);				//返回链表尺寸
static	uint32_t			List_GetCounter(ListHandle_Def*	ListHandle);				//返回链表内容长度
static	ListStatus_t	List_Add(ListHandle_Def*	ListHandle,void*	pData);	//插入链表一个内容
static	ListStatus_t	List_Del(ListHandle_Def*	ListHandle,uint32_t	CounterNum);						//删除链表一个内容
static	ListStatus_t	List_Look(ListHandle_Def*	ListHandle,uint32_t	CounterNum,void*	pData);						//从链表取出一个内容，不从链表删除,拷贝的方式取出
static	ListStatus_t	List_Take(ListHandle_Def*	ListHandle,uint32_t	CounterNum,void*	pData);						//从链表取出一个内容，并从链表删除,拷贝的方式取出






typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticEventGroup_t osStaticEventGroupDef_t;


//创建相关任务的信息
//写数据队列
static	osMessageQueueId_t DW_GUI_WriteDataHandle;
static	uint8_t DW_GUI_WriteDataBuffer[ 64 * sizeof( DW_Data_RW_Def ) ];
static	osStaticMessageQDef_t DW_GUI_WriteDataControlBlock;
const osMessageQueueAttr_t DW_GUI_WriteData_attributes = {
	.name = "DW_GUI_WriteData",
	.cb_mem = &DW_GUI_WriteDataControlBlock,
	.cb_size = sizeof(DW_GUI_WriteDataControlBlock),
	.mq_mem = &DW_GUI_WriteDataBuffer,
	.mq_size = sizeof(DW_GUI_WriteDataBuffer)
};	

//读数据队列
static	osMessageQueueId_t DW_GUI_GetDataHandle;
static	uint8_t DW_GUI_GetDataBuffer[ 64 * sizeof( DW_Data_RW_Def ) ];
static	osStaticMessageQDef_t DW_GUI_GetDataControlBlock;
const osMessageQueueAttr_t DW_GUI_GetData_attributes = {
	.name = "DW_GUI_GetData",
	.cb_mem = &DW_GUI_GetDataControlBlock,
	.cb_size = sizeof(DW_GUI_GetDataControlBlock),
	.mq_mem = &DW_GUI_GetDataBuffer,
	.mq_size = sizeof(DW_GUI_GetDataBuffer)
};

//全局变量操作队列
static	osMessageQueueId_t DW_GUI_GolbalDataHandle;
static	uint8_t DW_GUI_GolbalDataBuffer[ 64 * sizeof( GolbalData_Process_Def ) ];
static	osStaticMessageQDef_t DW_GUI_GolbalDataControlBlock;
const osMessageQueueAttr_t DW_GUI_GolbalData_attributes = {
	.name = "DW_GUI_GetData",
	.cb_mem = &DW_GUI_GolbalDataControlBlock,
	.cb_size = sizeof(DW_GUI_GolbalDataControlBlock),
	.mq_mem = &DW_GUI_GolbalDataBuffer,
	.mq_size = sizeof(DW_GUI_GolbalDataBuffer)
};

//创建任务
static	osThreadId_t	DW_GUI_MessageHandle;
const osThreadAttr_t DW_GUI_Message_attributes = {
	.name = "DW_GUI_Message",
	.stack_size = MessageProcessStackSize * 4,
	.priority = (osPriority_t) osPriorityNormal,
};


syncStatus_t	MagiSYNC_Init(void)//初始化GUI库，创建相关任务
{
	uint8_t				MessageProcess_Init_OK	=	0;
	uint32_t			Time_Wait	=	500;
	
	//创建队列
	//写数据队列
  DW_GUI_WriteDataHandle = osMessageQueueNew (64, sizeof(DW_Data_RW_Def), &DW_GUI_WriteData_attributes);
	if(DW_GUI_WriteDataHandle	==	NULL)return	syncError;
	
	//读数据队列
  DW_GUI_GetDataHandle = osMessageQueueNew (64, sizeof(DW_Data_RW_Def), &DW_GUI_GetData_attributes);
	if(DW_GUI_GetDataHandle	==	NULL)return	syncError;
	
	//全局变量操作队列：注册、反注册
  DW_GUI_GolbalDataHandle = osMessageQueueNew (64, sizeof(GolbalData_Process_Def), &DW_GUI_GolbalData_attributes);	
	if(DW_GUI_GolbalDataHandle	==	NULL)return	syncError;
	
	//创建任务
	DW_GUI_MessageHandle = osThreadNew(MessageProcess, &MessageProcess_Init_OK, &DW_GUI_Message_attributes);	
	if(DW_GUI_MessageHandle	==	NULL)return	syncError;
	
	
	//等待任务初始化完成
	while(MessageProcess_Init_OK	==	0)
	{
		Time_Wait--;
		HAL_Delay(10);//可以换成其他形式的延时函数
		if(Time_Wait	==	0)return	syncErrorTimeout;	
	}
	osThreadResume(DW_GUI_MessageHandle);
	return	syncOK;
}


//反初始化，删除相关任务和队列
syncStatus_t	MagiSYNC_DisInit(void)
{
	osStatus_t	osStatus;
	
	osStatus	=	osThreadTerminate(DW_GUI_MessageHandle);
	if(osStatus	!=	osOK)return	syncError;

	osStatus	=	osMessageQueueDelete(DW_GUI_WriteDataHandle);
	if(osStatus	!=	osOK)return	syncError;

	osStatus	=	osMessageQueueDelete(DW_GUI_GetDataHandle);
	if(osStatus	!=	osOK)return	syncError;

	osStatus	=	osMessageQueueDelete(DW_GUI_GolbalDataHandle);
	if(osStatus	!=	osOK)return	syncError;

	return	syncOK;
}






/*
Loop:
1-处理注册和反注册队列
2-处理普通读数据的请求操作
3-处理环形缓冲区接收到的数据
	|__清除写数据应答数据
	|__查找双向数据更新链表，不删除
	|__查找读操作请求链表，
	|__处理至环形缓冲区为空
	|__清空读操作环形缓冲区链表
4-处理发送数据
	|__处理单向数据更新
	|__处理双向数据更新
*/
void MessageProcess(void *argument)
{
	extern  UART_HandleTypeDef  GUI_SyncUsart;
	
	//消息队列处理缓存
	GolbalData_Process_Def	GolbalData_Process_Buff;
	
	RingBuff_t	Usart_RingBuff;
	uint32_t 		tick,tick_step,i; 
	uint8_t			Usart_Data;
	uint8_t			USART_Send_Buffer[UsartSendBuff_Length],SendLength;
	
	
	
	tick_step	=	osKernelGetTickFreq()/20;
  tick 			= osKernelGetTickCount();        // retrieve the number of system ticks
	
	
	*((uint8_t*)argument)	=	1;//初始化完成，向Init函数写1，否则Init返回ERROR
	osThreadSuspend(DW_GUI_MessageHandle);
	for(;;)
	{
		while(osOK	==	osMessageQueueGet(DW_GUI_GolbalDataHandle,&GolbalData_Process_Buff,NULL,0))
		{
			if(GolbalData_Process_Buff.RegOrDisReg	==	Register)
			{
				if(GolbalData_Process_Buff.Data_Direct	==	SimplexData)
				{
					
					*GolbalData_Process_Buff.osStatusReturn	=	osOK;
				}
				else if(GolbalData_Process_Buff.Data_Direct	==	SimplexData)
				{
				
					*GolbalData_Process_Buff.osStatusReturn	=	osOK;
				}	
				else
				{
					*GolbalData_Process_Buff.osStatusReturn	=	osErrorParameter;
				}
			}
			else if(GolbalData_Process_Buff.RegOrDisReg	==	DisRegister)
			{
				if(GolbalData_Process_Buff.Data_Direct	==	SimplexData)
				{
					
					*GolbalData_Process_Buff.osStatusReturn	=	osOK;
				}
				else if(GolbalData_Process_Buff.Data_Direct	==	SimplexData)
				{
				
					*GolbalData_Process_Buff.osStatusReturn	=	osOK;
				}	
				else
				{
					*GolbalData_Process_Buff.osStatusReturn	=	osErrorParameter;
				}			
			}
			else
			{
				*GolbalData_Process_Buff.osStatusReturn	=	osErrorParameter;
			}
		}
		
		
		
		
		
		
		
		
		
		/*
  extern  RingBuff_t          Usart_RingBuff;
  extern  uint8_t 		        Usart_Buff_IT[1];
  
  uint32_t          i,Pross,RunningTime;
  uint32_t          RXTX_RTime,APP_Restart; 
  uint8_t Read_Ring_Byte,Read_Length;  
  uint8_t USART_Send_Buffer[512],Receive_Buff_U[512];
  uint8_t SendLength;
  
  DW_GUI_RW_Def       DW_GUI_Data;
  GetData_LList_def   GetData_LList;
  DW_Data_RW_Def      GetD_SubmitReq;
  //uint32_t            CleanTimeOut_Num;
  GetData_LList_Init(&GetData_LList);  
  APP_Restart   =   1;
  HAL_UART_Receive_IT(&huart3,Usart_Buff_IT,1);//初始化串口接收中断
  
  //osThreadSuspend(DW_GUI_MessageHandle);
  for(;;)
  {
    if(APP_Restart  ==  1)
    {
      APP_Restart = 0;
      Pross       = 0;
      GetData_LList_Init(&GetData_LList);
      if(Print_RXTX_RTime)
      {
        RXTX_RTime  = osKernelGetTickCount();
      }     
    }
    
    while(1)
    {
      if(osOK ==  osMessageQueueGet(DW_GUI_GetDataHandle,&GetD_SubmitReq,NULL,0))
      {
        Pross++;
        GetData_LList_Add(&GetData_LList,&GetD_SubmitReq);
        *(GetD_SubmitReq.Pro_OK)  = 0;
        USART_Send_Buffer[0]  = 0x5A;
        USART_Send_Buffer[1]  = 0xA5;
        USART_Send_Buffer[2]  = 0x04;
        USART_Send_Buffer[3]  = ReadData;
        USART_Send_Buffer[4]  = (uint8_t)(GetD_SubmitReq.ADDR>>8)&0xff;
        USART_Send_Buffer[5]  = (uint8_t)(GetD_SubmitReq.ADDR)&0xff; 
        USART_Send_Buffer[6]  = (uint8_t)(GetD_SubmitReq.length); 
        HAL_UART_Transmit(&huart3, USART_Send_Buffer, 7, 10); 
        if(CopyGuiDataToUart1)
        {
          printf("Read Code is %02x %02x %02x %02x %02x %02x %02x \r\n",USART_Send_Buffer[0],USART_Send_Buffer[1],USART_Send_Buffer[2],USART_Send_Buffer[3],USART_Send_Buffer[4],USART_Send_Buffer[5],USART_Send_Buffer[6]);
        }             
      }
      else  
      {
        break;     
      }
    }
 
    
    while(1)
    {
      if(osOK ==  osMessageQueueGet(DW_GUI_WriteDataHandle,&GetD_SubmitReq,NULL,0))
      { 
        Pross++;
        SendLength  = (GetD_SubmitReq.length<<1);  
        USART_Send_Buffer[0]  = 0x5A;
        USART_Send_Buffer[1]  = 0xA5;
        USART_Send_Buffer[2]  = SendLength  + 3;
        USART_Send_Buffer[3]  = WriteData;
        USART_Send_Buffer[4]  = (uint8_t)(GetD_SubmitReq.ADDR>>8)&0xff;
        USART_Send_Buffer[5]  = (uint8_t)(GetD_SubmitReq.ADDR)&0xff;       
        Exchange_WordToChar(USART_Send_Buffer+6,GetD_SubmitReq.TargetADDR,GetD_SubmitReq.length);
        HAL_UART_Transmit(&huart3, USART_Send_Buffer, SendLength  + 6, 20);       
        if(CopyGuiDataToUart1)HAL_UART_Transmit(&huart1, USART_Send_Buffer, SendLength  + 6, 50);
        *(GetD_SubmitReq.Pro_OK)  = 1;
      }
      else
      {
        break;
      }  
    }
    
    RunningTime = osKernelGetTickCount()-RXTX_RTime;
    osDelay(20);    
    RXTX_RTime  = osKernelGetTickCount();
    
    
    while(Usart_RingBuff.Lenght	>=	7)
    {
      Pross++;
      Read_RingBuff(&Read_Ring_Byte);
      if(Read_Ring_Byte ==  0x5A)
      {
        Read_RingBuff(&Read_Ring_Byte);
        if(Read_Ring_Byte ==  0xA5)
        {
          //printf("Get Uart Head \r\n");
          Read_RingBuff(&Read_Ring_Byte);
          Read_Length = Read_Ring_Byte;
          while(Usart_RingBuff.Lenght	>=	Read_Length)
          {
            for(i=0;i<Read_Length;i++)
            {
              Read_RingBuff(Receive_Buff_U+i);                          
            }
            if(Receive_Buff_U[0]  ==  ReadData)
            {
              GetD_SubmitReq.ADDR   = u16_2CharTo1Word(Receive_Buff_U+1);
              GetD_SubmitReq.length = Receive_Buff_U[3];
              if(GetD_SubmitReq.ADDR  ==  DW_GUI_Touch_ADDR)
              {
                DW_GUI_Data.R_W     = ReadData;
                DW_GUI_Data.ADDR    = GetD_SubmitReq.ADDR;
                DW_GUI_Data.length  = GetD_SubmitReq.length;
                Exchange_CharToWord(DW_GUI_Data.Data,Receive_Buff_U+4,DW_GUI_Data.length<<1);
                if(DebugMode)printf("Touch code is %04x \r\n",DW_GUI_Data.Data[0]);
                if(osOK !=  DW_GUI_PutKey(DW_GUI_Data.Data[0]))
                  printf("DW_GUI_Message_Fun Put Touch code ERROR \r\n");
                
                //if(osOK !=  osMessageQueuePut(TouchCodeHandle,DW_GUI_Data.Data,NULL,10))
                //{
                //  printf("DW_GUI_Message_Fun Put Touch code ERROR \r\n");
                //}              
              }
              else
              {
								
								if(GetD_SubmitReq.ADDR	==	pcPage_ReAdjPoint_FanSpeed)//简单的全局广播机制，后面改进
								{									
									Exchange_CharToWord((uint16_t*)PWM_Get_Handle.pData,Receive_Buff_U+4,(GetD_SubmitReq.length<<1));
									*PWM_Get_Handle.DataFresh	=	1;								
								}
								
                GetD_SubmitReq.TargetADDR = GetData_LList_Take(&GetData_LList,&GetD_SubmitReq);
                Exchange_CharToWord((uint16_t*)GetD_SubmitReq.TargetADDR,Receive_Buff_U+4,(GetD_SubmitReq.length<<1));                           
                *(GetD_SubmitReq.Pro_OK)  = 1;                             
                break;             
              }
            }
            else if(Receive_Buff_U[0]  ==  WriteData)
            {
              //丢弃不处理
              break;
            }
            else
            {
              printf("UART CODE GET ERROR \r\n");
              break;
            }
          }
        }
      }
    }         
    
    
    APP_Restart = 1;
    if(Print_RXTX_RTime &&  Pross)printf("DW_GUI_Message_Fun Run Time:%d ms \r\n",osKernelGetTickCount()-RXTX_RTime+RunningTime);
		
		*/
		
		
		tick += tick_step;                      // delay 1000 ticks periodically
    osDelayUntil(tick);		
	}
}








ListStatus_t	List_Init(ListHandle_Def*	ListHandle)
{
	uint32_t	i;
	((ListCtrl_Def*)(ListHandle->pListData))->ListLastNum	=	0;
	((ListCtrl_Def*)(ListHandle->pListData))->ListNextNum	=	0;
	ListHandle->ListCounter	=	0;
	for(i=0;i<ListHandle->ListLength;i++)
	{
		((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	i))->ListCellEnable	=	0;	
	}	
	return	listOK;
}

uint32_t	List_GetLength(ListHandle_Def*	ListHandle)
{
	return	ListHandle->ListLength;
}

uint32_t	List_GetCounter(ListHandle_Def*	ListHandle)
{
	return	ListHandle->ListCounter;
}

ListStatus_t	List_Add(ListHandle_Def*	ListHandle,void*	pData)//只写了插入尾部的逻辑
{
	uint32_t	i,LastNumBuff,NextNumBuff,AddOffsetNum;
	void*			pBuffer;
	
	for(i=0;i<ListHandle->ListLength;i++)
	{
		if(((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	i))->ListCellEnable	==	0)
			break;
	}
	if(i	>=	ListHandle->ListLength)
			return	listErrorFull;
	AddOffsetNum	=	i;
	
	LastNumBuff	=	((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	0))->ListLastNum;
	NextNumBuff	=	((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	LastNumBuff))->ListNextNum;	
	
	pBuffer	=	((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	AddOffsetNum);
	((ListCtrl_Def*)pBuffer)->ListNextNum	=	NextNumBuff;
	((ListCtrl_Def*)pBuffer)->ListLastNum	=	LastNumBuff;
	((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	0))->ListLastNum	=	AddOffsetNum;
	((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	LastNumBuff))->ListNextNum	=	AddOffsetNum;
	for(i=0;i<(ListHandle->Data_Size	-	sizeof(ListCtrl_Def));i++)//数据拷贝
	{
		*(((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	AddOffsetNum	+	sizeof(ListCtrl_Def))	+	i)	=	*((uint8_t*)pData	+	i);
	}	
	((ListCtrl_Def*)pBuffer)->ListCellEnable	=	1;
	ListHandle->ListCounter++;
	return	listOK;	
	//((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	OffsetNum))//访问OFFset偏移量的内容的ListCtrl内容
	//(void*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	OffsetNum	+	sizeof(ListCtrl_Def)))访问OFFset偏移量的内容的Data的指针
}


ListStatus_t	List_Del(ListHandle_Def*	ListHandle,uint32_t	CounterNum)
{
	uint32_t	i,LastNumBuff,NextNumBuff,OffsetNum;
	void*			pBufferLast;
	void*			pBufferNext;
	void*			pBuffer;
	
	if(CounterNum	>=	(ListHandle->ListLength))
		return	listError;
	OffsetNum	=	0;
	for(i=0;i<CounterNum;i++)//找到实际地址
	{
		OffsetNum	=	((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	OffsetNum))->ListNextNum;
	}
	
	LastNumBuff	=	((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	OffsetNum))->ListLastNum;
	NextNumBuff	=	((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	OffsetNum))->ListNextNum;		
	
	pBuffer			=	((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	OffsetNum);
	pBufferLast	=	((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	LastNumBuff);
	pBufferNext	=	((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	NextNumBuff);
	
	
	((ListCtrl_Def*)pBufferLast)->ListNextNum	=	NextNumBuff;
	((ListCtrl_Def*)pBufferNext)->ListLastNum	=	LastNumBuff;
	((ListCtrl_Def*)pBuffer)->ListCellEnable	=	0;
	ListHandle->ListCounter--;
	
	return	listOK;	
}

ListStatus_t	List_Look(ListHandle_Def*	ListHandle,uint32_t	CounterNum,void*	pData)
{
	uint32_t	i,OffsetNum;	
	
	if(CounterNum	>=	(ListHandle->ListLength))
		return	listError;
	OffsetNum	=	0;
	for(i=0;i<CounterNum;i++)//找到实际地址
	{
		OffsetNum	=	((ListCtrl_Def*)((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	OffsetNum))->ListNextNum;
	}

	if(OffsetNum	>=	ListHandle->ListCounter)
		return	listError;
	
	for(i=0;i<(ListHandle->Data_Size	-	sizeof(ListCtrl_Def));i++)//数据拷贝
	{
		*((uint8_t*)pData	+	i)	=	*(((char*)(ListHandle->pListData)	+	(ListHandle->Data_Size)	*	OffsetNum	+	sizeof(ListCtrl_Def))	+	i);
	}	
	
	return	listOK;		
}


ListStatus_t	List_Take(ListHandle_Def*	ListHandle,uint32_t	CounterNum,void*	pData)
{
	List_Look(ListHandle,CounterNum,pData);
	List_Del(ListHandle,CounterNum);
	return	listOK;
}




