#ifndef _ALPS_LVDS_H_
#define _ALPS_LVDS_H_

#define LENR                (0x00)
#define BB_DAT_MUX          (0X04)
#define DUMP_MUX            (0x08)
#define OUTPUT_MUX          (0x0c)
#define LVDS_EN             (0x10)
#define LVDS_FRAME          (0x14)
#define LVDS_BIT_ORDER      (0x18)
#define DMACR               (0x1c)
#define DMATDLR             (0x20)
#define TXFLR               (0x24)
#define TXFTLR              (0x28)
#define SR                  (0x2c)
#define VIDR                (0x30)
#define IMR                 (0x34)
#define ISR                 (0x38)
#define RISR                (0x3c)
#define TXOICR              (0x40)
#define DRx                 (0x60)

#define LENR_ON             (0x1)
#define LENR_OFF            (0x0)

#define BB_MUX_DEF          (0x0)
#define BB_MUX_SAM          (0x1)
#define BB_MUX_CFR          (0x2)
#define BB_MUX_BFM          (0x3)
#define BB_MUX_DUMP         (0x4)

#define DUMP_MUX_DEF        (0x0)
#define DUMP_MUX_BB         (0x1)
#define DUMP_MUX_APB        (0x2)


#define OUTPUT_MUX_FIL      (0x0)
#define OUTPUT_MUX_APB      (0x1)

#define LVDS_EN_ON          (0x1)
#define LVDS_EN_OFF         (0x0)

#define LVDS_FRAME_POS      (0x0)
#define LVDS_FRAME_NEG      (0x1)

#define LVDS_BIT_ORDER_POS  (0x0)
#define LVDS_BIT_ORDER_NEG  (0x1)


#endif
