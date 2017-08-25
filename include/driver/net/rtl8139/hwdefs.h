/*
 * Part of P-OS kernel
 * Created: 22-08-2017 by Peter Bosch
 */

#ifndef RTL8193_HWDEFS
#define RTL8193_HWDEFS
#include <stdint.h>

#define RTL8139_PCI_VID		(0x10EC)
#define RTL8139_PCI_PID		(0x8139)

/* Hardware register addresses */
#define RTL8139_REG_IDR(i)	(0x0000+i)
#define RTL8139_REG_MAR(i)	(0x0008+i)
#define RTL8139_REG_TSD(i)	(0x0010+i*0x4)
#define RTL8139_REG_TSAD(i)	(0x0020+i*0x4)
#define RTL8139_REG_RBSTART	(0x0030)
#define RTL8139_REG_ERBCR	(0x0034)
#define RTL8139_REG_ERSR	(0x0036)
#define RTL8139_REG_CR		(0x0037)
#define RTL8139_REG_CAPR	(0x0038)
#define RTL8139_REG_CBR		(0x003A)
#define RTL8139_REG_IMR		(0x003C)
#define RTL8139_REG_ISR		(0x003E)
#define RTL8139_REG_TCR		(0x0040)
#define RTL8139_REG_RCR		(0x0044)
#define RTL8139_REG_TCTR	(0x0048)
#define RTL8139_REG_MPC		(0x004C)
#define RTL8139_REG_9346CR	(0x0050)
#define RTL8139_REG_CONFIG0	(0x0051)
#define RTL8139_REG_CONFIG1	(0x0052)
#define RTL8139_REG_TimerInt	(0x0054)
#define RTL8139_REG_MSR		(0x0058)
#define RTL8139_REG_CONFIG3	(0x0059)
#define RTL8139_REG_CONFIG4	(0x005A)
#define RTL8139_REG_MULINT	(0x005C)
#define RTL8139_REG_TSAD	(0x0060)
#define RTL8139_REG_BMCR	(0x0062)
#define RTL8139_REG_BMSR	(0x0064)
#define RTL8139_REG_ANAR	(0x0066)
#define RTL8139_REG_ANLPAR	(0x0068)
#define RTL8139_REG_ANER	(0x006A)
#define RTL8139_REG_DIS		(0x006C)
#define RTL8139_REG_FCSC	(0x006E)
#define RTL8139_REG_NWAYTR	(0x0070)
#define RTL8139_REG_REC		(0x0072)
#define RTL8139_REG_CSCR	(0x0074)
#define RTL8139_REG_PHY1_PARM	(0x0078)
#define RTL8139_REG_TW_PARM	(0x007C)
#define RTL8139_REG_PHY2_PARM	(0x0080)
#define RTL8139_REG_CRC(n)	(0x0084+n)
#define RTL8139_REG_Wakeup(n)	(0x008C+n)
#define RTL8139_REG_LSBCRC(n)	(0x00CC+n)
#define RTL8139_REG_CONFIG5	(0x00D8)
#define RTL8139_REG_REC	(0x0064)

/* TSD register bit definitions */
#define RTL8139_TSD_CRS		(1<<31)
#define RTL8139_TSD_TABT	(1<<30)
#define RTL8139_TSD_OWC		(1<<29)
#define RTL8139_TSD_CDH		(1<<28)
#define RTL8139_TSD_NCC_R(v)	((v>>24)&0xF)
#define RTL8139_TSD_ERTXTH_R(v)	((v>>16)&0x1F)
#define RTL8139_TSD_ERTXTH_W(v)	((v<<16)&0x1F0000)
#define RTL8139_TSD_TOK		(1<<15)
#define RTL8139_TSD_TUN		(1<<14)
#define RTL8139_TSD_OWN		(1<<13)
#define RTL8139_TSD_SIZE_R(v)	(v&0x1FFF)
#define RTL8139_TSD_SIZE_W(v)	(v&0x1FFF)

/* RSR register bit definitions */
#define RTL8139_RSR_MAR		(1<<15)
#define RTL8139_RSR_PAM		(1<<14)
#define RTL8139_RSR_BAR		(1<<13)
#define RTL8139_RSR_ISE		(1<<5)
#define RTL8139_RSR_RUNT	(1<<4)
#define RTL8139_RSR_LONG	(1<<3)
#define RTL8139_RSR_CRC		(1<<2)
#define RTL8139_RSR_FAE		(1<<1)
#define RTL8139_RSR_ROK		(1<<0)

