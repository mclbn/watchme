#ifndef NTP_TIME_H
#define NTP_TIME_H

#include <NTPClient.h>
#include <Timezone.h>
#include "watchme.h"

void update_rtc_from_ntp(time_t ntp_time);
time_t get_ntp_time(void);

#endif // NTP_TIME_H
