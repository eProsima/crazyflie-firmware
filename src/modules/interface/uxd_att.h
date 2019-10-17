#define STREAM_HISTORY  8

#define MAX_TRANSPORT_MTU UXR_CONFIG_SERIAL_TRANSPORT_MTU
#define BUFFER_SIZE    MAX_TRANSPORT_MTU * STREAM_HISTORY

typedef struct Vector3
{
    float roll;
    float pitch;
    float yaw;
} Vector3;

typedef struct Vector3_odometry
{
    float x;
    float y;
    float z;
} Vector3_odo;

struct ucdrBuffer;

void uxd_att_init();
