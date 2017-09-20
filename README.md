Project includes implementation of 'mutex' and 'condition variables' server for minix.

Installation:
- copy files to minix machine

Execute as root:
- cd /usr/src; make includes; make do-lib
- cd /usr/src/releasetools; make services; make install
- cp /usr/src/etc/usr/rc /usr/etc
- reboot
