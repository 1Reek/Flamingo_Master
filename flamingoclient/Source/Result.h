#pragma once

//��������״ֵ̬
enum LOGIN_RESULT
{
	LOGIN_SUCCESSFUL = 1,	//��¼�ɹ�
	LOGIN_CANNOT_CONNECT,	//���Ӳ��Ϸ�����
	LOGIN_INVALID_ACCOUNT,	//��Ч���û���������
	LOGIN_FAILED			//��¼ʧ��
};

enum REGISTER_RESULT
{
	REGISTER_SUCCESSFUL = 1,	//ע��ɹ�
	REGISTER_EXIST,				//�Ѿ���ע���
	REGISTER_FAILED				//ע��ʧ��
};
