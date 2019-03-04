/************************************************************************
* ��Ȩ���� (C)2016, �����������ܿƼ��ɷ����޹�˾��
*
* �ļ����ƣ� // ADE7858.H
* �ļ���ʶ�� // ADE7858�����ļ�ͷ
* ����ժҪ�� // 
* ����˵���� //
* ��ǰ�汾�� // V01.00
* �� �ߣ� // yangxing
* ������ڣ� // 2016��08��06��
*
* �޸ļ�¼1��//
* �޸����ڣ�
* �� �� �ţ�
* �� �� �ˣ�
* �޸����ݣ�
************************************************************************/









//У��˲ʱ���ṹ�嶨��
typedef struct
{
    unsigned long  UAdj[3]; //XXXX.XXXX A��B��C���ѹ BCD
    unsigned long  IAdj[3]; //XX.XXXX A��B��C����� BCD
    unsigned long  PAdj[4]; //XX.XXXXXX �ܡ�A��B��C���й����� BCD
    unsigned long  RAdj[4]; //XX.XXXXXX �ܡ�A��B��C���޹����� BCD
} REALADJ;
RN8302_EXT REALADJ RealAdj;

//˲ʱ���Ĵ����ṹ�嶨��
typedef struct
{
    unsigned long  Urms[3];//��ѹ�Ĵ�����Чֵ
    unsigned long  Irms[3];//����������Чֵ
    unsigned long  DmdP[2]; //���򡢷��򡢵�ǰ�й�������һ�����ۼƹ���
    unsigned long  DmdTouP[2];//���򡢷����ʱ����һ�����ۼƹ���
    unsigned long  CurDmdP[3];//��ǰ�С��޹�������һ�����ۼƹ���
    unsigned long  QuadrDmdP[4];//4�������޹�����һ�����ۼƹ���
    unsigned long  QuadrDmdTouP[4];//4���޷�ʱ�����޹�����һ�����ۼƹ���
    signed long    GrpDmdP[2]; //����޹�1��2������һ�����ۼƹ���
    signed long    GrpDmdTouP[2];//����޹�1��2��ʱ����һ�����ۼƹ���
    unsigned long    ActWatthrP;//�й�����������
    unsigned long    ActWatthrN;//�й�����������
    unsigned long    RactVarhr;//�޹�������
    unsigned long    Vahr;//����������
    unsigned long    EnergyBuf[9];
} CHIPREG;
RN8302_EXT CHIPREG ChipReg;

//��������ṹ�嶨��
typedef struct
{
    unsigned long    ActPowerP;//���й���������˲ʱֵ
    unsigned long    ActPowerN;//���й���������˲ʱֵ
    unsigned long    ActPowerPp[3]; //A��B��C�����й�����˲ʱֵ
    unsigned long    ActPowerPn[3]; //A��B��C�����й�����˲ʱֵ
    unsigned long    RactPower;//���޹�����˲ʱֵ
    unsigned long    RactPowerPp[3]; //A��B��C�޹�����˲ʱֵ
    unsigned long    VahrPower[2];//����������������˲ʱֵ
} POWERVALUE;
#define  LenValue  56
RN8302_EXT POWERVALUE PowerValue;

//У�������ṹ�嶨��
typedef struct
{
    unsigned short  Igain; //��������
    unsigned short  Ugain; //��ѹ����
    unsigned short  Pgain; //��������
    unsigned short  Iofst; //����ƫ��
    unsigned short  Uofst; //��ѹƫ��
    unsigned short  Pofst; //����ƫ��
    unsigned char  UTofst; //����ͨ��U��λƫ��
    unsigned char  ITofst[3]; //����ͨ��I��λƫ��
    unsigned short PTofst[3]; //�С��͡��߹�����λƫ��
    unsigned short AdjChk;
} ADJ;
#define  LenAdjBuff  24
typedef union
{
    unsigned char   AdjBuff[LenAdjBuff];
    ADJ 	    Adj;
} ADJ_BLOCK;
RN8302_EXT ADJ_BLOCK Adj_Block[3];


RN8302_EXT unsigned char  RN8302Status;
RN8302_EXT unsigned char  RN8302Errcum;

RN8302_EXT unsigned char  Second_8302;
RN8302_EXT unsigned char  Second_Power;
RN8302_EXT unsigned char  PowerRead_Flag;
RN8302_EXT unsigned char  Quadrant[4];//����ָʾ
RN8302_EXT unsigned char  Angle_I;//�ǶȲ���
RN8302_EXT unsigned char  U_InvOrder_Flag;//��ѹ������״̬λ
RN8302_EXT unsigned char  I_InvOrder_Flag;//����������״̬λ
RN8302_EXT unsigned char  FactoryCFDEN;
RN8302_EXT unsigned long  RN8302Checksum;
RN8302_EXT unsigned char  PowerReact1_Flag;
RN8302_EXT unsigned char  PowerReact2_Flag;

extern void RN8302Init(void);
extern void RN8302_Reset(void);
extern void Enable_SPI(void);
extern void RN8302_BasicPara(void);
extern void RN8302_Run(void);
extern void ParaMeasure(void);
extern void InstantPowerMeas(void);
extern unsigned char UMesure_Init(void);