import board,digitalio,time,random,microcontroller
from analogio import AnalogIn
#
led_on_board = digitalio.DigitalInOut(board.LED)
led_on_board.direction = digitalio.Direction.OUTPUT

analog_in = AnalogIn(board.A1)

def get_voltage(pin):
    return (pin.value * 3.3) /65536

start = time.monotonic()

try:
    with open("//dane_pomiarowe.txt", "a") as datalog: # measurement data 
        print("Udalo sie")
        while time.monotonic() - start < 5:
            t = microcontroller.cpu.temperature
            A1 = get_voltage(analog_in)
            temp = f'czas = {(time.monotonic()/10):7} ; napięcie = {A1:8} ; temperatura = {t:7}\n'
            datalog.write(temp)
            datalog.flush()
            led_on_board.value = not led_on_board.value
            time.sleep(1)

except OSError as e:
    delay = 0.5  
    if e.args[0] == 28:  
        delay = 0.1  
    print("Error")
    i = 20
    while i > 0:
        led_on_board.value = not led_on_board.value
        time.sleep(delay)
        i = i - 1

        
