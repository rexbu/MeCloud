import socket

hostname = socket.getfqdn(socket.gethostname())
print 'hostname: %s' % hostname
