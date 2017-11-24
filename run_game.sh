#!/usr/bin/env bash

set -e

cmake .
make MyBot

cd replays
../halite -d "240 160" "../MyBotOld" "../MyBot"
