#include"common.h"

namespace idc{
    
    char* deleteLeftchar(char*target,char cc)
    {
        if(target==nullptr)
        {
            return target;
        }
        char *p=target;
        while(*p==cc)
        {
            p++;
        }
        memmove(target,p,strlen(target)-(p-target)+1);
        return target;
    }
    std::string deleteLeftchar(std::string&target,char cc)
    {
        int pos=target.find_first_not_of(cc);
        if(pos==std::string::npos)
        {
            target.clear();
        }
        else{
            target.erase(0,pos);
        }
        return target;
    }
    
    char*deleteRightchar(char*target,char cc)
    {
        if(target==nullptr)
        {
            return target;
        }
        char*end=target;
        while(*end!='\0')
        {
            end++;
        }
        //end>target  避免target="    "
        while(end > target &&*(end-1)==cc)
        {
            end--;
        }
        *end='\0';
        return target;
    }
    std::string deleteRightchar(std::string&target,char cc)
    {
         int pos=target.find_last_not_of(cc);
        if(pos==std::string::npos)
        {
            target.clear();
        }
        else{
            target.erase(pos+1);
        }
        return target;
    }

    char*deleteLRchar(char*target,char cc)
    {
        deleteLeftchar(target,cc);
        deleteLeftchar(target,cc);
        return target;
    }
    std::string deleteLRchar(std::string&target,char cc)
    {
        deleteLeftchar(target,cc);
        deleteRightchar(target,cc);
        return target;
    }

    char* toUpper(char*target)
    {
        if(target==nullptr)
        {
            return target;
        }
        char*p=target;
        while(*p!='\0')
        {
            if((*p>='a')&&(*p<='z'))
            {
                *p-=32;
            }
        }
        return target;
    }
    std::string &toUpper(std::string&target)
    {
        for(auto &e:target)
        {
            if(e>='a'&&e<='z')
            {
                e-=32;
            }
        }
        return target;
    }

    char* toLower(char*target)
    {
        if(target==nullptr)
        {
            return target;
        }
        char*p=target;
        while(*p!='\0')
        {
            if((*p>='A')&&(*p<='Z'))
            {
                *p+=32;
            }
        }
        return target;
    }
    std::string &toLower(std::string&target)
    {
        for(auto &e:target)
        {
            if(e>='A'&&e<='Z')
            {
                e+=32;
            }
        }
        return target;
    }

    bool replaceStr(std::string&str,const std::string&str1,const std::string& str2,bool isloop)
    {
        if(str==""||str1=="")
        {
            return false;
        }
        if(isloop==true&&str2.find(str1)!=std::string::npos)
        {
            return false;
        }
        int noloopstart=0;
        int currentpos=0;
        while(true)
        {
            if(isloop)
            {
                currentpos=str.find(str1);
            }
            else{
                currentpos=str.find(str1,noloopstart);
            }

            if(currentpos==std::string::npos)
            {
                break;
            }
            str.replace(currentpos,str1.length(),str2);

            if(!isloop)
            {
                noloopstart=currentpos+str2.length();
            }
        }
        return true;
    }
    bool replaceStr(char*str,const std::string&str1,const std::string& str2,bool isloop)
    {
        if(str==nullptr)
        {
            return false;
        }
        std::string tmp(str);
        replaceStr(tmp,str1,str2,isloop);
         //无法保证操作一定成功-----因为无法检查str缓冲区大小
         tmp.copy(str,tmp.size());
         str[tmp.size()]='\0';
         return true;

    }

    std::string picknumber(const std::string &src, std::string &dest, bool isAdd_Sub, bool isDot)
    {
        //为实现scr与dest可以共用一个变量
        std::string tmp;
        for(auto e:src)
        {
            if(isAdd_Sub&&(e=='+'||e=='-'))
            {
                tmp.append(1,e);
            }
            if(isDot&&(e=='.'))
            {
                tmp.append(1,e);
            }
            if(isdigit(e))
            {
                tmp.append(1,e);
            }
        }
        dest=tmp;
        return dest;
    }
    std::string picknumber(const std::string &src, bool isAdd_Sub, bool isDot)
    {
        std::string tmp;
        picknumber(src,tmp,isAdd_Sub,isDot);
        return tmp;
    }
    char *picknumber(const std::string &src, char *dest, bool isAdd_Sub, bool isDot)
    {
        //没有指向内存,不能够获取源字符串内容
        if(dest==nullptr)
        {
            return nullptr;
        }
        std::string tmp;
        picknumber(src,tmp,isAdd_Sub,isDot);

        tmp.copy(dest,tmp.size());
        dest[tmp.size()-1]='\0';
        return dest;
    }

     // 辅助函数：分割字符串
    std::vector<std::string> splitRules(const std::string &rules)
    {
        std::vector<std::string> res;
        int begin=0;
        int pos=rules.find(',');
        while(pos!=std::string::npos)
        {
            res.push_back(rules.substr(begin,pos-begin));
            begin=pos+1;
            pos=rules.find(',',begin);
        }
        //避免rules="";
        if (begin < rules.length()) {
        res.push_back(rules.substr(begin));
        }
        return res;

    }
    // 辅助函数：通配符匹配（不区分大小写） 
    bool wildcardMatch(const std::string &str, const std::string &pattern)
    {

        // std::string lowerStr = str;
        // std::string lowerPattern =pattern;
        // toLower(lowerStr);
        // toLower(lowerPattern);
        // std::string::size_type flag=std::string::npos;

        // int strpos=0;
        // int patternpos=0;

        // int strflagpos;

        // while(strpos<lowerStr.size())
        // {
        //     if(patternpos<lowerPattern.size()&&lowerPattern[patternpos]=='*')
        //     {
        //         flag=patternpos;
        //         strflagpos=strpos;
        //         patternpos++;
        //     }
        //     else if(patternpos<lowerPattern.size()&&lowerStr[strpos]==lowerPattern[patternpos])
        //     {
        //         strpos++;
        //         patternpos++;
        //     }
        //     else if(flag!=std::string::npos)
        //     {
        //         patternpos=flag+1;
        //         strpos=strflagpos+1;
        //     }
        //     else{
        //         return false;
        //     }
        // }
        // //还有一种可能-----pattern没有匹配完
        // while(patternpos<lowerPattern.size()&&lowerPattern[patternpos]=='*')
        // {
        //     patternpos++;
        // }
        // return patternpos==lowerPattern.size();

        //力扣上面算法题----修改:*可以匹配一个或者多个字符,不能匹配空字符
        size_t sLen = str.length();
        size_t pLen = pattern.length();

        std::vector<std::vector<bool>> dp(sLen + 1, std::vector<bool>(pLen + 1, false));
        dp[0][0] = true; // 空字符串匹配空模式

        // 处理模式以 * 开头的情况（但 * 不能匹配空字符串，所以 dp[0][j] = false）
        //初始化为false
        dp[0][0]=true;

        for (size_t i = 1; i <= sLen; ++i)
        {
            for (size_t j = 1; j <= pLen; ++j)
            {
                if (pattern[j - 1] == '*')
                {
                    // * 必须匹配至少 1 个字符，所以只能依赖 dp[i-1][j]
                    dp[i][j] = dp[i - 1][j]||dp[i-1][j-1];
                }
                else
                {
                    // 普通字符必须严格匹配
                    dp[i][j] = (str[i - 1] == pattern[j - 1]) && dp[i - 1][j - 1];
                }
            }
        }

        return dp[sLen][pLen];
    }
    

