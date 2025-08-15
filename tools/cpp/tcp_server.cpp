#include"../../common/common.h"

struct Args
{
    int clienttype;          // 客户端类型，1-上传文件；2-下载文件，本程序固定填1。
    char ip[31];              // 服务端的IP地址。
    int port;                 // 服务端的端口。
    char clientpath[256];     // 本地文件存放的根目录。 /data /data/aaa /data/bbb
    int type;                 // 文件上传成功后本地文件的处理方式：1-删除文件；2-移动到备份目录。
    char clientpathback[256]; // 文件成功上传后，本地文件备份的根目录，当ptype==2时有效。
    char serverpathback[256]; 
    bool includeSubdirFiles;  // 是否上传clientpath目录下各级子目录的文件，true-是；false-否。
    char matchname[256];      // 待上传文件名的匹配规则，如"*.TXT,*.XML"。
    char serverpath[256];     // 服务端文件存放的根目录。/data1 /data1/aaa /data1/bbb
    int timeInterval;         // 扫描本地目录文件的时间间隔（执行文件上传任务的时间间隔），单位：秒。
    int timeout;              // 进程心跳的超时时间。
    char processname[51];     // 进程名，建议用"tcpputfiles_后缀"的方式。
};

idc::TcpServer tcpserver;
idc::FileLogger logfile;
std::string recvmessage;
std::string sendmessage;
Args args;
idc::HeartbeatController heartbeat;
bool iscontinue;

void EXITFATHER(int sig);
void EXITCHILD(int sig);
bool HandleFileUpload();
bool HandleFileDownload();
bool clientlogin();
bool receiveFileContentFromClient(const std::string clientfilename,const std::string filemtime,int filesize);
bool PutFiles();
bool SendHeartbeat();
bool sendFileContentToClient(const std::string&filename,int filesize);
bool clientRespHandler(const std::string &message);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("/home/ubuntu/bilibili/BigProjrct/tools/bin/tcp_server /home/ubuntu/bilibili/BigProjrct/tools/logs/tcp_server.log 8080\n");
        return -1;
    }
    // 屏蔽信号
    idc::ignore_all_signals();
    signal(SIGTERM, EXITFATHER);
    signal(SIGINT, EXITFATHER);
    signal(SIGCHLD, SIG_IGN); // 当子进程exit时,系统会回收,不会产生僵尸进程-----Linux 特有的机制
    if (logfile.Open(argv[1]) == false)
    {
        printf("日志文件:%s 打开失败\n", argv[1]);
        return -1;
    }
    if (tcpserver.Initserver(std::stoi(argv[2])) == false)
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
        logfile.Write("获取客户端连接,客户端ip=%s\n", tcpserver.GetClientIp().c_str());
        // 子进程
         if (fork() == 0)
         {
            //子进程只会接受父进程的15信号
            signal(SIGTERM, EXITCHILD);
            signal(SIGINT, SIG_IGN);
            // 关闭监听套接字
            tcpserver.CloseListen();
            // 处理业务函数,这是一个循环函数,一直读取消息处理
            if(clientlogin()==false)
            {
                 logfile.Write("获取客户端登录报文失败\n");
                 EXITCHILD(-1);
            }
            //每个子进程的心跳信息,除了pid不同,其余和客户端通信的信息相同
             heartbeat.AddProcessInfoToShm(args.timeout,args.processname);
             if (args.clienttype == 1)
             {
                 HandleFileUpload();
             }
             else
             {
                 HandleFileDownload();
             }

            EXITCHILD(0);
        }
        // 父进程
        // 关闭客户端连接
        tcpserver.CloseClient();
    }

    return 0;
}
bool HandleFileUpload()
{
    recvmessage.clear();
   while(tcpserver.Read(recvmessage,args.timeInterval+10))
   {
        heartbeat.UpRecenttime();//只要客户端没有消息,这里就不更新,就会被杀死
       //处理心跳
      if(recvmessage=="<heartbeat>ok</heartbeat>")
      {
        sendmessage="ok";
       if( tcpserver.Write(sendmessage)==false)
       {
        logfile.Write("发送客户端:ip=%s,心跳信息失败!\n",tcpserver.GetClientIp().c_str());
        return false;
       }
       logfile.Write("发送客户端:ip=%s,心跳响应:ok!\n",tcpserver.GetClientIp().c_str());

      }
      //处理上传业务
       if(recvmessage.find("<filename>")!=std::string::npos)
       {
          std::string filename,filemtime;
          int filesize;
          idc::getXmlValue(recvmessage,"filename",filename);
          idc::getXmlValue(recvmessage,"filesize",filesize);
          idc::getXmlValue(recvmessage,"filemtime",filemtime);

          // 接受内容
          logfile.Write("接收上传文件:%s(%d)... \n",filename.c_str(),filesize);
          if (receiveFileContentFromClient(filename, filemtime, filesize) == false)
          {
              logfile.Write("no... \n");
              idc::coutFormat(sendmessage, "<filename>%s</filename><result>no</result>", filename.c_str());
          }
          else
          {
              logfile.Write("ok... \n");
              idc::coutFormat(sendmessage, "<filename>%s</filename><result>ok</result>", filename.c_str());
          }
            //发回响应信息
          if(tcpserver.Write(sendmessage)==false)
          {
             logfile.Write("发送客户端:%s \n",sendmessage.c_str());
             return false;
          }

       }

   }
   logfile.Write("客户端:ip=%s,读取客户端信息失败/超时!\n",tcpserver.GetClientIp().c_str());
   return false;
}


