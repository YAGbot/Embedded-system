#!/usr/bin/env python
# -*- coding: utf-8 -*- 

import rospy
import numpy as np
from std_msgs.msg import String

import ctypes
from ctypes import *
import threading
import time
import sys
from time import ctime

#CAN设备类型
ACUSB_131B = 1   #ACUSB_131B 接口卡设备
ACUSB_132B = 2
ACPCI_251 = 3
ACPCI_252 = 4
ACPCI_254 = 5

ACNET_600 = 6
ACNET_622 = 7

LCPCIE_251 = 8
LCPCIE_252 = 9

LCPCIE_361 = 10
LCPCIE_362 = 11
LCPCIE_364 = 12

LCUSB_131B = 1
LCUSB_132B = 2
LCMiniPcie_431 = 1
LCMiniPcie_432 = 2
LCPCI_252 = 4

USBCAN_1CH = 13
USBCAN_C_1CH = 14
USBCAN_E_1CH = 15
USBCAN_E_2CH = 16
MPCIeCAN_1CH = 17
MPCIeCAN_2CH = 18

#CAN_RESULT_OK 表示启动成功,CAN_RESULT_ERROR 表示启动失败
CAN_RESULT_OK = 1
CAN_RESULT_ERROR = 0

#CAN错误码
class CAN_ErrorCode():
  CAN_E_NOERROR = 0x0000,   # 没有发现错误
  CAN_E_OVERFLOW = 0x0001,   # CAN控制器内部 FIFO 溢出
  CAN_E_ERRORALARM = 0x0002,   # CAN 控制器错误报警
  CAN_E_PASSIVE = 0x0004,   # CAN 控制器消极错误
  CAN_E_LOSE = 0x0008,   # CAN 控制器仲裁丢失
  CAN_E_BUSERROR = 0x0010,   # CAN 控制器总线错误

  CAN_E_RCV_BUF = 0x0020,   # dll CAN����BUF����
  CAN_E_ERR_BUF = 0x0040,   # dll CAN������ϢBUF����

  CAN_E_DEVICEOPENED = 0x0100,	# 设备打开
  CAN_E_DEVICEOPEN = 0x0200,	# 打开设备错误
  CAN_E_DEVICENOTOPEN = 0x0400,	# 设备没有打开
  CAN_E_BUFFEROVERFLOW = 0x0800,	# 缓冲区溢出
  CAN_E_DEVICENOTEXIST = 0x1000,	# 此设备不存在
  CAN_E_LOADKERNELDLL = 0x2000,	# 装载动态库失败
  CAN_E_CMDFAILED = 0x4000,	#执行命令失败错误码
  CAN_E_BUFFERCREATE = 0x8000,	# 内存不足

#print(CAN_ErrorCode.CAN_E_ERR_BUF)
# 
class ParamType():
  PARAM_WAKEUP_CFG  = 0x4000,      # ���û������ò���
# 帧唤醒
class tagWakeup_Config(Structure):
  _fields_ = [("dwAccCode", c_uint),
  ("dwAccMask", c_uint),
  ("nFilter", c_uint)
  ]

#//CAN 数据帧类型
class tagCAN_DataFrame(Structure):
  _fields_ = [("uTimeFlag", c_uint),    # 时间标识,对接收帧有效,从 CAN 通道启动开始计时
  ("nSendType", c_ubyte),               #发送帧类型,0-正常发送;1-单次发送;2-自发自收;3-单次自发自收
  ("bRemoteFlag", c_ubyte),               #是否是远程帧,0 表示数据帧,1 表示远程帧
  ("bExternFlag", c_ubyte),               #是否是扩展帧,0 表示标准帧,1 表示扩展帧
  ("nDataLen", c_ubyte),               #数据长度(<=8),也就是 arryData 的长度
  ("uID", c_uint),               #报文 DI
  ("arryData", c_ubyte*8)               #报文数据
  ]

