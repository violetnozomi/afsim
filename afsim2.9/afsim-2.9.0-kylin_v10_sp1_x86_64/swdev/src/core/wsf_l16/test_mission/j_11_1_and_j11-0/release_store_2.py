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
MESSAGE0="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=14 lat=35.9 lon=-118 alt=10000 freq=8014 mid=14 infinite_count=true\0"
MESSAGE1="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=15 lat=35.9 lon=-118 alt=10000 freq=8015 mid=15 infinite_count=true\0"
MESSAGE2="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=16 lat=35.9 lon=-118 alt=10000 freq=8016 mid=16 infinite_count=true\0"
MESSAGE3="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=17 lat=35.9 lon=-118 alt=10000 freq=8017 mid=17 infinite_count=true\0"
MESSAGE4="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=18 lat=35.9 lon=-118 alt=10000 freq=8018 mid=18 infinite_count=true\0"
MESSAGE5="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=19 lat=35.9 lon=-118 alt=10000 freq=8019 mid=19 infinite_count=true\0"
MESSAGE6="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=20 lat=35.9 lon=-118 alt=10000 freq=8020 mid=20 infinite_count=true\0"
MESSAGE7="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=21 lat=35.9 lon=-118 alt=10000 freq=8021 mid=21 infinite_count=true\0"
MESSAGE8="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=22 lat=35.9 lon=-118 alt=10000 freq=8022 mid=22 infinite_count=true\0"
MESSAGE9="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=23 lat=35.9 lon=-118 alt=10000 freq=8023 mid=23 infinite_count=true\0"
MESSAGE10="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=24 lat=35.9 lon=-118 alt=10000 freq=8024 mid=24 infinite_count=true\0"
MESSAGE11="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=25 lat=35.9 lon=-118 alt=10000 freq=8025 mid=25 infinite_count=true\0"
MESSAGE12="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=26 lat=35.9 lon=-118 alt=10000 freq=8026 mid=26 infinite_count=true\0"


MESSAGE13="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=27 lat=35.9 lon=-118 alt=10000 freq=8027 mid=27 infinite_count=true\0"
MESSAGE14="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=28 lat=35.9 lon=-118 alt=10000 freq=8028 mid=28 infinite_count=true\0"
MESSAGE15="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=29 lat=35.9 lon=-118 alt=10000 freq=8029 mid=29 infinite_count=true\0"
MESSAGE16="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=30 lat=35.9 lon=-118 alt=10000 freq=8030 mid=30 infinite_count=true\0"
MESSAGE17="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=31 lat=35.9 lon=-118 alt=10000 freq=8031 mid=31 infinite_count=true\0"
MESSAGE18="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=32 lat=35.9 lon=-118 alt=10000 freq=8032 mid=32 infinite_count=true\0"
MESSAGE19="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=33 lat=35.9 lon=-118 alt=10000 freq=8033 mid=33 infinite_count=true\0"
MESSAGE20="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=34 lat=35.9 lon=-118 alt=10000 freq=8034 mid=34 infinite_count=true\0"
MESSAGE21="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=35 lat=35.9 lon=-118 alt=10000 freq=8035 mid=35 infinite_count=true\0"
MESSAGE22="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=36 lat=35.9 lon=-118 alt=10000 freq=8036 mid=36 infinite_count=true\0"
MESSAGE23="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=37 lat=35.9 lon=-118 alt=10000 freq=8037 mid=37 infinite_count=true\0"
MESSAGE24="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=38 lat=35.9 lon=-118 alt=10000 freq=8038 mid=38 infinite_count=true\0"
MESSAGE25="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=39 lat=35.9 lon=-118 alt=10000 freq=8039 mid=39 infinite_count=true\0"

MESSAGE26="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=40 lat=35.9 lon=-118 alt=10000 freq=8040 mid=40 infinite_count=true\0"
MESSAGE27="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=41 lat=35.9 lon=-118 alt=10000 freq=8041 mid=41 infinite_count=true\0"
MESSAGE28="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=42 lat=35.9 lon=-118 alt=10000 freq=8042 mid=42 infinite_count=true\0"
MESSAGE29="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=43 lat=35.9 lon=-118 alt=10000 freq=8043 mid=43 infinite_count=true\0"
MESSAGE30="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=44 lat=35.9 lon=-118 alt=10000 freq=8044 mid=44 infinite_count=true\0"
MESSAGE31="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=45 lat=35.9 lon=-118 alt=10000 freq=8045 mid=45 infinite_count=true\0"
MESSAGE32="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=46 lat=35.9 lon=-118 alt=10000 freq=8046 mid=46 infinite_count=true\0"
MESSAGE33="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=47 lat=35.9 lon=-118 alt=10000 freq=8047 mid=47 infinite_count=true\0"
MESSAGE34="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=48 lat=35.9 lon=-118 alt=10000 freq=8048 mid=48 infinite_count=true\0"
MESSAGE35="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=49 lat=35.9 lon=-118 alt=10000 freq=8049 mid=49 infinite_count=true\0"
MESSAGE36="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=50 lat=35.9 lon=-118 alt=10000 freq=8050 mid=50 infinite_count=true\0"
MESSAGE37="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=51 lat=35.9 lon=-118 alt=10000 freq=8051 mid=51 infinite_count=true\0"
MESSAGE38="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=52 lat=35.9 lon=-118 alt=10000 freq=8052 mid=52 infinite_count=true\0"

