//增加心跳机制,保证短连接不会因为超时断开
#include"../common/common.h"
idc::TcpClient tcpclient;
std::string recvmessage;
std::string sendmessage;

bool heartbeat(const std::string &timeout);
bool login(const std::string &timeout);
bool query(const std::string &timeout);
bool transfer(const std::string &timeout);
int main(int argc,char*argv[])
{
    if(argc!=4)
    {
       printf("/home/ubuntu/bilibili/BigProjrct/test_tcp/tcp_client 127.0.0.1 8080 10\n");
       return -1;
    }
    if(tcpclient.Connect(argv[1],std::stoi(argv[2]))==false)
    {
        std::cout<<"客户端tcp connect失败\n"<<std::endl;
        return -1;      
    }
    login(argv[3]);
    query(argv[3]);

    sleep(6);
    heartbeat(argv[3]);
    sleep(6);

    transfer(argv[3]);
    //这里建议将所有接受消息统一处理,根据serverid区分响应
    tcpclient.Close();
    return 0;
    
}
bool login(const std::string &timeout)
{
    sendmessage="<serverid>0</serverid><usercount>1111</usercount><password>1234</password>";
    if(tcpclient.Write(sendmessage)==false)
    {
        std::cout<<"客户端登录消息发送失败\n"<<std::endl;
        return false;
    }
    recvmessage.clear();
    if(tcpclient.Read(recvmessage,std::stoi(timeout))==false)
    {
       std::cout<<"客户端登录消息接受失败\n"<<std::endl;
        return false;
    }
     std::cout<<"接收了:"<<recvmessage<<std::endl;
     return true;
}
bool query(const std::string &timeout)
{
     sendmessage="<serverid>1</serverid><usercount>1111</usercount>";
      if(tcpclient.Write(sendmessage)==false)
    {
        std::cout<<"客户端查询消息发送失败\n"<<std::endl;
        return false;
    }
    recvmessage.clear();
    if(tcpclient.Read(recvmessage,std::stoi(timeout))==false)
    {
       std::cout<<"客户端查询消息接受失败\n"<<std::endl;
        return false;
    }
     std::cout<<"接收了:"<<recvmessage<<std::endl;
      return true;


}
bool transfer(const std::string &timeout)
{
    sendmessage="<serverid>2</serverid><usercount>1111</usercount><othercount>2222</othercount><password>1234</password>\
<amount>100</amount>";
    if(tcpclient.Write(sendmessage)==false)
    {
        std::cout<<"客户端转账消息发送失败\n"<<std::endl;
        return false;
    }
    recvmessage.clear();
    if(tcpclient.Read(recvmessage,std::stoi(timeout))==false)
    {
       std::cout<<"客户端转账消息接受失败\n"<<std::endl;
        return false;
    }
     std::cout<<"接收了:"<<recvmessage<<std::endl;
      return true;
}
bool heartbeat(const std::string &timeout)
{
    sendmessage="<serverid>-1</serverid>";
    if(tcpclient.Write(sendmessage)==false)
    {
        std::cout<<"客户端心跳消息发送失败\n"<<std::endl;
        return false;
    }
    recvmessage.clear();
    if(tcpclient.Read(recvmessage,std::stoi(timeout))==false)
    {
       std::cout<<"客户端心跳消息接受失败\n"<<std::endl;
        return false;
    }
     std::cout<<"接收了:"<<recvmessage<<std::endl;
      return true;
}