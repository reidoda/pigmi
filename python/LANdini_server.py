import OSC #pyOSC
import time
import socket
import types
import netifaces as ni
import threading
import sys

c = OSC.OSCClient()

remote_ip = '192.168.1.4'

ni.ifaddresses('wlan0')
ip = ni.ifaddresses('wlan0')[2][0]['addr']
sc_port = 57120
server_name = "0_time_server"

broadcast_msg = OSC.OSCMessage()
broadcast_msg.setAddress("/landini/member/broadcast")
broadcast_msg.append(server_name)
broadcast_msg.append(ip)
broadcast_msg.append(str(sc_port))
broadcast_msg.append("0.23")

try:
    server = OSC.OSCServer( (ip, sc_port) )
except:
    print "port probably in use"

def handle_timeout(self):
    self.timed_out = True

def sync_callback(path, tags, args, source):
#     # response format: '/landini/sync/reply', me.name, theirTime, Main.elapsedTime
    ping_response = OSC.OSCMessage()
    ping_response.setAddress('/landini/sync/reply')
    ping_response.append(server_name)
    ping_response.append(args[1])
    ping_response.append(str(time.time()))
    c.sendto(ping_response, source)
    sys.stdout.flush()
    
def ping_callback(path, tags, args, source):
    pass

server.addMsgHandler( "/landini/sync/request", sync_callback )
server.addMsgHandler( "/landini/member/ping_and_msg_IDs", ping_callback )
server.handle_timeout = types.MethodType(handle_timeout, server)
server.timeout = 0
run = True

GD_ID = 0
broadcast_interval = .6
ping_interval = .3
last_broadcast = time.time()
last_ping = time.time()

st = threading.Thread( target = server.serve_forever )
st.start()

c.connect((remote_ip, sc_port))
c.socket.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
c.connect(('192.168.1.255',sc_port))

while run:
    #send broadcast
    if time.time() - last_broadcast > broadcast_interval:
        try:
#            c.sendto(broadcast_msg, ('192.168.1.255', sc_port))
            c.send(broadcast_msg)
        except:
            pass
        last_broadcast = time.time()

    if time.time() - last_ping > ping_interval:
        ping_msg = OSC.OSCMessage()
        ping_msg.setAddress('/landini/member/ping_and_msg_IDs')
        ping_msg.append('0_time_server') # me.name
        ping_msg.append('0') # userlist.me.x_pos
        ping_msg.append('0') # userlist.me.y_pos
        ping_msg.append(str(GD_ID)) # usr.last_outgoing_GD_ID
        ping_msg.append('-1') # usr.min_GD_ID
        ping_msg.append(str(GD_ID)) # usr.last_outgoing_OGD_ID
        ping_msg.append(str(GD_ID)) # usr.last_performed_OGD_ID
        ping_msg.append(server_name) #  sync_server_name
        try:
            c.sendto(ping_msg, (remote_ip, sc_port))
        except:
            pass
        GD_ID = GD_ID + 1
        last_ping = time.time()
        
    server.timed_out = False
    while not server.timed_out:
        server.handle_request()
