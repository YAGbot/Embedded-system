#!/usr/bin/env python
#coding=utf-8

import rospy
from wheelchair_bed_msgs.msg import dht 
#from std_msgs.msg import String

import socket
import threading
import time
import sys
from time import ctime
import json, types,string




def dht_talker():

    pub = rospy.Publisher('dht_chatter', dht, queue_size=10)
    rospy.init_node('dht_talker', anonymous=True)
    rate = rospy.Rate(10) # 10hz

    while not rospy.is_shutdown():
        
        temp = 11
        humi = 12

        rospy.loginfo(dht(temp,humi,rospy.Time.now()))
        pub.publish(dht(temp,humi,rospy.Time.now()))
        rate.sleep()

if __name__ == '__main__':
    try:
        dht_talker()
    except rospy.ROSInterruptException:
        pass
        






 

 
 
