#!/usr/bin/env bash

set -e

cmake .
make MyBot

cd replays
../halite -d "450 300" "../MyBot12" "../MyBot"
