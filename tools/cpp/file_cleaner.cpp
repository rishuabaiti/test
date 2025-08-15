//文件清理模块
#include"../../common/common.h"


//心跳机制
idc::HeartbeatController heartbeatcontrol;
void handle(int sig);
int main(int argc,char*argv[])
{
    if(argc!=4)
    {
        std::cout<<"/home/ubuntu/bilibili/BigProjrct/tools/bin/file_cleaner /home/ubuntu/bilibili/BigProjrct/surfdatas *.csv,*.json,*.xml 0.2"<<std::endl;
        return -1;
    }
    //屏蔽所有信号
    idc::ignore_all_signals();
    signal(SIGINT,handle);
    signal(SIGTERM,handle);

    heartbeatcontrol.AddProcessInfoToShm(30,"file_cleaner");

    //1.获取目录下所有文件名
    idc::DirScanner dir;
    if(dir.Opendir(argv[1],argv[2],5000,true)==false)
    {
        printf("目录:%s 打开失败!\n",argv[1]);
        return -1;
    }
    //2.遍历每一个文件进行删除
    std::string flietime=idc::calCurtime1("yyyymmddhh24miss",(int)(0-atof(argv[3])*24*60*60));
    while(dir.Readdir())
    {
        if(flietime>dir.m_mtime)
        {
            //需要删除
            if(remove(dir.absolute_path_filename.c_str())==0)
            {
                printf("删除文件:%s 成功!\n",dir.m_filename.c_str());
            }
            else{
                printf("删除文件:%s 失败!\n",dir.m_filename.c_str());
            }
        }
    }
    return 0;
}

void handle(int sig)
{
    std::cout<<"收到 %d 信号,退出程序\n"<<std::endl;
    exit(1);
}