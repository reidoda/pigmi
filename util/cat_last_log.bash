#!/bin/bash
curdir=$(pwd)
cd ~/global_metronome/log
cat "$(ls -1rt | tail -n1)"
cd $curdir
