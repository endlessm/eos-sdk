#!/bin/sh

# Runs a GTest binary with --tap
# The first argument to this script is the name of the test binary

$1 -k --tap
