#ifndef _CAN_BAUD_DESC_H_
#define _CAN_BAUD_DESC_H_

#define CAN_BAUD_DESC_10MHZ_100KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 3, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_10MHZ_200KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 1, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_10MHZ_250KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 3, .segment1 = 4, .segment2 = 3}
#define CAN_BAUD_DESC_10MHZ_500KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 1, .segment1 = 4, .segment2 = 3}
#define CAN_BAUD_DESC_10MHZ_1MBPS     {.sync_jump_width = 0, .bit_rate_prescale = 0, .segment1 = 4, .segment2 = 3}

#define CAN_BAUD_DESC_20MHZ_100KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 7, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_20MHZ_200KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 3, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_20MHZ_250KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 7, .segment1 = 4, .segment2 = 3}
#define CAN_BAUD_DESC_20MHZ_500KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 3, .segment1 = 4, .segment2 = 3}
#define CAN_BAUD_DESC_20MHZ_1MBPS     {.sync_jump_width = 0, .bit_rate_prescale = 1, .segment1 = 4, .segment2 = 3}
#define CAN_BAUD_DESC_20MHZ_2MBPS     {.sync_jump_width = 0, .bit_rate_prescale = 0, .segment1 = 4, .segment2 = 3}

#define CAN_BAUD_DESC_40MHZ_100KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 15, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_40MHZ_200KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 7, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_40MHZ_250KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 15, .segment1 = 4, .segment2 = 3}
#define CAN_BAUD_DESC_40MHZ_500KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 7, .segment1 = 5, .segment2 = 2}
#define CAN_BAUD_DESC_40MHZ_1MBPS     {.sync_jump_width = 0, .bit_rate_prescale = 3, .segment1 = 5, .segment2 = 2}
#define CAN_BAUD_DESC_40MHZ_2MBPS     {.sync_jump_width = 0, .bit_rate_prescale = 1, .segment1 = 6, .segment2 = 1}
#define CAN_BAUD_DESC_40MHZ_4MBPS     {.sync_jump_width = 0, .bit_rate_prescale = 0, .segment1 = 3, .segment2 = 4}

#define CAN_BAUD_DESC_50MHZ_100KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 19, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_50MHZ_200KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 9, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_50MHZ_250KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 7, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_50MHZ_500KBPS   {.sync_jump_width = 0, .bit_rate_prescale = 3, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_50MHZ_1MBPS     {.sync_jump_width = 0, .bit_rate_prescale = 1, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_50MHZ_2MBPS     {.sync_jump_width = 0, .bit_rate_prescale = 0, .segment1 = 11, .segment2 = 11}
#define CAN_BAUD_DESC_50MHZ_4MBPS     {.sync_jump_width = 0, .bit_rate_prescale = 0, .segment1 = 5, .segment2 = 4}

// 80% sampling point
#define CAN_BAUD_DESC_100MHZ_100KBPS  {.sync_jump_width = 0, .bit_rate_prescale = 39, .segment1 = 18, .segment2 = 4}
#define CAN_BAUD_DESC_100MHZ_200KBPS  {.sync_jump_width = 0, .bit_rate_prescale = 19, .segment1 = 18, .segment2 = 4}
#define CAN_BAUD_DESC_100MHZ_250KBPS  {.sync_jump_width = 0, .bit_rate_prescale = 15, .segment1 = 18, .segment2 = 4}
#define CAN_BAUD_DESC_100MHZ_500KBPS  {.sync_jump_width = 0, .bit_rate_prescale = 7, .segment1 = 18, .segment2 = 4} 
#define CAN_BAUD_DESC_100MHZ_1MBPS    {.sync_jump_width = 0, .bit_rate_prescale = 3, .segment1 = 18, .segment2 = 4}
#define CAN_BAUD_DESC_100MHZ_2MBPS    {.sync_jump_width = 0, .bit_rate_prescale = 1, .segment1 = 18, .segment2 = 4}
#define CAN_BAUD_DESC_100MHZ_4MBPS    {.sync_jump_width = 0, .bit_rate_prescale = 0, .segment1 = 18, .segment2 = 4}
#define CAN_BAUD_DESC_100MHZ_5MBPS    {.sync_jump_width = 0, .bit_rate_prescale = 0, .segment1 = 14, .segment2 = 3} 
#endif
