#	include	"MagiSYNC_PublicData.h"

//�ò��ֿ���������

//�ؼ�����
#	define		GUI_SyncUsart								huart3






//����ʹ������
#	define		TransferMaxLimitEnable			1													//�Ƿ�������ͨѶ����







//�����������
#	define		GUIRefresh_Freq							20								//ˢ����,Ĭ��20����ÿ�봦��20��UI
#	define		MessageProcessStackSize			2048							//���ڻ��λ��������ȣ����ݽ�����ͨ�����λ�����ʵ�ֵģ�����̫С
#	define		UsartRing_Length						1024							//���ڻ��λ��������ȣ����ݽ�����ͨ�����λ�����ʵ�ֵģ�����̫С
#	define		UsartSendBuff_Length				512								//���ڷ��ͻ���������
#	define		GlobalRegRefreshEnable			1									//ʹ�ܱ���ȫ��ˢ�£�ȫ��ˢ�µı�����Ҫʹ��MagiSYNC_GlobalReg_Initע�����ʹ��

#	define		DuplexDataSYNC_Mode					ShadowReg_Mode		//˫����������ͬ������


#if(GUI_SYNC_Mode	==	PageSYNC_Mode)||(GUI_SYNC_Mode	==	PageGolbalMixedSYN_CMode)
	#	define		GUI_PageBuffSize						0x100							//ҳ���建���С
#endif

#if(GUI_SYNC_Mode	==	GolbalSYNC_Mode)||(GUI_SYNC_Mode	==	PageGolbalMixedSYN_CMode)
	#	define		GUI_GolbalSYNC_StartADDR		0x0000						//ͬ��������ʼ��ַ
	#	define		GUI_GolbalSYNC_Size					0x0000						//ͬ���������������С
#endif

#if	TransferMaxLimitEnable
	# define 	TransferMaxLimit						TransferMaxLimitDefault		//Ĭ��64���֣���128�ֽ�		
#else
	# define 	TransferMaxLimit						120												//���259�ֽڣ�����ĵ�λΪ�֣���240�ֽ�
#endif

#	define		GUI_Touch_ADDR							0X5000						//���ر������ص�ַ��Ĭ��0X5000��Ҳ�ɸĳ�����



//�ò��־�����Ҫ�Ķ������ܻ�����Ӱ�������ٶȻ����������
#	define		GUI_MessProBuffLength			128							//���ݽ��������ڲ����������ȣ���С�ᵼ���޷����������ַ�����������ҪС��32





