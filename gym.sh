#!/usr/bin/env bash

set -e

cmake .
make MyBot

#cd replays
 ./hlt_client/hlt_client/client.py gym -r "./MyBot17" -r "./MyBot" -b ./halite -i 200 -H 200 -W 300
#../halite -d "300 200" "../MyBot17" "../MyBot"

