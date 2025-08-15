#include"../common/common.h"
idc::TcpServer tcpserver;
idc::FileLogger logfile;
std::string recvmessage;
std::string sendmessage;

void EXITFATHER(int sig);
void EXITCHILD(int sig);
bool handle(const std::string&timeout);
bool heartbeat();
bool  login();
bool query();
bool transfer();
int main(int argc,char*argv[])
{
    if(argc!=4)
    {
        printf("/home/ubuntu/bilibili/BigProjrct/test_tcp/tcp_server /home/ubuntu/bilibili/BigProjrct/test_tcp/tcp_log 8080 10\n");
        return -1;
    }
    //屏蔽信号
    idc::ignore_all_signals();
    signal(SIGTERM,EXITFATHER);
    signal(SIGINT,EXITFATHER);
    signal(SIGCHLD,SIG_IGN);//当子进程exit时,系统会回收,不会产生僵尸进程-----Linux 特有的机制
    if(logfile.Open(argv[1])==false)
    {
        printf("日志文件:%s 打开失败\n",argv[1]);
        return -1;
    }
    if(tcpserver.Initserver(std::stoi(argv[2]))==false)
    {
       logfile.Write("tcpserver 初始化失败\n");
       return -1;
    }
    while (1)
    {
        if (tcpserver.Accept() == false)
        {
            logfile.Write("获取客户端连接失败\n");
            return -1;
        }
        // 子进程
        if (fork() == 0)
        {
            signal(SIGTERM, EXITCHILD);
            signal(SIGINT, EXITCHILD);
            // 关闭监听套接字
            tcpserver.CloseListen();
            //处理业务函数,这是一个循环函数,一直读取消息处理
            handle(argv[3]);
            EXITCHILD(0);
        }
        // 父进程
        // 关闭客户端连接
        tcpserver.CloseClient();
    }

    return 0;
}
bool handle(const std::string&timeout)
{
    recvmessage.clear();
    while (tcpserver.Read(recvmessage,std::stoi(timeout)))
    {
        int id;
        if (idc::getXmlValue(recvmessage, "serverid", id) == false)
        {
            logfile.Write("解析业务代码失败\n");
            return false;
        }
        switch (id)
        {
        case -1:
            heartbeat();
            break;
        case 0:
            login();
            break;
        case 1:
            query();
            break;
        case 2:
            transfer();
            break;
        default:
            sendmessage = "<serverid>9</serverid><message>错误信息!</message>";
            tcpserver.Write(sendmessage);
            break;
        }
    }
    return false;
}
bool heartbeat()
{
    std::string sendmessage="心跳机制已增加";
    if (tcpserver.Write(sendmessage)==false)
    {
        logfile.Write("心跳信息相应失败\n");
        return false;
    }
    return true;
}

bool  login()
{
    std::string usercount,password;
    if(idc::getXmlValue(recvmessage,"usercount",usercount)==false)
    {
        logfile.Write("解析用户账号失败\n");
        return false;
    }
    if(idc::getXmlValue(recvmessage,"password",password)==false)
    {
        logfile.Write("解析用户密码失败\n");
        return false;
    }
    if (usercount == "1111" & password == "1234")
    {
        sendmessage = "<serverid>0</serverid><message>登录成功!</message>";
        if (tcpserver.Write(sendmessage))
        {
            logfile.Write("账号:%s,密码:%s,客户端登录成功\n", usercount.c_str(), password.c_str());
            return true;
        }
    }
    return false;
}
bool query()
{
    std::string usercount, password;
    if (idc::getXmlValue(recvmessage, "usercount", usercount) == false)
    {
        logfile.Write("解析用户账号失败\n");
        return false;
    }
    if (usercount == "1111")
    {
        sendmessage = "<serverid>1</serverid><message>余额:100.6</message>";
        if (tcpserver.Write(sendmessage))
        {
            logfile.Write("账号:%s,余额:100.6\n", usercount.c_str());
            return true;
        }
    }
    return false;
}
bool transfer()
{
    std::string usercount, othercount,amount;
    if (idc::getXmlValue(recvmessage, "usercount", usercount) == false)
    {
        logfile.Write("解析用户账号失败\n");
        return false;
    }
     if (idc::getXmlValue(recvmessage, "othercount", othercount) == false)
    {
        logfile.Write("解析对方账号失败\n");
        return false;
    }
     if (idc::getXmlValue(recvmessage, "amount", amount) == false)
    {
        logfile.Write("解析转账余额失败\n");
        return false;
    }
    if (usercount == "1111")
    {
        sendmessage = "<serverid>2</serverid><message>转账:100</message>";
        if (tcpserver.Write(sendmessage))
        {
            logfile.Write("转账:100 成功\n");
            return true;
        }
    }
    return false;
}

void EXITCHILD(int sig)
{
    //防止处理该函数时退出
     signal(SIGTERM,SIG_IGN);
     signal(SIGINT,SIG_IGN);
     tcpserver.CloseClient();
     exit(1);

}
void EXITFATHER(int sig)
{
    //防止处理该函数时退出
     signal(SIGTERM,SIG_IGN);
     signal(SIGINT,SIG_IGN);
     kill(0,15);//杀死所有子进程
     tcpserver.CloseListen();
     exit(1);
  
}