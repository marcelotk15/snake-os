/******************************************************************************
 *
 *  Copyright (c) 2008 Cavium Networks 
 * 
 *  This file is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License, Version 2, as 
 *  published by the Free Software Foundation. 
 *
 *  This file is distributed in the hope that it will be useful, 
 *  but AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or 
 *  NONINFRINGEMENT.  See the GNU General Public License for more details. 
 *
 *  You should have received a copy of the GNU General Public License 
 *  along with this file; if not, write to the Free Software 
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA or 
 *  visit http://www.gnu.org/licenses/. 
 *
 *  This file may also be available under a different license from Cavium. 
 *  Contact Cavium Networks for more information
 *
 ******************************************************************************/


#ifndef	_STAR_POWERMGT_H_
#define	_STAR_POWERMGT_H_


#include <asm/arch/star_sys_memory_map.h>


#if defined(__UBOOT__)
#define	PWRMGT_MEM_MAP_VALUE(reg_offset)	(*((u32	volatile *)(SYSPA_POWER_MANAGEMENT_BASE_ADDR + reg_offset)))
#elif defined(__LINUX__)
#define	PWRMGT_MEM_MAP_VALUE(reg_offset)	(*((u32	volatile *)(SYSVA_POWER_MANAGEMENT_BASE_ADDR + reg_offset)))
#else
#error "NO SYSTEM DEFINED"
#endif


#define	PWRMGT_CLOCK_MASK_CONTROL_REG		PWRMGT_MEM_MAP_VALUE(0x00)
#define	PWRMGT_SOFTWARE_RESET_CONTROL_REG	PWRMGT_MEM_MAP_VALUE(0x04)
#define	PWRMGT_SYSTEM_CLOCK_CONTROL_REG		PWRMGT_MEM_MAP_VALUE(0x08)
#define	PWRMGT_CPU_INITIALIZATION_REG		PWRMGT_MEM_MAP_VALUE(0x0C)
#define	PWRMGT_PLL_POWER_DOWN_CONTROL_REG	PWRMGT_MEM_MAP_VALUE(0x10)
#define	PWRMGT_RESET_LATCH_CONFIGURATION_REG	PWRMGT_MEM_MAP_VALUE(0x14)
#define	PWRMGT_REGULATOR_CONTROL_REG		PWRMGT_MEM_MAP_VALUE(0x18)
#define	PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG	PWRMGT_MEM_MAP_VALUE(0x1C)
#define	PWRMGT_SOFTWARE_BUNDLE_CHECK_REG	PWRMGT_MEM_MAP_VALUE(0x20)
#define	PWRMGT_IDP_FUNCTION_CHECK_REG		PWRMGT_MEM_MAP_VALUE(0x24)


/*
 * define constants macros
 */
#define	PWRMGT_HCLK_SMC_MASK_BIT_INDEX			0
#define	PWRMGT_HCLK_PCMCIA_MASK_BIT_INDEX		1
#define	PWRMGT_HCLK_PCI_BRIDGE_MASK_BIT_INDEX		2
#define	PWRMGT_HCLK_DMA_MASK_BIT_INDEX			3
#define	PWRMGT_HCLK_PCLK_GSW_MASK_BIT_INDEX		4
#define	PWRMGT_HCLK_USB_MASK_BIT_INDEX			5
#define	PWRMGT_HCLK_IDP_MASK_BIT_INDEX			6

#define	PWRMGT_PCLK_UART_MASK_BIT_INDEX			16


#define	PWRMGT_GLABAL_SOFTWARE_RESET_MASK_BIT_INDEX	0
#define	PWRMGT_PCMCIA_SOFTWARE_RESET_MASK_BIT_INDEX	1
#define	PWRMGT_PCI_BRIDGE_SOFTWARE_RESET_MASK_BIT_INDEX	2
#define	PWRMGT_DMA_SOFTWARE_RESET_MASK_BIT_INDEX	3
#define	PWRMGT_GSW_SOFTWARE_RESET_MASK_BIT_INDEX	4
#define	PWRMGT_USB_SOFTWARE_RESET_MASK_BIT_INDEX	5
#define	PWRMGT_UART_SOFTWARE_RESET_MASK_BIT_INDEX	6
#define	PWRMGT_IDP_SOFTWARE_RESET_MASK_BIT_INDEX	7


