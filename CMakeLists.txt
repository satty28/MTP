cmake_minimum_required(VERSION 3.6)
project(MTP)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    Bcast/bcast.c
    Bcast/bcast_send.c
    Document/Hello
    "Document/Message Formats.pdf"
    Source/feature_payload.c
    Source/feature_payload.h
    Source/main.c
    Source/mtp_send.c
    Source/mtp_send.h
    clean
    files
    install
    README.md)

add_executable(MTP ${SOURCE_FILES})