#CAN 初始化配置
class tagCAN_InitConfig(Structure):
  _fields_ = [("bMode", c_ubyte),  #模式,0 表示正常模式,1 表示只听模式
  ("nBtrType", c_ubyte),           #位定时参数模式,0 表示 LPC21XX ,1 表示 SJA1000
  ("dwBtr", c_ubyte*4),           #位定时参数, dwBtr[0]表示 BTR0,dwBtr[1]表示 BTR1
  ("dwAccCode", c_uint),           #验收码
  ("dwAccMask", c_uint),           #屏蔽码
  ("nFilter", c_uint),           #// 滤波方式(0 表示未设置滤波,1 表示双滤波,2 表示单滤波)
  ("dwReserved", c_ubyte)           #预留字段
  ]

#CAN设备信息
class tagCAN_DeviceInformation(Structure):
  _fields_ = [("uHardWareVersion", c_ushort),  #硬件版本,用十六进制表示,比如 0x0100 表示 V1.00
  ("uFirmWareVersion", c_ushort),           #固件版本,用十六进制表示
  ("uDriverVersion", c_ushort),          #驱动版本,用十六进制表示
  ("uInterfaceVersion", c_ushort),          #接口库版本,用十六进制表示
  ("uInterruptNumber", c_ushort),          #中断号
  ("bChannelNumber", c_ubyte),          #设备总共有几路 CAN
  ("szSerialNumber", c_byte*20),          #设备序列号
  ("szHardWareType", c_byte*40),          #硬件类型
  ("szDescription", c_byte*20)          #设备描述
  ]

#CAN 错误信息
class tagCAN_ErrorInformation(Structure):
  _fields_ = [("uErrorCode", c_uint),      #// 错误码,详情请参见 CAN_ErrorCode 定义
  ("PassiveErrData", c_ubyte*3),           #当错误码为消极错误时,表示消极错误数据
  ("ArLostErrData", c_ubyte)              #当错误码为仲裁错误时,表示仲裁错误数据
  ]

class RCV_THREAD_ARG(Structure):
  _fields_ = [("Run", c_uint),
#      ("ch", DWORD)
    ("ch", c_ulong)
    ]
class SED_THREAD_ARG(Structure):
  _fields_ = [("ch", c_ulong),
#      ("ch", DWORD)
    ("sndType", c_ulong),   #0 - 正常发送;1 - 单 次 发送;2 - 自发自收;3 - 单 次 自发自收
    ("sndFrames", c_ulong), # 每次发送帧数
    ("sndTimes", c_ulong),  # 发送次数
    ]

DEF_DEV_INDEX = 0        # CAN设备索引,从0开始
DEF_USE_CAN_NUM = 1      # 1-使用CAN0；2-使用CAN1；3-同时使用CAN0、CAN1
DEF_SEND_TYPE = 2	    # CAN发送类型,0-正常发送;1-单次发送;2-自发自收;3-单次自发自收
DEF_SEND_FRAMES = 4      # 每次发送帧数
DEF_SEND_TIMES = 10000  # 发送次数
DEF_SEND_DELY = 5      # 发送前延时,单位秒

#发送统计
CanSendcount=[0,0]
# 定义函数
def exit():
  print("CAN_ChannelStop 0\r\n")
  canDLL.CAN_ChannelStop(dwDeviceHandle, 0)

  canDLL.CAN_DeviceClose(dwDeviceHandle)
  print("CAN_DeviceClose\r\n")
  print("Press Enter key to exit the program\r\n")

###########################
######  CAN总线操作  #######
###########################
# load the some.so 
CanDLLName = './dll/libCanCmd.so'
canDLL = cdll.LoadLibrary(CanDLLName)
print(CanDLLName)
print(canDLL)

# 打开设备
dwDeviceHandle = canDLL.CAN_DeviceOpen(LCMiniPcie_432, DEF_DEV_INDEX, 0)
print(dwDeviceHandle)
if dwDeviceHandle == CAN_RESULT_ERROR:
  print('CAN_DeviceOpen error\r\n')
  exit()
