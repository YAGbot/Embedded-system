#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <sstream>

using namespace std;

std_msgs::Bool flag;
ros::Publisher pub;

void socket_server()
{
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
    int fd = accept(socket_fd,(struct sockaddr*)&client, &len);
    if(fd == -1)
    {
        cout<<"aceept fiald!"<<endl;
        exit(-1);
    }

    //6.使用第5步返回sockt描述符，进行读写通信。
    char *ip = inet_ntoa(client.sin_addr);
    cout<<"client ["<<ip<<"] connect !"<<endl;

    char buffer[255] = {};
    //第一个参数：accept 返回的文件描述符
    //第二个参数：存放读取的内容
    //第三个参数：内容的大小
    int size = read(fd,buffer,sizeof(buffer));

    cout<<"accept bet:"<<size<<endl;
    cout<<"cantent:"<<buffer<<endl;
    write(fd,"welcome",7);

    if(buffer);
    {
        flag.data = 1;
        pub.publish(flag);
    }

    //7.关闭sockfd。
    close(fd);
    close(socket_fd);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "star_navigation");
    ros::NodeHandle n;
    //准备发布器为服务器接收到消息发布开始命令做准备
    pub = n.advertise<std_msgs::Bool>("navigation_star",100);
    socket_server();
    return 0;
}
