#include "uart7_vision_analysis.h"
#include "protect.h"
#include "auto_aim.h"
//#include "stm32f4xx.h"

extern YUN_MOTOR_DATA 			yunMotorData;

VisionDataTypeDef	VisionData={0};
VisionReceiveDataTypeDef VisionReceiveData={0};

#define DATA_LEN 10
u32 t_vision_count=0;
void VisionData_Receive(u8 data)	//�����崫���������ݽ�����������ͨ�ã�
{
//	LostCountFeed(&Error_Check.count[LOST_VICEBOARD]);
	if(data==0x5A&&VisionReceiveData.headOK_state==0)
	{
		VisionReceiveData.valid_state=0;	//���ݽ����ڼ䲻�������ݽ���
		VisionReceiveData.headOK_state=1;	
		VisionReceiveData.count=0;	//����count
	}
	
	if(VisionReceiveData.headOK_state==1)	//֡ͷ���ҵ�
	{
		VisionReceiveData.databuffer[VisionReceiveData.count]=data;
		VisionReceiveData.count++;
		if((data==0xA5&&VisionReceiveData.count!=DATA_LEN)||(VisionReceiveData.count>DATA_LEN))	//ʧЧ
		{
			VisionReceiveData.valid_state=0;
			VisionReceiveData.headOK_state=0;
			VisionReceiveData.count=0;	//����count
		}
		else if(data==0xA5&&VisionReceiveData.count==DATA_LEN)
		{
			VisionReceiveData.valid_state=1;
			VisionReceiveData.headOK_state=0;
			VisionReceiveData.count=0;	//����count
		}
	}
	t_vision_count++;
	//////////////////////////////��������ݽ�������-->����Ϊ��ʵ����
	if(VisionReceiveData.valid_state==1)	//����������Ч
	{
//		t_vision_count++;
		LostCountFeed(&Error_Check.count[LOST_VISION]);
		VisionData_Deal(VisionReceiveData.databuffer);
	}
	
}


float test_pitch;

void VisionData_Deal(volatile u8 *pData)	//�����������ڳ���֡ͷ�ĵ�1֡
{
	VisionData.armor_sign=*(pData+1)>>(7)&0x01;
	VisionData.armor_type=*(pData+1)>>(5)&0x03;
	VisionData.armor_dis=*(pData+2);
	VisionData.tar_x=*(pData+3)<<8|*(pData+4);
	VisionData.tar_y=*(pData+5)<<8|*(pData+6);
	VisionData.pix_x_v=*(pData+7)<<8|*(pData+8);
	
	
	//if(RC_Ctl.rc.switch_right==RC_SWITCH_UP&&GetWorkState()==NORMAL_STATE )	//�����ж�������
	//{
		test_pitch=0;
		Vision_Task(&yunMotorData.yaw_tarP,&test_pitch);	//���Ƽ�λ�������ڲ�
	//}
}