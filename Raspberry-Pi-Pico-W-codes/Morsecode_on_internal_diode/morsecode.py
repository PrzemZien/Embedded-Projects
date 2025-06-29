import board,digitalio,time,random
#
led_on_board = digitalio.DigitalInOut(board.LED)
led_on_board.direction = digitalio.Direction.OUTPUT

def dot_diode():
            time.sleep(1)
            led_on_board.value = True
            time.sleep(1)
            led_on_board.value = False
            time.sleep(1)
            
def line_diode():
            led_on_board.value = True
            time.sleep(2)
            led_on_board.value = False
            time.sleep(1)
                        
def morsecode(strData):
    strData = list(strData)
    dictData ={"a":"*-","b":"-***","c":"-*-*",
               "d":"-**","e":"*","f":"**-*","g":"--*",
               "h":"****","i":"**","j":"*---","k":"-*-","l":"*-**",
               "m":"--","n":"-*","o":"---","p":"*--*","q":"--*-",
               "r":"*-*","s":"***","t":"-","u":"**-","v":"***-",
               "w":"*--","x":"-**-","y":"-*--","z":"--**"}
    for letter in strData:
        print(f"{letter}")
        for morse_code in dictData[letter]:
            if morse_code == "*":
                dot_diode()
            elif morse_code == "-":
                line_diode()
            else:
                print("Error")

# Głowna pętla
while True:
    user_input = str(input("Wpisz wyraz:"))
    if user_input == "koniec":
        break
    elif isinstance(user_input,str):
            morsecode(user_input.lower())
    else:
        print("Error")
        break








