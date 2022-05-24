cmake_minimum_required(VERSION 3.9)
project(DNS_From_Scratch C)

set(CMAKE_C_STANDARD 11)

if(MSVC)        
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)  # Necessary for CMake to generate the .lib symbol files 
        set(BUILD_SHARED_LIBS TRUE)
endif()

add_library(dnsloader SHARED
        dnsloader.c)

add_executable(testDNS
        testDNS.c)

if(WIN32)
        target_link_libraries(dnsloader iphlpapi)
endif()

target_link_libraries(testDNS dnsloader)