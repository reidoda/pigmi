import logging
import sys
import time
import rtmidi
import numpy as np
import threading
n_pulses = 100000


def main():
    midiout = rtmidi.MidiOut()
    available_ports = midiout.get_ports()

    if available_ports:
        midiout.open_port(2)
    else:
        midiout.open_virtual_port("My virtual output")    
    def send_pulse():
        midiout.send_message([248])
    def send_start():
        midiout.send_message([250])

    def send_midi_clock(tempo,start_time):
        cdef float period
        cdef double start_time, now, next_pulse_time
        start_time = time.time() # maybe change to absolute time later
	started = False
        period = (60.0/tempo)/24.0
        lookahead = 0.004
    
        while True:
            now = time.time()
	    if not started:
                if start_time - now < lookahead:
		    threading.Timer(start_time - time.time(), send_start, ()).start()
            next_pulse_time = start_time + (np.floor((now-start_time)/period) + 1) * period
            if next_pulse_time - now < lookahead:
                threading.Timer(next_pulse_time - time.time(), send_pulse, ()).start()
                time.sleep(lookahead*2)
            else:
                time.sleep(0.0005)

    def start_on_measure(tempo):
        threshold = 0.001
        period = (60.0/tempo) * 4
        now = time.time()
        next_measure_time = (np.floor(now/period) + 1) * period
        while next_measure_time - time.time() > threshold:
            time.sleep(0.001)
        send_midi_clock(tempo,next_measure_time)

    start_on_measure(120)

if __name__=="__main__":
    main()

