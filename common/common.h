#pragma once
#include<string>
#include<cctype>
#include <cstring>
#include<vector>
#include<sys/time.h>
#include<unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <utime.h>  
#include <fstream>
#include<iostream>
#include <ios>
#include<dirent.h>
#include<algorithm>
#include<mutex>
#include <sys/socket.h>  // 主要socket函数和数据结构
#include <netinet/in.h>   // IPv4/IPv6地址结构
#include <arpa/inet.h>    // 地址转换函数（如inet_pton）
#include <unistd.h>       // close()函数
#include <netdb.h>        // 主机名解析（如gethostbyname）
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
namespace idc{
   
    char* deleteLeftchar(char*target,char cc=' ');
    std::string deleteLeftchar(std::string&target,char cc=' ');
    
    char*deleteRightchar(char*target,char=' ');
    std::string deleteRightchar(std::string&target,char cc=' ');

    char*deleteLRchar(char*target,char=' ');
    std::string deleteLRchar(std::string&target,char cc=' ');

    char* toUpper( char*target);
    std::string &toUpper( std::string&target);

    char* toLower( char*target);
    std::string &toLower( std::string&target);

    bool replaceStr(std::string&str,const std::string&str1,const std::string& str2,bool isloop);
    bool replaceStr(char*str,const std::string&str1,const std::string& str2,bool isloop);

    std::string picknumber(const std::string&src,std::string&dest,bool isAdd_Sub,bool isDot);
    std::string picknumber(const std::string&src,bool isAdd_Sub,bool isDot);
    char*picknumber(const std::string&src,char*dest,bool isAdd_Sub,bool isDot);


   
    // 辅助函数：分割字符串
    std::vector<std::string> splitRules(const std::string &rules);
    // 辅助函数：通配符匹配（不区分大小写）
    bool wildcardMatch(const std::string &str, const std::string &pattern);
    
    //需要2个辅助函数
    //匹配带*的字符串
    bool matchstr(const std::string &str,const std::string &rules);


    class Cmdstr{
        public:
        Cmdstr(){};
        Cmdstr(const std::string&scr,const std::string&splitflag,bool isdeletespace=true);

        Cmdstr(const Cmdstr&) =delete;
        Cmdstr& operator=(const Cmdstr&) =delete;

        std::string &operator[](int i);


        void splitToCmd(const std::string&scr,const std::string&splitflag,bool isdeletespace=true);

        int size()  const;

        bool getvalue(int i,std::string &value,const int len=0) const;  //len表示想要值的字节数,默认表示全要
        bool getvalue(int i,char* value,const int len=0) const;
        bool getvalue(int i,int &value,const int len=0) const;
        bool getvalue(int i,long &value,const int len=0) const;
        bool getvalue(int i,unsigned int &value,const int len=0) const;
        bool getvalue(int i,unsigned long &value,const int len=0) const;
        bool getvalue(int i,double &value,const int len=0) const;
        bool getvalue(int i,float &value,const int len=0) const;
        bool getvalue(int i,bool &value,const int len=0) const;

        ~Cmdstr(){};

