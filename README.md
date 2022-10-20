git clone或直接下载本文件夹
1. 安装依赖
sudo apt install ros-noetic-serial
2. 在本文件夹内，编译，catkin_make，编译报权限错误，可在上级文件夹内sudo chmod 777 -R catkin_test赋予权限
3. 创建文件:  
    sudo gedit /etc/udev/rules.d/70-ttyusb.rules  
    在打开的文件中添加  
    KERNEL=="ttyUSB[0-9]*", MODE="0666"  
    创建文件:  
    sudo gedit /etc/udev/rules.d/70-ttyACM.rules  
    在打开的文件中添加  
    KERNEL=="ttyACM[0-9]*", MODE="0666"

4.  * 单独测试IMU  
        (IMU在装设备的盒子里，没有安装在车上，外形为一小块黑色长方体，上有文字wheel,插着一小段白色USB连接线)
        1. 连接IMU到电脑，终端输入  
            ls /dev/ttyACM*  
            应有一个ttyACM0，若端口为其他名称，则需要在src/fdilink_ahrs/launch/ahrs_driver.launch中更改“ param name="port"  value="/dev/ttyACM0" ”为对应的端口名称。  
            端口名称可以通过连接前后ls /dev/*输出变化获得。
        2. source devel/setup.bash  
        3. roslaunch fdilink_ahrs ahrs_driver.launch
        4. 新建一个终端，rostopic list查看是否有/imu/data话题输出，可通过rostopic echo /imu/data实时监测IMU输出数值
        5. 晃动IMU，查看输出变化
        6. 静置1小时左右，观察长时间输出结果是由发生变化，以及检验串口通讯稳定性。可再次新建终端，rosbag record /imu/data记录输出（非必须）。 

    * 单独测试RTK  
        1. 必须在室外，较为空旷环境，开启RTK,等待语音提示，或通过RTK屏幕观察搜星情况
        2. 搜星结束，连接电脑与RTK，终端输入
            ls /dev/ttyUSB*  
            应有一个ttyUSB0，若端口为其他名称，则需要在src/serial_rec/launch/serial.launch中更改“ param name="Port" value="/dev/ttyUSB0" ”为对应的端口名称。  
            端口名称可以通过连接前后ls /dev/*输出变化获得。
        3. source devel/setup.bash
        4. roslaunch serial_rec serial.launch
        5. 新建一个终端，rostopic list查看是否有/gps话题输出，可通过 rostopic echo /gps 实时监测RTK输出
        6. 再新建终端，rosbag record /gps，录制gps数据，可以尝试移动RTK，以及观察静置下输出是否存在漂移，应至少记录半个小时以上
        7. 若驱动在运行时报错，或gps无输出，请及时与我联系。遮挡RTK顶部天线，输出值全为0，属正常现象。

    * 同时开启RTK和IMU  
        sudo ./record.sh
