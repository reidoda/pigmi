import numpy as np
import time
import pyaudio
import wave
import sys
import sched

def next_tick_time(tempo, cur_time):
    sec_per_tick = 60.0 / tempo
    return np.ceil(cur_time / sec_per_tick) * sec_per_tick
    
def sec_per_tick(tempo):
    return 60.0 / tempo

def play_tick(stream,data):
    # print 'time: %3.4f' % time.time()
    stream.write(data)


def main(tempo):
    CHUNK = 100000
    lookahead = 0.005
    n_ticks = 80

    scheduler = sched.scheduler(time.time, time.sleep)
    wf = wave.open('/home/pi/global_metronome/onset_tick.wav')
    p = pyaudio.PyAudio()
    stream = p.open(format=p.get_format_from_width(wf.getsampwidth()),
                    channels=wf.getnchannels(),
                    rate= wf.getframerate(),
                    output=True)

    data = wf.readframes(CHUNK)

    i = 0
    last_tick = 0.0
    next_tick = next_tick_time(tempo, time.time())
    tick_period = sec_per_tick(tempo)
    while i < n_ticks:
        offset = next_tick - time.time()
        if offset < lookahead:
            scheduler.enter(offset*1000, 1, play_tick, ([stream, data]))
            # print 'offset: %.4f' % offset
            last_tick = next_tick
            next_tick = next_tick + tick_period
            i = i + 1
        time.sleep(0.001)
    p.terminate()

if __name__ == "__main__":
    tempo = 120.0
    for arg in sys.argv[1:]:
        tempo = float(arg)

    main(tempo)