        private:
        std::vector<std::string> m_cmdstr;


    };

    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, std::string &value, const int len = 0);
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, char *value, const int len = 0);
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, int &value);
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, long &value);
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, unsigned int &value);
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, unsigned long &value);
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, double &value);
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, float &value);
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, bool &value);

    // c++格式化输出
    template <typename... Args>
        bool coutFormat(std::string &str, const char *format, Args... args)
        {
            int len=snprintf(nullptr,0,format,args...);
            if(len<0)
            {
                return false;
            }
            if(len==0)
            {
                str.clear();
                return false;
            }
            str.resize(len+1);//保证capacity>=len+1,
            snprintf(&str[0],len+1,format,args...);
            str.resize(len);//保证字符串最后一个字符不是\0
            return true;
        }
         
        template <typename... Args> // 声明可变参数模板
        std::string coutFormat(const char *format, Args... args)
        {
            std::string tmp;
            if constexpr (sizeof...(args) == 0)
            {
                 tmp = format;
            }
            else{
                int len=snprintf(nullptr,0,format,args...);
            if(len<0)
            {
                return tmp;
            }
            if(len==0)
            {
                tmp.clear();
                return tmp;
            }
            tmp.resize(len+1);//保证capacity>=len+1,
            snprintf(&tmp[0],len+1,format,args...);
            tmp.resize(len);//保证字符串最后一个字符不是\0
            }
            return tmp;
        } 
       
     
    //将时间戳转换为指定格式字符串
    std::string &timeTostr(const time_t time, std::string &strtime, const std::string &format = "");
    char *timeTostr(const time_t time, char *strtime, const std::string &format = "");
    
    std::string timeTostr(const time_t time, const std::string &format = "");

    //将指定格式字符串转化为时间戳
    time_t strTotime(const std::string &strtime);

    //对当前时间进行加减
    std::string &calCurtime(std::string &strtime, const std::string &format = "", const int timetvl = 0);
    char *calCurtime(char *strtime, const std::string &format = "", const int timetvl = 0);
    // 为了避免重载的岐义，增加calCurtime1函数。
    std::string calCurtime1(const std::string &format = "", const int timetvl = 0);

    //对输入时间进行加减
    bool calIntime(const std::string &in_time,char *out_time  ,const int timetvl,const std::string &format="");
    bool calIntime(const std::string &in_time,std::string &out_time,const int timetvl,const std::string &format="");

    //微妙计时器
    class ctimer
    {
    public:
        ctimer(); // 构造函数中会调用start方法。

        void start(); // 开始计时。

        // 计算已逝去的时间，单位：秒，小数点后面是微秒。
        // 每调用一次本方法之后，自动调用start方法重新开始计时。
        double elapsed();

        private:
        struct timeval m_start; // 计时开始的时间点。 结构体里面有秒和毫秒
        struct timeval m_end;   // 计时结束的时间点。
    };

    // 创建新文件或者目录
    bool newdir(const std::string &pathOrfilename, bool isfilename = true);

    bool renamefile(const std::string &srcFilename, const std::string &desFilename);

    int getFilesize(const std::string &filename);

    bool getFiletime(const std::string &filename, char *time, const std::string &format = "yyyymmddhh24miss");
    bool getFiletime(const std::string &filename, std::string &time, const std::string &format = "yyyymmddhh24miss");

    bool setFiletime(const std::string &filename, const std::string &time);

    // 读取文件的类。
    class ReadFile 
    {
    public:
        ReadFile() {}

        // 判断文件是否已打开。
        bool isopen() const ;

        // 打开文件。
        // filename，待打开的文件名。
        // mode，打开文件的模式。
        bool Open(const std::string &filename, const std::ios::openmode mode = std::ios::in);

        // 以行的方式读取文本文件，endFlag指定行的结尾标志，缺省为空，没有结尾标志。
        bool Readline(std::string &buf, const std::string &endFlag = "");

        // 读取二进制文件，返回实际读取到的字节数。
        int Read(void *buf, const int bufsize);

        // 关闭并删除文件。
        bool Closeandremove();

        // 只关闭文件。
        void Close();

        ~ReadFile();

        private:
        std::ifstream infile;      // 读取文件的对象。
        std::string  m_filename; // 文件名，建议采用绝对路径。
    };

    // 写文件的类。
    class WriteFile
    {
    public:
        WriteFile() {}
        bool isopen() const;

        bool Open(const std::string &filename, const bool istmp = true, const std::ios::openmode mode = std::ios::out, const bool isbuffer = false);

       // 把数据以文本的方式格式化输出到文件。
        template <typename... Args>
        bool Writeline(const char *fmt, Args... args)
        {
            if(!outfile.is_open())
            {
                return false;
            }
            outfile<<coutFormat(fmt,args...);
            return outfile.good();
        }

        // 重载<<运算符，把数据以文本的方式输出到文件。
        // 注意：换行只能用\n，不能用endl。
        template <typename T>
        WriteFile &operator<<(const T &value)
        {
            if (outfile.is_open())
            {
                outfile << value;
            }
            // 无论是否写入，都返回 *this
            return *this;
        }

        // 把二进制数据写入文件。
        bool Write(void *buf, int bufsize);

        // 关闭文件，并且把临时文件名改为正式文件名。
        bool Closeandrename();

        // 关闭文件，如果有临时文件，则删除它。
        void Close();

        ~WriteFile();
        private:
        std::ofstream outfile;      // 写入文件的对象。
        std::string m_filename;     // 文件名，建议采用绝对路径。
        std::string m_filename_tmp; // 临时文件名，在m_filename后面加".tmp"。
    };


    bool copyfile(const std::string &srcfilename,const std::string &desfilename);

    // 获取目录下所有文件
    class DirScanner
    {
    public:
        DirScanner(const DirScanner &) = delete;            // 禁用拷贝构造函数。
        DirScanner &operator=(const DirScanner &) = delete; // 禁用赋值函数。
        DirScanner();

        // 设置文件时间的格式，支持"yyyy-mm-dd hh24:mi:ss"和"yyyymmddhh24miss"两种，缺省是后者。
        void setformat(const std::string &format);

        bool Opendir(const std::string &dirname, const std::string &rules, const int maxfiles = 10000, const bool expand_dir_children = false, bool issort = false);

        bool Readdir();

        unsigned int FileListSize();

        ~DirScanner(){};
    private:
        // 这是一个递归函数，被opendir()的调用，在cdir类的外部不需要调用它。
        bool _Opendir(const std::string &dirname, const std::string &rules, const int maxfiles, const bool expand_dir_children);

    public:
        // /project/public/_public.h   给Read用的
        std::string m_dirname;              // 目录名，例如：/project/public
        std::string m_filename;             // 文件名，不包括目录名，例如：_public.h
        std::string absolute_path_filename; // 绝对路径的文件，例如：/project/public/_public.h
        int m_filesize;                     // 文件的大小，单位：字节。
        std::string m_mtime;                // 文件最后一次被修改的时间，即stat结构体的st_mtime成员。
        std::string m_ctime;                // 文件生成的时间，即stat结构体的st_ctime成员。
        std::string m_atime;                // 文件最后一次被访问的时间，即stat结构体的st_atime成员。

    private:
        std::vector<std::string> m_filelist; // 存放文件列表的容器（绝对路径的文件名）。
        int m_pos;                           // 从文件列表m_filelist中已读取文件的位置。
        std::string m_format;                // 文件时间格式，缺省"yyyymmddhh24miss"。
    };

    // 日志文件
    // 对于日志文件:因为进程有独立的文件描述符 inode,所以不能切换日志文件;对于进程可以
    class FileLogger
    {
    public:
        // 构造函数，日志文件的大小缺省100M。
        FileLogger(int maxsize=100);

        bool Open(const std::string &filename, const std::ios::openmode mode = std::ios::app|std::ios::out, const bool rotate = false, const bool buffer_enabled = false);

        // 把日志内容以文本的方式格式化输出到日志文件，并且，在日志内容前面写入时间。
       template <typename... Args>
        bool Write(const char *fmt, Args... args)
        {
            if (!logfile.is_open())
            {
                return false;
            }
            std::unique_lock<std::mutex> lock(_mutex);
            RotateLogfile();

            std::string currenttime;
            logfile << calCurtime(currenttime) << " " << coutFormat(fmt, args...);

            return true;
        }

        template <typename T>
        FileLogger &operator<<(const T &value)
        {

            // 先检查文件是否打开
            if (!logfile.is_open())
            {
                return *this; // 返回自身以保持链式调用
            }
            // 在锁保护下执行所有文件操作
            std::unique_lock<std::mutex> lock(_mutex);
            RotateLogfile();

            std::string currenttime;
            logfile << value;

            return *this; // 返回自身以支持链式调用
        }
       
        void Close();

        ~FileLogger();

    private:
        // 注意，在多进程的程序中，日志文件不可切换，多线的程序中，日志文件可以切换。
        bool RotateLogfile();

        private:
        std::ofstream logfile;           // 日志文件对象。
        std::string logfilename;       // 日志文件名，建议采用绝对路径。
        std::ios::openmode _mode;    // 日志文件的打开模式。
        bool _rotate;           // 是否自动切换日志。
        int  _maxsize;           // 当日志文件的大小超过本参数时，自动切换日志。
        bool _buffer_enabled;         // 是否启用文件缓冲区。
        std::mutex _mutex;
    };


 ////////////////////////////////////////////////////////////////////////////////////
 //网络通信
 // socket通讯的客户端类
    class TcpClient
    {
    public:
        TcpClient();

        bool Connect(const std::string &ip, const int port);
        // 接收文本数据。没有指定消息格式:文件长度+文件内容    //-1阻塞 0不等  >0等的时间
        bool Read(std::string &buffer, const int socket_timeout = -1);      
        // 接收二进制数据。默认指定文件消息格式          
        bool Read(void *buffer, const int bufferlenth, const int socket_timeout = -1); 

        bool Write(const std::string &buffer);                  // 发送文本数据。
        bool Write(const void *buffer, const int bufferlenth); // 发送二进制数据。

        // 断开与服务端的连接
        void Close();

        ~TcpClient(); // 析构函数自动关闭socket，释放资源。
         private:
        int clientsocket; // 客户端的socket.
        std::string _ip;  // 服务端的ip地址。
        int _port;   // 服务端通讯的端口。
    };

    bool ReadByByte(const int socket,char*buf,int bufsize);
    bool WriteByByte(const int socketfd,const char*buf,int bufsize);

    // socket通讯的服务端类
    class TcpServer
    {
    public:
        TcpServer();
        //到监听状态
        bool Initserver(const unsigned int port, const int socketqueue = 5);
        //接受一个连接
        bool Accept();

        std::string GetClientIp();

        bool Read(std::string &buffer, const int timeout = -1);                 // 接收文本数据。
        bool Read(void *buffer, const int bufferlenth, const int timeout = -1); // 接收二进制数据。

        bool Write(const std::string &buffer);             // 发送文本数据。
        bool Write(const void *buffer, const int bufferlenth); // 发送二进制数据。

        void CloseListen();
        void CloseClient();

        ~TcpServer(); // 析构函数自动关闭socket，释放资源。

    private:
        struct sockaddr_in _client_addr; // 客户端的地址信息。
        struct sockaddr_in _server_addr; // 服务端的地址信息。
        int _listenfd;                   // 服务端用于监听的socket。
        int _clientfd;                   // 客户端连接上来的socket。
    };

    // 忽略64个信号
    void ignore_all_signals();


    //信号量封装
    class SystemVSem
    {

        SystemVSem(const SystemVSem &) = delete;            // 禁用拷贝构造函数。
        SystemVSem &operator=(const SystemVSem &) = delete; // 禁用赋值函数。
    public:
        SystemVSem() ;

        // 如果信号量已存在，获取信号量；如果信号量不存在，则创建它并初始化为value。
        // 如果用于互斥锁，value填1，sem_flg填SEM_UNDO。
        // 如果用于生产消费者模型，value填0，sem_flg填0。
        bool init(key_t key, unsigned short value = 1, short sem_flg = SEM_UNDO);
        bool wait(short value = -1); // 信号量的P操作，如果信号量的值是0，将阻塞等待，直到信号量的值大于0。
        bool post(short value = 1);  // 信号量的V操作。
        int getValue();              // 获取信号量的值，成功返回信号量的值，失败返回-1。
        bool remove();              // 销毁信号量。
        ~SystemVSem();
         int getSemid();  

    private:
        int _semid;
        int _sem_flg;//P V操作的sem_flg

        union semun // 用于信号量操作的共同体。
        {
            int val;
            struct semid_ds *buf;
            unsigned short *arry;
        };
        // struct sembuf
        // {
        //     unsigned short sem_num; // 信号量在集合中的索引
        //     short sem_op;           // 操作值（正数、负数或零）
        //     short sem_flg;          // 操作标志（如 IPC_NOWAIT, SEM_UNDO）
        // };
    };


