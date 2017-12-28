#!/usr/bin/env bash

set -e

cmake .
make MyBot

cd replays
../halite "../MyBot15" "../MyBot"