/* ERSR register bit definitions */
#define RTL8139_ERSR_ERGood	(1<<3)
#define RTL8139_ERSR_ERBad	(1<<2)
#define RTL8139_ERSR_EROVW	(1<<1)
#define RTL8139_ERSR_EROK	(1<<0)

/* CR register bit definitions */
#define RTL8139_CR_RST		(1<<4)
#define RTL8139_CR_RE		(1<<3)
#define RTL8139_CR_TE		(1<<2)
#define RTL8139_CR_BUFE		(1<<0)

/* IM/SR register bit definitions */
#define RTL8139_InR_SERR	(1<<15)
#define RTL8139_InR_TimeOut	(1<<14)
#define RTL8139_InR_LenChg	(1<<13)
#define RTL8139_InR_FOVW	(1<<6)
#define RTL8139_InR_PUNLinkChg	(1<<5)
#define RTL8139_InR_RXOVW	(1<<4)
#define RTL8139_InR_TER		(1<<3)
#define RTL8139_InR_TOK		(1<<2)
#define RTL8139_InR_RER		(1<<1)
#define RTL8139_InR_ROK		(1<<0)

/* TCR register bit definitions */
#define RTL8139_TCR_HWVERID_A(v)	((v>>26)&0x7F)
#define RTL8139_TCR_IFG_R(v)	((v>>24)&0x3)
#define RTL8139_TCR_IFG_W(v)	((v<<24)&0x03000000)
#define RTL8139_TCR_HWVERID_B(v)	((v>>22)&0x3)
#define RTL8139_TCR_LBK		(0x3<<17)
#define RTL8139_TCR_CRC		(1<<16)
#define RTL8139_TCR_MXDMA_R(v)	((v>>8)&0x7)
#define RTL8139_TCR_MXDMA_W(v)	((v<<8)&0x700)
#define RTL8139_TCR_TXRR_R(v)	((v>>4)&0xF)
#define RTL8139_TCR_TXRR_W(v)	((v<<4)&0xF0)
#define RTL8139_TCR_CLRABT	(1<<0)

/* RCR register bit definitions */
#define RTL8139_RCR_ERTH_R(v)	((v>>24)&0xF)
#define RTL8139_RCR_ERTH_W(v)	((v<<24)&0x0F000000)
#define RTL8139_RCR_MulERINT	(1<<17)
#define RTL8139_RCR_RER8	(1<<16)
#define RTL8139_RCR_RXFTH_R(v)	((v>>13)&0x7)
#define RTL8139_RCR_RXFTH_W(v)	((v<<13)&0xE000)
#define RTL8139_RCR_RBLEN_R(v)	((v>>11)&0x3)
#define RTL8139_RCR_RBLEN_W(v)	((v<<11)&0x1800)
#define RTL8139_RCR_MXDMA_R(v)	((v>>8)&0x7)
#define RTL8139_RCR_MXDMA_W(v)	((v<<8)&0x700)
#define RTL8139_RCR_WRAP	(1<<7)
#define RTL8139_RCR_AER		(1<<5)
#define RTL8139_RCR_AR		(1<<4)
#define RTL8139_RCR_AB		(1<<3)
#define RTL8139_RCR_AM		(1<<2)
#define RTL8139_RCR_APM		(1<<1)
#define RTL8139_RCR_AAP		(1<<0)

/* 9346 Command Register bit definitions */
#define RTL8139_9346CR_EEM_R(v)	((v>>6)&0x3)
#define RTL8139_9346CR_EEM_W(v)	((v<<6)&0xC)
#define RTL8139_9346CR_EECS	(1<<3)
#define RTL8139_9346CR_EESK	(1<<2)
#define RTL8139_9346CR_EEDI	(1<<1)
#define RTL8139_9346CR_EEDO	(1<<0)

/* CONFIG0 Register bit definitions */
#define RTL8139_CONFIG0_SCR	(1<<7)
#define RTL8139_CONFIG0_PCS	(1<<6)
#define RTL8139_CONFIG0_T10	(1<<5)
#define RTL8139_CONFIG0_PL1	(1<<4)
#define RTL8139_CONFIG0_PL0	(1<<3)
#define RTL8139_CONFIG0_BS_W(v)	(v&0x7)
#define RTL8139_CONFIG0_BS_R(v)	(v&0x7)

#endif
