# More details can be found in TechToTinker.blogspot.com 
# George Bantique | tech.to.tinker@gmail.com

from machine import Pin
from time import sleep_ms
from rotary_irq import RotaryIRQ

r = RotaryIRQ(pin_num_clk=32, 
              pin_num_dt=33, 
              min_val=0, 
              max_val=19, 
              reverse=True, 
              range_mode=RotaryIRQ.RANGE_WRAP)
sw = Pin(34, Pin.IN)              
val_old = r.value()
isRotaryEncoder = False

while True:
    if sw.value()==1:
        isRotaryEncoder = not isRotaryEncoder
        if isRotaryEncoder==True:
            print('Rotary Encoder is now enabled.')
        else:
            print('Rotary Encoder is now disabled.')
            
    if isRotaryEncoder==True:
        val_new = r.value()
        if val_old != val_new:
            val_old = val_new
            print('result = {}'.format(val_new))

    sleep_ms(200)