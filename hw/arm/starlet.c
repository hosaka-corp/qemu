
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "qemu/error-report.h"

#include "target/arm/cpu.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "cpu.h"

#include "sysemu/sysemu.h"
#include "exec/address-spaces.h"

/* ----------------------------------------------------------------------------
 * Various Starlet [machine]-specific type declarations.
 */

typedef struct StarletState
{
	DeviceState parent_obj;

	ARMCPU cpu;
	MemoryRegion mem1;
	MemoryRegion mem2;
	MemoryRegion rom;
	MemoryRegion sram;

	// Various mmio device state goes here
	// ...
} StarletState;

enum StarletMemoryMap
{
	MEM1_ADDR	= 0x00000000,
	MEM1_SIZE	= 0x01800000,

	MEM2_ADDR	= 0x10000000,
	MEM2_SIZE	= 0x04000000,

	ROM_ADDR	= 0xffff0000,
	ROM_SIZE	= 0x00002000,

	SRAM_ADDR	= 0x0d400000,
	SRAM_SIZE	= 0x00020000
};

enum StarletIRQ
{
	STARLET_TIMER		= 0,
	STARLET_NAND		= 1,
	STARLET_AES		= 2,
	STARLET_SHA		= 3,
	STARLET_EHCI		= 4,
	STARLET_OHCI0		= 5,
	STARLET_OHCI1		= 6,
	STARLET_SDHC		= 7,
	STARLET_WIFI		= 8,

	STARLET_GPIO_PPC	= 10,
	STARLET_GPIO_ARM	= 11,

	STARLET_RESET_BUTTON	= 17,
	STARLET_DI		= 18,

	STARLET_IPC_PCC		= 30,
	STARLET_IPC_ARM		= 31,
};


/* ----------------------------------------------------------------------------
 * Initialization/realization functions
 */


static void starlet_init(MachineState *machine)
{
	StarletState *s = g_new0(StarletState, 1);

	// Initialize the CPU and set some relevant properties
	object_initialize_child(OBJECT(machine), "cpu", &s->cpu, sizeof(s->cpu),
			ARM_CPU_TYPE_NAME("arm926"), &error_abort, NULL);

	//s->cpu.env.cp15.sctlr_el[1] |= SCTLR_B;
	object_property_set_bool(OBJECT(&s->cpu), true, "cfgend", &error_abort);
	object_property_set_bool(OBJECT(&s->cpu), true, "reset-hivecs", &error_abort);
	object_property_set_bool(OBJECT(&s->cpu), true, "realized", &error_abort);

	// Initialize memory regions
	memory_region_init_rom(&s->rom,0,"starlet.rom", ROM_SIZE, &error_abort);
	memory_region_add_subregion(get_system_memory(), ROM_ADDR, &s->rom);

	memory_region_init_ram(&s->sram,0,"starlet.sram", SRAM_SIZE, &error_abort);
	memory_region_add_subregion(get_system_memory(), SRAM_ADDR, &s->sram);

	memory_region_init_ram(&s->mem1,0,"starlet.mem1", MEM1_SIZE, &error_abort);
	memory_region_add_subregion(get_system_memory(), MEM1_ADDR, &s->mem1);

	memory_region_init_ram(&s->mem2,0,"starlet.mem2", MEM2_SIZE, &error_abort);
	memory_region_add_subregion(get_system_memory(), MEM2_ADDR, &s->mem2);

	// Load the boot ROM into physical memory
	if (bios_name) 
	{
		char *filename;
		int filesize;
		filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, bios_name);
		if (!filename) {
			error_report("Couldn't find ROM '%s'", bios_name);
			exit(1);
		}
		filesize = load_image_targphys(filename, ROM_ADDR, ROM_SIZE);
		g_free(filename);
		if (filesize < 0) {
			error_report("Couldn't find ROM '%s'", bios_name);
			exit(1);
		}
	}


}

/* ----------------------------------------------------------------------------
 * Class/type initialization
 */

static void starlet_machine_init(MachineClass *mc)
{
	mc->desc = "Starlet (Wii I/O Processor) (ARM926EJ-S)";
	mc->init = starlet_init;
	mc->default_cpu_type = ARM_CPU_TYPE_NAME("arm926");
	mc->ignore_memory_transaction_failures = false;
}

DEFINE_MACHINE("starlet", starlet_machine_init)



