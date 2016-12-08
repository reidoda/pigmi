#include "rtmidi/RtMidi.h"
int main()
{
    RtMidiIn *midiin = 0;
    // RtMidiIn constructor
    try {
        midiin = new RtMidiIn();
        std::cout << "It worked!" << std::endl;
    }
    catch (RtMidiError &error) {
    // Handle the exception here
       error.printMessage();
    }
    // Clean up
    delete midiin;
}
