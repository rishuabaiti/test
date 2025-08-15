
//上传的服务端路径不存在不会创建路径,下载到客户端路径会创建
//获取客户端目录下文件----直接查找
#include"../../common/common.h"
#include"../../common/ftp.h"
#include<list>
//使用ubuntu用户名登录,本地文件全路径,远端文件/==ubuntu
//本地路径:用户能访问的本地路径范围 
//远程路径:chroot_local_user=YES可将用户限制在其主目录下，无法访问上层目录（即 “chroot 监狱”)

// 程序运行参数的结构体。
struct Args 
{
    char host[31];                    // 远程服务端的IP和端口。
    int  mode;                         // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
    char username[31];           // 远程服务端ftp的用户名。
    char password[31];           // 远程服务端ftp的密码。
    char remotepath[256];     // 远程服务端存放文件的目录。
    char localpath[256];         // 本地文件存放的目录。
    char matchname[101];  // 待上传文件匹配的规则。
    int  type;            //1-什么也不做,2-下载后删除服务器文件,3-下载后重命名服务器文件 ----目的防止下载重复
    char localdirback[256];
    char updatefilename[256];  //更新1号容器的文件名
    int  timeout;                         // 进程心跳超时的时间。
    char processname[51];   
};

// 文件信息的结构体。
struct FileInfo{
    std::string _filename;
    std::string _mtime;
    FileInfo()=default;
    FileInfo(const std::string &filename,const std::string &mtime):_filename(filename),_mtime(mtime) {}
    void clear() { _filename.clear(); _mtime.clear(); }

};
std::unordered_map<std::string,std::string> uploadedfilesList;
std::list<struct FileInfo> allfilesList;
std::list<struct FileInfo> nouploadfilesList;
std::list<struct FileInfo> touploadfilesList;

idc::FileLogger logfile;
idc::FtpClient ftpclient;
idc::HeartbeatController heartbeat;
Args args;

void help();
void handle(int sig);
bool parseXmlParameters(char*strxml);
bool ScannerToAllfilesList(const std::string&dirname);
bool PostLocalfileToRemote(const std::string& remotedir,const std::string& localdir);
bool ReadToUploadedfilesList(const std::string&filename);
void ClassifyFilesByContainer();
bool WriteUpdatefile();
bool Updatefile(const std::string &filename, const struct FileInfo &fileinfo);


int main(int argc,char*argv[])
{
    if(argc!=3)
    {
        help();
        return -1;
    }
    idc::ignore_all_signals();
    signal(SIGTERM,handle);
    signal(SIGINT,handle);
    //1.打开日志文件
    if(logfile.Open(argv[1])==false)
    {
        printf("日志文件打开失败 %s",argv[1]);
        return -1;
    }
    //2.拆分命令行
    memset(&args,0,sizeof(args));
    if(parseXmlParameters(argv[2])==false)
    {
        logfile.Write("xml格式参数解析失败\n");
        return -1;
    }
    heartbeat.AddProcessInfoToShm(args.timeout,args.processname);
    //3.登录ftp服务器
    ftpclient.Init();
   if(ftpclient.Login(args.host,args.username,args.password,args.mode)==false)
   {
      logfile.Write("ftp登录失败,host:%s,username:%s,password:%s\n",\
        args.host,args.username,args.password);
        return -1;
   }

    // //4.进入ftp服务器存放文件目录
    // if(ftpclient.ChangeDir(args.remotepath)==false)
    // {
    //      logfile.Write("进入服务端目录:%s 失败\n",args.remotepath);
    //     return -1;
    // }

    // //5.获取ftp服务器存放文件目录下的所有文件和目录,存放到本地文件
    // std::string locallistfile = std::string(args.localpath) + "/" + "locallistfile";
    // if(ftpclient.getNameList(".",locallistfile)==false)
    // {
    //      logfile.Write("获取服务端目录下::%s ,所有信息失败\n",args.remotepath);
    //      return -1;
    // }
    // //6.从本地文件读取符合匹配规则的文件名放到容器中,删除本地文件
    // if(ReadToAllfilesList(locallistfile)==false)
    // {
    //     logfile.Write("读取本地文件::%s ,符合匹配规则的信息失败\n",locallistfile);
    //     return -1;
    // }

    //4.遍历本地目录,获取目录下所有文件
    ScannerToAllfilesList(args.localpath);
     

    heartbeat.UpRecenttime();
    for(auto &ee:allfilesList)
    {
        logfile.Write("列表容器内容:%s\n",ee._filename.c_str());
    }
    //7.根据type,将容器初始化
    if(args.type==1)
    {
          //7.1 读取更新文件更新1号容器
          ReadToUploadedfilesList(args.updatefilename);
          //7.2 根据1,2号容器更新3,4号容器
          ClassifyFilesByContainer();
          //7.3 将3号容器内容写到更新文件
          WriteUpdatefile();

          heartbeat.UpRecenttime();
    }
    else{
        allfilesList.swap(touploadfilesList);
    }
    //8.上传4号容器中的文件,放到远端确定的目录中(该目录必须存在,不会再远端创建)
    std::string remotedir="/bilibili/BigProjrct/surfdatas";
    std::string localdir="/home/ubuntu/bilibili/BigProjrct/PostDatas";
    if(PostLocalfileToRemote(remotedir,localdir)==false)
    {
        logfile.Write("上传文件失败\n");
        return -1;
    }
    logfile.Write("上传文件成功\n");
   
    return 0;
}
void handle(int sig)
{
     printf("收到了 %d 信号\n",sig);
     exit(1);
}

