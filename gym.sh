#!/usr/bin/env bash

set -e

cmake .
make MyBot

#cd replays
 ./hlt_client/hlt_client/client.py gym -r "./MyBot22" -r "./MyBot" -b ./halite -i 200

