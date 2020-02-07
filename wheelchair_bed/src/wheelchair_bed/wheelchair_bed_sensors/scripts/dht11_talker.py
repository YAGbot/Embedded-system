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
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(('192.168.1.103', 8086))
        s.listen(10)
    except socket.error as msg:
        print(msg)
        sys.exit(1)
    print('Waiting connection...')

    pub = rospy.Publisher('dht_chatter', dht, queue_size=10)
    rospy.init_node('dht_talker', anonymous=True)
    rate = rospy.Rate(10) # 10hz

    while not rospy.is_shutdown():
        conn, addr = s.accept()
        data = conn.recv(512)
        nPos = data.index('{')
        mPos = data.index('}')
        if nPos < mPos:
            data = data[nPos:mPos+1]        
#        print('{0}'.format(data.decode()))#b'\xe8\xbf\x99\xe6\xac\xa1\xe5\x8f\xaf\xe4\xbb\xa5\xe4\xba\x86'
        text = json.loads(data)
        temp = text["temp"]
        humi = text["humi"]
#        print temp,humi
        if data == 'exit' or not data:
            print('{0} connection close'.format(addr))
            conn.send(('Connection closed!').encode())
            break
#        conn.send(bytes('Hello, {0}'.format('data'),"UTF-8"))#TypeError: a bytes-like object is required, not 'str'
#        conn.send(('%s &' %(u1)).encode())

        #这里test就像构造函数般使用，若有多个msg，那么写成test(a,b,c,d...)
        rospy.loginfo(dht(temp,humi,time.time()))
        pub.publish(dht(temp,humi,time.time()))
        rate.sleep()
        time.sleep(1)
        conn.close()

if __name__ == '__main__':
    try:
        dht_talker()
    except rospy.ROSInterruptException:
        pass
        






 

 
 
