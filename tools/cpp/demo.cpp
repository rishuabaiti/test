#include"../../common/common.h"
#include <sys/ipc.h>
#include <sys/shm.h>
//心跳模块测试
#define SHAREMEM_KEY 0X5095
//里面不能STL容器,因为这是共享内存内容,STL不属于共享内存
struct ProcessHeartbeat{
    int  processpid;
    char processname[51];
    int outtime;
    time_t recenttime;

    ProcessHeartbeat():processpid(-1),outtime(0),recenttime(0)
    {
        memset(processname, 0, sizeof(processname));
    }

    ProcessHeartbeat(int pid,const std::string&name,int otime,time_t rtime):\
    processpid(pid),outtime(otime),recenttime(rtime)
    {
        memcpy(processname,name.c_str(),50);
        processname[50]='\0';
    }

};

ProcessHeartbeat *shm_ptr=nullptr;
int pos=-1;
int shmid=-1;//共享内存标识符

 idc::SystemVSem sem;
void  signal_handler(int sig);
int main(int argc,char* argv[])
{
   sem.init((key_t)SHAREMEM_KEY);
    signal(SIGINT,signal_handler);
    signal(SIGTERM,SIG_IGN);
    
    
   if((shmid=shmget((key_t)SHAREMEM_KEY, 1000*sizeof(ProcessHeartbeat), IPC_CREAT | 0666))==-1)
   {
        perror("创建/获取共享内存失败!\n");
         return -1;
   }
   //将共享内存加到进程地址空间
     shm_ptr= (ProcessHeartbeat*)shmat(shmid, NULL, 0);
     if(shm_ptr==(void*)-1)
     {
        perror("shmat 失败\n");
        return -1;
     }
    
     std::string pname=argv[0]+std::to_string(getpid());
     ProcessHeartbeat pht(getpid(),pname,atoi(argv[1]),time(NULL));
     //找到空位置,放进心跳
     sem.wait();
     for(int i=0;i<1000;i++)
     {
        if(pht.processpid==shm_ptr[i].processpid)
        {
            pos=i;
            printf("找到旧位置 %d ",pos);
            break;
        }
     }
     if(pos==-1)
     {
        for(int i=0;i<1000;i++)
        {
            if(shm_ptr[i].processpid==0)
            {
                pos=i;
            printf("找到新位置 %d ",pos);
            break;
            }
        }
     }
     if(pos==-1)
     {
         // 没找到---内存满了
         perror("共享内存用完了");
         sem.post();
         signal_handler(-1);
        //分离内存---return
     }
     memcpy(shm_ptr+pos,&pht,sizeof(pht));
     sem.post();
     
    

     while(1)//处理心跳
     {
        
        std::cout<<"处理心跳.....\n"<<std::endl;
        std::cout<<"信号量:"<<sem.getSemid()<<std::endl;
        sem.wait();
        shm_ptr[pos].recenttime = time(NULL);
        for (int i = 0; i < 1000; i++)
        {
            if (shm_ptr[i].processpid != 0)
            {
                std::cout <<i<<" "<< shm_ptr[i].processpid << " " << shm_ptr[i].processname << " " << shm_ptr[i].outtime << " " << shm_ptr[i].recenttime << std::endl;
            }
        }
        sem.post();
        sleep(atoi(argv[2]));

     }
}

void  signal_handler(int sig)
{
    printf("收到%d 信号\n", sig);
    //清除结构体内容
     if(pos!=-1)
     {
        sem.wait();
       memset(&shm_ptr[pos],0,sizeof(ProcessHeartbeat));
       sem.post();
     }
    //分离共享内存
    if(shm_ptr!=nullptr)  
    {
        shmdt(shm_ptr);
    }

    exit(0);
}