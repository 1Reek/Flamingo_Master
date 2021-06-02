/** 
 * �ļ�����Э�����Ͷ���, FileMsg.h
 * zhangyl 2017.03.17
 **/
#pragma once

#include <stdint.h>

enum file_msg_type
{
    file_msg_type_unknown,
    msg_type_upload_req,
    msg_type_upload_resp,
    msg_type_download_req,
    msg_type_download_resp,
};

enum file_msg_error_code
{
    file_msg_error_unknown,        //δ֪����
    file_msg_error_progress,       //�ļ��ϴ��������ؽ�����
    file_msg_error_complete,       //�ļ��ϴ������������
    file_msg_error_not_exist       //�ļ�������
};

//�ͻ�����������
enum client_net_type
{
    client_net_type_broadband,    //���
    client_net_type_cellular      //�ƶ�����
};

#pragma pack(push, 1)
//Э��ͷ
struct file_msg_header
{
    int64_t  packagesize;       //ָ������Ĵ�С
};

#pragma pack(pop)

/** 
 *  �ļ��ϴ�
 */
/**
    //filemd5Ϊ�����ļ����ݵ�md5ֵ��offsetΪÿ���ļ��ϴ���ƫ����, filesizeΪ�ļ���С��filedataΪÿ���ϴ����ļ��ֽ�����
    �ͻ��ˣ�cmd = msg_type_upload_req(int32), seq(int32), filemd5(string, ������32), offset(int64) ,filesize(int64), filedata(string)
    ������Ӧ�� cmd = msg_type_upload_resp(int32), seq(int32), errorcode(int32), filemd5(string, ������32), offset(int64), filesize(int64)
 **/

/** 
 *  �ļ�����
 */
/** 
    //filemd5Ϊ�����ļ����ݵ�md5ֵ��offsetΪÿ���ļ��ϴ���ƫ����, filesizeΪ�ļ���С��filedataΪÿ���ϴ����ļ��ֽ�����, 
    //clientNetType(�ͻ����������ͣ��������Wifi��0��3G/4G/5G��1��
    �ͻ��ˣ�cmd = msg_type_download_req, seq(int32), filemd5(string, ������32), offset(int64) ,filesize(int64), filedata(string), clientNetType(int32)
    ������: cmd = msg_type_download_resp(int32), seq(int32), errorcode(int32), filemd5(string, ������32), offset(int64), filesize(int64)
 **/

