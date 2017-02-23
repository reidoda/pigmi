# Introduction

The Global Metronome is a technique for synchronizing tempo over long distances using GPS. This repository is in early alpha stages and consists of a number of tools to test whether the Global Metronome approach is viable. The PIGMI (Pi Global Metronome Implementation) is a Raspberry Pi based implementation of the Global Metronome that depends on a custom Raspbian image and the software in this repository. To obtain the Raspbian image, contact the repository owner. 

# How to use the PIGMI as a master MIDI clock
The `midiclock-old` program controls the starting of MIDI devices. It quantizes the start time of the connected devices so that they start at the beginning of the next Global Metronome 4/4 measure (i.e. similar to how Ableton Live launches clips). Currently there may be drift in *MIDI clock* pulses, but generally the devices will stay in time with one another for at least 10 minutes. As the software is improved so will the drift problem. Instruction are below:

1. Plug into out B on your Midisport. 
2. Log into PIGMI either via the Raspberry Pi desktop or via SSH. 
2. First check to see that the midiclock program has been built by typing `ls global_metronome/bin`. You should see the file `midiclock-old`.
3. If the file is not there we need to build it by typing `cd ~/global_metronome/cpp; make midiclock-old`.
4. To start the midi clock type `global_metronome/bin/midiclock-old <tempo>` where `<tempo>` is the desired tempo. *MIDI clock* will be sent immediately, followed by a *MIDI start* message that is timed to be sent at the start of the next Global Metronome measure. 

# Troubleshooting
If your device starts playing immediately rather than waiting for the start of the next measure, it may be set to play immediately upon receiving *MIDI clock messages*, rather than waiting for a *MIDI start* message. We're currently unsure if this is a widespread problem, so please let us know if you experience this. 

To check if your device's clock is properly connected to the GPS signal enter `ntpq -p` and look for the line that begins with **oPPS**. You should see something like the following:
```
     remote           refid      st t when poll reach   delay   offset  jitter
==============================================================================
oPPS(0)          .PPS.            0 l    4    8  377    0.000   -0.002   0.002
*195.21.152.161  195.66.241.2     2 u    1   16  377    7.177    0.992   1.555
+feathers.jonlig 130.207.244.240  2 u    1   16  377   70.343    1.089   1.066
+leeloo.scurvyne 128.4.1.1        4 u   14   16  377   94.902    3.338   1.243
-blue.1e400.net  109.229.128.40   3 u   15   16  377   87.155    3.661   2.155
```

The important column is *reach*, which tells us how strong the PPS signal is (from the point of view of NTP). It should be at 377. *Delay*, *offset* and *jitter* are in milliseconds and should be less than 1. 

# How to hear your collaborators
We are using JamKazam as the audio data platform for PIGMI use. It's free and comes with a suite of tools to troubleshoot network performance and connectivity. Go to their site and follow the directions there. 
