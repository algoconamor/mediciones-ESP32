# More details can be found in TechToTinker.blogspot.com 
# George Bantique | tech.to.tinker@gmail.com
import utime
from machine import I2C,Pin
from time import sleep_ms
from rotary_irq import RotaryIRQ  #encoder rotatorio
from lcd_api import LcdApi
from pico_i2c_lcd import I2cLcd

#Direccion del I2C y tamaño del LCD
I2C_ADDR = 0x27
I2C_NUM_ROWS = 4
I2C_NUM_COLS = 19

#ESP32
i2c = I2C(sda=Pin(4), scl=Pin(5), freq=100000)

#Configuracion
lcd = I2cLcd(i2c, I2C_ADDR, I2C_NUM_ROWS, I2C_NUM_COLS)

#graficos
VAC_0 = [0x1D,
  0x15,
  0x17,
  0x00,
  0x11,
  0x1B,
  0x0A,
  0x04]
VDC_1 = [0x1F,
  0x00,
  0x15,
  0x00,
  0x11,
  0x1B,
  0x0A,
  0x04]
R = [0x00,
  0x0E,
  0x11,
  0x11,
  0x11,
  0x0A,
  0x1B,
  0x00]
A_0 = [0x1F,
  0x00,
  0x15,
  0x00,
  0x04,
  0x0A,
  0x0E,
  0x0A]
A_1 = [0x1D,
  0x15,
  0x07,
  0x00,
  0x04,
  0x0A,
  0x0E,
  0x0A]

buz = [  0x05,
  0x04,
  0x0D,
  0x1C,
  0x1D,
  0x0C,
  0x04,
  0x05]
diode = [0x00,
  0x00,
  0x09,
  0x0D,
  0x1F,
  0x0D,
  0x09,
  0x00]
fle_0 = [ 0x04,
  0x04,
  0x06,
  0x1F,
  0x1F,
  0x06,
  0x04,
  0x04]
fle_1 = [ 0x04,
  0x0E,
  0x1F,
  0x1F,
  0x1F,
  0x0E,
  0x0E,
  0x0E]


def lcd_str(message, col, row):
    lcd.move_to(col, row)
    lcd.putstr(message)
    
#encoder
r = RotaryIRQ(pin_num_clk=32, 
              pin_num_dt=33, 
              min_val=0, 
              max_val=19, 
              reverse=True, 
              range_mode=RotaryIRQ.RANGE_WRAP)
sw = Pin(34, Pin.IN)              
val_old = r.value()
isRotaryEncoder = False


    #lcd.custom_char(0, bytearray(VAC_0))
    #lcd.custom_char(1, bytearray(VDC_1))
    #lcd.custom_char(2, bytearray(R))
    #lcd.custom_char(3, bytearray(A_0))
    #lcd.custom_char(4, bytearray(A_1))
    #lcd.custom_char(5, bytearray(buz))
    #lcd.custom_char(6, bytearray(diode))
    #lcd.custom_char(7, bytearray(fle_0))
    #lcd.custom_char(8, bytearray(fle_1))


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

    sleep_ms(80)
    
    lcd.clear()
    lcd_str(str(val_new),1,2) # la funcion str(val_new) es para convertir a numerico a stregin
    utime.sleep(1)

    

        
        
        