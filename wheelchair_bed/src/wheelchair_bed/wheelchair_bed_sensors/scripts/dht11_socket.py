#!/usr/bin/env python
# -*- coding: utf-8 -*-  

import socket
import threading
import time
import sys
from time import ctime
import json, types,string
 
def socket_service():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(('192.168.1.103', 8086))
        s.listen(10)
    except socket.error as msg:
        print(msg)
        sys.exit(1)
    print('Waiting connection...')
 
    while 1:
        conn, addr = s.accept()
        t = threading.Thread(target=deal_data, args=(conn, addr))
        t.start()
 
def deal_data(conn, addr):
    print('Accept new connection from {0}'.format(addr))
    conn.send(('Hi, Welcome to the server!').encode())
    while 1:
        data = conn.recv(512)
        if data == 'exit' or not data:
            print('{0} connection close'.format(addr))
            conn.send(('Connection closed!').encode())
            break
        nPos = data.find('{')
        mPos = data.find('}')
        if nPos >= 0:
            data = data[nPos:mPos+1]        
            text = json.loads(data)
            temp = text["temp"]
            humi = text["humi"]
        else:
            temp = temp
            humi = humi
        print temp,humi,time.time()
        time.sleep(1)       
#        conn.send(bytes('Hello, {0}'.format('data'),"UTF-8"))#TypeError: a bytes-like object is required, not 'str'
#        conn.send(('%s &' %(u1)).encode())
    conn.close()
 
 
if __name__ == '__main__':
    socket_service()