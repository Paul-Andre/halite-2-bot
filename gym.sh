#!/usr/bin/env bash

set -e

cmake .
make MyBot

#cd replays
hlt gym -r "./MyBot12" -r "./MyBot" -b ./halite -i 50 -H 200 -W 300
#../halite -d "300 200" "../MyBot8" "../MyBot"