#define	PWRMGT_PLL_FREQUENCY_175MHZ			(0 << 0)
#define	PWRMGT_PLL_FREQUENCY_200MHZ			(1 << 0)
#define	PWRMGT_PLL_FREQUENCY_225MHZ			(2 << 0)
#define	PWRMGT_PLL_FREQUENCY_250MHZ			(3 << 0)

#define	PWRMGT_FCLK_DIVIDER_BY_1			(0 << 2)
#define	PWRMGT_FCLK_DIVIDER_BY_2			(1 << 2)
#define	PWRMGT_FCLK_DIVIDER_BY_3			(2 << 2)
#define	PWRMGT_FCLK_DIVIDER_BY_4			(3 << 2)

#define	PWRMGT_HCLK_DIVIDER_BY_1			(0 << 4)
#define	PWRMGT_HCLK_DIVIDER_BY_2			(1 << 4)
#define	PWRMGT_HCLK_DIVIDER_BY_3			(2 << 4)
#define	PWRMGT_HCLK_DIVIDER_BY_4			(3 << 4)

#define	PWRMGT_PCICLK_SOURCE_66M			(0 << 8)
#define	PWRMGT_PCICLK_SOURCE_APBCLK			(1 << 8)

#define	PWRMGT_PCICLK_DIVIDER_BY_1			(0 << 10)
#define	PWRMGT_PCICLK_DIVIDER_BY_2			(1 << 10)
#define	PWRMGT_PCICLK_DIVIDER_BY_3			(2 << 10)
#define	PWRMGT_PCICLK_DIVIDER_BY_4			(3 << 10)


#define	PWRMGT_PLLCLK_TO_FCLK_RATIO_BY_1		(1)
#define	PWRMGT_PLLCLK_TO_FCLK_RATIO_BY_2		(2)
#define	PWRMGT_PLLCLK_TO_FCLK_RATIO_BY_3		(3)
#define	PWRMGT_PLLCLK_TO_FCLK_RATIO_BY_4		(4)

#define	PWRMGT_FCLK_TO_HCLK_RATIO_BY_1			(1)
#define	PWRMGT_FCLK_TO_HCLK_RATIO_BY_2			(2)
#define	PWRMGT_FCLK_TO_HCLK_RATIO_BY_3			(3)
#define	PWRMGT_FCLK_TO_HCLK_RATIO_BY_4			(4)


#define	PWRMGT_PLL125_GSW_PWD_BIT_INDEX			0
#define	PWRMGT_PLL300_USB_PWD_BIT_INDEX			1
#define	PWRMGT_PLL330_PCIB_PWD_BIT_INDEX		2
#define	PWRMGT_PLLUSB_PHY_PWD_BIT_INDEX			3
#define	PWRMGT_XTALUSB_PHY_PWD_BIT_INDEX		4
#define	PWRMGT_SYSTEM_XTAL_PWD_BIT_INDEX		5


/*
 * Macro defines for Clock Mask	Control
 */
#define	HAL_PWRMGT_READ_CLOCK_MASK_CONTROL(clock_mask_control) \
    ((clock_mask_control) = (PWRMGT_CLOCK_MASK_CONTROL_REG))


#define	HAL_PWRMGT_WRITE_CLOCK_MASK_CONTROL(clock_mask_control)	\
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) = (clock_mask_control))


#define	HAL_PWRMGT_ENABLE_SMC_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_SMC_MASK_BIT_INDEX)))


#define	HAL_PWRMGT_DISABLE_SMC_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) |= (0x1 << PWRMGT_HCLK_SMC_MASK_BIT_INDEX))


#define	HAL_PWRMGT_ENABLE_PCMCIA_HCLK()	\
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_PCMCIA_MASK_BIT_INDEX)))


#define	HAL_PWRMGT_DISABLE_PCMCIA_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) |= (0x1 << PWRMGT_HCLK_PCMCIA_MASK_BIT_INDEX))


#define	HAL_PWRMGT_ENABLE_PCMCIA() \
{ \
    (PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_PCMCIA_MASK_BIT_INDEX)); \
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) |= (0x1 << PWRMGT_PCMCIA_SOFTWARE_RESET_MASK_BIT_INDEX); \
}


#define	HAL_PWRMGT_ENABLE_PCI_BRIDGE_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_PCI_BRIDGE_MASK_BIT_INDEX)))


