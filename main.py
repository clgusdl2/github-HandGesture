
print(hex(3))
from time import sleep

leds = LEDBoard(18, 17, 16, 23)
button = Button(21)

def check369(n):
    s = str(n)
    if '3' in s or '6' in s or '9' in s:
        return True
    else:
        return False

def led369(leds, _n):
    
    #check369(_n)
    led_loc = _n % 4
    if check369(_n):
        leds[led_loc].on()
        sleep(0.5)
        leds[led_loc].off()
    # for led in leds:
    #     led.on()
    #     sleep(0.5)
    #     led.off()



def led4bit(leds,n):
    leds.off()
    s=format(n,'04b')
    for i in range(4):
        if s[i] == '1':
            leds[i].on()
def led4bitCounter(leds):
    for n in range(1,16):
        led4bit(leds,n)
        sleep(0.5)


while True:
    if button.is_pressed:
        led4bitCounter(leds)
        sleep(2)
        leds.off()