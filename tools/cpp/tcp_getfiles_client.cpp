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
    char serverpathback[256]; // 文件成功下载后，服务端文件备份的根目录，当ptype==2时有效。
    bool includeSubdirFiles;  // 是否上传clientpath目录下各级子目录的文件，true-是；false-否。
    char matchname[256];      // 待上传文件名的匹配规则，如"*.TXT,*.XML"。
    char serverpath[256];     // 服务端文件存放的根目录。/data1 /data1/aaa /data1/bbb
    int timeInterval;         // 扫描本地目录文件的时间间隔（执行文件上传任务的时间间隔），单位：秒。
    int timeout;              // 进程心跳的超时时间。
    char processname[51];     // 进程名，建议用"tcpputfiles_后缀"的方式。
};

void help();
bool parseXmlParameters(char*strxml);
bool login(const std::string&argv);
bool GetFiles();
bool receiveFileContentFromServer(const std::string serverfilename, const std::string filemtime, int filesize);

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
       printf("/home/ubuntu/bilibili/BigProjrct/tools/bin/tcp_getfiles_client \
/home/ubuntu/bilibili/BigProjrct/tools/logs/tcp_getfiles_clinet.log \
\"<ip>127.0.0.1</ip><port>8080</port><clientpath>/home/ubuntu/bilibili/BigProjrct/TcpDatas/Client_GetDatas</clientpath>\
<serverpathback>/home/ubuntu/bilibili/BigProjrct/TcpDatas/Server_BackDatas</serverpathback><type>1</type>\
<includeSubdirFiles>false</includeSubdirFiles><matchname>*.csv,*.json,*.xml</matchname>\
<serverpath>/home/ubuntu/bilibili/BigProjrct/TcpDatas/ServerDatas</serverpath><timeInterval>30</timeInterval>\
<timeout>40</timeout><processname>tcp_getfiles_client</processname>\"\n");
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
     // 下载文件
     GetFiles();

     tcpclient.Close();
     return 0;
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
    idc::getXmlValue(strxml, "serverpathback", args.serverpathback, 250);
    if (strlen(args.serverpathback) == 0)
    {
        logfile.Write("serverpathback 字段格式不正确\n");
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
    sendmessage=idc::coutFormat("%s<clienttype>2</clienttype>",argv.c_str());
    if(tcpclient.Write(sendmessage)==false)
    {
        logfile.Write("tcpclient.Write 失败\n");
        return false;
    }
    if(tcpclient.Read(recvmessage,-1)==false)
    {
        logfile.Write("tcpclient.Read 失败\n");
        return false;
    }
    logfile.Write("login:%s\n",recvmessage.c_str());
    return true;
}

bool GetFiles()
{
    recvmessage.clear();
   while(tcpclient.Read(recvmessage))//args.timeInterval+10
   {
        heartbeat.UpRecenttime();//只要服务端没有消息,这里就不更新,就会被杀死
       //处理心跳
      if(recvmessage=="<heartbeat>ok</heartbeat>")
      {
        sendmessage="ok";
       if(tcpclient.Write(sendmessage)==false)
       {
        logfile.Write("tcpclient.Write() 失败!\n");
        return false;
       }
       logfile.Write("收到服务端:ip=%s,心跳信息:%s\n",args.ip,recvmessage.c_str());

      }
      //处理下载业务
       if(recvmessage.find("<filename>")!=std::string::npos)
       {
          std::string filename,filemtime;
          int filesize;
          idc::getXmlValue(recvmessage,"filename",filename);
          idc::getXmlValue(recvmessage,"filesize",filesize);
          idc::getXmlValue(recvmessage,"filemtime",filemtime);

          // 接收内容
          logfile.Write("正在下载文件 %s(%d)... \n",filename.c_str(),filesize);
          if (receiveFileContentFromServer(filename, filemtime, filesize) == false)
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
          if(tcpclient.Write(sendmessage)==false)
          {
             logfile.Write("tcpclient.Write() 失败");
             return false;
          }

       }

   }
   logfile.Write("读取服务端信息失败/超时!\n");
   return false;
}


bool receiveFileContentFromServer(const std::string serverfilename, const std::string filemtime, int filesize)
{
    int onReceiveBytes = 0;
    int receiveBytes = 0;
    int totalBytes = filesize;
    char buf[8 * 1024];
    idc::WriteFile writefile;

    // 1.以二进制写打开文件---使用临时文件
    std::string clientfilename = serverfilename;
    idc::replaceStr(clientfilename, std::string(args.serverpath), std::string(args.clientpath), false);
    if (writefile.Open(clientfilename, true, std::ios::out | std::ios::binary) == false)
    {
        logfile.Write("writefile.Open() 失败\n");
        return false;
    }
    // 2.循环读取服务端消息
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
        if (tcpclient.Read(buf, onReceiveBytes, 10) ==false)
        {
            logfile.Write("tcpclient.Read() 失败\n");
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
    idc::setFiletime(clientfilename,filemtime);
    return true;
}