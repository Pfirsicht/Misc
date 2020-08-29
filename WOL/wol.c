#include "wol.h"

int main(int argc, char *argv[]) {


    // initialize winsock if on windows
    #if defined(_WIN32)
        WSADATA d;
        if (WSAStartup(MAKEWORD(2, 2), &d)) {
            return 1;
        }
    #endif


    // buffer for target and mac_addr; maximum size of FQDN is 255 characters and size of mac is always 12 characters
    unsigned char target_buff[256], mac_buff[13];
    memset(target_buff, 0, sizeof(target_buff)/sizeof(target_buff[0]));
    memset(mac_buff, 0, sizeof(mac_buff)/sizeof(mac_buff[0]));


    // process (optional) command line arguments
    if(argc < 2) {
        print_invalidcall();
        return 1;
    }
    if(argc == 2) {
        if(!is_macaddr(argv[1])) {
            print_invalidcall();
            return 1;
        }
        memcpy(target_buff, "255.255.255.255", 15);
        memcpy(mac_buff, argv[1], strlen(argv[1]));
    }
    if(argc == 3) {
        int tar_given = 0;
        int mac_given = 0;
        for (int i = 1; i < argc; i++) {
            if (strncmp(argv[i], "/tar=", 5) == 0) {
                tar_given = 1;
                memcpy(target_buff, &argv[i][5], strlen(argv[i]) - 5);
                if (!is_valid_target(target_buff)) {
                    print_invalidcall();
                    return 1;
                }
            }
            else {
                mac_given = 1;
                if (!is_macaddr(argv[i])) {
                    print_invalidcall();
                    return 1;
                }
                memcpy(mac_buff, &argv[i][0], strlen(argv[i]));
            }
        }
        if(!(tar_given || mac_given)) {
            print_invalidcall();
            return 1;
        }
    }
    if(argc > 3) {
        print_invalidcall();
        return 1;
    }
    

    // create magic packet
    unsigned char* magic_packet;
    magic_packet = (unsigned char*) malloc(MAG_PKG_LEN * sizeof(unsigned char));
    if(magic_packet == NULL) {
        return 1;
    }
    memset(magic_packet, 0, MAG_PKG_LEN);
    for (int j = 0; j < 6; j++) {
        magic_packet[j] = 0xFF;
    }
    unsigned char mac_hex[6];
    memset(mac_hex, 0, sizeof(mac_hex)/sizeof(mac_hex[0]));
    unsigned char* tmp_pos1;
    unsigned char* tmp_pos2;
    tmp_pos1 = mac_buff;
    tmp_pos2 = mac_hex;
    while(*tmp_pos1) {
        char hex[2];
        hex[0] = *tmp_pos1;
        hex[1] = *(++tmp_pos1);
        *tmp_pos2 = (unsigned char)strtol(hex, NULL, 16);
        tmp_pos1++;
        tmp_pos2++;
    }
    for(short int j = 1; j <= 16; j++) {
        memcpy(&magic_packet[j * 6], mac_hex, 6 * sizeof(unsigned char));
    }


    // configure remote address with user input
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_ADDRCONFIG;
    struct addrinfo* remote_address;
    if(getaddrinfo(target_buff, "9", &hints, &remote_address)) {
        free(magic_packet);
        return 1;
    }


    // create udp socket
    SOCKET socket_udp;
    socket_udp = socket(remote_address->ai_family, remote_address->ai_socktype, remote_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_udp)) {
        free(magic_packet);
        freeaddrinfo(remote_address);
        return 1;
    }


    // configure socket to allow ethernet broadcasts
    int optval;
    optval = 1;
    if (setsockopt(socket_udp, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(optval))) {
        free(magic_packet);
        freeaddrinfo(remote_address);
        CLOSESOCKET(socket_udp);
        return 1;
    }


    // send magic_packet over socket
    int bytes_sent;
    bytes_sent = sendto(socket_udp, magic_packet, MAG_PKG_LEN, 0, remote_address->ai_addr, remote_address->ai_addrlen);
    printf("%i bytes sent.\n", bytes_sent);


    // cleanup
    free(magic_packet);
    freeaddrinfo(remote_address);
    CLOSESOCKET(socket_udp);


    // cleanup winsock if on windows
    #if defined(_WIN32)
        WSACleanup();
    #endif


    // return OK
    return 0;


}


void print_invalidcall(void) {
    fprintf(stderr, "%s\n", "---- simple wol cmd program ----");
    fprintf(stderr, "%s\n", "Syntax:");
    fprintf(stderr, " >>\t%s\n", "wol.exe mac-address");
    fprintf(stderr, " >>\t%s\n", "wol.exe mac-address /tar=valid-target");
    fprintf(stderr, " %s\n", "valid target can be any valid ip-address or hostname; if switch is omitted, ethernet broadcast is assumed");
    fprintf(stderr, "%s\n", "Examples:");
    fprintf(stderr, " >>\t%s\n", "wol.exe 0019ef3ecc0b");
    fprintf(stderr, " >>\t%s\n", "wol.exe 3e77aa00317d /tar=192.168.10.255");
}


int is_valid_target(unsigned char* tar) {
    if (!tar) {
        return 0;
    }
    struct addrinfo testhints;
    memset(&testhints, 0, sizeof(testhints));
    testhints.ai_family = AF_UNSPEC;
    testhints.ai_socktype = SOCK_DGRAM;
    testhints.ai_flags = AI_ADDRCONFIG;
    struct addrinfo* testbuffer;
    if(getaddrinfo(tar, "9", &testhints, &testbuffer)) {
        return 0;
    }
    freeaddrinfo(testbuffer);
    return 1;
}


int is_macaddr(unsigned char* arg_mac) {
    if (!arg_mac) {
        return 0;
    }
    if (strlen(arg_mac) != 12) {
        return 0;
    }
    while (*arg_mac) {
        if (!((*arg_mac >= '0' && *arg_mac <= '9') || (*arg_mac >= 'a' && *arg_mac <= 'f') || (*arg_mac >= 'A' && *arg_mac <= 'F'))) {
            return 0;
        }
        else {
            arg_mac++;
        }
    }
    return 1;
}
