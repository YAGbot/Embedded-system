#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <ros/ros.h>
#include <std_msgs/Bool.h>
//这个是我的包msg文件
#include "wheelchair_bed_msgs/dht.h"
#include "std_msgs/String.h"
#include<jsoncpp/json/json.h>
#include<curl/curl.h>
#include<string>
#include<exception>

using namespace std;

wheelchair_bed_msgs::dht dhtmsg;
std_msgs::String msg;

int fd;
int nTemp,nHumi;

void object_detectedCallback(const std_msgs::String::ConstPtr& msg)
{
//此处代码忽略
    ROS_INFO("[%s]",msg->data.c_str());
    char send_buf[255] = {};
    string send_str;
    send_str = msg->data.c_str();

    if (send_str != "")
//    if (send_str.GetLength() != 0)
    {
    send_str = send_str.append("&");
    cout<<"send_str:"<<send_str<<endl;
    int i;
    for( i=0;i<send_str.length();i++)
        send_buf[i] = send_str[i];
    send_buf[i] = '\0';
    printf("%s\n",send_buf);
    cout<<send_buf<<endl;;

    write(fd,send_buf,sizeof(send_buf));
    }
}
 
int main(int argc,char **argv)
{
	ros::init(argc,argv,"sub_and_pub");
    ros::NodeHandle n;


	ros::Subscriber sub = n.subscribe("dht11_listener",1,object_detectedCallback);
    ros::Publisher identify_pub = n.advertise<wheelchair_bed_msgs::dht>("dht11_talker",1000);//锁定helloworld这个topic准备发布消息
    ros::Rate loop_rate(10);

    /*服务端：
    1、socket（）获得一个sockfd。注意第二个参数必须SOCK_STREAM.
    2、准备通信地址（必须服务器的）
    3、bind（）绑定。（开放了端口，允许客户端连接）
    4、监听客户端 listen()函数
    5、等待客户端的连接 accept(),返回用于交互的socket描述符
    6、使用第5步返回sockt描述符，进行读写通信。
    7、关闭sockfd。
    */
    //1.socket（）获得一个sockfd。注意第二个参数必须SOCK_STREAM.
    //第一个参数是地址描述符，常用AF_INET（表示IPV4）
    //第三个参数是套接口所用的协议，不想调用就用0
    int socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd == -1)
    {
        cout<<"socket faild!"<<endl;
        exit(-1);
    }
    //2.准备通讯地址（必须是服务器的）
    //sin_family：协议簇一般用AF_INET（表示IPV4）
    //sin_port: 端口，一般用htons(填端口：数字随便要和服务器的一样)将十进制转为网络进制
    //sin_addr.s_addr:一般用inet_addr(" "填IP地址)或者htonl(INADDR_ANY)直接设置为本地ip
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8086);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //3.bind() 绑定。（开发了端口，允许客服端链接）
    //参数一：0的返回值（socket_fd）
    //参数二：(struct sockaddr*)&addr 前面结构体，即地址
    //参数三: addr结构体的长度
    int res = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if(res == -1)
    {
        cout<<"binf fiald!"<<endl;
        exit(-1);
    }
    cout<<"bind ok"<<endl;
    //4.监听客户端 listen()函数
    //参数二：进程上限，一般小于30
    listen(socket_fd,30);

    //5.等待客户端的连接 accept(),返回用于交互的socket描述符
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    fd = accept(socket_fd,(struct sockaddr*)&client, &len);
    if(fd == -1)
    {
        cout<<"aceept fiald!"<<endl;
        exit(-1);
    }

    while(ros::ok())
    {
        //6.使用第5步返回sockt描述符，进行读写通信。
        char *ip = inet_ntoa(client.sin_addr);
        cout<<"client ["<<ip<<"] connect !"<<endl;

        char reciv_buf[255] = {};                     
        //第一个参数：accept 返回的文件描述符
        //第二个参数：存放读取的内容
        //第三个参数：内容的大小
        int size = read(fd,reciv_buf,sizeof(reciv_buf));

        cout<<"accept bet:"<<size<<endl;
        cout<<"cantent:"<<reciv_buf<<endl;

        if(reciv_buf);
        {
            string strJsonContent(reciv_buf);
            int nPos1,nPos2;
            nPos1 = strJsonContent.find_first_of("{");
            nPos2 = strJsonContent.find_first_of("}");
            if(nPos1 !=-1 && nPos2 != -1)
            {
                strJsonContent = strJsonContent.substr(nPos1 , nPos2 + 1);   
            }
            else
            {
                strJsonContent = strJsonContent;
            }
            std::cout<< "strJsonContent:" << strJsonContent << std::endl;
            replace(strJsonContent.begin(),strJsonContent.end(),' ','\0');
            std::cout<< "strJsonContent:" << strJsonContent << std::endl;
        
//            strJsonContent = "{\"temp\": 1,\"humi\": 2}";
            std::cout<< "strJsonContent:" << strJsonContent << std::endl;

            Json::Value root;
            Json::Reader reader;
            
            if (reader.parse(strJsonContent, root))
            {
                nTemp = root["temp"].asInt();
                nHumi = root["humi"].asInt();
            }
//            const Json::Value nTemp = root["temp"];
//            const Json::Value nHumi = root["humi"];
//          result = text.asString();
//          flag = 1;
 
            std::cout<< "response code:" << nTemp << std::endl;
            std::cout<< "response text:" << nHumi << std::endl;              
        }


        dhtmsg.temp = nTemp;
        dhtmsg.humi = nHumi;
//        dhtmsg.stamp = ros::Time::now();
        ROS_INFO("%f", dhtmsg.temp);
        identify_pub.publish(dhtmsg);//向刚才锁定的helloworld这个topic发布消息
        
        

        ros::spinOnce();//这句就是同时发布节点和订阅节点的关键了
        loop_rate.sleep();
    }
    close(fd);
    close(socket_fd);
	return 0;	
}
