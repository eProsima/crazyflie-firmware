#define STREAM_HISTORY  8

#define MAX_TRANSPORT_MTU UXR_CONFIG_SERIAL_TRANSPORT_MTU
#define BUFFER_SIZE    MAX_TRANSPORT_MTU * STREAM_HISTORY

typedef struct Vector3
{
    double x;
    double y;
    double z;
} Vector3;

struct ucdrBuffer;

void uxd_att_init();
