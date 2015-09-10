#!/bin/sh
set -x
rsync -av ~/Google\ Drive/hacking/eagle/"Crazy Volts"/*.{brd,sch} .
rsync -av ~/Google\ Drive/hacking/Arduino/crazy* .
