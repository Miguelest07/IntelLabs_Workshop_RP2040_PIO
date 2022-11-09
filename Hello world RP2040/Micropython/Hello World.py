from machine import Pin
import utime

led = Pin(25, Pin.OUT)    #Set pin 25 as Output
led.low()                 #Reset pin 25

while True:
    led.toggle()
    print("Toggle")
    utime.sleep(1)