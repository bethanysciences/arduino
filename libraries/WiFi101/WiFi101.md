typedef enum {
	WL_NO_SHIELD = 255,
	WL_IDLE_STATUS = 0,
	WL_NO_SSID_AVAIL,
	WL_SCAN_COMPLETED,
	WL_CONNECTED,
	WL_CONNECT_FAILED,
	WL_CONNECTION_LOST,
	WL_DISCONNECTED,
	WL_AP_LISTENING,
	WL_AP_CONNECTED,
	WL_AP_FAILED,
	WL_PROVISIONING,
	WL_PROVISIONING_FAILED
} wl_status_t;


enum wl_enc_type {
	ENC_TYPE_WEP  = M2M_WIFI_SEC_WEP,
	ENC_TYPE_TKIP = M2M_WIFI_SEC_WPA_PSK,
	ENC_TYPE_CCMP = M2M_WIFI_SEC_802_1X,
	ENC_TYPE_NONE = M2M_WIFI_SEC_OPEN,
	ENC_TYPE_AUTO = M2M_WIFI_SEC_INVALID
};

typedef enum {
	WL_RESET_MODE = 0,
	WL_STA_MODE,
	WL_PROV_MODE,
	WL_AP_MODE
} wl_mode_t;

typedef enum {
	WL_PING_DEST_UNREACHABLE = -1,
	WL_PING_TIMEOUT = -2,
	WL_PING_UNKNOWN_HOST = -3,
	WL_PING_ERROR = -4
} wl_ping_result_t;

	uint32_t _localip;
	uint32_t _submask;
	uint32_t _gateway;
	int _dhcp;
	uint32_t _resolve;
	byte *_remoteMacAddress;
	wl_mode_t _mode;
	wl_status_t _status;
	char _scan_ssid[M2M_MAX_SSID_LEN];
	uint8_t _scan_auth;
	uint8_t _scan_channel;
	char _ssid[M2M_MAX_SSID_LEN];
	WiFiClient *_client[TCP_SOCK_MAX];
	WiFiClass();
	void setPins(int8_t cs, int8_t irq, int8_t rst, int8_t en = -1);
	int init();
	char* firmwareVersion();
	uint8_t *macAddress(uint8_t *mac);
	uint32_t localIP();
	uint32_t subnetMask();
	uint32_t gatewayIP();
	char* SSID();
	int32_t RSSI();
	uint8_t encryptionType();
	uint8_t* BSSID(uint8_t* bssid);
	uint8_t* APClientMacAddress(uint8_t* mac);
	int8_t scanNetworks();
	char* SSID(uint8_t pos);
	int32_t RSSI(uint8_t pos);
	uint8_t encryptionType(uint8_t pos);
	uint8_t* BSSID(uint8_t pos, uint8_t* bssid);
	uint8_t channel(uint8_t pos);
	uint8_t status();
	int hostByName(const char* hostname, IPAddress& result);
	int hostByName(const String &hostname, IPAddress& result) { return hostByName(hostname.c_str(), result); }
	int ping(const char* hostname, uint8_t ttl = 128);
	int ping(const String &hostname, uint8_t ttl = 128);
	int ping(IPAddress host, uint8_t ttl = 128);
	unsigned long getTime();
	void refresh(void);
	void lowPowerMode(void);
	void maxLowPowerMode(void);
	void noLowPowerMode(void);

