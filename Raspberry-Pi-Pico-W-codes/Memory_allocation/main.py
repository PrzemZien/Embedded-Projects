import machine
import time
import gc
import random
#
led_on_board = machine.Pin("LED",machine.Pin.OUT)
listData = []
#
def memory_usage():
    free_memory = gc.mem_free() # pamiec ktora jest wola
    memory_allocated = gc.mem_alloc() # pamiec uzytkowana
    total_memory = free_memory + memory_allocated # całkowita pamiec pi pico
    used_memory = memory_allocated / total_memory
    delay = 0.1 + ( used_memory * 1)
    return free_memory,memory_allocated,total_memory,used_memory,delay
    
def memory_data(free_memory_data,memory_allocated_data,total_memory_data,used_memory_data):
    print(" ")
    print("Free memory:",free_memory_data)
    print("Memory allocated:",memory_allocated_data)
    print("Total memory of rasberry PI Pico:",total_memory_data)
    print(f"Procent użycia pamięci: {round(used_memory_data,3) * 100} %")
    print(" ")
       
#
while True:
    i = 0
    i += 100
    listData.append(i) # zwiększam zajętość listy w pamięci 
    memory_info = memory_usage()
    memory_data(memory_info[0],memory_info[1],memory_info[2],memory_info[3])
    led_on_board.value(1)
    time.sleep(memory_info[4])
    led_on_board.value(0)
    time.sleep(memory_info[4])


    
    