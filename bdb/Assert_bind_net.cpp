#include <stdio.h>;
#include <string.h>;
#include <sys/socket.h>;
#include <netpacket/packet.h>;
#include <net/ethernet.h>;
#include <sys/ioctl.h>;
#include <net/if.h>;
#include <assert.h>;


int Assert_bind_net ()
{
	int sock;
	struct ifreq ifr;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	memset(&ifr, 0x00, sizeof(ifr));
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZE);
	setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));

        struct sockaddr_ll sll;
        int fd;
        struct ifreq ifr;
        char *dev;

        fd = socket (PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

        dev = "eth0";
        strncpy ((char *)ifr.ifr_name, dev, sizeof(ifr.ifr_name));
        assert (ioctl (fd, SIOCGIFINDEX, &ifr)==0);

        memset (&sll, 0, sizeof(sll));
        sll.sll_family = AF_PACKET;
        sll.sll_protocol = htons (ETH_P_ALL);
        sll.sll_ifindex = ifr.ifr_ifindex;

        assert (bind (fd, (struct sockaddr *)&sll, sizeof(sll))==0);

}

