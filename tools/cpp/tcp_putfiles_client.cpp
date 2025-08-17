//增加心跳机制,保证短连接不会因为超时断开
#include"../../common/common.h"
// 程序运行的参数结构体。
struct Args
{
    int clienttype;          // 客户端类型，1-上传文件；2-下载文件，本程序固定填1。
    char ip[31];              // 服务端的IP地址。
    int port;                 // 服务端的端口。
    char clientpath[256];     // 本地文件存放的根目录。 /data /data/aaa /data/bbb
    int type;                 // 文件上传成功后本地文件的处理方式：1-删除文件；2-移动到备份目录。
    char clientpathback[256]; // 文件成功上传后，本地文件备份的根目录，当ptype==2时有效。
    bool includeSubdirFiles;  // 是否上传clientpath目录下各级子目录的文件，true-是；false-否。
    char matchname[256];      // 待上传文件名的匹配规则，如"*.TXT,*.XML"。
    char serverpath[256];     // 服务端文件存放的根目录。/data1 /data1/aaa /data1/bbb
    int timeInterval;         // 扫描本地目录文件的时间间隔（执行文件上传任务的时间间隔），单位：秒。
    int timeout;              // 进程心跳的超时时间。
    char processname[51];     // 进程名，建议用"tcpputfiles_后缀"的方式。
};

bool SendHeartbeat();
void help();
bool parseXmlParameters(char*strxml);
bool login(const std::string&argv);
bool PutFiles();
bool serverRespHandler(const std::string &message);
bool sendFileContentToServer(const std::string&filename,int filesize);

idc::FileLogger logfile;
idc::TcpClient tcpclient;
std::string recvmessage;
std::string sendmessage;
Args args;
bool iscontinue;
idc::HeartbeatController heartbeat;

int main(int argc,char*argv[])
{
    if(argc!=3)
    {
       printf("/home/ubuntu/bilibili/BigProjrct/tools/bin/tcp_putfiles_client \
/home/ubuntu/bilibili/BigProjrct/tools/logs/tcp_putfiles_clinet.log \
\"<ip>127.0.0.1</ip><port>8080</port><clientpath>/home/ubuntu/bilibili/BigProjrct/TcpDatas/Client_PutDatas</clientpath>\
<clientpathback>/home/ubuntu/bilibili/BigProjrct/TcpDatas/Client_PutBackDatas</clientpathback><type>1</type>\
<includeSubdirFiles>false</includeSubdirFiles><matchname>*.csv,*.json,*.xml</matchname>\
<serverpath>/home/ubuntu/bilibili/BigProjrct/TcpDatas/ServerDatas</serverpath><timeInterval>30</timeInterval>\
<timeout>40</timeout><processname>tcp_putfiles_client</processname>\"\n");
       return -1;
    }
    if(logfile.Open(argv[1])==false)
    {
        printf("日志文件:%s 打开失败\n",argv[1]);
        return -1;
    }
    if(parseXmlParameters(argv[2])==false)
    {
        logfile.Write("解析命令行:%s 失败\n",argv[3]);
        return -1;
    }
    heartbeat.AddProcessInfoToShm(args.timeout,args.processname);
    if(tcpclient.Connect(args.ip,args.port)==false)
    {
        logfile.Write("客户端tcp connect失败\n");
        return -1;      
    }
     logfile.Write("客户端tcp connect成功\n");

     if(login(argv[2])==false)
     {
        logfile.Write("客户端tcp login失败\n");
        return -1; 
     }
     //1次上传任务中,若上传了文件--不休眠,若没有文件上传---则休眠
    while (1)
    {
        if (PutFiles() == false)
        {
            return -1;
        }
        if (iscontinue == false)//没有文件需要上传
        {
            sleep(args.timeInterval); // 30秒
            //短连接,需要心跳,避免关闭
            if (SendHeartbeat() == false)
            {
                return -1;
            }
        }
        heartbeat.UpRecenttime();
    }
    tcpclient.Close();
    return 0;
}

bool SendHeartbeat()
{
    sendmessage="<heartbeat>ok</heartbeat>";
    if(tcpclient.Write(sendmessage)==false)
    {
       logfile.Write("客户端发送心跳信息失败\n");
        return false;
    }
    recvmessage.clear();
    if(tcpclient.Read(recvmessage,10)==false)
    {
        logfile.Write("客户端接受心跳信息响应失败\n");
        return false;
    }
     logfile.Write("接收了心跳响应:%s \n",recvmessage.c_str());
     return true;
}

