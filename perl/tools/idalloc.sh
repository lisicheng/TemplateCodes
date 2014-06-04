#!/bin/bash
#set -eu

log_fatal() {
	echo $@
	exit 1;
}

log_debug() {
	echo $@
}

log_info() {
	echo $@
}