void EXITCHILD(int sig)
{
    //防止处理该函数时退出
     signal(SIGTERM,SIG_IGN);
     signal(SIGINT,SIG_IGN);
     logfile.Write("子进程:pid=%d,收到:%d 信号退出\n",getpid(),sig);
     tcpserver.CloseClient();
     exit(0);

}
void EXITFATHER(int sig)
{
    //防止处理该函数时退出,主要防止被自己发出的15杀掉
     signal(SIGTERM,SIG_IGN);
     signal(SIGINT,SIG_IGN);

     logfile.Write("父进程:pid=%d,收到:%d 信号退出\n",getpid(),sig);
     tcpserver.CloseListen();
     kill(0,15);//杀死所有子进程
     exit(0);
  
}

bool clientlogin()
{
     recvmessage.clear();
     memset(&args,0,sizeof(args));
     if(tcpserver.Read(recvmessage,10)==false)
     {
            logfile.Write("读取客户端登录报文失败\n");
            return false;
     }
     idc::getXmlValue(recvmessage, "clienttype", args.clienttype);
     idc::getXmlValue(recvmessage, "clientpath", args.clientpath);
     idc::getXmlValue(recvmessage, "serverpath", args.serverpath);

     idc::getXmlValue(recvmessage, "timeInterval", args.timeInterval);
     idc::getXmlValue(recvmessage, "timeout", args.timeout);
     idc::getXmlValue(recvmessage, "processname", args.processname);

     if (args.clienttype == 2)
     {
         idc::getXmlValue(recvmessage, "matchname", args.matchname);
         idc::getXmlValue(recvmessage, "includeSubdirFiles", args.includeSubdirFiles);
         idc::getXmlValue(recvmessage, "serverpathback", args.serverpathback);
         idc::getXmlValue(recvmessage, "type", args.type);
     }

     if((args.clienttype!=1)&&(args.clienttype!=2))
     {
         sendmessage = "fail";
     }
     else
     {
         sendmessage = "ok";
     }
     // 发回响应信息
     if (tcpserver.Write(sendmessage) == false)
     {
         logfile.Write("tcpserver.Write()失败\n");
         return false;
     }
     logfile.Write("ip=%s clinetlogin %s\n",tcpserver.GetClientIp().c_str(),sendmessage.c_str());
     return true;
}

