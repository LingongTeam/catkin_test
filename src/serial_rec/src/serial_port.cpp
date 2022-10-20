//serial_port.cpp
#include <ros/ros.h>
#include <serial/serial.h>
#include <iostream>
#include <std_msgs/String.h>
#include <sensor_msgs/NavSatFix.h>
#include <string>
#include <sstream>
#include <math.h>
#include <exception>  
using namespace std;
int main(int argc, char** argv)
{
    ros::init(argc, argv, "serial_port");
    //创建句柄（虽然后面没用到这个句柄，但如果不创建，运行时进程会出错）
    ros::NodeHandle n("~");
    //
    //ros::Publisher gps_pub = n.advertise<std_msgs::String>("/gps", 1);
    ros::Publisher gps_pub = n.advertise<sensor_msgs::NavSatFix>("/gps", 1);
    //创建一个serial类
    serial::Serial sp;
    //创建timeout
    // serial::Timeout to = serial::Timeout::simpleTimeout(100);
    serial::Timeout to = serial::Timeout::simpleTimeout(serial::Timeout::max());
    //设置要打开的串口名称
    string port_num;
    int Baudrate_;
    // n("~");
    n.param("Port", port_num, std::string("/dev/ttyUSB0"));
    // cout<< port_num << endl;
    n.param("Baudrate", Baudrate_, 115200);
    sp.setPort(port_num);
    //设置串口通信的波特率
    sp.setBaudrate(Baudrate_);
    //串口设置timeout
    sp.setTimeout(to);
 
    try
    {
        //打开串口
        sp.open();
    }
    catch(serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port.");
        return -1;
    }
    
    //判断串口是否打开成功
    if(sp.isOpen())
    {
        ROS_INFO_STREAM(port_num + " is opened.");
    }
    else
    {
        return -1;
    }
    // 发布频率
    ros::Rate loop_rate(10);

    while(ros::ok())
    {
    	sensor_msgs::NavSatFix gps_data;
    	gps_data.header.stamp = ros::Time::now();
    	gps_data.header.frame_id = "base_link";
    	//gps_data.status="working";
        //获取缓冲区内的字节数
        size_t n = sp.available();
        if(n!=0)
        {
            try{
                uint8_t buffer[1024];
                //读出数据
                n = sp.read(buffer, n);
                
                //for(int i=0; i<n; i++)
                //{
                    //16进制的方式打印到屏幕
                // std::cout << std::hex << (buffer[i] & 0xff) << " ";
                //   std::cout<<buffer[i] ;
                //}
                //std::cout << std::endl;
                //zaoyin
                //std_msgs::String gps_msg;
                //gps_msg.data = gps_real.str();
                //gps_pub.publish(gps_msg);
                stringstream gps_real;
                gps_real << buffer;
                string gps_tmp;
                string gps_msg[30];
                int i=0;
                while(getline(gps_real,gps_tmp,',')){
                    gps_msg[i]=gps_tmp;
                    i++;
                }
                if(gps_msg[6] == "1" ||  gps_msg[6] == "2"){
                    gps_data.latitude=double(floor(stold(gps_msg[2])/100.0)) + double(stold(gps_msg[2]) - floor(stold(gps_msg[2])/100.0) * 100)/60.0;
                    gps_data.longitude=double(floor(stold(gps_msg[4])/100.0)) + double(stold(gps_msg[4]) - floor(stold(gps_msg[4])/100.0) * 100)/60.0;
                    gps_data.altitude=stold(gps_msg[9]);
                    gps_msg[6] = "0";
                }
            }catch (...) {
                sensor_msgs::NavSatFix gps_data1;
                gps_pub.publish(gps_data1);
                continue;
                // cout << "catch (...)" << endl;
            }
            gps_pub.publish(gps_data);
        }
        loop_rate.sleep();
    }
    
    //关闭串口
    sp.close();
 
    return 0;
}