#define	HAL_PWRMGT_DISABLE_PCI_BRIDGE_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) |= (0x1 << PWRMGT_HCLK_PCI_BRIDGE_MASK_BIT_INDEX))


#define	HAL_PWRMGT_ENABLE_PCI_BRIDGE_33M() \
{ \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_PLL300_USB_PWD_BIT_INDEX)); \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_PLL330_PCIB_PWD_BIT_INDEX)); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) &= (~(PWRMGT_PCICLK_SOURCE_APBCLK)); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) &= (~(0x3 << 10)); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) |= (PWRMGT_PCICLK_DIVIDER_BY_2); \
    (PWRMGT_CLOCK_MASK_CONTROL_REG)	&= (~(0x1 << PWRMGT_HCLK_PCI_BRIDGE_MASK_BIT_INDEX)); \
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) |= (0x1 << PWRMGT_PCI_BRIDGE_SOFTWARE_RESET_MASK_BIT_INDEX); \
}


#define	HAL_PWRMGT_ENABLE_PCI_BRIDGE_66M() \
{ \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_PLL300_USB_PWD_BIT_INDEX)); \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_PLL330_PCIB_PWD_BIT_INDEX)); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) &= (~(PWRMGT_PCICLK_SOURCE_APBCLK)); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) &= (~(0x3 << 10)); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) |= (PWRMGT_PCICLK_DIVIDER_BY_1); \
    (PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_PCI_BRIDGE_MASK_BIT_INDEX)); \
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) |= (0x1 << PWRMGT_PCI_BRIDGE_SOFTWARE_RESET_MASK_BIT_INDEX); \
}


#define	HAL_PWRMGT_ENABLE_DMA_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_DMA_MASK_BIT_INDEX)))


#define	HAL_PWRMGT_DISABLE_DMA_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) |= (0x1 << PWRMGT_HCLK_DMA_MASK_BIT_INDEX))


#define	HAL_PWRMGT_ENABLE_DMA()	\
{ \
    (PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_DMA_MASK_BIT_INDEX));	\
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) |= (0x1 << PWRMGT_DMA_SOFTWARE_RESET_MASK_BIT_INDEX); \
}


#define	HAL_PWRMGT_ENABLE_GSW_HCLK_PCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_PCLK_GSW_MASK_BIT_INDEX)))


#define	HAL_PWRMGT_DISABLE_GSW_HCLK_PCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) |= (0x1 << PWRMGT_HCLK_PCLK_GSW_MASK_BIT_INDEX))


#define	HAL_PWRMGT_ENABLE_GSW()	\
{ \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_PLL125_GSW_PWD_BIT_INDEX)); \
    (PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_PCLK_GSW_MASK_BIT_INDEX)); \
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) |= (0x1 << PWRMGT_GSW_SOFTWARE_RESET_MASK_BIT_INDEX); \
}


#define	HAL_PWRMGT_ENABLE_USB_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_USB_MASK_BIT_INDEX)))


#define	HAL_PWRMGT_DISABLE_USB_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) |= (0x1 << PWRMGT_HCLK_USB_MASK_BIT_INDEX))


#define	HAL_PWRMGT_ENABLE_USB_FROM_EXTERNAL_12M() \
{ \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_XTALUSB_PHY_PWD_BIT_INDEX)); \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_PLLUSB_PHY_PWD_BIT_INDEX)); \
    (PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_USB_MASK_BIT_INDEX));	\
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) |= (0x1 << PWRMGT_USB_SOFTWARE_RESET_MASK_BIT_INDEX); \
}


#define	HAL_PWRMGT_ENABLE_USB_FROM_INTERNAL_12M() \
{ \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) |= (0x1 << PWRMGT_XTALUSB_PHY_PWD_BIT_INDEX); \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_PLL300_USB_PWD_BIT_INDEX)); \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_PLLUSB_PHY_PWD_BIT_INDEX)); \
    (PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_USB_MASK_BIT_INDEX));	\
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) |= (0x1 << PWRMGT_USB_SOFTWARE_RESET_MASK_BIT_INDEX); \
}


#define	HAL_PWRMGT_ENABLE_IDP_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_IDP_MASK_BIT_INDEX)))


#define	HAL_PWRMGT_DISABLE_IDP_HCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) |= (0x1 << PWRMGT_HCLK_IDP_MASK_BIT_INDEX))


