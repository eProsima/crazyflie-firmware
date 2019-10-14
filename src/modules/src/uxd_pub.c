/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2011-2012 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * info.c - Receive information requests and send them back to the client
 */

#include <string.h>
#include <math.h>

/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

#include "crtp.h"
#include "info.h"
#include "version.h"
#include "pm.h"
#include "debug.h"

#include "crtp.h"

#include "syslink.h"
#include "radiolink.h"

#include "uxr/client/client.h"
#include "ucdr/microcdr.h"


#include <stdio.h> //printf
#include <string.h> //strcmp
#include <stdlib.h> //atoi
#include <fcntl.h>  // O_RDWR, O_NOCTTY, O_NONBLOCK

#include <stdint.h>
#include <stdbool.h>

uxrSession session;
uxrSerialTransport transport;
uxrSerialPlatform serial_platform;
/*!
 * @brief This struct represents the structure HelloWorld defined by the user in the IDL file.
 * @ingroup HELLOWORLD
 */
typedef struct HelloWorld
{
    uint32_t index;
    char message[255];

} HelloWorld;

struct ucdrBuffer;

#define STREAM_HISTORY  8

#define MAX_TRANSPORT_MTU UXR_CONFIG_SERIAL_TRANSPORT_MTU
#define BUFFER_SIZE    MAX_TRANSPORT_MTU * STREAM_HISTORY

static CRTPPacket messageToPrint;

void uxd_pub_Task(void *param);
bool HelloWorld_serialize_topic(ucdrBuffer* writer, const HelloWorld* topic);
bool HelloWorld_deserialize_topic(ucdrBuffer* reader, HelloWorld* topic);
uint32_t HelloWorld_size_of_topic(const HelloWorld* topic, uint32_t size);

void uxd_pub_Init()
{
  xTaskCreate(uxd_pub_Task, UXD_PUB_TASK_NAME,
              UXD_PUB_STACKSIZE, NULL, UXD_PUB__PRI, NULL);
  //crtpInitTaskQueue(crtpInfo);
}

void uxd_pub_Task(void *param)
{

  vTaskDelay(10000);
  if(!uxr_init_serial_transport(&transport, &serial_platform, 0, 0, 1))
    {

        return 1;
    }

    uxr_init_session(&session, &transport.comm, 0xAAAABBBB);
    //uxr_set_topic_callback(&session, on_topic, &count);
    if(!uxr_create_session(&session))
    {
        printf("Error at create session.\n");
        return 1;
    }

    uint8_t output_reliable_stream_buffer[BUFFER_SIZE];
    uxrStreamId reliable_out = uxr_create_output_reliable_stream(&session, output_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    uint8_t input_reliable_stream_buffer[BUFFER_SIZE];
    uxr_create_input_reliable_stream(&session, input_reliable_stream_buffer, BUFFER_SIZE, STREAM_HISTORY);

    // Create entities
    uxrObjectId participant_id = uxr_object_id(0x01, UXR_PARTICIPANT_ID);
    const char* participant_xml = "<dds>"
                                      "<participant>"
                                          "<rtps>"
                                              "<name>default_xrce_participant</name>"
                                          "</rtps>"
                                      "</participant>"
                                  "</dds>";
    uint16_t participant_req = uxr_buffer_create_participant_xml(&session, reliable_out, participant_id, 0, participant_xml, UXR_REPLACE);

    uxrObjectId topic_id = uxr_object_id(0x01, UXR_TOPIC_ID);
    const char* topic_xml = "<dds>"
                                "<topic>"
                                    "<name>HelloWorldTopic</name>"
                                    "<dataType>HelloWorld</dataType>"
                                "</topic>"
                            "</dds>";
    uint16_t topic_req = uxr_buffer_create_topic_xml(&session, reliable_out, topic_id, participant_id, topic_xml, UXR_REPLACE);

    uxrObjectId publisher_id = uxr_object_id(0x01, UXR_PUBLISHER_ID);
    const char* publisher_xml = "";
    uint16_t publisher_req = uxr_buffer_create_publisher_xml(&session, reliable_out, publisher_id, participant_id, publisher_xml, UXR_REPLACE);

    uxrObjectId datawriter_id = uxr_object_id(0x01, UXR_DATAWRITER_ID);
    const char* datawriter_xml = "<dds>"
                                     "<data_writer>"
                                         "<topic>"
                                             "<kind>NO_KEY</kind>"
                                             "<name>HelloWorldTopic</name>"
                                             "<dataType>HelloWorld</dataType>"
                                         "</topic>"
                                     "</data_writer>"
                                 "</dds>";
    uint16_t datawriter_req = uxr_buffer_create_datawriter_xml(&session, reliable_out, datawriter_id, publisher_id, datawriter_xml, UXR_REPLACE);

    // Send create entities message and wait its status
    uint8_t status[4];
    uint16_t requests[4] = {participant_req,topic_req,publisher_req,datawriter_req};
    if(!uxr_run_session_until_one_status(&session, 1000, requests, status, 4))
    {
        //printf("Error at create entities: participant: %i topic: %i publisher: %i darawriter: %i\n", status[0], status[1], status[2], status[3]);
        return 1;
    }

    // Write topics
    uint32_t count_0 = 0;


  while(1){
    HelloWorld topic = {++count_0, "Hello DDS world!"};

     ucdrBuffer ub;
     uint32_t topic_size = HelloWorld_size_of_topic(&topic, 0);
     uxr_prepare_output_stream(&session, reliable_out, datawriter_id, &ub, topic_size);
     HelloWorld_serialize_topic(&ub, &topic);

     //printf("Send topic: %s, id: %i\n", topic.message, topic.index);
     uxr_run_session_time(&session, 1000);
    vTaskDelay(100);

  }

}

bool HelloWorld_serialize_topic(ucdrBuffer* writer, const HelloWorld* topic)
{
    (void) ucdr_serialize_uint32_t(writer, topic->index);

    (void) ucdr_serialize_string(writer, topic->message);

    return !writer->error;
}

bool HelloWorld_deserialize_topic(ucdrBuffer* reader, HelloWorld* topic)
{
    (void) ucdr_deserialize_uint32_t(reader, &topic->index);

    (void) ucdr_deserialize_string(reader, topic->message, 255);

    return !reader->error;
}

uint32_t HelloWorld_size_of_topic(const HelloWorld* topic, uint32_t size)
{
    uint32_t previousSize = size;
    size += (uint32_t)(ucdr_alignment(size, 4) + 4);

    size += (uint32_t)(ucdr_alignment(size, 4) + 4 + strlen(topic->message) + 1);

    return size - previousSize;
}
