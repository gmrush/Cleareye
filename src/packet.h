//George Rush 20222 | github.com/gmrush

#pragma once
#ifndef PACKET_H
#define PACKET_H

/*  Connection packet
    Any connections that do not send this first are rejected,
    verifies that the connection is coming from another Cleareye client
*/

const uint8_t packetConnect[16] = 
    {
        0x1, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA,
        0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA, 0xA
    };

#endif //PACKET_H
