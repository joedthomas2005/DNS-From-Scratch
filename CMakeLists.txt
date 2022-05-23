cmake_minimum_required(VERSION 3.00)
project(DNS_From_Scratch C)

set(CMAKE_C_STANDARD 11)

add_library(dnsloader SHARED
        getConfig.c)
add_executable(testDNS
        testDNS.c)

if(WIN32)
        target_link_libraries(dnsloader iphlpapi)
endif()

target_link_libraries(testDNS dnsloader)