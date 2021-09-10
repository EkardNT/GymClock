#ifndef GYMCLOCK_NTP_H
#define GYMCLOCK_NTP_H

void initializeNtp();
void shutdownNtp();
void updateNtp();
unsigned long getEpochTime();
int getHours(unsigned long epochTime) const;
int getMinutes(unsigned long epochTime) const;
int getSeconds(unsigned long epochTime) const;

#endif