MESSAGE39="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=53 lat=35.9 lon=-118 alt=10000 freq=8053 mid=53 infinite_count=true\0"
MESSAGE40="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=54 lat=35.9 lon=-118 alt=10000 freq=8054 mid=54 infinite_count=true\0"
MESSAGE41="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=55 lat=35.9 lon=-118 alt=10000 freq=8055 mid=55 infinite_count=true\0"
MESSAGE42="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=56 lat=35.9 lon=-118 alt=10000 freq=8056 mid=56 infinite_count=true\0"
MESSAGE43="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=57 lat=35.9 lon=-118 alt=10000 freq=8057 mid=57 infinite_count=true\0"
MESSAGE44="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=58 lat=35.9 lon=-118 alt=10000 freq=8058 mid=58 infinite_count=true\0"
MESSAGE45="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=59 lat=35.9 lon=-118 alt=10000 freq=8059 mid=59 infinite_count=true\0"
MESSAGE46="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=60 lat=35.9 lon=-118 alt=10000 freq=8060 mid=60 infinite_count=true\0"
MESSAGE47="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=61 lat=35.9 lon=-118 alt=10000 freq=8061 mid=61 infinite_count=true\0"
MESSAGE48="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=62 lat=35.9 lon=-118 alt=10000 freq=8062 mid=62 infinite_count=true\0"
MESSAGE49="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=63 lat=35.9 lon=-118 alt=10000 freq=8063 mid=63 infinite_count=true\0"
MESSAGE50="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=64 lat=35.9 lon=-118 alt=10000 freq=8064 mid=64 infinite_count=true\0"
MESSAGE51="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=65 lat=35.9 lon=-118 alt=10000 freq=8065 mid=65 infinite_count=true\0"

MESSAGE52="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=66 lat=35.9 lon=-118 alt=10000 freq=8053 mid=66 infinite_count=true\0"
MESSAGE53="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=67 lat=35.9 lon=-118 alt=10000 freq=8054 mid=67 infinite_count=true\0"
MESSAGE54="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=68 lat=35.9 lon=-118 alt=10000 freq=8055 mid=68 infinite_count=true\0"
MESSAGE55="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=69 lat=35.9 lon=-118 alt=10000 freq=8056 mid=69 infinite_count=true\0"
MESSAGE56="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=70 lat=35.9 lon=-118 alt=10000 freq=8057 mid=70 infinite_count=true\0"
MESSAGE57="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=71 lat=35.9 lon=-118 alt=10000 freq=8058 mid=71 infinite_count=true\0"
MESSAGE58="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=72 lat=35.9 lon=-118 alt=10000 freq=8059 mid=72 infinite_count=true\0"
MESSAGE59="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=73 lat=35.9 lon=-118 alt=10000 freq=8060 mid=73 infinite_count=true\0"
MESSAGE60="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=74 lat=35.9 lon=-118 alt=10000 freq=8061 mid=74 infinite_count=true\0"
MESSAGE61="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=75 lat=35.9 lon=-118 alt=10000 freq=8062 mid=75 infinite_count=true\0"
MESSAGE62="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=76 lat=35.9 lon=-118 alt=10000 freq=8063 mid=76 infinite_count=true\0"
MESSAGE63="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=77 lat=35.9 lon=-118 alt=10000 freq=8064 mid=77 infinite_count=true\0"
MESSAGE64="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=78 lat=35.9 lon=-118 alt=10000 freq=8065 mid=78 infinite_count=true\0"
MESSAGE65="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=79 lat=35.9 lon=-118 alt=10000 freq=8066 mid=79 infinite_count=true\0"


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

s.send(MESSAGE14)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE15)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE16)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE17)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE18)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE19)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE20)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE21)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE22)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE23)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE24)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE25)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE26)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)


s.send(MESSAGE27)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE28)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE29)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE30)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE31)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE32)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE33)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE34)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE35)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE36)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE37)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE38)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE39)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)


s.send(MESSAGE40)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE41)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE42)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE43)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE44)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE45)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE46)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE47)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE48)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE49)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE50)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE51)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE52)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE53)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE54)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE55)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE56)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE57)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE58)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE59)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE60)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE61)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE62)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE63)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE64)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)

s.send(MESSAGE65)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data


s.close()

