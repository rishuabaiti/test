//压缩指定文件为.gz的功能程序
#include"../../common/common.h"

void  handle(int sig);
//心跳模块
idc::HeartbeatController heartbeat;
int main(int argc,char*argv[])
{
    if(argc!=4)
    {
        std::cout<<"/home/ubuntu/bilibili/BigProjrct/tools/bin/file_compressor /home/ubuntu/bilibili/BigProjrct/logs *.txt 0"<<std::endl;
        return -1;
    }
    heartbeat.AddProcessInfoToShm(120,"file_compressor");
    //1.找到目录下所有文件
    idc::DirScanner dir;
    if(dir.Opendir(argv[1],argv[2],5000,true)==false)
    {
         printf("打开目录:%s 失败\n",dir.absolute_path_filename.c_str());
         return -1;
    }
    //2.遍历目录下的文件进行压缩gz
    std::string currenttime=idc::calCurtime1("yyyymmddhh24miss",(int)(0-atof(argv[3])*24*60*60));
    
    if(dir.Readdir())
    {
        std::string gzipcmd="/usr/bin/gzip -f "+dir.absolute_path_filename;
        if((currenttime>dir.m_mtime)&&(idc::matchstr(dir.m_filename,"*.gzip")==false))
        {
             if(system(gzipcmd.c_str())==0)
             {
                printf("压缩文件:%s 成功\n",dir.m_filename.c_str());
             }
             else{
                 printf("压缩文件:%s 失败\n",dir.m_filename.c_str());
             }
        }
        //跟新心跳  这个压缩的时间可能需要持续
        heartbeat.UpRecenttime();
    }
    return 0;
}
void handle(int sig)
{
    printf("收到了 %d 信号退出\n",sig);
    exit(1);
}