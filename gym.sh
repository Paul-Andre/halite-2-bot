#!/usr/bin/env bash

set -e

cmake .
make MyBot

#cd replays
 ./hlt_client/hlt_client/client.py gym -r "./MyBot14" -r "./MyBot" -b ./halite -i 200
#../halite -d "300 200" "../MyBot17" "../MyBot"

