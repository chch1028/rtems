/*
 * Copyright (c) 2013 Hesham AL-Matary.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */
#include <bsp/arm-cp15-start.h>
#include <bsp/linker-symbols.h>
#include <bsp/mm.h>
#include <bsp/start.h>

BSP_START_TEXT_SECTION void bsp_memory_management_initialize(void)
{
  uint32_t ctrl = arm_cp15_get_control();

  ctrl |= ARM_CP15_CTRL_AFE | ARM_CP15_CTRL_S | ARM_CP15_CTRL_XP;

  arm_cp15_start_setup_translation_table_and_enable_mmu_and_cache(
    ctrl,
    (uint32_t *) bsp_translation_table_base,
    ARM_MMU_DEFAULT_CLIENT_DOMAIN,
    &bsp_mm_config_table[0],
    bsp_mm_config_table_size
  );
}
