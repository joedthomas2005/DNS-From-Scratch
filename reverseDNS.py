import DNSrequests

if __name__ == "__main__":
    # address = input("PLEASE ENTER IP ADDRESS: ")
    # response = DNSrequests.get_host(address, "10.6.16.19")
    # print("REQUEST FOR ", response.address_lookup_name)
    # if(response.server_is_authoritative):
    #     print("AUTHORITATIVE ANSWER: ")
    # else:
    #     print("NON-AUTHORITATIVE ANSWER: ")
    # print(response.domain_name)
    # # print(req)
    print(DNSrequests.get_addr("google.com", "10.6.16.19"))