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
#TPC_IP = '10.34.209.138'
TCP_PORT = 7654
BUFFER_SIZE = 1024
#MESSAGE0="release_store disid=79:2000:1 target_disid=79:2000:2 #wpn_type=SL_MEDIUM_RNG infinite_count=true impact_value=true\0"

#MESSAGE0="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 wpn_disid=1:1:4 WPN_TAIL_NUMBER=4 lat=35.9 lon=-118 alt=10000 freq=8000 mid=4 infinite_count=true\0"
MESSAGE0="release_store disid=79:79:100 wpn_type=AIM120C CTRL_TRACK_NUM=1000 WPN_TAIL_NUMBER=4 lat=35.9 lon=-118 alt=10000 freq=8000 mid=4 infinite_count=true\0"


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
print "RELEASE_STORE"
s.send(MESSAGE0)
data = s.recv(BUFFER_SIZE)
print "recvd release_store | ", data

time.sleep(0.5)
s.close()