#define	HAL_PWRMGT_ENABLE_IDP()	\
{ \
    (PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_IDP_MASK_BIT_INDEX));	\
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) |= (0x1 << PWRMGT_IDP_SOFTWARE_RESET_MASK_BIT_INDEX); \
}


#define	HAL_PWRMGT_ENABLE_UART_PCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_PCLK_UART_MASK_BIT_INDEX)))


#define	HAL_PWRMGT_DISABLE_UART_PCLK() \
    ((PWRMGT_CLOCK_MASK_CONTROL_REG) |= (0x1 << PWRMGT_PCLK_UART_MASK_BIT_INDEX))


#define	HAL_PWRMGT_ENABLE_UART() \
{ \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) |= (0x1 << PWRMGT_XTALUSB_PHY_PWD_BIT_INDEX); \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_PLL300_USB_PWD_BIT_INDEX)); \
    (PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << PWRMGT_PLLUSB_PHY_PWD_BIT_INDEX)); \
    (PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_HCLK_USB_MASK_BIT_INDEX));	\
    (PWRMGT_CLOCK_MASK_CONTROL_REG) &= (~(0x1 << PWRMGT_PCLK_UART_MASK_BIT_INDEX)); \
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) |= (0x1 << PWRMGT_UART_SOFTWARE_RESET_MASK_BIT_INDEX); \
}


/*
 * Macro defines for Reset Control
 */
#define	HAL_PWRMGT_READ_SOFTWARE_RESET_CONTROL(software_reset_control) \
    ((software_reset_control) =	(PWRMGT_SOFTWARE_RESET_CONTROL_REG))


#define	HAL_PWRMGT_WRITE_SOFTWARE_RESET_CONTROL(software_reset_control)	\
    ((PWRMGT_SOFTWARE_RESET_CONTROL_REG) = (software_reset_control))


#define	HAL_PWRMGT_DEVICES_EXIT_RESET_STATE_AFTER_POWER_ON() \
    ((PWRMGT_SOFTWARE_RESET_CONTROL_REG) = (0xFF))


#define	HAL_PWRMGT_GLOBAL_SOFTWARE_RESET() \
{ \
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) |= (0x1 << PWRMGT_GLABAL_SOFTWARE_RESET_MASK_BIT_INDEX); \
    (PWRMGT_SOFTWARE_RESET_CONTROL_REG) &= (~(0x1 << PWRMGT_GLABAL_SOFTWARE_RESET_MASK_BIT_INDEX)); \
}


/*
 * Macro defines for System Clock Control
 */
#define	HAL_PWRMGT_READ_SYSTEM_CLOCK_CONTROL(system_clock_control) \
{ \
    (system_clock_control) = (PWRMGT_SYSTEM_CLOCK_CONTROL_REG); \
}


#define	HAL_PWRMGT_WRITE_SYSTEM_CLOCK_CONTROL(system_clock_control) \
{ \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) = (system_clock_control); \
}


#define	HAL_PWRMGT_SET_PLL_FREQUENCY_175MHZ() \
{ \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) &= (~0x3); \
}


#define	HAL_PWRMGT_SET_PLL_FREQUENCY_200MHZ() \
{ \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) &= (~0x3); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) |= (0x1); \
}


#define	HAL_PWRMGT_SET_PLL_FREQUENCY_225MHZ() \
{ \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) &= (~0x3); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) |= (0x2); \
}


#define	HAL_PWRMGT_SET_PLL_FREQUENCY_250MHZ() \
{ \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) &= (~0x3); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) |= (0x3); \
}


#define	HAL_PWRMGT_CONFIG_PLLCLK_TO_CPUCLK_RATIO(ratio)	\
{ \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) &= (~(0x3 << 2)); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) |= (((ratio - 1) & 0x3) << 2); \
}


#define	HAL_PWRMGT_CONFIG_CPUCLK_TO_HCLK_RATIO(ratio) \
{ \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) &= (~(0x3 << 4)); \
    (PWRMGT_SYSTEM_CLOCK_CONTROL_REG) |= (((ratio - 1) & 0x3) << 4); \
}


/*
 * Macro defines for CPU Initialization
 */
#define	HAL_PWRMGT_CONFIG_CPU_PLL_ACQUISITION_TIME(value) \
{ \
    (PWRMGT_CPU_INIT_SETTING_REG) &= (~((value & 0x3F) << 16)); \
    (PWRMGT_CPU_INIT_SETTING_REG) |= ((value & 0x3F) << 16); \
}


