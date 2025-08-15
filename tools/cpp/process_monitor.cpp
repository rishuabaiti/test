//进程监控模块----进程健康度检测（心跳、响应时间）,超时就杀掉程序
#include"../../common/common.h"


int main(int argc,char* argv[])
{
    if (argc != 2)
    {
        std::cout << "./process_monitor /home/ubuntu/bilibili/BigProjrct/tools/logs/process_monitor.log" << std::endl;
        return -1;
    }
    signal(SIGINT, SIG_IGN);// Ctrl+C (SIGINT)
    signal(SIGTERM, SIG_IGN);//kill -15 <PID>

    idc::FileLogger filelog;
    if (!filelog.Open(argv[1]))
    {
        std::cout << "filelog 打开失败" << std::endl;
    }
     int shmid=-1;
     idc::ProcessHeartbeat *shm_ptr;
    if((shmid=shmget((key_t)SHMKEY, MAXNUM*sizeof(idc::ProcessHeartbeat), IPC_CREAT | 0666))==-1)
    {
        filelog.Write("创建/获取共享内存 shmkey:%d失败!\n",SHMKEY);
        return -1;
    }

     // 将共享内存加到进程地址空间
    shm_ptr = (idc::ProcessHeartbeat *)shmat(shmid, NULL, 0);

    //对共享内存进行遍历
    for(int i=0;i<MAXNUM;i++)
    {
        if(shm_ptr[i].processpid==0)
        {
            continue;
        }
        //不是空的,也可能残留
       if(kill(shm_ptr[i].processpid,0)==-1)
       {
         //清空数据
         memset(&shm_ptr[i],0,sizeof(idc::ProcessHeartbeat));
         continue;

       }
       //货真价实的进程
       filelog.Write("processpid:%d,processname:%s,timeout:%d,recenttime:%d\n",shm_ptr[i].processpid,\
        shm_ptr[i].processname,shm_ptr[i].outtime,shm_ptr[i].recenttime);

        //检查进程是否超时
        idc::ProcessHeartbeat shmcopy=shm_ptr[i];
        int currenttime=time(NULL);
        if(currenttime-shm_ptr[i].recenttime<shm_ptr[i].outtime)
        {
            continue;//正常
        }
         filelog.Write("i=%d,pid=%d 进程已经超时!\n",i,shm_ptr[i].processpid);
        // 心跳不正常,进程存在
        // kill 15
        if (kill(shmcopy.processpid, 15) == -1)
        {
            // 强杀
            kill(shmcopy.processpid, 9);
            filelog.Write("i=%d,pid=%d 进程被强制杀死!\n", i, shmcopy.processpid);
            memset(&shm_ptr[i], 0, sizeof(idc::ProcessHeartbeat));
        }
        else
        {
            // 被正常杀死,需要检查进程是否不存在
            for (int j = 0; j < 5; j++)
            {
                sleep(1);
                if (kill(shmcopy.processpid, 0) == -1)
                {
                    break;
                }
            }
            if (kill(shmcopy.processpid, 0) == -1)
            {
                filelog.Write("i=%d,pid=%d进程正常退出!\n", i, shmcopy.processpid);
            }
            else
            {
                // 强杀
                kill(shmcopy.processpid, 9);
                filelog.Write("i=%d,pid=%d进程被强制杀死!\n", i, shmcopy.processpid);
                memset(&shm_ptr[i], 0, sizeof(idc::ProcessHeartbeat));
            }
        }
    }

    shmdt(shm_ptr);
}