#ifndef EMU_REG_H
#define EMU_REG_H

/*--- EMU ADDRESS (BB LBIST) ------------------------*/
#define EMU_REG_BASEADDR                         0xB00000

#define EMU_REG_BB_LBIST_CLK_ENA                 (EMU_REG_BASEADDR+0x208)
#define EMU_REG_BB_LBIST_MODE                    (EMU_REG_BASEADDR+0x20c)
#define EMU_REG_BB_LBIST_ENA                     (EMU_REG_BASEADDR+0x210)
#define EMU_REG_BB_LBIST_CLR                     (EMU_REG_BASEADDR+0x214)

#define EMU_REG_LBIST_STA                        (EMU_REG_BASEADDR+0x504)
#define EMU_REG_LBIST_STA_FAIL_MASK              0x1
#define EMU_REG_LBIST_STA_FAIL_SHIFT             0x1

#define EMU_REG_LBIST_STA_DONE_MASK              0x1
#define EMU_REG_LBIST_STA_DONE_SHIFT             0x0

/*--- EMU VALUE (BB LBIST)------------------------*/
#define BB_LBIST_CLEAR                           0x4
#define DIG_ERR_CLEAR_ALL                        0x1fff

#endif