    bool matchstr(const std::string &str,const std::string &rules)
    {
        if(str==""||rules=="")
        {
            return false;
        }
         std::vector<std::string> res=splitRules(rules);
        for(auto rule:res)
        {
            if(wildcardMatch(str,rule))
            {
                return true;
            }
        }
        return false;
    }
    
        Cmdstr::Cmdstr(const std::string&scr,const std::string& splitflag,bool isdeletespace)
        {
            splitToCmd(scr,splitflag,isdeletespace);
        }


        std::string &Cmdstr::operator[](int i)
        {
            return m_cmdstr[i];
        }

        void Cmdstr::splitToCmd(const std::string &scr, const std::string &splitflag, bool isdeletespace)
        {
            m_cmdstr.clear();
            std::string scrcopy = scr;
            if (scrcopy == "")
            {
                return;
            }
            if (splitflag == "")
            {
                if (isdeletespace)
                {
                    deleteLRchar(scrcopy);
                }
                m_cmdstr.push_back(scrcopy);
                return;
            }
            int begin = 0;
            int pos = scr.find(splitflag);
            while (pos != std::string::npos)
            {
                std::string tmp = scr.substr(begin, pos - begin);
                if (isdeletespace)
                {
                    deleteLRchar(tmp);
                }
                m_cmdstr.push_back(tmp);
                begin = pos + 1;
                pos = scr.find(splitflag, begin);
            }

            std::string tmp = scr.substr(begin);
            if (isdeletespace)
            {
                deleteLRchar(tmp);
            }
            m_cmdstr.push_back(tmp);
        }

        int Cmdstr::size()  const
        {
            return m_cmdstr.size();
        }

