#!/bin/bash

mkdir -p /tmp/cgroupv2
mount -t cgroup2 none /tmp/cgroupv2
mkdir -p /tmp/cgroupv2/foo
