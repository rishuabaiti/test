#include"../../common/common.h"


//生成气象站观测数据111
// /home/ubuntu/bilibili/BigProjrct/idc/ini/stcode.ini (气象站点参数文件名。) 
// /home/ubuntu/bilibili/BigProjrct/surfdatas  (气象站点数据文件存放的目录。)
//  /home/ubuntu/bilibili/BigProjrct/logs  (本程序运行的日志文件名。)
idc:: FileLogger  filelog;
void cleanup(int sig);
void EXIT(int sig);
bool LoadStcodeToList(const std::string path);
void Crtsurfdata();
bool CrtsurfdataToFormatFile(const std::string &path,const std::string &format);


char currenttime[20];//数据时间：格式yyyy-mm-dd hh24:mi:ss，精确到分钟，秒固定填00。
// 省   站号  站名 纬度   经度  海拔高度
// 安徽,58015,砀山,34.27,116.2,44.2
struct Stcode{
     char province[31];     // 省
     char id[11];           // 站号
     char name[31];         // 站名
    double latitude;          // 纬度
    double longitude;         // 经度
    double altitude;          // 海拔高度
};
std::vector<Stcode> stcode_lists;

struct Surfdata{
    char id[11];          // 站号。
    char datetime[20];     // 数据时间：格式yyyy-mm-dd hh24:mi:ss，精确到分钟，秒固定填00。
    int temp;       // 气温 (0.1°C)
    int pressure; // 气压 (0.1 hPa)
    int humidity;  // 湿度 (0-100%)
    int wind_dir;       // 风向 (0-360°)
    int wind_speed;// 风速 (0.1 m/s)
    int rainfall;    // 降雨量 (0.1 mm)
    int vis;        // 能见度 (0.1 m) 
};
std::vector<Surfdata> surfdata_lists;
//每个进程都得心跳机制
idc::HeartbeatController heartbeatcontrol;//全局保证exit能够调用析构
int main(int argc,char *argv[])
{
    if(argc!=4)
    {
        std::cout<<"格式错误!"<<std::endl;
        std::cout<<"./crtsurfdata /home/ubuntu/bilibili/BigProjrct/idc/ini/stcode.ini /home/ubuntu/bilibili/BigProjrct/surfdatas /home/ubuntu/bilibili/BigProjrct/logs/log.txt"<<std::endl;
        return -1;
    }
   idc::ignore_all_signals();
   signal(SIGINT, cleanup);// Ctrl+C (SIGINT)
   signal(SIGTERM, cleanup);//kill -15 <PID>
   if(!filelog.Open(argv[3]))
   {
    std::cout<<"filelog 打开失败"<<std::endl;
   }
   filelog.Write("程序开始运行 ...\n");
   heartbeatcontrol.AddProcessInfoToShm(10,"crtsurfdtata");
   //1.将站点信息存储结构体列表
    if(LoadStcodeToList(argv[1])==false)
    {
        EXIT(-1);
    }
    //2.创造气象数据
    
    
    idc::calCurtime(currenttime,"yyyymmddhh24miss");
    memcpy(currenttime+17,"00",2);
    Crtsurfdata();

    //3.将list中的气象数据转换为csv,xml,json格式
    
    CrtsurfdataToFormatFile(argv[2],"csv");
    CrtsurfdataToFormatFile(argv[2],"xml");
    CrtsurfdataToFormatFile(argv[2],"json");
    

  

  
   filelog.Write("程序结束运行 ...\n");

}

void cleanup(int sig)
{
    filelog.Write("程序遇到:%d 信号终止\n",sig);
    exit(0);
}
void EXIT(int sig)
{
    filelog.Write("程序遇到:%d 信号终止\n",sig);
    exit(0);
}

bool LoadStcodeToList(const std::string path)
{
    idc::ReadFile readfile;
    if (!readfile.Open(path))
    {
        filelog.Write("stcode.ini 文件打开失败 ...\n");
        return false;
    }
    std::string message;
    readfile.Readline(message);

    struct Stcode stcode;
    idc::Cmdstr cmdstr;
    while (readfile.Readline(message))
    {
        // filelog.Write("%s",message.c_str());
        cmdstr.splitToCmd(message, ",");
        // 省   站号  站名 纬度   经度  海拔高度
        // 安徽,58015,砀山,34.27,116.2,44.2
        cmdstr.getvalue(0, stcode.province, 30);
        cmdstr.getvalue(1, stcode.id, 10);
        cmdstr.getvalue(2, stcode.name, 30);
        cmdstr.getvalue(3, stcode.latitude);
        cmdstr.getvalue(4, stcode.longitude);
        cmdstr.getvalue(5, stcode.altitude);
        stcode_lists.push_back(stcode);
    }
    //    for(auto&aa:stcode_lists)
    //    {
    //       filelog<<aa.province<<","<<aa.id<<","<<aa.name<<","<<aa.latitude<<","<<aa.longitude<<","<<aa.altitude<<"\n";

    //    }
    return true;
}

