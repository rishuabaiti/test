//进程重启模块
#include <unistd.h>  // 包含 fork()、exec()、getpid() 等进程控制函数
#include <sys/types.h>
#include<iostream>
#include <signal.h>
#include <sys/wait.h> 
#include <string.h> 
#include<vector>
int main(int argc,char *argv[])
{
    if(argc<3)
    {
        std::cout<<"/home/ubuntu/bilibili/BigProjrct/tools/bin/process_control 60 /home/ubuntu/bilibili/BigProjrct/idc/bin/crtsurfdata /home/ubuntu/bilibili/BigProjrct/idc/ini/stcode.ini /home/ubuntu/bilibili/BigProjrct/surfdatas /home/ubuntu/bilibili/BigProjrct/logs/log.txt"<<std::endl;
        return -1;
    }

    //1.创建一个孤儿进程,屏蔽所有信号,保证在后台稳定运行
    int ret=fork();
    if(ret>0)
    {
        //父进程直接退出
        _exit(1);
    }

    //子进程变为孤儿进程  由init pid=1的进程进行管理 屏蔽子进程所有信号,放出退出信号,因为他要作为父进程wait
    for (int sig = 1; sig < NSIG; sig++)
    {
        signal(sig, SIG_IGN); // 设置为忽略
    }
    signal(SIGCHLD, SIG_DFL);

    //
    // 安全示例：
    std::vector<std::string> args_save; // 在fork前保存
    for (int i = 0; i < argc; i++)
    {
        args_save.emplace_back(argv[i]); // 深拷贝
    }

    
    // char *argvv[argc-2];
    // for(int i=2;i<argc;i++)
    // {
    //     argvv[i-2]=argv[i];
    // }
    // argvv[argc-2]=NULL;

    while(1)
    {
        if(fork()==0)
        {
            // 子进程中使用：
           char*argvv[argc - 2];
            for (int i = 2; i < argc; i++)
            {
                argvv[i - 2] = args_save[i].data();
            }
            argvv[argc - 2] = NULL;
           
            //子进程进行启动别的程序
            // execv(argv[2],argvv);
            // _exit(1);
            if (execv(args_save[2].data(), argvv) == -1)
            {
                std::cerr << "execv 失败: " << strerror(errno) << std::endl;
                _exit(1); // 退出码非 0，父进程可通过 WEXITSTATUS 检测
            }
        }
        //父进程等待
        int status;
        wait(&status); // 若子进程不退出,会阻塞
        if (WIFEXITED(status))
        {
            std::cout << "子进程正常退出，返回值: " << WEXITSTATUS(status) << std::endl;
        }
        else if (WIFSIGNALED(status))
        {
            std::cout << "子进程被信号终止: " << WTERMSIG(status) << std::endl;
        }
        std::cout<<args_save[1]<<"秒后重新启动"<<std::endl;
        sleep(stoi(args_save[1]));

    }
    return 0;
}