bool receiveFileContentFromClient(const std::string clientfilename, const std::string filemtime, int filesize)
{
    int onReceiveBytes = 0;
    int receiveBytes = 0;
    int totalBytes = filesize;
    char buf[8 * 1024];
    idc::WriteFile writefile;

    // 1.以二进制写打开文件---使用临时文件
    std::string serverfilename = clientfilename;
    idc::replaceStr(serverfilename, std::string(args.clientpath), std::string(args.serverpath), false);
    if (writefile.Open(serverfilename, true, std::ios::out | std::ios::binary) == false)
    {
        logfile.Write("writefile.Open() 失败\n");
        return false;
    }
    // 2.循环读取客户端消息
    while (1)
    {
        if (totalBytes - receiveBytes > 8 * 1024)
        {
            onReceiveBytes = 8 * 1024;
        }
        else
        {
            onReceiveBytes = totalBytes - receiveBytes;
        }
        if (tcpserver.Read(buf, onReceiveBytes, 10)==false)
        {
            logfile.Write("tcpserver.Read() 失败\n");
            return false;
        }
        if (writefile.Write(buf, onReceiveBytes) == false)
        {
            logfile.Write("writefile.Write() 失败\n");
            return false;
        }
        receiveBytes += onReceiveBytes;
        if (receiveBytes == totalBytes)
        {
            break;
        }
    }
    writefile.Closeandrename();
    idc::setFiletime(serverfilename,filemtime);
    return true;
}
//客户端的文件下载相当于服务端文件上传
bool HandleFileDownload()
{
     while (1)
    {
        if (PutFiles() == false)
        {
            return false;
        }
        if (iscontinue == false)//没有文件需要上传
        {
            sleep(args.timeInterval); // 30秒
            //短连接,需要心跳,避免关闭
            if (SendHeartbeat() == false)
            {
                return false;
            }
        }
        heartbeat.UpRecenttime();
    }
    return false;
}
bool SendHeartbeat()
{
    sendmessage="<heartbeat>ok</heartbeat>";
    if(tcpserver.Write(sendmessage)==false)
    {
       logfile.Write("tcpserver.Write() 失败\n");
        return false;
    }
    recvmessage.clear();
    if(tcpserver.Read(recvmessage,10)==false)
    {
        logfile.Write("tcpserver.Read() 失败\n");
        return false;
    }
     logfile.Write("接收了心跳响应:%s \n",recvmessage.c_str());
     return true;
}
bool PutFiles()
{
    iscontinue=false;
    // 1.扫描上传的文件目录
    idc::DirScanner dir;
    if (dir.Opendir(args.serverpath, args.matchname, 5000, args.includeSubdirFiles) == false)
    {
        logfile.Write("下载文件目录:%s 打开失败\n", args.serverpath);
        return false;
    }
    int filecount = 0; // 未实际上传文件数量,上传+1,收到响应-1s
    while (dir.Readdir())
    {
        iscontinue=true;
        // 2.发送报文:文件名(全路径),文件大小,文件时间
        logfile.Write("开始下载文件...:%s(%d)\n", dir.m_filename.c_str(), dir.m_filesize);
        idc::coutFormat(sendmessage, "<filename>%s</filename><filesize>%d</filesize><filemtime>%s</filemtime>",
                        dir.absolute_path_filename.c_str(), dir.m_filesize, dir.m_mtime.c_str());
        if (tcpserver.Write(sendmessage) == false)
        {
            logfile.Write("tcpserver.Write() 失败\n");
            return false;
        }
        // 3.发送文件内容
        if (sendFileContentToClient(dir.absolute_path_filename, dir.m_filesize) == false)
        {
            logfile.Write("fail...\n");
            tcpserver.CloseClient();
            return false;
        }
        else
        {
            filecount++;
            logfile.Write("ok...\n");
        }
        // 4.处理服务端响应
        while (filecount > 0)
        {
            // Read采用poll,0查看当前状态立即返回,只会超时
            if (tcpserver.Read(recvmessage, 0) == false)
            {
                break;
            }
            filecount--;
            logfile.Write("收到客户端端响应:%s\n", recvmessage.c_str());
            clientRespHandler(recvmessage);
        }
          heartbeat.UpRecenttime();
    }
    while (filecount > 0)
    {
        //会出错
        if (tcpserver.Read(recvmessage,10) == false)
        {
            logfile.Write("tcpserver.Read() 失败\n");
            return false;
        }
        filecount--;
        logfile.Write("收到客户端端响应:%s\n", recvmessage.c_str());
        clientRespHandler(recvmessage);
    }
    return true;
}

bool sendFileContentToClient(const std::string&filename,int filesize)
{
   int onSendBytes=0;
   int sentBytes=0;
   int totalBytes=filesize;
   char buf[8*1024];
   idc::ReadFile readfile;

   //1.以二进制读打开文件
   if (readfile.Open(filename, std::ios::in | std::ios::binary) == false)
   {
       logfile.Write("readfile.Open() 失败\n");
       return false;
   }
   //2.循环读取
   while(1)
   {
       if(totalBytes-sentBytes>8*1024)
       {
         onSendBytes=8*1024;
       }
       else{
          onSendBytes=totalBytes-sentBytes;
       }
       if(readfile.Read(buf,onSendBytes)<0)
       {
         logfile.Write("readfile.Read() 失败\n");
         return false;
       }
       if(tcpserver.Write(buf,onSendBytes)==false)
       {
         logfile.Write("tcpclient.Write(上传的文件内容)失败\n");
         return false;
       }
       sentBytes+=onSendBytes;
       if(sentBytes==totalBytes)
       {
        break;
       }
   }
   readfile.Close();
   return true;
}

bool clientRespHandler(const std::string &message)
{
   //1.解析 文件名(全路径) 响应信息
   std::string filename,result;
   idc::getXmlValue(message,"filename",filename);
   idc::getXmlValue(message,"result",result);
   //第二次还会继续重传
   if(result!="ok")
   {
    return false;
   }
   
   //1-删除文件；2-移动到备份目录。
   if(args.type==1)
   {
      if(remove(filename.c_str())==-1)
      {
        logfile.Write("remove 失败\n");
        return false;
      }
   }
   if(args.type==2)
   {
      std::string filebackname=filename;
      if(idc::replaceStr(filebackname,std::string(args.serverpath),std::string(args.serverpathback),false)==false)
      {
          logfile.Write("replaceStr 失败\n");
           return false;
      }
      if(idc::renamefile(filename,filebackname)==false)
      {
        logfile.Write("renamefile 失败\n");
           return false;
      }
   }
   return true;

}