bool parseXmlParameters(char *strxml)
{
    idc::getXmlValue(strxml, "ip", args.ip, 30);
    if (strlen(args.ip) == 0)
    {
        logfile.Write("ip 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "port", args.port);
    if (args.port == 0)
    {
        logfile.Write("ip 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "clientpath", args.clientpath, 250);
    if (strlen(args.clientpath) == 0)
    {
        logfile.Write("clientpath 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "clientpathback", args.clientpathback, 250);
    if (strlen(args.clientpathback) == 0)
    {
        logfile.Write("clientpathback 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "type", args.type);
    if ((args.type != 1) && (args.type != 2))
    {
        logfile.Write("type 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "includeSubdirFiles", args.type);
    if (args.includeSubdirFiles != true && args.includeSubdirFiles != false)
    {
        logfile.Write("includeSubdirFiles 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "matchname", args.matchname, 250);
    if (strlen(args.matchname) == 0)
    {
        logfile.Write("matchname 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "serverpath", args.serverpath, 250);
    if (strlen(args.serverpath) == 0)
    {
        logfile.Write("serverpath 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "timeInterval", args.timeInterval);
    if (args.timeInterval == 0)
    {
        logfile.Write("timeInterval 字段格式不正确\n");
        return false;
    }
    if (args.timeInterval > 30)
    {
        args.timeInterval = 30;
    }
    idc::getXmlValue(strxml, "timeout", args.timeout);
    if (args.timeout <= args.timeInterval)
    {
        logfile.Write("timeout:%d<= timeInterval:%d\n", args.timeout, args.timeInterval);
        return false;
    }
    idc::getXmlValue(strxml, "processname", args.processname);
    return true;
}

bool login(const std::string&argv)
{
    sendmessage.clear();
    sendmessage=idc::coutFormat("%s<clienttype>1</clienttype>",argv.c_str());
    if(tcpclient.Write(sendmessage)==false)
    {
        logfile.Write("tcpclient.Write 失败\n");
        return false;
    }
    if(tcpclient.Read(recvmessage,10)==false)
    {
        logfile.Write("tcpclient.Read 失败\n");
        return false;
    }
    logfile.Write("login:%s\n",recvmessage.c_str());
    return true;
}

bool PutFiles()
{
    iscontinue=false;
    // 1.扫描上传的文件目录
    idc::DirScanner dir;
    if (dir.Opendir(args.clientpath, args.matchname, 5000, args.includeSubdirFiles) == false)
    {
        logfile.Write("上传文件目录:%s 打开失败\n", args.clientpath);
        return false;
    }
    int filecount = 0; // 未实际上传文件数量,上传+1,收到响应-1
    while (dir.Readdir())
    {
        iscontinue=true;
        // 2.发送报文:文件名(全路径),文件大小,文件时间
        logfile.Write("开始上传文件...:%s(%d)\n", dir.m_filename.c_str(), dir.m_filesize);
        idc::coutFormat(sendmessage, "<filename>%s</filename><filesize>%d</filesize><filemtime>%s</filemtime>",
                        dir.absolute_path_filename.c_str(), dir.m_filesize, dir.m_mtime.c_str());
        if (tcpclient.Write(sendmessage) == false)
        {
            logfile.Write("tcpclient.Write() 失败\n");
            return false;
        }
        // 3.发送文件内容
        if (sendFileContentToServer(dir.absolute_path_filename, dir.m_filesize) == false)
        {
            logfile.Write("fail...\n");
            tcpclient.Close();
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
            if (tcpclient.Read(recvmessage, 0) == false)
            {
                break;
            }
            filecount--;
            logfile.Write("收到服务端响应:%s\n", recvmessage.c_str());
            serverRespHandler(recvmessage);
        }
          heartbeat.UpRecenttime();
    }
    while (filecount > 0)
    {
        //会出错
        if (tcpclient.Read(recvmessage,10) == false)
        {
            logfile.Write("tcpclient.Read() 失败\n");
            return false;
        }
        filecount--;
        logfile.Write("收到服务端响应:%s\n", recvmessage.c_str());
        serverRespHandler(recvmessage);
    }
    return true;
}

bool serverRespHandler(const std::string &message)
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
      if(idc::replaceStr(filebackname,std::string(args.clientpath),std::string(args.clientpathback),false)==false)
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

bool sendFileContentToServer(const std::string&filename,int filesize)
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
       if(tcpclient.Write(buf,onSendBytes)==false)
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