#define	HAL_PWRMGT_CONFIG_CPU_CCKR(value) \
{ \
    (PWRMGT_CPU_INIT_SETTING_REG) &= (~((value & 0x3F) << 5)); \
    (PWRMGT_CPU_INIT_SETTING_REG) |= ((value & 0x3F) << 5);	\
}


/*
 * Macro defines for Power Down	Control
 */
#define	HAL_PWRMGT_READ_PLL_POWER_DOWN_CONTROL(pll_power_down) \
    ((pll_power_down) =	((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) & 0x3F))


#define	HAL_PWRMGT_WRITE_PLL_POWER_DOWN_CONTROL(pll_power_down)	\
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) = ((pll_power_down) & 0x3F))


#define	HAL_PWRMGT_POWER_DOWN_PLL125_GSW() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) |= (0x1))


#define	HAL_PWRMGT_POWER_ON_PLL125_GSW() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~0x1))


#define	HAL_PWRMGT_POWER_DOWN_PLL300_USB() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) |= (0x1 << 1))


#define	HAL_PWRMGT_POWER_ON_PLL300_USB() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << 1)))


#define	HAL_PWRMGT_POWER_DOWN_PLL330_PCIB() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) |= (0x1 << 2))


#define	HAL_PWRMGT_POWER_ON_PLL330_PCIB() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << 2)))


#define	HAL_PWRMGT_POWER_DOWN_PLL_USB_PHY() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) |= (0x1 << 3))


#define	HAL_PWRMGT_POWER_ON_PLL_USB_PHY() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << 3)))


#define	HAL_PWRMGT_POWER_DOWN_XTAL_USB_PHY() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) |= (0x1 << 4))


#define	HAL_PWRMGT_POWER_ON_XTAL_USB_PHY() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << 4)))


#define	HAL_PWRMGT_POWER_DOWN_SYSTEM_XTAL() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) |= (0x1 << 5))


#define	HAL_PWRMGT_POWER_ON_SYSTEM_XTAL() \
    ((PWRMGT_PLL_POWER_DOWN_CONTROL_REG) &= (~(0x1 << 5)))


/*
 * Macro defines for Reset Latch Configuration
 */
#define	HAL_PWRMGT_READ_RESET_LATCH_CONFIGURATION(reset_latch) \
    ((reset_latch) = (PWRMGT_RESET_LATCH_CONFIGURATION_REG))


/*
 * Macro defines for Regulator Control
 */
#define	HAL_PWRMGT_READ_REGULATOR_CONTROL(regulator_control) \
    ((regulator_control) = (PWRMGT_REGULATOR_CONTROL_REG))


#define	PWRMGT_WRITE_REGULATOR_CONTROL(regulator_control) \
    ((PWRMGT_REGULATOR_CONTROL_REG) = (regulator_control))


/*
 * Macro defines for Pad Drive Strength	Control
 */
#define	HAL_PWRMGT_CONFIG_PCI_IO_PAD_PCMCIA_MODE() \
    ((PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG) &= (~0x3))


#define	HAL_PWRMGT_CONFIG_PCI_IO_PAD_PCI_MODE()	\
{ \
    (PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG) &= (~0x3); \
    (PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG) |= (0x1); \
}


#define	HAL_PWRMGT_CONFIG_PCI_IO_PAD_CARDBUS_MODE() \
{ \
    (PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG) &= (~0x3); \
    (PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG) |= (0x2); \
}


#define	HAL_PWRMGT_CONFIG_GSW_MAC_PORT0_IO_PAD_10M_100M_MODE() \
    ((PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG) |= (0x1 << 2))


#define	HAL_PWRMGT_CONFIG_GSW_MAC_PORT0_IO_PAD_1000M_MODE() \
    ((PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG) &= (~(0x1 << 2)))


#define	HAL_PWRMGT_CONFIG_GSW_MAC_PORT1_IO_PAD_10M_100M_MODE() \
    ((PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG) |= (0x1 << 3))


#define	HAL_PWRMGT_CONFIG_GSW_MAC_PORT1_IO_PAD_1000M_MODE() \
    ((PWRMGT_PAD_DRIVE_STRENGTH_CONTROL_REG) &= (~(0x1 << 3)))


#endif	// end of #ifndef _STAR_POWERMGT_H_

