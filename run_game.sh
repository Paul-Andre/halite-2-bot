#!/usr/bin/env bash

set -e

cmake .
make MyBot

cd replays
../halite -d "300 200" "../MyBotPrevPrev" "../MyBot"
