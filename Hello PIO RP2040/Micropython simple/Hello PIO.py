from machine import Pin
import rp2
from rp2 import PIO, StateMachine, asm_pio
from time import sleep

@asm_pio(
         out_init=(rp2.PIO.OUT_HIGH,) * 8, #Set 8 pins as output
         out_shiftdir=PIO.SHIFT_RIGHT,     #Set the 8 least significant bits from OSR for pull
         autopull=True,                    #Automaitcally refill the OSR from tx fifo if data is pulled
         pull_thresh=32                    #Tells autopull how many bits have to be passed to auto pull from tx fifo
         )

def paral_prog():
    pull()                                 #Block with tx fifo deasserted until data available
    out(pins, 8)                           #Send data from OSR to pins and declaring that we want 8 bits

paral_sm = StateMachine(
    0,                                     #State machine number
    paral_prog,                            #PIO Program name
    freq=100000000,                           #State machine clock frequency
    out_base=Pin(0),                       #Output pin base (set as 8 pins)
    )

paral_sm.active(1)                         #Activate state machine

while True:
    for i in range(255):
        paral_sm.put(i)                    #Put data to tx fifo 