#!/usr/bin/env python
# -*- coding: utf-8 -*- 

import rospy
import numpy as np
from std_msgs.msg import String

def callback(control_data):
    
    servo_motor_pub = rospy.Publisher('servo_motor_info', String, queue_size=10)  # 发布的话题：'image_raw'

    servo_motor_order = control_data.data    # 获取选择指令
    switch.get(servo_motor_order, default)()            # 执行对应的函数，如果没有就执行默认的函数


    servo_motor_state = control_data.data

    servo_motor_pub.publish(servo_motor_state)  # 发布电机运行状态
 
    rospy.loginfo(servo_motor_state)
 

def control_listener():
    rospy.init_node('control_listener', anonymous=True)  # 创建节点的名称：'webcam_display'

    rospy.Subscriber('wheelchair_bed_control', String, callback)  #  订阅的话题： 'image_raw0' 
    rospy.spin()

def left():                            # 第一种情况执行的函数
    print('This is the case1')
def right():                            # 第二种情况执行的函数
    print('This is the case2')
def up():                            # 第三种情况执行的函数
    print('This is the case3')
def down():                            # 第三种情况执行的函数
    print('This is the case3')
def start():                            # 第三种情况执行的函数
    print('This is the case3')
def stop():                            # 第三种情况执行的函数
    print('This is the case3')
def default():                          # 默认情况下执行的函数
    print('No such case')


switch = {'start': start,                # 注意此处不要加括号
          'stop': stop,                # 注意此处不要加括号
          'up': up,                # 注意此处不要加括号
          'down': down,                # 注意此处不要加括号
          'left': left,                # 注意此处不要加括号
          'right': right,                # 注意此处不要加括号
          }




if __name__ == '__main__':
    control_listener()
