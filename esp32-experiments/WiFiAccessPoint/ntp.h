#ifndef GYMCLOCK_NTP_H
#define GYMCLOCK_NTP_H

#include <AceTime.h>

void initializeNtp();
void shutdownNtp();
void updateNtp();
bool isNtpTimeSet();
ace_time::ZonedDateTime getLocalTime();

#endif