void Crtsurfdata()
{
    srand(time(NULL));

    for(auto &e:stcode_lists)
    {
         struct Surfdata surfdata;
        memset(&surfdata, 0, sizeof(surfdata));
        strcpy(surfdata.id, e.id);
         strcpy(surfdata.datetime, currenttime);
        surfdata.temp = rand() % 350;             // 气温 (0.1°C)
        surfdata.pressure = rand() % 265 + 10000; // 气压 (0.1 hPa)
        surfdata.humidity = rand() % 101;         // 湿度 (0-100%)
        surfdata.wind_dir = rand() % 361;         // 风向 (0-360°)
        surfdata.wind_speed = rand() % 150;       // 风速 (0.1 m/s)
        surfdata.rainfall = rand() % 16;          // 降雨量 (0.1 mm)
        surfdata.vis = rand() % 5001 + 100000;    // 能见度 (0.1 m)

        surfdata_lists.push_back(surfdata);
    }
    // for(auto&aa:surfdata_lists)
    // {
    //     filelog<<aa.id<<" "<<aa.datetime<<" "<<aa.temp<<" "<<aa.pressure<<" "\
    //     <<aa.humidity<<" "<<aa.wind_dir<<" "<<aa.wind_speed<<" "<<aa.rainfall<<" "<<aa.vis<<"\n";
    // }

}


bool CrtsurfdataToFormatFile(const std::string &path,const std::string &format)
{
    std::string  formatfilename=path+"/"+"SUF"+"_"+std::string(currenttime)+"_"+std::to_string(getpid())+"."+format;
    idc::WriteFile  formatfile;
    if(!formatfile.Open(formatfilename))
    {
        filelog.Write("%s 格式文件打开失败",format.c_str());
        return false;
    }
    if(format=="csv")
    {
      formatfile.Writeline("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");
    }
    if(format=="xml")
    {
        formatfile.Writeline("<data>\n");
    }
    if(format=="json")
    {
       formatfile.Writeline("{\"data\":[\n");
    }
    for(auto&e:surfdata_lists)
    {
        if(format=="csv")
        {
            formatfile.Writeline("%s,%s,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n",e.id,e.datetime,e.temp/10.0,e.pressure/10.0,\
                e.humidity/10.0,e.wind_dir/10.0,e.wind_speed/10.0,e.rainfall/10.0,e.vis/10.0);
        }
        if(format=="xml")
        {
            formatfile.Writeline("<id>%s</id>\n<datetime>%s</datetime>\n<temp>%.1f</temp>\n<pressure>%.1f</pressure>\n<humidity>%.1f</humidity>\n<wind_dir>%.1f</wind_dir>\n<wind_speed>%.1f</wind_speed>\n<rainfall>%.1f</rainfall>\n<vis>%.1f</vis>\n\n",\
                e.id,e.datetime,e.temp/10.0,e.pressure/10.0,\
                e.humidity/10.0,e.wind_dir/10.0,e.wind_speed/10.0,e.rainfall/10.0,e.vis/10.0);
        }
        if(format=="json")
        {
              formatfile.Writeline("{\"id\":\"%s\",\"datetime\":\"%s\",\"temp\":%.1f,\"pressure\":%.1f,\"humidity\":%.1f,\"wind_dir\":%.1f,\"wind_speed\":%.1f,\"rainfall\":%.1f,\"vis\":%.1f}",e.id,\
                e.datetime,e.temp/10.0,e.pressure/10.0,\
                e.humidity/10.0,e.wind_dir/10.0,e.wind_speed/10.0,e.rainfall/10.0,e.vis/10.0);

                static int time=0;
                if(time<surfdata_lists.size()-1)
                {
                     formatfile.Writeline(",\n");
                     time++;
                }
                else{
                    formatfile.Writeline("\n");
                }
        }
    }
    if (format == "xml")
    {
         formatfile.Writeline("</data>\n");
    }
    if (format == "json")
    {
        formatfile.Writeline("]}\n");
    }
   formatfile.Closeandrename();
   filelog.Write("%s 格式文件已经生成,%d 数据已经录入!\n",format.c_str(),surfdata_lists.size());
   return true;

}
