import pyaudio
import wave

def main():
    CHUNK = 100000

    wf = wave.open('/home/pi/global_metronome/onset_tick.wav')
    p = pyaudio.PyAudio()
    stream = p.open(format=p.get_format_from_width(wf.getsampwidth()),
                    channels=wf.getnchannels(),
                    rate= wf.getframerate(),
                    output=True)

    data = wf.readframes(CHUNK)
    stream.write(data)
    p.terminate()
    
if __name__ == "__main__":
    main()
