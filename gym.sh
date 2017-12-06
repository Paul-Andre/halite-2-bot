#!/usr/bin/env bash

set -e

cmake .
make MyBot

#cd replays
hlt gym -r "./MyBot9" -r "./MyBot" -b ./halite -i 50 -H 300 -W 200
#../halite -d "300 200" "../MyBot8" "../MyBot"

