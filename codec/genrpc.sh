#!/bin/sh
protoc -I=. --cpp_out=. rpc.proto
