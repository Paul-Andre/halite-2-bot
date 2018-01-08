#!/usr/bin/env bash

set -e

cmake .
make MyBot

cd replays
../halite "../MyBot24" "../MyBot"
