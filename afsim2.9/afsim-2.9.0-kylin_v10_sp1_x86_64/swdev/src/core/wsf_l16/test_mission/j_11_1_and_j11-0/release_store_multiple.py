# ****************************************************************************
# CUI//REL TO USA ONLY
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
import socket
import time


TCP_IP = '127.1.1.1'
TCP_PORT = 7654
BUFFER_SIZE = 1024
MESSAGE0="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=1 lat=35.9 lon=-118 alt=10000 freq=8001 mid=1 infinite_count=true\0"
MESSAGE1="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=2 lat=35.9 lon=-118 alt=10000 freq=8002 mid=2 infinite_count=true\0"
MESSAGE2="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=3 lat=35.9 lon=-118 alt=10000 freq=8003 mid=3 infinite_count=true\0"
MESSAGE3="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=4 lat=35.9 lon=-118 alt=10000 freq=8004 mid=4 infinite_count=true\0"
MESSAGE4="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=5 lat=35.9 lon=-118 alt=10000 freq=8005 mid=5 infinite_count=true\0"
MESSAGE5="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=6 lat=35.9 lon=-118 alt=10000 freq=8006 mid=6 infinite_count=true\0"
MESSAGE6="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=7 lat=35.9 lon=-118 alt=10000 freq=8007 mid=7 infinite_count=true\0"
MESSAGE7="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=8 lat=35.9 lon=-118 alt=10000 freq=8008 mid=8 infinite_count=true\0"
MESSAGE8="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=9 lat=35.9 lon=-118 alt=10000 freq=8009 mid=9 infinite_count=true\0"
MESSAGE9="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=10 lat=35.9 lon=-118 alt=10000 freq=8010 mid=10 infinite_count=true\0"
MESSAGE10="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=11 lat=35.9 lon=-118 alt=10000 freq=8011 mid=11 infinite_count=true\0"
MESSAGE11="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=12 lat=35.9 lon=-118 alt=10000 freq=8012 mid=12 infinite_count=true\0"
MESSAGE12="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=13 lat=35.9 lon=-118 alt=10000 freq=8013 mid=13 infinite_count=true\0"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
print "RELEASE_STORE"
s.send(MESSAGE0)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE1)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE2)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE3)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE4)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE5)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE6)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE7)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE8)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE9)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE10)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE11)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE12)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE13)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.close()

