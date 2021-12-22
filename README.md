# OCC (Orlaco Camera Configurator)

This application can be used to configure some aspects of Orlaco EMOS IP Cameras.

### Background

I was working on a project that required multiple rugged IP cameras and
ended up ordering some Orlaco EMOS IP cameras. These looked ideal for the
application, but I didn't really know much about them in advance.

When they arrived, I found that configuring them required a PC application
that Orlaco themselves don't supply (or if they do, I've not been able to find it).
So, after some research I discovered they use ISO 17215 protocol which sits on top
of SOME/IP protocol, and that configuration utilities are available commercially.
I obtained a quote for one such utility that came in at around 1000 Euros.
Rather a lot I thought, and far more than we wanted to pay, so as any self
respecting software engineer would do, with the help of Google and Wireshark,
I knocked up this utility to allow various settings to be changed.

Note: I don't have access to ISO17215, I've only seen the snippets that are
available for free on the various websites selling it, as such there are some
features that I don't have documentation for. If anyone has
access to a commercial configuration utility and a copy of wireshark, logs
and more info would be very welcome so that more features can be completed.

Hopefully this will be useful to someone, please let me know if so!

### Binaries
I've included a binary built for 64 bit Windows. Linux binaries
are not included, but I have tested that it builds and works under Ubuntu.


### Use
Open a command prompt and type something like this:

occ.exe --help

This will give you a list of command line options.

### Examples

#### Discovering cameras on a network:

.\occ.exe -d 192.168.2.255
Found 2 devices
0: IP=192.168.2.10 Type=01 ServiceID=433f InstanceID=000a V=1.0
1: IP=192.168.2.11 Type=01 ServiceID=433f InstanceID=000b V=1.0

#### Read all registers on the camera with IP address 192.168.2.10

.\occ.exe -R : -i 192.168.2.10
Read registers 0 to 64

Registers
Index   Address Hex     Decimal Ascii   Name
00      0xb00c  0x01      1             LED Mode
01      0xb041  0x00      0             Stream Protocol
02      0xb042  0xc0    192             IP Address 0
03      0xb043  0xa8    168             IP Address 1
04      0xb044  0x02      2             IP Address 2
05      0xb045  0x0a     10             IP Address 3
06      0xb046  0xff    255             Network Mask 0
07      0xb047  0xff    255             Network Mask 1
08      0xb048  0xff    255             Network Mask 2
09      0xb049  0x00      0             Network Mask 3
10      0xb04a  0x70    112     p       MAC Address 0
11      0xb04b  0xb3    179             MAC Address 1
12      0xb04c  0xd5    213             MAC Address 2
13      0xb04d  0x4f     79     O       MAC Address 3
14      0xb04e  0x53     83     S       MAC Address 4
15      0xb04f  0x34     52     4       MAC Address 5
16      0xb055  0x00      0             VLAN ID 0
17      0xb056  0x00      0             VLAN ID 1
18      0xb057  0x45     69     E       Stream ID 0
19      0xb058  0x4d     77     M       Stream ID 1
20      0xb059  0x4f     79     O       Stream ID 2
21      0xb05a  0x53     83     S       Stream ID 3
22      0xb05b  0x20     32             Stream ID 4
23      0xb05c  0x20     32             Stream ID 5
24      0xb05d  0x20     32             Stream ID 6
25      0xb05e  0x20     32             Stream ID 7
26      0xb05f  0xc0    192             Destination IP Address 0
27      0xb060  0xa8    168             Destination IP Address 1
28      0xb061  0x02      2             Destination IP Address 2
29      0xb062  0x01      1             Destination IP Address 3
30      0xb063  0xff    255             Destination MAC Address 0
31      0xb064  0xff    255             Destination MAC Address 1
32      0xb065  0xff    255             Destination MAC Address 2
33      0xb066  0xff    255             Destination MAC Address 3
34      0xb067  0xff    255             Destination MAC Address 4
35      0xb068  0xff    255             Destination MAC Address 5
36      0xb069  0xc3    195             Destination Port 0
37      0xb06a  0x54     84     T       Destination Port 1
38      0xb06b  0x01      1             Selected ROI
39      0xb06c  0x00      0             No Stream At Boot ?
40      0xb06d  0x43     67     C       UDP Communication Port 0
41      0xb06e  0x3f     63     ?       UDP Communication Port 1
42      0xb06f  0xc3    195             RTP Stream Source Port 0
43      0xb070  0x54     84     T       RTP Stream Source Port 1
44      0xb071  0x01      1             HDR ?
45      0xb072  0x01      1             Overlay ?
46      0xb073  0x00      0             DHCP Enabled ?
47      0xb078  0x00      0             Wait For MAC ?
48      0xb079  0x00      0             Wait For PTP Sync ?
49      0xb171  0x4f     79     O       DHCP Hostname 0
50      0xb172  0x72    114     r       DHCP Hostname 1
51      0xb173  0x6c    108     l       DHCP Hostname 2
52      0xb174  0x61     97     a       DHCP Hostname 3
53      0xb175  0x63     99     c       DHCP Hostname 4
54      0xb176  0x6f    111     o       DHCP Hostname 5
55      0xb177  0x20     32             DHCP Hostname 6
56      0xb178  0x45     69     E       DHCP Hostname 7
57      0xb179  0x4d     77     M       DHCP Hostname 8
58      0xb17a  0x4f     79     O       DHCP Hostname 9
59      0xb17b  0x53     83     S       DHCP Hostname 10
60      0xb17c  0x20     32             DHCP Hostname 11
61      0xb17d  0x20     32             DHCP Hostname 12
62      0xb17e  0x20     32             DHCP Hostname 13
63      0xb17f  0x20     32             DHCP Hostname 14
64      0xb180  0x20     32             DHCP Hostname 15

#### Read the LED Mode register

.\occ.exe -r 0 -i 192.168.2.10
Read register 0

Registers
Index   Address Hex     Decimal Ascii   Name
00      0xb00c  0x01      1             LED Mode


#### Write the LED Mode register

.\occ -w 0=0 -i 192.168.2.10
Write register 0 value 00

Registers
Index   Address Hex     Decimal Ascii   Name
00      0xb00c  0x00      0             LED Mode


#### Read all regions of interest

 .\occ.exe -G :   
Read ROI's 1 to 10

Regions Of Interest
ROI     P1X     P1Y     P2X     P2Y     Width   Height  Mbps    Fps     Mode
1       0       0       1280    960     1280    960     5       25      2
2       0       0       1280    960     1280    960     5       25      1
3       0       960     1280    0       1280    960     50      30      1
4       0       0       1280    960     800     600     50      30      1
5       1280    0       0       960     800     600     50      30      1
6       0       960     1280    0       800     600     50      30      1
7       0       0       1280    960     640     480     50      30      1
8       1280    0       0       960     640     480     50      30      1
9       0       960     1280    0       640     480     50      30      1
10      1280    960     0       0       1280    960     50      30      1

#### Configure region of interest 10

 .\occ.exe -s 10=1280,960,0,0,1280,960,60,25,2

Regions Of Interest
ROI     P1X     P1Y     P2X     P2Y     Width   Height  Mbps    Fps     Mode
10      1280    960     0       0       1280    960     60      25      2

