from socket import socket, AF_PACKET, SOCK_RAW

s = socket(AF_PACKET,SOCK_RAW)
s.bind(("eth0",0))

src_addr = b"\x01\x02\x03\x04\x05\x06"
dst_addr = b"\x01\x02\x03\x04\x05\x06"
# payload = ("["*30)+"PAYLOAD"+("]"*30)
# checksum = "\x1a\x2b\x3c\x4d"
# ethertype = "\x08\x01"

# s.send(dst_addr+src_addr+ethertype+payload+checksum)
s.send(dst_addr+src_addr)