import socket

request_types = {
    0: 'Reserved',
    1: 'A',
    2: 'NS',
    5: 'CNAME',
    6: 'SOA',
    11: 'WKS',
    12: 'PTR',
    13: 'HINFO',
    14: 'MINFO',
    15: 'MX',
    16: 'TXT',
    23: 'NSAP-PTR',
    24: 'SIG',
    25: 'KEY',
    27: 'GPOS',
    28: 'AAAA',
    29: 'LOC',
    35: 'NAPTR',
    36: 'KX',
    44: 'SSHFP'
}

request_classes = {
    0: 'Reserved',
    1: 'Internet (IN)',
    2: 'Unassigned',
    3: 'Chaos (CH)',
    4: 'Hesiod (HS)',
    254: 'QCLASS NONE',
    255: 'QCLASS ANY',
}

lookupDomainEncoded = bytes.fromhex('07696e2d61646472046172706100')  # .in-addr.arpa

def get_host(addr, dns_server="192.168.1.1", dns_port=53):
    octets = addr.split('.')
    octets.reverse()
    raw_address = b''
    for octet in octets:
        length = hex(len(octet)).split('x')[1]
        # python annoyingly formats hex as "\0xAA". This is safe as x is not a hex char
        raw_address += bytes.fromhex(f'0{length}')
        # this is probably bad, but I need two hex bytes and there will never be more than 3 chars per octet
        raw_address += octet.encode('utf-8')

    txid = bytes.fromhex('0001')  # TXID (should be randomised really)
    flags_bitmask = bytes.fromhex(f'0100')  # STANDARD QUERY RECURSION DESIRED 0b0000000100000000
    qs = bytes.fromhex('0001')  # 1 QUESTION
    ans = bytes.fromhex('0000')  # 0 ANSWERS
    auths = bytes.fromhex('0000')  # 0 AUTHS
    additional = bytes.fromhex('0000')  # 0 ADDITIONAL
    req_type = bytes.fromhex("000c")  # DOMAIN NAME POINTER (PTR)
    req_class = bytes.fromhex('0001')  # IN
    data = b''.join(
        [txid, flags_bitmask, qs, ans, auths, additional, raw_address, lookupDomainEncoded, req_type, req_class])

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(("0.0.0.0", 8998))
    s.sendto(data, (dns_server, dns_port))

    res = s.recv(2048)
    return DNSMessage(res)


class DNSMessage():
    def __init__(self, data):
        split_bytes = [hex(x)[2:] for x in data]
        self.txid = int(''.join([split_bytes[0], split_bytes[1]]), 16)
        flags_bitmask = int(''.join([split_bytes[2], split_bytes[3]]), 16)
        self.message_type = 'r' if 0b1000000000000000 & flags_bitmask == 0b1000000000000000 else 'q'
        self.opcode = 0b0111100000000000 & flags_bitmask
        self.server_is_authoritative = 0b0000010000000000 & flags_bitmask == 0b0000010000000000
        self.truncated = 0b0000001000000000 & flags_bitmask == 0b0000001000000000
        self.recursion_desired = 0b0000000100000000 & flags_bitmask == 0b0000000100000000
        self.recursion_available = 0b0000000010000000 & flags_bitmask == 0b0000000010000000
        self.status_code = 0b0000000000001111 & flags_bitmask

        self.questions = int(''.join([split_bytes[4], split_bytes[5]]), 16)
        self.answers = int(''.join([split_bytes[6], split_bytes[7]]), 16)
        self.authorities = int(''.join([split_bytes[8], split_bytes[9]]), 16)
        self.additional = int(''.join([split_bytes[10], split_bytes[11]]), 16)

        name_start = 12
        name_end = 13
        while split_bytes[name_end - 1] != '0':
            name_end += 1
        # Each part of the ipaddr.in-addr.arpa name is a byte of how many characters are in the next "subdomain" part
        # followed by that many bytes representing the ascii characters
        cur = name_start
        self.address_lookup_name = ""
        while split_bytes[cur] != '0':
            char = cur
            for i in range(int(split_bytes[cur], 16)):
                char += 1
                self.address_lookup_name += chr(int(split_bytes[char], 16))

            if split_bytes[cur + int(split_bytes[cur], 16) + 1] != '0':
                self.address_lookup_name += '.'
            cur += int(split_bytes[cur], 16) + 1
        cur += 1

        self.req_type = "Unknown"
        try:
            self.req_type = request_types[int(''.join([split_bytes[cur], split_bytes[cur + 1]]), 16)]
        except KeyError as k:
            print("WARNING: UNKNOWN QUERY TYPE")

        self.req_class = "Unknown"
        try:
            self.req_class = request_classes[int(''.join([split_bytes[cur + 2], split_bytes[cur + 3]]), 16)]

        except KeyError as k:
            print("WARNING: UNKNOWN QUERY CLASS")

        self.time_to_live = int(''.join(split_bytes[cur + 10: cur + 15]), 16)

        # Same system as the query address above
        current = cur + 16
        self.domain_name = ""
        while split_bytes[current] != '0':
            char = current
            for i in range(int(split_bytes[current], 16)):
                char += 1
                self.domain_name += chr(int(split_bytes[char], 16))
            if split_bytes[current + int(split_bytes[current], 16) + 1] != '0':
                self.domain_name += '.'
            current += int(split_bytes[current], 16) + 1
            char += 1

