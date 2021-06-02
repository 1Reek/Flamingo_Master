/** 
 *  �ļ�������, FileManager.h
 *  zhangyl 2017.03.17
 **/
#pragma once
#include <string>
#include <list>
#include <mutex>

class FileManager final
{
public:
    FileManager();
    ~FileManager();

    FileManager(const FileManager& rhs) = delete;
    FileManager& operator = (const FileManager& rhs) = delete;

    bool init(const char* basepath);

    bool isFileExsit(const char* filename);
    void addFile(const char* filename);

private:
    //�ϴ����ļ������ļ���md5ֵ����
    std::list<std::string>      m_listFiles;
    std::mutex                  m_mtFile;
    std::string                 m_basepath;
};