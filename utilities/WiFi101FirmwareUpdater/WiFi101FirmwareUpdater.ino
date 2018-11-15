// https://www.arduino.cc/en/Tutorial/FirmwareUpdater

#include <WiFi101.h>
#include <spi_flash/include/spi_flash.h>

typedef struct __attribute__((__packed__)) {
  uint8_t command;
  uint32_t address;
  uint32_t arg1;
  uint16_t payloadLength;
} UartPacket;

static const int MAX_PAYLOAD_SIZE = 1024;

#define CMD_READ_FLASH        0x01
#define CMD_WRITE_FLASH       0x02
#define CMD_ERASE_FLASH       0x03
#define CMD_MAX_PAYLOAD_SIZE  0x50
#define CMD_HELLO             0x99

void setup() {
    Serial.begin(115200);
    WiFi.setPins(8,7,4,2);   // pins for Adafruit ATWINC1500 Feather
    nm_bsp_init();
    if (m2m_wifi_download_mode() != M2M_SUCCESS) {
        Serial.println("Failed to put the WiFi module in download mode");
        while (true) ;
    }
}

void receivePacket(UartPacket *pkt, uint8_t *payload) {
    uint8_t *p = reinterpret_cast<uint8_t *>(pkt);
    uint16_t l = sizeof(UartPacket);
    while (l > 0) {
        int c = Serial.read();
        if (c == -1)
        continue;
        *p++ = c;
        l--;
    }
    pkt->address = fromNetwork32(pkt->address);
    pkt->arg1 = fromNetwork32(pkt->arg1);
    pkt->payloadLength = fromNetwork16(pkt->payloadLength);
    l = pkt->payloadLength;
    while (l > 0) {
        int c = Serial.read();
        if (c == -1)
            continue;
            *payload++ = c;
            l--;
    }
}
static UartPacket pkt;
static uint8_t payload[MAX_PAYLOAD_SIZE];

void loop() {
    receivePacket(&pkt, payload); 
    if (pkt.command == CMD_HELLO) {
        if (pkt.address == 0x11223344 && pkt.arg1 == 0x55667788)
        Serial.print("v10000");
    }
    if (pkt.command == CMD_MAX_PAYLOAD_SIZE) {
        uint16_t res = toNetwork16(MAX_PAYLOAD_SIZE);
        Serial.write(reinterpret_cast<uint8_t *>(&res), sizeof(res));
    }
    if (pkt.command == CMD_READ_FLASH) {
        uint32_t address = pkt.address;
        uint32_t len = pkt.arg1;
        if (spi_flash_read(payload, address, len) != M2M_SUCCESS) {
            Serial.println("ER");
        } 
        else {
            Serial.write(payload, len);
            Serial.print("OK");
        }
    }
    if (pkt.command == CMD_WRITE_FLASH) {
        uint32_t address = pkt.address;
        uint32_t len = pkt.payloadLength;
        if (spi_flash_write(payload, address, len) != M2M_SUCCESS) {
            Serial.print("ER");
        } 
        else {
            Serial.print("OK");
        }
    }
    if (pkt.command == CMD_ERASE_FLASH) {
        uint32_t address = pkt.address;
        uint32_t len = pkt.arg1;
        if (spi_flash_erase(address, len) != M2M_SUCCESS) {
            Serial.print("ER");
        } 
        else {
            Serial.print("OK");
        }
    }
}
