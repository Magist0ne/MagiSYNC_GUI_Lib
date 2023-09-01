#	include	"MagiSYNC_Config.h"

//Config  �������
#if	(GUIRefresh_Freq	>	50)
#error GUIRefresh_Freq Too High , Don't Higher Than 30
#endif

#if	(GUI_MessProBuffLength	<	32)
#error GUI_MessProBuffLength Too Few
#endif

#if	(TransferMaxLimit				>	124)
#error TransferMaxLimit Too More
#endif

typedef struct//���ڻ��λ��������ݽṹ
{
    uint16_t Head;           
    uint16_t Tail;
    uint16_t Lenght;
    uint8_t  Ring_data[UsartRing_Length];
}RingBuff_t;

/*
�����ӿ���ƣ�

//�����ӿں���
syncStatus_t	MagiSYNC_Init();//��ʼ��GUI�⣬�����������
syncStatus_t	MagiSYNC_DisInit();//����ʼ��GUI�⣬ɾ���������

syncStatus_t	MagiSYNC_Reg_Init();//ע�����������ҳ��ˢ�£���Ҫ����ҳ����
syncStatus_t	MagiSYNC_Reg_DisInit();//��ע�����

syncStatus_t	MagiSYNC_GlobalReg_Init();//ע��ȫ�ֱ�����ȫ�ֱ�����ʵʱ���£���Ϊ������º�˫����£�һ����ַ������ע������ֱ���
syncStatus_t	MagiSYNC_GlobalReg_DisInit();//��ע��ȫ�ֱ���


syncStatus_t	MagiSYNC_Reg_Read();//�Ĵ�����д
syncStatus_t	MagiSYNC_Reg_Write();


syncStatus_t	MagiSYNC_Data_Read();//���ݶ�д��ͨ�����üĴ�����д����ʵ�֣���Ҫ������������
syncStatus_t	MagiSYNC_Data_Write();


syncStatus_t	MagiSYNC_Char_Read();//�ַ�����д��ͨ�����üĴ�����д����ʵ�֣���Ҫ�����ַ�������
syncStatus_t	MagiSYNC_Char_Write();


//ͨѶ��ӿں���
syncStatus_t	MagiSYNC_GetUartData();//��ȡһЩ����
syncStatus_t	MagiSYNC_SendUartData();//����һЩ����





//���η�װ���ܽӿ�





*/



void MessageProcess(void *argument);

syncStatus_t	MagiSYNC_Init(void);//��ʼ��GUI�⣬�����������
syncStatus_t	MagiSYNC_DisInit(void);










