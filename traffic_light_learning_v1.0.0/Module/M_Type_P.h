/***********************************************************************
�ļ����ƣ�
��    �ܣ�
��дʱ�䣺
�� д �ˣ�
ע    �⣺
***********************************************************************/
 
#ifndef _M_TYPE_P_H_
#define _M_TYPE_P_H_


/*
 * ʹ�ø�ģ����Ҫ����Ļ�����������
 *
 *   BOOLEAN: ������
 *   INT8U  : �޷��ŵ�8λ��
 *   INT8S  : �з��ŵ�8λ��
 *   INT16U : �޷��ŵ�16λ��
 *   INT16S : �з��ŵ�16λ��
 *   INT32U : �޷��ŵ�32λ��
 *   INT32S : �з��ŵ�32λ��
 * 	 FP32   : �����ȸ�����
 *   FP64   : ˫���ȸ�����
 * 
 *   Type_DataWidth  :  ���ݿ������
 *
 */


typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U; 
typedef signed   char  INT8S; 
typedef unsigned short INT16U;
typedef signed   short INT16S;
typedef unsigned int   INT32U;
typedef signed   int   INT32S;
typedef float          FP32;   
typedef double         FP64;   


//���ݿ������
typedef enum{
    DATA_W8=0,	// 8λ���
    DATA_W16,   //16λ���
    DATA_W32,	//32λ���
    DATA_W64	//64λ���
}Type_DataWidth;


// ������������
#ifndef M_TRUE
	#define M_TRUE	(1)
#endif

#ifndef M_FALSE
	#define M_FALSE (0)
#endif

#endif