// 查看共享内存：  ipcs -m
// 删除共享内存：  ipcrm -m shmid
// 查看信号量：      ipcs -s
// 删除信号量：      ipcrm sem semid

// 以下几个宏用于进程的心跳。
#define MAXNUM    1000     // 最大的进程数量。
#define SHMKEY    0x5095     // 共享内存的key。
#define SEMKEY    0x5095     // 信号量的key。

    // 里面不能STL容器,因为这是共享内存内容,STL不属于共享内存
    struct ProcessHeartbeat
    {
        int processpid;
        char processname[51];
        int outtime;
        time_t recenttime;

        ProcessHeartbeat() : processpid(-1), outtime(0), recenttime(0)
        {
            memset(processname, 0, sizeof(processname));
        }

        ProcessHeartbeat(int pid, const std::string &name, int otime, time_t rtime) : processpid(pid), outtime(otime), recenttime(rtime)
        {
            memcpy(processname, name.c_str(), 50);
            processname[50] = '\0';
        }
    };
    // 进程心跳模块
    class HeartbeatController
    {
    public:
        HeartbeatController(); // 初始化成员变量。

        // 把当前进程的信息加入共享内存进程组中。
        bool AddProcessInfoToShm(const int timeout, const std::string &processname = "");

        // 更新共享内存进程组中当前进程的心跳时间。
        bool UpRecenttime();

        ~HeartbeatController(); // 从共享内存中删除当前进程的心跳记录。
    private:
        ProcessHeartbeat *shm_ptr; // 指向共享内存的地址空间
        int pos;
        int shmid; // 共享内存标识符
        idc::SystemVSem sem;
    };

    // 修改文件最近修改时间为指定时间
    bool SetFileModTime(const std::string &filename, std::string &new_mtime);
     
    //获取文件最近修改时间->string
     bool GetFileModTime(const std::string &filename, std::string &mtime);

     //获取文件大小
     bool GetFileSize(const std::string &filename,unsigned int& size); 

    
};