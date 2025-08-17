//封装ftp.c=>ftp.cpp
 #include"ftp.h"

namespace idc
{

    FtpClient::FtpClient() // 类的构造函数。
    {
        m_connectfailed = true; // 如果网络连接失败，该成员的值为true。
        m_loginfailed = true;   // 如果登录失败，用户名和密码不正确，或没有登录权限，该成员的值为true。
        m_optionfailed = true;
    }

    // 初始化m_size和m_mtime成员变量。
    void FtpClient::Init()
    {
        m_size = 0;
        m_mtime.clear();
    }

    // 登录ftp服务器。
    // host：ftp服务器ip地址和端口，中间用":"分隔，如"192.168.1.1:21"。
    // username：登录ftp服务器用户名。
    // password：登录ftp服务器的密码。
    // imode：传输模式，1-FTPLIB_PASSIVE是被动模式，2-FTPLIB_PORT是主动模式，缺省是被动模式。
    bool FtpClient::Login(const std::string &host, const std::string &username, \
        const std::string &password, const int imode )
    {
        // 1.如果有连接,关闭连接
        if (ftpconn != nullptr)
        {
            FtpQuit(ftpconn);
            ftpconn = nullptr;
        }
        // 2.获取连接
        if (FtpConnect(host.c_str(), &ftpconn) == 0)
        {
            m_connectfailed = false;
            return false;
        }
        //2.登录
        if(FtpLogin(username.c_str(),password.c_str(), ftpconn)==0)
        {
            m_loginfailed = false;
            return false;
        }
        //3.设置传输模式
        if(FtpOptions(FTPLIB_CONNMODE, imode, ftpconn)==0)
        {
            m_optionfailed = false;
            return false;
        }
        return true;
    }

    // 注销。
    bool FtpClient::Logout()
    {
        // 1.如果没有连接,直接返回
        if (ftpconn == nullptr)
        {
            return false;
        }
        FtpQuit(ftpconn);
        ftpconn = nullptr;
        return true;
    }

     bool FtpClient::getRemotefileMtime(const std::string &remotefilename)
     {
         // 1.如果没有连接,直接返回
        if (ftpconn == nullptr)
        {
            return false;
        }
        //UTC:14位时间
        std::string filetime;
        filetime.resize(14);  // 预先分配14字节空间
        if(FtpModDate(remotefilename.c_str(), &filetime[0], 14, ftpconn)==0)
        {
            return false;
        }
        //UTC+8:本地时间
        calIntime(filetime,m_mtime,8*60*60,"yyyymmddhh24miss");

        return true;
        

     }

        // 获取ftp服务器上文件的大小。
        // remotefilename：待获取的文件名。
        // 返回值：false-失败；true-成功，获取到的文件大小存放在m_size成员变量中。
     bool FtpClient::getRemotefileSize(const std::string &remotefilename)
     {
         // 1.如果没有连接,直接返回
         if (ftpconn == nullptr)
         {
             return false;
         }
         unsigned int size;
         if (FtpSize(remotefilename.c_str(), &size,FTPLIB_ASCII, ftpconn) == 0)
         {
             return false;
         }
         m_size = size;
         return true;
     }
       bool FtpClient::ChangeDir(const std::string &remotedir)
       {
           // 1.如果没有连接,直接返回
         if (ftpconn == nullptr)
         {
             return false;
         }
         if(FtpChdir(remotedir.c_str(),ftpconn)==0)
         {
            return false;
         }
         return true;
       }

        // 在ftp服务器上创建目录。
        // remotedir：ftp服务器上待创建的目录名。
        // 返回值：true-成功；false-失败。
       bool FtpClient::CreateDir(const std::string &remotedir)
       {
           // 1.如果没有连接,直接返回
           if (ftpconn == nullptr)
           {
               return false;
           }
           if (FtpMkdir(remotedir.c_str(), ftpconn) == 0)
           {
               return false;
           }
           return true;
       }

       // 删除ftp服务器上的目录。
       // remotedir：ftp服务器上待删除的目录名。
       // 返回值：true-成功；如果权限不足、目录不存在或目录不为空会返回false。
       bool FtpClient::RemoveDir(const std::string &remotedir)
       {
           // 1.如果没有连接,直接返回
           if (ftpconn == nullptr)
           {
               return false;
           }
           if (FtpRmdir(remotedir.c_str(), ftpconn) == 0)
           {
               return false;
           }
           return true;
       }

        bool FtpClient::getNameList(const std::string &remotedir, const std::string &locallistfilename)
        {
             // 1.如果没有连接,直接返回
           if (ftpconn == nullptr)
           {
               return false;
           }
           if(FtpNlst(locallistfilename.c_str(), remotedir.c_str(),ftpconn)==0)
           {
            return false;
           }
           return true;
        }

