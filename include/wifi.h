#ifndef WIFI_H
#define WIFI_H

#include "watchme.h"

typedef struct s_wifi_hotspot {
	const char * ssid;
	const char * pass;
} t_wifi_hotspot;

bool connectWiFi(void);
void check_wireless_interfaces(void);
bool check_for_known_ap(void);

extern t_wifi_hotspot know_wifi_hotspots[];

#endif // WIFI_H

