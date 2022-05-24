import DNSrequests
import ctypes
import os

if __name__ == "__main__":
    
    if os.name == "nt":
        if os.path.exists("build/libdnsloader.dll"):
            dnslib = ctypes.WinDLL("build/libdnsloader.dll")
        elif os.path.exists("build/dnsloader.dll"):
            dnslib = ctypes.WinDLL("build/dnsloader.dll")
        else:
            print("ERROR: DNS loader shared library not found. Did you run CMake?")
            exit()
    else:
        if os.path.exists("build/libdnsloader.so"):
            dnslib = ctypes.CDLL("build/libdnsloader.so")
        else:
            print("ERROR: DNS loader shared library not found. Did you run CMake?")
    dnslib.getCurrentDNS.restype = ctypes.POINTER(ctypes.c_char)
    rawCharArray = dnslib.getCurrentDNS()
    dnsServer = ""
    for i in range(16):
        if rawCharArray[i] == b'\x00':
            break
        dnsServer += rawCharArray[i].decode()
    
    print(f"USING NAMESERVER: {dnsServer}")
    
    address = input("PLEASE ENTER IP ADDRESS: ")
    response = DNSrequests.get_host(address, dnsServer)
    print("REQUEST FOR ", response.address_lookup_name)
    if(response.server_is_authoritative):
        print("AUTHORITATIVE ANSWER: ")
    else:
        print("NON-AUTHORITATIVE ANSWER: ")
    print(response.domain_name)