        // 从ftp服务器上获取文件。
        // remotefilename：待获取ftp服务器上的文件名。
        // localfilename：保存到本地的文件名。
        // bcheckmtime：文件传输完成后，是否核对远程文件传输前后的时间，保证文件的完整性。
        // 返回值：true-成功；false-失败。
        // 注意：文件在传输的过程中，采用临时文件命名的方法，即在localfilename后加".tmp"，在传输
        // 完成后才正式改为localfilename。
        bool FtpClient::getFile(const std::string &remotefilename, const std::string &localfilename, const bool checkmtime )
        {
             // 1.如果没有连接,直接返回
           if (ftpconn == nullptr)
           {
               return false;
           }
           //2.获取服务端文件最近修改时间
          if(getRemotefileMtime(remotefilename)==false)
          {
              return false;
          }
          std::string recenttime=m_mtime;
          //3.启用临时文件
          std::string tmplocalfilename=localfilename+".tmp";
          if(newdir(tmplocalfilename,true)==false)
          {
            return false;
          }
          //4.下载服务器文件->tmp文件
          if(FtpGet(tmplocalfilename.c_str(), remotefilename.c_str(),FTPLIB_ASCII,ftpconn)==0)
          {
            return false;
          }
          if (checkmtime)
          {
              // 5.获取服务端文件最近修改时间
              if (getRemotefileMtime(remotefilename) == false)
              {
                  //remove(tmplocalfilename.c_str());
                  return false;
              }
              // 6.服务器文件在下载期间修改了
              if (recenttime != m_mtime)
              {
                  //remove(tmplocalfilename.c_str());
                  return false;
              }
          }
          //7.重命名本地文件
          if(rename(tmplocalfilename.c_str(),localfilename.c_str())==-1)
          {
             //remove(tmplocalfilename.c_str());
             return false;
          }
          //8.将本地文件的最近修改时间改为服务端文件时间
          if(SetFileModTime(localfilename,m_mtime)==false)
          {
             //remove(localfilename.c_str());
             return false;
          }
          return true;

        }
         bool FtpClient::putFile(const std::string &localfilename, const std::string &remotefilename, const bool checksize )
         {
             // 1.如果没有连接,直接返回
           if (ftpconn == nullptr)
           {
               return false;
           }
           //2.获取本地文件最近修改时间
           if(GetFileModTime(localfilename,m_mtime)==false)
           {
              return false;
           }
           std::string recenttime=m_mtime;
           //3.启用临时文件
           std::string tmpremotefilename=remotefilename+".tmp";
          //这里就不再服务端创建文件目录了

          //4.上传本地文件->服务端tmp文件
          if(FtpPut(localfilename.c_str(), tmpremotefilename.c_str(),FTPLIB_ASCII,ftpconn)==0)
          {
            return false;
          }
          //5.检查上传期间本地文件是否更改
           if(GetFileModTime(localfilename,m_mtime)==false)
           {
              RemoveFile(tmpremotefilename);
              return false;
           }
           if(recenttime!=m_mtime)
           {
             RemoveFile(tmpremotefilename);
             return false;
           }
           if (checksize)
           {
               // 对比文件大小
               if (getRemotefileSize(tmpremotefilename) == false)
               {
                    RemoveFile(tmpremotefilename);
                   return false;
               }
               unsigned int size;
               if (GetFileSize(localfilename, size) == false)
               {
                   RemoveFile(tmpremotefilename);
                   return false;
               }
               if (size != m_size)
               {
                   RemoveFile(tmpremotefilename);
                   return false;
               }
           }
           if(RenameFile(tmpremotefilename,remotefilename)==false)
           {
            //RemoveFile(tmpremotefilename);
            return false;
           }
           return true;
         }

        // 删除ftp服务器上的文件。
        // remotefilename：待删除的ftp服务器上的文件名。
        // 返回值：true-成功；false-失败。
         bool FtpClient::RemoveFile(const std::string &remotefilename)
         {
             // 1.如果没有连接,直接返回
             if (ftpconn == nullptr)
             {
                 return false;
             }
             if (FtpDelete(remotefilename.c_str(), ftpconn) == 0)
             {
                 return false;
             }
             return true;
         }

         bool FtpClient::RenameFile(const std::string &srcremotefilename, const std::string &dstremotefilename)
         {
             // 1.如果没有连接,直接返回
             if (ftpconn == nullptr)
             {
                 return false;
             }
             if (FtpRename(srcremotefilename.c_str(), dstremotefilename.c_str(), ftpconn) == 0)
             {
                 return false;
             }
             return true;
         }

        // 向ftp服务器发送site命令。
        // command：命令的内容。
        // 返回值：true-成功；false-失败。
        bool FtpClient::Site(const std::string &command)
        {
            // 1.如果没有连接,直接返回
             if (ftpconn == nullptr)
             {
                 return false;
             }
            if(FtpSite(command.c_str(), ftpconn)==0)
            {
                return false;
            }
            return true;
        }

        // 获取服务器返回信息的最后一条(return a pointer to the last response received)。
        char *FtpClient::Response()
        {
            // 1.如果没有连接,直接返回
             if (ftpconn == nullptr)
             {
                 return nullptr;
             }
            return FtpLastResponse(ftpconn);
        }

         FtpClient::~FtpClient() // 类的析构函数。
         {
            Logout();
         }

};