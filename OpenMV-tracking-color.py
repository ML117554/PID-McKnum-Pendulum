# Untitled - By: ML117554 - Fri Oct 13 2023
"""
P4=TX
P5=RX
lcd画幅大小:128x160
"""
import sensor, image, time,lcd
from pyb import UART,LED
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.LCD)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
lcd.init(refresh=120)
clock = time.clock()
#白光(60, 100, -12, 11, -16, 11)        红色块(0, 100, 25, 41, 4, 33)
threshold = [(0, 100, 25, 41, 4, 33)
             ]# Middle L, A, B values.
uart = UART(3, 115200)
led = LED(2)
tansmit_data = "@0*0#"
receive_data = [0,0,0,0,0,0]
blob_xy = [0,0]
fps = 0
#t1 = time.time_ns()
#print((time.time_ns()-t1)/1e6,"ms")


while(True):
    t1 = time.time_ns()
    clock.tick()

    #UART判断接收数据，保证一次循环中发送一次，接收一次
#    if (uart.any()):
#        #velocity_x,velocity_y,move_x,move_y,RX_Frequency,count
#        receive_data = str(uart.read())[2:-1]
#        #判断数据包的完整性
#        temp_list = receive_data[1:-1].split(',')
#        if receive_data[0]=='@' and receive_data[-1]=='#' and len(temp_list)==6:
#            receive_data = temp_list
#        else:
#            receive_data = [0,0,0,0,0,0]

#    else:
#        receive_data=[0,0,0,0,0,0]

    if (uart.any()):
        receive_data = str(uart.read())[2:-1]
        print(receive_data)

    img = sensor.snapshot()   #耗时操作

    blobs = img.find_blobs(threshold) #按阈值找标签
    if blobs:   #如果找到标签
        led.on()
        blob = max(blobs, key = lambda b: b[4])#获取面积最大的标签
        img.draw_rectangle(blob.rect())
        blob_xy[:] = blob.cx(),blob.cy()
#        print(blob_xy)
    else:
        led.off()
        blob_xy = [0,0]
#        print()
    transmit_data = "@{:03d}*{:03d}${}#".format(blob_xy[0],blob_xy[1],(blob_xy[0]+blob_xy[1])%10)

    uart.write(transmit_data)

#    print(receive_data)
#    img.draw_string(0,0,"TX:{}Hz RX:{}Hz".format(int(fps),receive_data[4]),(255,0,0),x_spacing=-2)
    img.draw_string(0,10,"Blob: X={} Y={}".format(blob_xy[0],blob_xy[1]),(255,0,0),x_spacing=-2)
#    img.draw_string(0,20,"Dist: X={} Y={}".format(receive_data[0],receive_data[1]),(255,0,0),x_spacing=-2)
#    img.draw_string(0,30,"Move: X={} Y={}".format(receive_data[2],receive_data[3]),(255,0,0),x_spacing=-2)


#    print(receive_data)


#    lcd.display(img)   # 耗时操作
    fps = clock.fps()
