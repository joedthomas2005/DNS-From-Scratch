import DNSrequests
import ctypes
import os

if __name__ == "__main__":
    
    if os.name == "nt":
        dnslib = ctypes.WinDLL("build/libdnsloader.dll")
    else:
        dnslib = ctypes.CDLL("build/libdnsloader.so")
    dnslib.getCurrentDNS.restype = ctypes.POINTER(ctypes.c_char)
    rawCharArray = dnslib.getCurrentDNS()
    dnsServer = ""
    for i in range(16):
        if i == '\x00':
            break
        dnsServer += rawCharArray[i].decode()
    
    address = input("PLEASE ENTER IP ADDRESS: ")
    response = DNSrequests.get_host(address, dnsServer)
    print("REQUEST FOR ", response.address_lookup_name)
    if(response.server_is_authoritative):
        print("AUTHORITATIVE ANSWER: ")
    else:
        print("NON-AUTHORITATIVE ANSWER: ")
    print(response.domain_name)

