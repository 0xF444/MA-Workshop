#include <stdio.h>
#include <pcap.h>

    void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data) {
        printf("Packet captured: %d bytes\n", header->len);
    }

    int main() {
        pcap_if_t *alldevs;
        pcap_if_t *d;
        pcap_t *adhandle;
        char errbuf[PCAP_ERRBUF_SIZE];

        if (pcap_findalldevs(&alldevs, errbuf) == -1) {
            printf("Error in pcap_findalldevs: %s\n", errbuf);
            return 1;
        }

        for (d = alldevs; d != NULL; d = d->next) {
            printf("%d. %s\n", d->next, (d->description) ? d->description : "No description available");
        }

        if ((adhandle = pcap_open_live(alldevs->name, 65536, 1, 1000, errbuf)) == NULL) {
            printf("Unable to open the adapter. %s is not supported by pcap\n", alldevs->name);
            return 1;
        }

        pcap_loop(adhandle, 0, packet_handler, NULL);

        pcap_freealldevs(alldevs);
        return 0;
    }