else:
  print('CAN_DeviceOpen success\r\n')

#启动CAN通道
ubyte_array = c_ubyte*4
Btr = ubyte_array(0x00,0x1c,0,0)
can_initconfig = tagCAN_InitConfig(0, 1, Btr,
                 0, 0xffffffff, 0)
ret = canDLL.CAN_ChannelStart(dwDeviceHandle, DEF_DEV_INDEX, byref(can_initconfig))

#config.dwAccCode = 0;
#config.dwAccMask = 0xffffffff;
#config.nFilter  = 0;       // 滤波方式(0表示未设置滤波功能,1表示双滤波,2表示单滤波)
#config.bMode    = 0;             // 工作模式(0表示正常模式,1表示只听模式)
#config.nBtrType = 1;      // 位定时参数模式(1表示SJA1000,0表示LPC21XX)
#config.dwBtr[0] = 0x00;   // BTR0   0014 -1M 0016-800K 001C-500K 011C-250K 031C-12K 041C-100K 091C-50K 181C-20K 311C-10K BFFF-5K
#config.dwBtr[1] = 0x1c;   // BTR1
#config.dwBtr[2] = 0;
#config.dwBtr[3] = 0;

if ret != CAN_RESULT_OK:
    print('调用 CAN_ChannelStart 出错\r\n')
    exit()
else:
    if (DEF_USE_CAN_NUM & 1) == 1:
    #标准帧唤醒
        wakeup_config = tagWakeup_Config(0x123 << 21,0x001FFFFF, 2)
        #wakeup_config.dwAccCode = (0x123 << 21);  // 帧ID为0x123的标准帧 唤醒
        #wakeup_config.dwAccMask = 0x001FFFFF;     // 固定
        #wakeup_config.nFilter   = 2;              // 滤波方式(0表示未设置滤波功能-收到任意CAN数据都能唤醒,1表示双滤波,2表示单滤波,3-关闭唤醒功能)
        if ( canDLL.CAN_SetParam(dwDeviceHandle, 0, PARAM_WAKEUP_CFG, byref(wakeup_config)) != CAN_RESULT_OK ):
        print("CAN 0 not support Wakeup\n")

    if (DEF_USE_CAN_NUM & 2) == 2:
        #扩展帧唤醒
        wakeup_config = tagWakeup_Config(0x1234 << 3,0x00000007, 2)
        #wakeup_config.dwAccCode = (0x123 << 21);  // 帧ID为0x123的标准帧 唤醒
        #wakeup_config.dwAccMask = 0x001FFFFF;     // 固定
        #wakeup_config.nFilter   = 2;              // 滤波方式(0表示未设置滤波功能-收到任意CAN数据都能唤醒,1表示双滤波,2表示单滤波,3-关闭唤醒功能)
        if ( canDLL.CAN_SetParam(dwDeviceHandle, 1, PARAM_WAKEUP_CFG, byref(wakeup_config)) != CAN_RESULT_OK ):
        print("CAN 1 not support Wakeup\n")

#获取当前设备信息
DevInfo = tagCAN_DeviceInformation()
can_initconfig = canDLL.CAN_GetDeviceInfo(dwDeviceHandle, byref(DevInfo))
if ( can_initconfig != CAN_RESULT_OK ):
  print("GetDeviceInfo error\n")
  exit()
print(" %s\n" %(DevInfo.szDescription))
print("SN: %s\n" %(DevInfo.szSerialNumber))
print("CAN通道数: %d\n" %(DevInfo.bChannelNumber))
print("硬件 版本: %x\n" %(DevInfo.uHardWareVersion))
print("固件 版本: %x\n" %(DevInfo.uFirmWareVersion))
print("驱动 版本: %x\n" %(DevInfo.uDriverVersion))
print("接口库版本: %x\n" %(DevInfo.uInterfaceVersion))
#print('{0} connection close'.format(addr))


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