void help()
{
    printf("/home/ubuntu/bilibili/BigProjrct/tools/bin/ftp_putfiles /home/ubuntu/bilibili/BigProjrct/tools/logs/ftp_putfiles.log \
\"<host>127.0.0.1:21</host><mode>1</mode><username>ubuntu</username><password>wqswzymWQ123</password>\
<localpath>/home/ubuntu/bilibili/BigProjrct/PostDatas</localpath><remotepath>/bilibili/BigProjrct/surfdatas</remotepath>\
<matchname>*.csv,*.json,*.xml</matchname><type>1</type><localdirback>/home/ubuntu/bilibili/BigProjrct/PostDatas/localfileback</localdirback>\
<updatefilename>/home/ubuntu/bilibili/BigProjrct/updatefile</updatefilename><timeout>30</timeout><processname>ftp_putfiles</processname>\"\n");
}

bool parseXmlParameters(char*strxml)
{
    idc::getXmlValue(strxml,"host",args.host,30);
    if(strlen(args.host)==0)
    {
        logfile.Write("host 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml,"mode",args.mode);
    if(args.mode!=2)
    {
        args.mode=1;
    }
    idc::getXmlValue(strxml,"username",args.username,30);
    if(strlen(args.username)==0)
    {
        logfile.Write("username 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml,"password",args.password,30);
    if(strlen(args.password)==0)
    {
        logfile.Write("password 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "remotepath", args.remotepath, 250);
    if (strlen(args.remotepath) == 0)
    {
        logfile.Write("remotepath 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "localpath", args.localpath, 250);
    if (strlen(args.localpath) == 0)
    {
        logfile.Write("localpath 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "matchname", args.matchname, 100);
    if (strlen(args.matchname) == 0)
    {
        logfile.Write("matchname 字段格式不正确\n");
        return false;
    }
    idc::getXmlValue(strxml, "type", args.type);
    if ((args.type != 1) && (args.type != 2) && (args.type != 3))
    {
        logfile.Write("type 字段格式不正确,%d\n", args.type);
        return false;
    }
    idc::getXmlValue(strxml, "localdirback", args.localdirback, 250);
    if (args.type == 3)
    {
        if (strlen(args.localdirback) == 0)
        {
            logfile.Write("localdirback 字段格式不正确\n");
            return false;
        }
    }

    // char updatefilename[256];  //更新1号容器的文件名
    idc::getXmlValue(strxml, "updatefilename", args.updatefilename, 250);
    if (args.type == 1)
    {
        if (strlen(args.updatefilename) == 0)
        {
            logfile.Write("updatefilename 字段格式不正确\n");
            return false;
        }
    }
    // int  timeout;                         // 进程心跳超时的时间。
    idc::getXmlValue(strxml, "timeout", args.timeout);
    if (args.timeout <= 0)
    {
        logfile.Write("checkmtime 字段格式不正确\n");
        return false;
    }
    // char processname[51];
    idc::getXmlValue(strxml, "processname", args.processname);
    return true;
}
bool ScannerToAllfilesList(const std::string&dirname)
{
    //1.清空alllist容器
    allfilesList.clear();
    //2.获取本地目录下所有文件
    idc::DirScanner dir;
    if(dir.Opendir(args.localpath,args.matchname)==false)
    {
         logfile.Write("%s 本地目录下,查找符合条件的文件失败\n",args.localpath);
        return false;
    }
    FileInfo info;
    while (dir.Readdir())
    {
        info.clear();
        info._filename = dir.m_filename;
        info._mtime = dir.m_mtime;
        allfilesList.push_back(info);
    }
    return true;
}



bool  PostLocalfileToRemote(const std::string& remotedir,const std::string& localdir)
{
    for(auto fileinfo:touploadfilesList)
    {
        std::string remotefile=remotedir+"/"+fileinfo._filename;
        std::string localfile=localdir+"/"+fileinfo._filename;
        logfile.Write("开始上传...\n");
        if(ftpclient.putFile(localfile,remotefile,true)==false)
        {
            logfile.Write("文件 %s 上传失败,%s\n",fileinfo._filename.c_str(),ftpclient.Response());
            return false;
        }
        logfile.Write("%s\n",fileinfo._filename.c_str());
        logfile.Write("ok\n");
        heartbeat.UpRecenttime();
         if(args.type==1)
         {
            //更新---更新文件
            Updatefile(args.updatefilename, fileinfo);
            
         }
        if(args.type==2)
        {
            if(ftpclient.RemoveFile(localfile)==false)
            {
                logfile.Write("客户端文件 %s 删除失败\n",fileinfo._filename.c_str());
                return false;
            }
            logfile.Write("客户端文件 %s 删除成功\n",fileinfo._filename.c_str());
        }
        if(args.type==3)
        {
            std::string localfileback=idc::coutFormat("%s/%s",args.localdirback,fileinfo._filename.c_str());
            if(ftpclient.RenameFile(localfile,localfileback)==false)
            {
                logfile.Write("客户端文件 %s 备份失败,%s\n",localfileback.c_str(),ftpclient.Response());
                return false;
            }
            logfile.Write("客户端文件 %s 备份成功\n",fileinfo._filename.c_str());
        }
      

    }
    return true;
}

bool ReadToUploadedfilesList(const std::string&filename)
{
    //1.清空容器
    uploadedfilesList.clear();
    //2.读取文件内容
      idc::ReadFile readfile;
      //以只读方式打开文件,没有文件返回false,但是这里后面写文件会创建,返回true
   if(readfile.Open(filename)==false)
   {
    logfile.Write("文件 %s 打开失败\n",filename.c_str());
    return true;
   }
   std::string buf;
   std::string key,value;
   while (readfile.Readline(buf))
   {
       // 里面是xml格式  文件名 文件最近修改时间
       key.clear();
       value.clear();
       idc::getXmlValue(buf, "filename", key);
       idc::getXmlValue(buf, "mtime", value);
       
       uploadedfilesList.emplace(key, value);
   }
   readfile.Close();
   return true;
}

void  ClassifyFilesByContainer()
{
    //1.将2个容器清空
    nouploadfilesList.clear();
    touploadfilesList.clear();
    //2.遍历all文件列表
    for(auto &fileinfo:allfilesList)
    {
        auto it=uploadedfilesList.find(fileinfo._filename);
        if(it!=uploadedfilesList.end())
        {
           
              if(it->second==fileinfo._mtime)
              {
                nouploadfilesList.push_back(fileinfo);
              }
              else{
                touploadfilesList.push_back(fileinfo);
              }
         
        }
        else{
            //没找到,说明未下载
            touploadfilesList.push_back(fileinfo);
            
        }
    }
}
bool Updatefile(const std::string &filename, const struct FileInfo &fileinfo)
{
    // 1.操作文件以追加方式写,不启用临时文件
    idc::WriteFile writefile;
    if (writefile.Open(filename, false, std::ios::out | std::ios::app) == false)
    {
        logfile.Write("文件 %s 打开失败\n", filename.c_str());
        return false;
    }

    writefile.Writeline("<filename>%s</filename><mtime>%s</mtime>\n", fileinfo._filename.c_str(), fileinfo._mtime.c_str());
    writefile.Close();
    return true;
}
bool  WriteUpdatefile()
{
    idc::WriteFile writefile;
    if (writefile.Open(args.updatefilename) == false)
    {
        logfile.Write("文件 %s 打开失败\n",args.updatefilename);
        return false;
    }
    std::string mtime;
    for (auto &fileinfo : nouploadfilesList)
    {

        writefile.Writeline("<filename>%s</filename><mtime>%s</mtime>\n", fileinfo._filename.c_str(), fileinfo._mtime.c_str());
    }
     //采用临时文件,需要修改正式名
    writefile.Closeandrename();
    return true;
}