        bool Cmdstr::getvalue(int i, std::string &value, const int len) const // len表示想要值的字节数,默认表示全要
        {
            if (i >= m_cmdstr.size())
            {
                return false;
            }
            int targetlen = m_cmdstr[i].length();
            if ((len > 0) && (len < targetlen))
            {
                targetlen = len;
            }
            value = m_cmdstr[i].substr(0, targetlen);

            return true;
        }
        bool Cmdstr::getvalue(int i, char *value, const int len) const
        {
            if (i >= m_cmdstr.size() || value == nullptr)
            {
                return false;
            }
            if(len>0)
            {
                memset(value, 0, len + 1);
            }
          
            int targetlen = m_cmdstr[i].length();
            if ((len > 0) && (len < targetlen))
            {
                targetlen = len;
            }
            m_cmdstr[i].copy(value, targetlen);
            value[targetlen] = '\0';
            return true;
        }
        bool Cmdstr::getvalue(int i,int &value,const int len) const
        {
             if (i >= m_cmdstr.size())
            {
                return false;
            }
            std::string tmp;
            try{
                value=stoi(picknumber(m_cmdstr[i],tmp,true,false));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
        }
        bool Cmdstr::getvalue(int i,long &value,const int len) const
        {
             if (i >= m_cmdstr.size())
            {
                return false;
            }
            std::string tmp;
            try{
                value=stoi(picknumber(m_cmdstr[i],tmp,true,false));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
        }
        bool Cmdstr::getvalue(int i,unsigned int &value,const int len) const
        {
            if (i >= m_cmdstr.size())
            {
                return false;
            }
            std::string tmp;
            try{
                value=stoi(picknumber(m_cmdstr[i],tmp,false,false));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
        }
        bool Cmdstr::getvalue(int i,unsigned long &value,const int len) const
        {
             if (i >= m_cmdstr.size())
            {
                return false;
            }
            std::string tmp;
            try{
                value=stoi(picknumber(m_cmdstr[i],tmp,false,false));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
        }
        bool Cmdstr::getvalue(int i,double &value,const int len) const
        {
             if (i >= m_cmdstr.size())
            {
                return false;
            }
            std::string tmp;
            try{
                value=stod(picknumber(m_cmdstr[i],tmp,true,true));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
        }
        bool Cmdstr::getvalue(int i,float &value,const int len) const
        {
              if (i >= m_cmdstr.size())
            {
                return false;
            }
            std::string tmp;
            try{
                value=stod(picknumber(m_cmdstr[i],tmp,true,true));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
        }
        bool Cmdstr::getvalue(int i,bool &value,const int len) const
        {
              if (i >= m_cmdstr.size())
            {
                return false;
            }
            std::string tmp=m_cmdstr[i];
          
            if( toUpper(tmp)=="TRUE")
            {
               value=true;
            }
            else{
                value=false;
            }
        
            return true;
        }
        //获取xml文本内容
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, std::string &value, const int len )
    {
           std::string begintext="<"+tagtext+">";
           std::string endtest="</"+tagtext+">";
           int begin=xmltext.find(begintext);
           if(begin==std::string::npos)
           {
            return false;
           }
           int end=xmltext.find(endtest);
           if(end==std::string::npos)
           {
            return false;
           }
           int strlen=end-begin-begintext.size();
            if ((len > 0) && (len < strlen))
            {
                strlen = len;
            }
            value = xmltext.substr(begin+begintext.size(), strlen);

            return true;
    }
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, char *value, const int len )
    {

            if(value==nullptr)
            {
                return false;
            }
            if(len!=0)
            {
                 memset(value, 0, len + 1);
            }
          std::string begintext="<"+tagtext+">";
           std::string endtest="</"+tagtext+">";
           int begin=xmltext.find(begintext);
           if(begin==std::string::npos)
           {
            return false;
           }
           int end=xmltext.find(endtest);
           if(end==std::string::npos)
           {
            return false;
           }
           int strlen=end-begin-begintext.size();
            if ((len > 0) && (len < strlen))
            {
                strlen = len;
            }
             xmltext.copy(value, strlen,begin+begintext.size());
             value[strlen] = '\0';
            return true;
      
    }
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, int &value)
    {
           std::string begintext="<"+tagtext+">";
           std::string endtest="</"+tagtext+">";
           int begin=xmltext.find(begintext);
           if(begin==std::string::npos)
           {
            return false;
           }
           int end=xmltext.find(endtest);
           if(end==std::string::npos)
           {
            return false;
           }
           int strlen=end-begin-begintext.size();
            std::string tmp=xmltext.substr(begin+begintext.size(),strlen);
            try{
                value=stoi(picknumber(tmp,true,false));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
    }
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, long &value)
    {
        std::string begintext="<"+tagtext+">";
           std::string endtest="</"+tagtext+">";
           int begin=xmltext.find(begintext);
           if(begin==std::string::npos)
           {
            return false;
           }
           int end=xmltext.find(endtest);
           if(end==std::string::npos)
           {
            return false;
           }
           int strlen=end-begin-begintext.size();
            std::string tmp=xmltext.substr(begin+begintext.size(),strlen);
            try{
                value=stoi(picknumber(tmp,true,false));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
    }
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, unsigned int &value)
    {
         std::string begintext="<"+tagtext+">";
           std::string endtest="</"+tagtext+">";
           int begin=xmltext.find(begintext);
           if(begin==std::string::npos)
           {
            return false;
           }
           int end=xmltext.find(endtest);
           if(end==std::string::npos)
           {
            return false;
           }
           int strlen=end-begin-begintext.size();
             std::string tmp=xmltext.substr(begin+begintext.size(),strlen);
            try{
                value=stoi(picknumber(tmp,false,false));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
    }
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, unsigned long &value)
    {
          std::string begintext="<"+tagtext+">";
           std::string endtest="</"+tagtext+">";
           int begin=xmltext.find(begintext);
           if(begin==std::string::npos)
           {
            return false;
           }
           int end=xmltext.find(endtest);
           if(end==std::string::npos)
           {
            return false;
           }
           int strlen=end-begin-begintext.size();
            std::string tmp=xmltext.substr(begin+begintext.size(),strlen);
            try{
                value=stoi(picknumber(tmp,false,false));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
    }
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, double &value)
    {
          std::string begintext="<"+tagtext+">";
           std::string endtest="</"+tagtext+">";
           int begin=xmltext.find(begintext);
           if(begin==std::string::npos)
           {
            return false;
           }
           int end=xmltext.find(endtest);
           if(end==std::string::npos)
           {
            return false;
           }
           int strlen=end-begin-begintext.size();
            std::string tmp=xmltext.substr(begin+begintext.size(),strlen);
            try{
                value=stof(picknumber(tmp,true,true));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
    }
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, float &value)
    {
          std::string begintext="<"+tagtext+">";
           std::string endtest="</"+tagtext+">";
           int begin=xmltext.find(begintext);
           if(begin==std::string::npos)
           {
            return false;
           }
           int end=xmltext.find(endtest);
           if(end==std::string::npos)
           {
            return false;
           }
           int strlen=end-begin-begintext.size();
            std::string tmp=xmltext.substr(begin+begintext.size(),strlen);
            try{
                value=stof(picknumber(tmp,true,true));
            }
            catch(const std::exception&){
                 return false;
            }
            return true;
    }
    bool getXmlValue(const std::string &xmltext, const std::string &tagtext, bool &value)
    {
        std::string begintext = "<" + tagtext + ">";
        std::string endtest = "</" + tagtext + ">";
        int begin = xmltext.find(begintext);
        if (begin == std::string::npos)
        {
            return false;
        }
        int end = xmltext.find(endtest);
        if (end == std::string::npos)
        {
            return false;
        }
        std::string tmp;
        int strlen = end - begin - begintext.size();

        tmp = xmltext.substr(begin + begintext.size(), strlen);

        if (toUpper(tmp) == "TRUE")
        {
            value = true;
        }
        else
        {
            value = false;
        }

        return true;
    }

       

// "yyyy-mm-dd hh24:mi:ss"
//   "yyyymmddhh24miss"
//   "yyyy-mm-dd"
//   "yyyymmdd"
//   "hh24:mi:ss"
//   "hh24miss"
//   "hh24:mi"
//   "hh24mi"
//   "hh24"
//   "mi"

    std::string &timeTostr(const time_t time, std::string &strtime, const std::string &format )
    {
        //将时间戳转换为结构体
        struct tm tmtime;
        localtime_r(&time,&tmtime);
        tmtime.tm_year+=1900;
        tmtime.tm_mon+=1;
        tmtime.tm_isdst=0;//标准时间
        if(format==""||format=="yyyy-mm-dd hh24:mi:ss")
        {
            strtime=coutFormat("%04u-%02u-%02u %02u:%02u:%02u",tmtime.tm_year,tmtime.tm_mon,tmtime.tm_mday,\
            tmtime.tm_hour,tmtime.tm_min,tmtime.tm_sec);

        }
        else if(format=="yyyymmddhh24miss")
        {
            strtime=coutFormat("%04u%02u%02u%02u%02u%02u",tmtime.tm_year,tmtime.tm_mon,tmtime.tm_mday,\
            tmtime.tm_hour,tmtime.tm_min,tmtime.tm_sec);
        }
        else if(format=="yyyy-mm-dd")
        {
            strtime=coutFormat("%04u-%02u-%02u",tmtime.tm_year,tmtime.tm_mon,tmtime.tm_mday);
        }
        else if(format=="hh24:mi:ss")
        {
            strtime=coutFormat("%02u:%02u:%02u",tmtime.tm_hour,tmtime.tm_min,tmtime.tm_sec);
        }
        else if(format=="hh24miss")
        {
            strtime=coutFormat("%02u%02u%02u",tmtime.tm_hour,tmtime.tm_min,tmtime.tm_sec);
        }
        else if(format=="hh24:mi")
        {
            strtime=coutFormat("%02u:%02u",tmtime.tm_min,tmtime.tm_sec);
        }
        else if(format=="hh24mi")
        {
              strtime=coutFormat("%02u%02u",tmtime.tm_min,tmtime.tm_sec);
        }
        else if(format=="hh24")
        {
            strtime=coutFormat("%02u",tmtime.tm_hour);
        }
        else if(format=="mi")
        {
            strtime=coutFormat("%02u",tmtime.tm_min);
        }
        return strtime;

    }
    char *timeTostr(const time_t time, char *strtime, const std::string &format )
    {
        if(strtime==nullptr)
        {
            return nullptr;
        }
        std::string tmp;
        timeTostr(time,tmp,format);
        tmp.copy(strtime,tmp.size());
        strtime[tmp.size()]='\0';
        return strtime;

    }
    
    std::string timeTostr(const time_t time, const std::string &format )
    {
        std::string tmp;
        timeTostr(time,tmp,format);
        return tmp;
    }

    //将指定格式字符串转化为时间戳
    time_t strTotime(const std::string &strtime)
    {
         // 2021-12-05 08:30:45
         // 2021/12/05 08:30:45
         // 20211205083045
         
        struct tm tmtime;
        std::string tmp= picknumber(strtime,false,false);
        if(tmp.size()!=14)
        {
            return -1;
        }
        tmtime.tm_year=stoi(tmp.substr(0,4))-1900;
        tmtime.tm_mon=stoi(tmp.substr(4,2))-1;
        tmtime.tm_mday=stoi(tmp.substr(6,2));

        tmtime.tm_hour=stoi(tmp.substr(8,2));
        tmtime.tm_min=stoi(tmp.substr(10,2));
        tmtime.tm_sec=stoi(tmp.substr(12,2));

        tmtime.tm_isdst=0;

        //将结构体转化为时间戳
       return  mktime(&tmtime);

        
    }

    //对当前时间进行加减
    std::string &calCurtime(std::string &strtime, const std::string &format , const int timetvl )
    {
        time_t  curtime;
        time(&curtime );  
        curtime+=timetvl;
        timeTostr(curtime,strtime,format);
        return strtime;
        
    }
    char *calCurtime(char *strtime, const std::string &format , const int timetvl )
    {
       if(strtime==nullptr)
       {
        return nullptr;
       }
       std::string tmp;
       calCurtime(tmp,format,timetvl);
       tmp.copy(strtime,tmp.size());
       strtime[tmp.size()]='\0';
       return strtime;

       
    }
    // 为了避免重载的岐义，增加calCurtime1函数。
    std::string calCurtime1(const std::string &format, const int timetvl )
    {
        std::string tmp;
       calCurtime(tmp,format,timetvl);
       return tmp;
    }

     bool calIntime(const std::string &in_time,char *out_time  ,const int timetvl,const std::string &format)
     {
        if(out_time==nullptr)
        {
            return false;
        }
        time_t in=strTotime(in_time);;
        if(in==-1)
        {
            return false;
        }
        in+=timetvl;
        timeTostr(in,out_time,format);
        return true;
        

     }
    bool calIntime(const std::string &in_time,std::string &out_time,const int timetvl,const std::string &format)
    {
        time_t in=strTotime(in_time);
        if(in==-1)
        {
            return false;
        }
        in+=timetvl;
        timeTostr(in,out_time,format);
        return true;
    }

        ctimer::ctimer() // 构造函数中会调用start方法。
        {
            start();
        }

        void ctimer::start() // 开始计时。
        {
            gettimeofday(&m_start, NULL);  // 开始计时,获取当前时间
        }

        // 计算已逝去的时间，单位：秒，小数点后面是微秒。
        // 每调用一次本方法之后，自动调用start方法重新开始计时。
        double ctimer::elapsed()
        {
            gettimeofday(&m_end, NULL);

            std::string str;
            str = coutFormat("%ld.%06ld", m_start.tv_sec, m_start.tv_usec);
            double start = stod(str);

            str = coutFormat("%ld.%06ld", m_end.tv_sec, m_end.tv_usec);
            double end = stod(str);
            ctimer::start();

            return end - start;
        }

        bool newdir(const std::string &pathOrfilename, bool isfilename)
        {
            mode_t mode=0755;
            int pos=1;
            while (true)
            {
                int ret = pathOrfilename.find('/', pos);
                if (ret == std::string::npos)
                {
                     break;
                }
                std::string subPath = pathOrfilename.substr(0, ret);
                if (access(subPath.c_str(), F_OK) == -1)
                {
                    if (mkdir(subPath.c_str(), mode) == -1)
                    {
                        return false;
                    }
                }
                pos = ret + 1;
            }
            if(!isfilename)
            {
                if (access(pathOrfilename.c_str(), F_OK) == -1)
                {
                    if (mkdir(pathOrfilename.c_str(), mode) == -1)
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        bool renamefile(const std::string &srcFilename, const std::string &desFilename)
        {
            if(access(srcFilename.c_str(),F_OK)==-1)
            {
                return false;
            }
            if(rename(srcFilename.c_str(),desFilename.c_str())==-1)
            {
                return false;
            }
            return true;
        }

        int getFilesize(const std::string &filename)
        {
            struct stat filestruct;
            if(stat(filename.c_str(),&filestruct)==-1)
            {
                return -1;
            }
            return filestruct.st_size;

        }

        bool getFiletime(const std::string &filename, char *time, const std::string &format)
        {
            struct stat filestruct;
            if(stat(filename.c_str(),&filestruct)==-1)
            {
                return false;
            }
            time_t filetime=filestruct.st_mtime;
            timeTostr(filetime,time,format);
            return true;
        }
        bool getFiletime(const std::string &filename, std::string &time, const std::string &format )
        {
            struct stat filestruct;
            if(stat(filename.c_str(),&filestruct)==-1)
            {
                return false;
            }
            time_t filetime=filestruct.st_mtime;
            timeTostr(filetime,time,format);
            return true;
        }

        bool setFiletime(const std::string &filename, const std::string &time)
        {
            struct utimbuf new_times;
            time_t time1=strTotime(time);
            new_times.actime = time1;  // 访问时间
            new_times.modtime = time1; // 修改时间

            if (utime(filename.c_str(), &new_times) == -1)
            {
                perror("utime failed");
                return false;
            }
            return true;
        }

         // 判断文件是否已打开。
        bool ReadFile::isopen() const 
        {
            return infile.is_open();
        }

        // 打开文件。
        // filename，待打开的文件名。
        // mode，打开文件的模式。
        bool ReadFile::Open(const std::string &filename, const std::ios::openmode mode)
        {
            m_filename=filename;
            infile.open(filename,mode);
            return infile.is_open();
        }

        // 以行的方式读取文本文件，endFlag指定行的结尾标志，缺省为空，读取一行
        bool ReadFile::Readline(std::string &buf, const std::string &endFlag)
        {
            buf.clear();
            std::string str;
            while(std::getline(infile,str))
            {
                buf.append(str);
                if(endFlag=="")
                {
                    return true;
                }
                else{
                    if(buf.find(endFlag,0)!=std::string::npos)
                    {
                       return true;
                    }
                }
                buf.append("\n");
            }
            return false;
        }

        // 读取二进制文件，返回实际读取到的字节数。
        int ReadFile::Read(void *buf, const int bufsize)
        {
            if(buf==nullptr)
            {
                return -1;
            }
            if(!infile.is_open())
            {
                return -2;
            }
           infile.read(static_cast<char*>(buf),bufsize);
           //返回最近一次 无格式读取操作（如 read()、get()、getline() 等）实际读取的字符数。
           return infile.gcount(); 
        }

        // 关闭并删除文件。
        bool ReadFile::Closeandremove()
        {
            if(infile.is_open())
            {
                infile.close();
            }
            if(std::remove(m_filename.c_str())==-1)
            {
                return false;
            }
            return true;
        }

        // 只关闭文件。
        void ReadFile::Close()
        {
            if(!infile.is_open())
            {
                return ;
            }
            infile.close();
        }

        ReadFile::~ReadFile()
        {
            Close();
        }

      bool WriteFile::isopen() const
      {
        return outfile.is_open();
      }

      bool WriteFile::Open(const std::string &filename, const bool istmp , const std::ios::openmode mode , const bool isbuffer)
      {
        if (outfile.is_open()) outfile.close();

           m_filename=filename;
        if(!newdir(filename,true))
        {
            return false;
        }
        
        if(istmp)
        {
            m_filename_tmp=filename+".tmp";
            outfile.open(m_filename_tmp,mode);

        }
        else{
            m_filename_tmp.clear();
            outfile.open(m_filename,mode);
        }
        // 不启用文件缓冲区。
         if (isbuffer==false)
         {
            outfile.setf(std::ios::unitbuf); 
         }
        return outfile.is_open();
      }

        

        // 把二进制数据写入文件。
        bool WriteFile::Write(void *buf, int bufsize)
        {
            if(!outfile.is_open())
            {
                return false;
            }
            outfile.write(static_cast<char*>(buf),bufsize);
            return outfile.good();

        }

        // 关闭文件，并且把临时文件名改为正式文件名。
        bool WriteFile::Closeandrename()
        {
            if(!outfile.is_open())
            {
                return false;
            }
            outfile.close();
            if(!m_filename_tmp.empty())
            {
                if(rename(m_filename_tmp.c_str(),m_filename.c_str())==-1)
                {
                    return false;
                }
            }
            return true;

        }

        // 关闭文件，如果有临时文件，则删除它。
        void WriteFile::Close()
        {
            if(!outfile.is_open())
            {
                return;
            }
            outfile.close();
        }

        WriteFile::~WriteFile()
        {
            Close();
        }


        bool copyfile(const std::string &srcfilename,const std::string &desfilename)
        {
            if(!newdir(desfilename,true))
            {
                return false;
            }

            ReadFile readfile;
            WriteFile writefile;
            readfile.Open(srcfilename,std::ios::in|std::ios::binary);
            writefile.Open(desfilename,true,std::ios::out|std::ios::binary,true);//启用临时文件

            char buffer[5000];
            int buffersize=0;
            int currentsize=0;
            int totalsize=getFilesize(srcfilename);

            while(true)
            {
                if(totalsize-currentsize>5000)
                {
                   buffersize=5000;
                }
                else{
                    buffersize=totalsize-currentsize;
                }
                memset(buffer,0,sizeof(buffer));

                readfile.Read(buffer,buffersize);
                writefile.Write(buffer,buffersize);

                currentsize+=buffersize;
                if(currentsize>=totalsize)
                {
                    break;
                }

            }
            readfile.Close();
            writefile.Closeandrename();//需要将临时文件重命名

            return true;
  

        }


         DirScanner::DirScanner():m_pos(0),m_format("yyyymmddhh24miss")
         {
             m_filelist.clear();
         }

        // 设置文件时间的格式，支持"yyyy-mm-dd hh24:mi:ss"和"yyyymmddhh24miss"两种，缺省是后者。
        void DirScanner::setformat(const std::string &format)
        {
            m_format=format;
        }

        bool DirScanner::Opendir(const std::string &dirname, const std::string &rules, const int maxfiles, const bool expand_dir_children , bool issort)
        {
            m_filelist.clear(); // 清空文件列表容器。
            m_pos = 0;
            bool ret=_Opendir(dirname, rules, maxfiles, expand_dir_children);
            if(issort)
            {
                sort(m_filelist.begin(),m_filelist.end());
            }
            return ret;

        }

        bool DirScanner::Readdir()
        {
            if (m_pos >= m_filelist.size())
            {
                return false;
            }
            absolute_path_filename = m_filelist[m_pos];
            int pos = absolute_path_filename.find_last_of('/');
            m_dirname = absolute_path_filename.substr(0, pos);
            m_filename = absolute_path_filename.substr(pos + 1);

            struct stat filestruct;
            if (stat(absolute_path_filename.c_str(), &filestruct) == -1)
            {
                return false;
            }
            m_filesize = filestruct.st_size;
            timeTostr(filestruct.st_mtim.tv_sec, m_mtime, m_format);
            timeTostr(filestruct.st_ctim.tv_sec, m_ctime, m_format);
            timeTostr(filestruct.st_atim.tv_sec, m_atime, m_format);
            m_pos++;
            return true;
        }

        unsigned int DirScanner::FileListSize()
        {
            return m_filelist.size();
        }

      

        bool DirScanner::_Opendir(const std::string &dirname, const std::string &rules, const int maxfiles, const bool expand_dir_children)
        {

            if (newdir(dirname.c_str(), false) == false)
            {
                return false;
            }
            DIR *dir;
            dir = opendir(dirname.c_str());
            if (dir == nullptr)
            {
                return false;
            }
            struct dirent *dirContent;
            // 等于NULL,表示读到末尾或者发生错误
            errno = 0; // 重置errno以检测readdir错误
            while ((dirContent = readdir(dir)) != NULL)
            {
                std::string path = dirname + '/';
                path += dirContent->d_name;
                if (dirContent->d_name[0] == '.')
                {
                    continue;
                }
                if (m_filelist.size() >= maxfiles)
                {
                    break;
                }
                if (dirContent->d_type == DT_REG)
                {
                    //通配符匹配 
                    if (matchstr(dirContent->d_name, rules))
                    {
                        m_filelist.push_back(path);
                        continue;
                    }
                }
                if (expand_dir_children)
                {
                    if (dirContent->d_type == DT_DIR)
                    {
                        if (_Opendir(path, rules, maxfiles, expand_dir_children) == false)
                        {
                            // 递归失败不需要特殊处理，因为每个_Opendir自己负责清理
                        }
                    }
                }
            }
            // 检查是否因错误退出
            if (errno != 0)
            {
                closedir(dir);
                return false;
            }

            closedir(dir);
            return true;
        }


 ////////////////////////////////////////////////////////////////////////////////////////////
 //日志文件
        FileLogger::FileLogger(int maxsize):_maxsize(maxsize){}

        bool FileLogger::Open(const std::string &filename, const std::ios::openmode mode , const bool rotate , const bool buffer_enabled )
        {
            if (logfile.is_open()) 
            {
                logfile.close();
            }
            if(newdir(filename,true)==false)
            {
                std::cout<<"新建log.txt失败"<<std::endl;
                return false;
            }

            logfilename = filename;
            _mode = mode;
            _rotate = rotate;
            _buffer_enabled = buffer_enabled;
    
             logfile.open(logfilename,_mode);
         
             if(_buffer_enabled==false)
            {
                logfile.setf(std::ios::unitbuf); 
            }

           
            return logfile.is_open();
        }

        // 把日志内容以文本的方式格式化输出到日志文件，并且，在日志内容前面写入时间。
        

        void FileLogger::Close()
        {
            logfile.close();
        }

        FileLogger::~FileLogger()
        {
            Close();
        }

        // 注意，在多进程的程序中，日志文件不可切换，多线的程序中，日志文件可以切换。
        bool FileLogger::RotateLogfile()
        {
            if(!_rotate)
            {
                return false;
            }
            if(logfile.tellp()<_maxsize*1024*1024)
            {
               return false;
            }
            std::string newpath=logfilename+'.'+calCurtime1();
            rename(logfilename.c_str(),newpath.c_str());

            logfile.open(logfilename,_mode);

            return logfile.is_open();

        }

 ///////////////////////////////////////////////////////////////////////////////////////////////
 //网络通信  客户端
        TcpClient::TcpClient() : clientsocket(-1), _port(0) {}

        bool TcpClient::Connect(const std::string &ip, const int port)
        {
            

            clientsocket = socket(AF_INET, SOCK_STREAM, 0);
            signal(SIGPIPE, SIG_IGN);  // 保证向已经关闭的socket,发送信息程序不会关闭
            if (clientsocket == -1)
            {
                perror("client socket failed");
                return false;
            }
          
             _ip=ip;
             _port=port;
            struct sockaddr_in clientaddr;
            memset(&clientaddr,0,sizeof(clientaddr));
            clientaddr.sin_family=AF_INET;
           
            clientaddr.sin_port=htons(_port);
            if (inet_pton(AF_INET, ip.c_str(), &clientaddr.sin_addr) <= 0)
            {
                perror("inet_pton failed");
                close(clientsocket); // 关闭 socket
                return false;
            }

            int retconnect=connect(clientsocket,(sockaddr*)&clientaddr,sizeof(clientaddr));
            if(retconnect==-1)
            {
                 perror("client connect failed");
                  close(clientsocket); // 关闭 socket
                return false;
            }
            return true;
        }
        // 接收文本数据。没有指定消息格式:文件长度+文件内容
        bool TcpClient::Read(std::string &buffer, const int socket_timeout )
        {
            // 读的消息放在buffer,不粘包
            if (clientsocket == -1)
            {
                perror("client socket failed");
                return false;
            }
            // 设置poll等待
            struct pollfd pollfds;
            pollfds.fd = clientsocket;
            pollfds.events = POLLIN;

            // 转换超时时间为毫秒
            //poll 的 timeout 参数为负数时会无限等待
            int timeout_ms = socket_timeout * 1000;
            int ret = poll(&pollfds, 1, timeout_ms);

            if (ret == 0)
            {
                // 超时不是错误，不需要perror
                //std::cerr << "Client read timeout" << std::endl;
                return false;
            }
            else if (ret < 0)
            {
                perror("poll error");
                return false;
            }
            uint32_t headmessage = 0;
            if (ReadByByte(clientsocket, (char *)&headmessage, 4)==false)
            {
                std::cerr << "Failed to read message header" << std::endl;
                return false;
            }
            headmessage = ntohl(headmessage); // 32位网络转主机
            buffer.resize(headmessage);
            if (ReadByByte(clientsocket, &buffer[0], headmessage)==false)
            {
                std::cerr << "Failed to read message body" << std::endl;
                return false;
            }
            return true;
        }
        // 接收二进制数据。默认指定文件消息格式
        bool TcpClient::Read(void *buffer, const int bufferlenth, const int socket_timeout )
        {
            if (clientsocket == -1)
            {
                perror("client socket failed");
                return false;
            }
            // 设置poll等待
            struct pollfd pollfds;
            pollfds.fd = clientsocket;
            pollfds.events = POLLIN;

            // 转换超时时间为毫秒
            //poll 的 timeout 参数为负数时会无限等待
            int timeout_ms = socket_timeout * 1000;
            int ret = poll(&pollfds, 1, timeout_ms);

            if (ret == 0)
            {
                // 超时不是错误，不需要perror
                std::cerr << "Client read timeout" << std::endl;
                return false;
            }
            else if (ret < 0)
            {
                perror("poll error");
                return false;
            }
             if (ReadByByte(clientsocket, (char*)buffer, bufferlenth)==false)
            {
                std::cerr << "Failed to read message " << std::endl;
                return false;
            }
            return true;
        }

        bool TcpClient::Write(const std::string &buffer)                // 发送文本数据。
        {
           if (clientsocket == -1)
            {
                perror("client socket failed");
                return false;
            }
             uint32_t headmessage = buffer.size();
             headmessage = htonl(headmessage); // 主机转网络
            if (WriteByByte(clientsocket, (char *)&headmessage, 4)==false)
            {
                std::cerr << "Failed to write message header" << std::endl;
                return false;
            }
            //.c_str()是const char*
            if (WriteByByte(clientsocket,buffer.c_str(), buffer.size())==false)
            {
                std::cerr << "Failed to write message body" << std::endl;
                return false;
            }
            return true;
        }
        bool TcpClient::Write(const void *buffer, const int bufferlenth) // 发送二进制数据。
        {
            if (clientsocket == -1)
            {
                perror("client socket failed");
                return false;
            }
           if (WriteByByte(clientsocket,(char*)buffer, bufferlenth)==false)
            {
                std::cerr << "Failed to write message body" << std::endl;
                return false;
            }
            return true;
        }

        // 断开与服务端的连接
        void TcpClient::Close()
        {
            close(clientsocket); // 关闭 socket
        }

        TcpClient::~TcpClient()
        {
            Close();
        }

        bool ReadByByte(const int socketfd,char*buf,int bufsize)
        {
            int pos = 0;
            int totalsize = bufsize;
            int remainsize = bufsize;

            while(remainsize>0)
            {
                int readsize=recv(socketfd,buf+pos,remainsize,0);
                if(readsize<0)
                {
                    //链接出错
                    close(socketfd);
                    return false;
                }
                if(readsize==0)
                {
                    close(socketfd);
                    printf("客户端连接已关闭\n");
                    return false;
                }
                pos+=readsize;
                remainsize-=readsize;
            }
            return true;
        }
         bool WriteByByte(const int socketfd,const char*buf,int bufsize)
         {
            int pos = 0;
            int totalsize = bufsize;
            int remainsize = bufsize;

            while(remainsize>0)
            {
                int writesize=send(socketfd,buf+pos,remainsize,0);
                if(writesize<=0)
                {
                    //链接已经出错
                    close(socketfd);
                    perror("socketfd 出错");
                    return false;
                }
                
                pos+=writesize;
                remainsize-=writesize;
            }
            return true;
         }

//网络通信服务端
         TcpServer::TcpServer() : _listenfd(-1), _clientfd(-1) {}

         bool TcpServer::Initserver(const unsigned int port, const int socketqueue )
         {
            
             // 1. 创建套接字
             _listenfd = socket(AF_INET, SOCK_STREAM, 0);
             if (_listenfd == -1)
             {
                 perror("socket failed");
                 return false;
             }

             signal(SIGPIPE, SIG_IGN); // 传统方式
             //作用:保证服务端口处于TIME_WAIT 状态,客户端也能迅速连接上------重用端口。
             int optval = 1;
             setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

             // 2. 绑定 IP + 端口
             memset(&_server_addr, 0, sizeof(_server_addr));
             _server_addr.sin_family = AF_INET;
             _server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
             _server_addr.sin_port = htons(port); //16位主机转网络

             int retbind=bind(_listenfd,(sockaddr*)&_server_addr,sizeof(_server_addr));
             if (retbind == -1)
             {
                 perror("bind failed");
                 close(_listenfd);
                 return false;
             }
             // 3. 进入监听状态（允许最多 5 个客户端排队）
             if (listen(_listenfd, socketqueue) == -1)
             { // 检查是否出错
                 perror("listen failed");
                 close(_listenfd);
                return false;
             }
             return true;
         }

         bool TcpServer::Accept()
         {
             if (_listenfd == -1)
             {
                 perror("socket failed");
                 return false;
             }
             memset(&_client_addr,0,sizeof(_client_addr));
             socklen_t client_len = sizeof(_client_addr);
             _clientfd = accept(_listenfd, (struct sockaddr *)&_client_addr, &client_len);
             if (_clientfd <0)
             {
                 perror("accept failed");
                 return false;
             }
             return true;
         }

        std::string TcpServer::GetClientIp()
        {
            if (_listenfd == -1)
             {
                 perror("socket failed");
                 return "";
             }
             char buf[INET_ADDRSTRLEN];
             const char*ret=inet_ntop(AF_INET, &_client_addr.sin_addr,buf, sizeof(buf));//16字节
             return ret==NULL?"":std::string(buf);
        }

        bool TcpServer::Read(std::string &buffer, const int timeout )                // 接收文本数据。
        {
             // 读的消息放在buffer,不粘包
            if (_clientfd == -1)
            {
                perror("client socket failed");
                return false;
            }
            // 设置poll等待
            struct pollfd pollfds;
            pollfds.fd = _clientfd;
            pollfds.events = POLLIN;

            // 转换超时时间为毫秒
            //poll 的 timeout 参数为负数时会无限等待
            int timeout_ms = timeout * 1000;
            int ret = poll(&pollfds, 1, timeout_ms);

            if (ret == 0)
            {
                // 超时不是错误，不需要perror
                //std::cerr << "Server read timeout" << std::endl;
                return false;
            }
            else if (ret < 0)
            {
                perror("poll error");
                return false;
            }
            uint32_t headmessage = 0;
            if (!ReadByByte(_clientfd, (char *)&headmessage, 4))
            {
                std::cerr << "Failed to read message header" << std::endl;
                return false;
            }
            headmessage = ntohl(headmessage); // 32位网络转主机
            buffer.resize(headmessage);
            if (!ReadByByte(_clientfd, &buffer[0], headmessage))
            {
                std::cerr << "Failed to read message body" << std::endl;
                return false;
            }
            return true;
        }
        bool TcpServer::Read(void *buffer, const int bufferlenth, const int timeout) // 接收二进制数据。
        {
            if (_clientfd == -1)
            {
                perror("client socket failed");
                return false;
            }
            // 设置poll等待
            struct pollfd pollfds;
            pollfds.fd = _clientfd;
            pollfds.events = POLLIN;

            // 转换超时时间为毫秒
            //poll 的 timeout 参数为负数时会无限等待
            int timeout_ms = timeout * 1000;
            int ret = poll(&pollfds, 1, timeout_ms);

            if (ret == 0)
            {
                // 超时不是错误，不需要perror
                std::cerr << "Server read timeout" << std::endl;
                return false;
            }
            else if (ret < 0)
            {
                perror("poll error");
                return false;
            }
             if (!ReadByByte(_clientfd, (char*)buffer, bufferlenth))
            {
                std::cerr << "Failed to read message " << std::endl;
                return false;
            }
            return true;
        }

        bool TcpServer::Write(const std::string &buffer)          // 发送文本数据。
        {
             if (_clientfd == -1)
            {
                perror("client socket failed");
                return false;
            }
             uint32_t headmessage = buffer.size();
             headmessage = htonl(headmessage); // 主机转网络
            if (!WriteByByte(_clientfd, (char *)&headmessage, 4))
            {
                std::cerr << "Failed to write message header" << std::endl;
                return false;
            }
            //.c_str()是const char*
            if (!WriteByByte(_clientfd,buffer.c_str(), buffer.size()))
            {
                std::cerr << "Failed to write message body" << std::endl;
                return false;
            }
            return true;
        }
        bool TcpServer::Write(const void *buffer, const int bufferlenth)// 发送二进制数据。
        {
             if (_clientfd == -1)
            {
                perror("server socket failed");
                return false;
            }
           if (!WriteByByte(_clientfd,(char*)buffer, bufferlenth))
            {
                std::cerr << "Failed to write message body" << std::endl;
                return false;
            }
            return true;
        }

        void TcpServer::CloseListen()
        {
            close(_listenfd);
        }
        void TcpServer::CloseClient()
        {
            close(_clientfd);
        }

        TcpServer::~TcpServer()// 析构函数自动关闭socket，释放资源。       
        {
            CloseListen();
            CloseClient();
        }

        void ignore_all_signals()
        {
            for (int sig = 1; sig < NSIG; sig++)
            {
                signal(sig, SIG_IGN); // 设置为忽略
            }
        }


 ///////////////////////////////////////////////////////////////////////////////////////
 //信号量
          SystemVSem::SystemVSem():_semid(-1){}

        // 如果信号量已存在，获取信号量；如果信号量不存在，则创建它并初始化为value。
        // 如果用于互斥锁，value填1，sem_flg填SEM_UNDO。
        // 如果用于生产消费者模型，value填0，sem_flg填0。
          bool SystemVSem::init(key_t key, unsigned short value , short sem_flg )
          {
                _sem_flg=sem_flg;
              // 单纯在信号量集中找一个信号量
              if ((_semid = semget(key, 1, 0666))== -1)
              {
                  if (errno == ENOENT)
                  {
                      // 单纯创建,有则返回-1
                      if ((_semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666) )== -1)
                      {
                          // 创建失败-----可能别的进程创建成功
                          if (errno == EEXIST)
                          {

                              if ((_semid = semget(key, 1, 0666) )== -1)
                              {
                                  std::cout << "信号量初始化失败-2" << std::endl;
                                  return false;
                              }
                              return true;
                          }
                          else
                          {
                              std::cout << "信号量初始化失败-3" << std::endl;
                              return false;
                          }
                      }
                      else
                      {
                          // 创建成功-----初始化value
                          union semun arg;
                          arg.val = value; // 设置初始值
                          // 从信号集的第0个信号找该信号,设置value
                          if (semctl(_semid, 0, SETVAL, arg) == -1)
                          {
                              perror("semctl SETVAL failed");
                              return false;
                          }
                          return true;
                      }
                  }
                  else
                  {
                      std::cout << "信号量初始化失败-1" << std::endl;
                      return false;
                  }
              }
              return true;
          }
         // 信号量的P操作，如果信号量的值是0，将阻塞等待，直到信号量的值大于0。
        bool SystemVSem::wait(short value )
        {
            if (_semid == -1)
            {
                return false;
            }
            struct sembuf op;
            op.sem_num = 0;        // 信号量编号  对那个信号量操作
            op.sem_op = value;     // P操作（等待）
            op.sem_flg = _sem_flg; // 进程崩溃时自动恢复

            // 1:表示执行一次操作数
            if (semop(_semid,&op, 1) == -1)
            {
                perror("wait failed");
                return false;
            }
            return true;
        }
        bool SystemVSem::post(short value ) // 信号量的V操作。
        {
            if (_semid == -1)
            {
                return false;
            }
            struct sembuf op;
            op.sem_num = 0;          // 信号量编号  对那个信号量操作
            op.sem_op = value;    // P操作（等待）
            op.sem_flg = _sem_flg; // 进程崩溃时自动恢复

            // 1:表示执行一次操作数
            if (semop(_semid,&op, 1) == -1)
            {
                perror("post failed");
                return false;
            }
            return true;
        }
        int SystemVSem::getValue()             // 获取信号量的值，成功返回信号量的值，失败返回-1。
        {
            if (_semid == -1)
            {
                return -1;
            }
             return semctl(_semid, 0, GETVAL);
        }
        int SystemVSem::getSemid()
        {
            return _semid;
        }
        bool SystemVSem::remove()          
        {
            if (_semid == -1)
            {
                return false;
            }
            if (semctl(_semid, 0, IPC_RMID) == -1) 
            {
                perror("SystemVSem remove failed");
                return false;
            }
            return true;
        }
        
        SystemVSem::~SystemVSem()
        {}      //不能销毁信号量


//////////////////
//进程心跳
        HeartbeatController::HeartbeatController() // 初始化成员变量。
        {
            shm_ptr = nullptr;
            pos = -1;
            shmid = -1; // 共享内存标识符
        }

        // 把当前进程的信息加入共享内存进程组中。
        bool HeartbeatController::AddProcessInfoToShm(const int timeout, const std::string &processname)
        {
            if (pos == -1)
            {
                return false;
            }
            if (sem.init((key_t)SEMKEY) == false)
            {
                std::cout << "信号量 key:%d 初始化失败!"<< std::endl;
                return false;
            }
            if ((shmid = shmget((key_t)SHMKEY, MAXNUM * sizeof(ProcessHeartbeat), IPC_CREAT | 0666)) == -1)
            {
                std::cout << "创建/获取共享内存失败!"<< std::endl;
                return false;
            }
            // 将共享内存加到进程地址空间
            shm_ptr = (ProcessHeartbeat *)shmat(shmid, NULL, 0);
            if (shm_ptr == (void *)-1)
            {
                std::cout << "shmat 失败"<< std::endl;
                return false;
            }

            ProcessHeartbeat pht(getpid(), processname, timeout, time(NULL));
            // 找到空位置,放进心跳
            sem.wait();
            for (int i = 0; i < MAXNUM; i++)
            {
                if (pht.processpid == shm_ptr[i].processpid)
                {
                    pos = i;
                    printf("找到旧位置 %d\n", pos);
                    break;
                }
            }
            if (pos == -1)
            {
                for (int i = 0; i < MAXNUM; i++)
                {
                    if (shm_ptr[i].processpid == 0)
                    {
                        pos = i;
                        printf("找到新位置 %d\n", pos);
                        break;
                    }
                }
            }
            if (pos == -1)
            {
                // 没找到---内存满了
                printf("共享内存用完了,shmid: %d\n", shmid);
                sem.post();
                return false;
            }
            memcpy(shm_ptr + pos, &pht, sizeof(pht));
            sem.post();
            return true;
        }

        // 更新共享内存进程组中当前进程的心跳时间。
        bool HeartbeatController::UpRecenttime()
        {
            if (pos == -1)
            {
                return false;
            }
            sem.wait();
            shm_ptr[pos].recenttime = time(NULL);
            sem.post();
            return true;
        }
          //不能删除共享内存
        HeartbeatController::~HeartbeatController() // 从共享内存中删除当前进程的心跳记录。
        {
            if (pos != -1)
            {
                sem.wait();
                memset(&shm_ptr[pos], 0, sizeof(ProcessHeartbeat));
                sem.post();
            }
            // 分离共享内存
            if (shm_ptr != nullptr)
            {
                shmdt(shm_ptr);
            }
        }
        bool SetFileModTime(const std::string &filename, std::string &new_mtime)
        {
            // 1. 将字符串时间转换为 time_t
            time_t new_time = strTotime(new_mtime);
            if (new_time == -1)
            {
                return false; // 时间转换失败
            }
            // 2. 使用更高精度的 utimensat (Linux/macOS)
            struct timespec times[2] = {
                {new_time, 0}, // 访问时间
                {new_time, 0}  // 修改时间
            };

            return utimensat(AT_FDCWD, filename.c_str(), times, 0) == 0;
        }

        bool GetFileModTime(const std::string &filename, std::string &mtime)
        {
            struct stat filestruct;
            if (stat(filename.c_str(), &filestruct) == -1)
            {
                return false;
            }
            timeTostr(filestruct.st_mtim.tv_sec, mtime, "yyyymmddhh24miss");
            return true;
           
        }

        bool GetFileSize(const std::string &filename,unsigned int& size)
        {
            struct stat filestat;
            if (stat(filename.c_str(), &filestat) == 0)
            {
                size=filestat.st_size; 
                return true;
            }
            return false; // 失败
        }
};