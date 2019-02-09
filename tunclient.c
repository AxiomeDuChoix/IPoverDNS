//Header Files
#include <stdio.h> //printf
#include <string.h> //strlen
#include <stdlib.h> //malloc
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr , inet_ntoa , ntohs etc
#include <netinet/in.h>
#include <unistd.h> //getpid
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

int tun_alloc(char *dev, int flags);

int main(int args, char* argv[])
  {
    char tun_name[IFNAMSIZ], buffer[2048];
    
    /* Connect to the device */
    strcpy(tun_name, "tap0");
    int tun_fd = tun_alloc(tun_name, IFF_TAP);  /* tun interface */

    if(tun_fd < 0){
      perror("Allocating interface");
      exit(1);
    }

    int nread;
    
    /* Now read data coming from the kernel */
    while(1) {
      /* Note that "buffer" should be at least the MTU size of the interface, eg 1500 bytes */
      nread = read(tun_fd,buffer,sizeof(buffer));
      if(nread < 0) {
        perror("Reading from interface");
        close(tun_fd);
        exit(1);
      }

      /* Do whatever with the data */
      printf("Read %d bytes from device %s\n", nread, tun_name);
    }
    return 0;
  }

int tun_alloc(char *dev, int flags) {
  struct ifreq ifr;
  int fd, err;

  if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
    perror("Opening /dev/net/tun");
    return fd;
  }

  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags = flags;

  if (*dev) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
    perror("ioctl(TUNSETIFF)");
    close(fd);
    return err;
  }

  strcpy(dev, ifr.ifr_name);

  return fd;
}