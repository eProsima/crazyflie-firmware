#include <uxr/client/util/time.h>
#include "FreeRTOS.h"
#include "task.h"

#ifdef WIN32
#include <Windows.h>
#endif

//==================================================================
//                             PUBLIC
//==================================================================
int64_t uxr_millis(void)
{
  uint64_t time = usecTimestamp();
  return time/1000;
}

int64_t uxr_nanos(void)
{
#ifdef WIN32
    SYSTEMTIME epoch_tm = {1970, 1, 4, 1, 0, 0, 0, 0};
    FILETIME epoch_ft;
    SystemTimeToFileTime(&epoch_tm, &epoch_ft);
    uint64_t epoch_time = (((uint64_t) epoch_ft.dwHighDateTime) << 32) + epoch_ft.dwLowDateTime;

    SYSTEMTIME tm;
    FILETIME ft;
    GetSystemTime(&tm);
    SystemTimeToFileTime(&tm, &ft);
    uint64_t current_time = (((uint64_t) ft.dwHighDateTime) << 32) + ft.dwLowDateTime;

    return (current_time - epoch_time) * 100;
#else
  uint64_t time = usecTimestamp();
  return time * 1000;
#endif
}
