# Introduction

The Global Metronome is a technique for synchronizing tempo over long distances using GPS. This repository is in early alpha stages and consists of a number of tools to test whether the Global Metronome approach is viable. The PIGMI (Pi Global Metronome Implementation) is a Raspberry Pi based implementation of the Global Metronome that depends on a custom Raspbian image and the software in this repository. To obtain the Raspbian image, contact the repository owner. 

# How to use the PIGMI as a master MIDI clock
The `run_midiclock` program controls the starting of MIDI devices. It quantizes the start time of the connected devices so that they start at the beginning of the next Global Metronome 4/4 measure (i.e. similar to how Ableton Live launches clips). Instruction are below:

1. Plug into out B on your Midisport. 
2. Log into PIGMI either via the Raspberry Pi desktop or via SSH. 
2. First check to see that the midiclock program has been built by typing `ls global_metronome/bin`. You should see the file `run_midiclock`.
3. If the file is not there we need to build it by typing `cd ~/global_metronome/cpp; make run_midiclock`. You may have to install some libraries in order for this to work. 
4. To start the midi clock type `global_metronome/bin/run_midiclock <tempo> <your public ip> <ip of our collaborator>`. *MIDI clock* will be sent immediately, followed by a *MIDI start* message that is timed to be sent at the start of the next Global Metronome measure. 

# Troubleshooting
1. To check if your device's clock is properly connected to the GPS signal enter `ntpq -p` and look for the line that begins with **oPPS**. You should see something like the table below. The important column is *reach*, which tells us how strong the PPS signal is (from the point of view of NTP). It should be at 377. *Delay*, *offset* and *jitter* are in milliseconds and should be less than 1. 

 ```
      remote           refid      st t when poll reach   delay   offset  jitter
 ==============================================================================
 oPPS(0)          .PPS.            0 l    4    8  377    0.000   -0.002   0.002
 *195.21.152.161  195.66.241.2     2 u    1   16  377    7.177    0.992   1.555
 +feathers.jonlig 130.207.244.240  2 u    1   16  377   70.343    1.089   1.066
 +leeloo.scurvyne 128.4.1.1        4 u   14   16  377   94.902    3.338   1.243
 -blue.1e400.net  109.229.128.40   3 u   15   16  377   87.155    3.661   2.155
 ```
2. If you think that your MIDI clock might be drifing, the PIGMI outputs a click that is tightly synchronized to the GPS sensor and does not drift from the Global Metronome tempo and phase. You can hear it by attaching alligator clips to physical (not BCM) pins 18 and 39 of the GPIO pins. The signal is strong enough to drive headphones, but you might want to run the signal through a mixer to amplify and hear it better. Because this tick as no latency you might find it happens a few MS before your connected MIDI device, but let it run for 20 minutes and see if the timing relationship between the two devices changes. 

# How to hear your collaborators
We are using JamKazam as the audio data platform for PIGMI use. It's free and comes with a suite of tools to troubleshoot network performance and connectivity. Go to their site and follow the directions there. 
