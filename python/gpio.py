import numpy as np
import time
import threading
import sys
import RPi.GPIO as GPIO

pin_num = 24

def next_tick_time(tempo, cur_time):
    sec_per_tick = 60.0 / tempo
    return np.ceil(cur_time / sec_per_tick) * sec_per_tick
    
def sec_per_tick(tempo):
    return 60.0 / tempo

def play_tick():
    GPIO.output(pin_num,False)
    GPIO.output(pin_num,True)
#print time.time()

def main(tempo):
    lookahead = 0.05
    n_ticks = 12000

    GPIO.setmode(GPIO.BCM)
    GPIO.setup(pin_num, GPIO.OUT)
    GPIO.output(pin_num,True)
    
    i = 0
    last_tick = 0.0
    next_tick = next_tick_time(tempo, time.time())
    tick_period = sec_per_tick(tempo)
    while i < n_ticks:
        offset = next_tick - time.time()
        if offset < lookahead:
            threading.Timer(offset, play_tick, ()).start()
            last_tick = next_tick
            next_tick = next_tick + tick_period
            i = i + 1
            time.sleep(offset+lookahead)
        else:
            time.sleep(0.001)
    p.terminate()

if __name__ == "__main__":
    tempo = 120.0
    for arg in sys.argv[1:]:
        tempo = float(arg)

    main(tempo)
