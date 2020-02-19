
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "qemu/error-report.h"

#include "target/arm/cpu.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "hw/block/flash.h"
#include "cpu.h"

#include "sysemu/sysemu.h"
#include "exec/address-spaces.h"

/* ----------------------------------------------------------------------------
 * Various Starlet [machine]-specific type declarations.
 */

enum HollywoodOffsets
{
	// IPC device
	IPC_P2S_MSG		= 0x000,
	IPC_PPC_CTRL		= 0x004,
	IPC_S2P_MSG		= 0x008,
	IPC_IOP_CTRL		= 0x010,

	// Protection control
	ACR_SRN_PROT		= 0x060,
	ACR_BUS_PROT		= 0x064,

	// ARM-side GPIOs
	GPIO_IOP_EN		= 0x0dc,
	GPIO_IOP_OUT		= 0x0e0,
	GPIO_IOP_OE		= 0x0e4,
	GPIO_IOP_IN		= 0x0e8,
	GPIO_IOP_INT_POL	= 0x0ec,
	GPIO_IOP_INT_STS	= 0x0f0,
	GPIO_IOP_INT_EN		= 0x0f4,
	GPIO_IOP_STRAPS		= 0x0f8,
	GPIO_IOP_PPC_OWNER	= 0x0fc,

	// OTP/EFUSE
	EFUSE_CMD		= 0x1ec,
	EFUSE_DATA		= 0x1f0,

};

typedef struct StarletState
{
	DeviceState parent_obj;

	ARMCPU cpu;
	MemoryRegion mem1;
	MemoryRegion mem2;
	MemoryRegion rom;
	MemoryRegion sram;

	// Various mmio device state goes here
	MemoryRegion sha_mem;
	MemoryRegion nand_mem;
	MemoryRegion aes_mem;
	MemoryRegion hlwd_mem;

	DeviceState *nand_dev;
} StarletState;

enum StarletMemoryMap
{
	// 24MB, 1T-SRAM
	MEM1_ADDR	= 0x00000000,
	MEM1_SIZE	= 0x01800000,

	// 64MB, GDDR3
	MEM2_ADDR	= 0x10000000,
	MEM2_SIZE	= 0x04000000,

	// Various device MMIO
	NAND_ADDR	= 0x0d010000,
	AES_ADDR	= 0x0d020000,
	SHA_ADDR	= 0x0d030000,
	EHCI_ADDR	= 0x0d040000,
	OCHI0_ADDR	= 0x0d050000,
	OCHI1_ADDR	= 0x0d060000,
	SDHC_ADDR	= 0x0d070000,
	WIFI_ADDR	= 0x0d080000,
	MMIO_SIZE	= 0x00000200,

	// On-die SRAM (unmirrored)
	SRAM_ADDR	= 0x0d400000,
	SRAM_SIZE	= 0x00020000,

	// Hollywood MMIO
	HLWD_ADDR	= 0x0d800000,
	HLWD_SIZE	= 0x00000220,

	// Memory controller MMIO
	MC_ADDR		= 0x0d8b4200,
	MC_SIZE		= 0x000000d0,

	// On-die Mask ROM
	ROM_ADDR	= 0xffff0000,
	ROM_SIZE	= 0x00002000,
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
 * Hollywood MMIO
 */

static uint64_t hlwd_read(void *opaque, hwaddr offset, unsigned size)
{
	switch (offset) {
	default:
		printf("HLWD: unimp read on %08lx\n", (HLWD_ADDR + offset));
		break;
	}
	return 0;
}
static void hlwd_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)
{
	switch (offset) {
	default:
		printf("HLWD: unimp write on %08lx\n", (HLWD_ADDR + offset));
		break;

	}
}

/* ----------------------------------------------------------------------------
 * NAND controller MMIO
 */

static uint64_t nand_read(void *opaque, hwaddr offset, unsigned size)
{
	switch (offset) {
	default:
		printf("NAND: unimp read on %08lx\n", (NAND_ADDR + offset));
		break;
	}
	return 0;
}
static void nand_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)
{
	switch (offset) {
	default:
		printf("NAND: unimp write on %08lx\n", (NAND_ADDR + offset));
		break;

	}
}

/* ----------------------------------------------------------------------------
 * AES controller MMIO
 */

static uint64_t aes_read(void *opaque, hwaddr offset, unsigned size)
{
	switch (offset) {
	default:
		printf("AES: unimp read on %08lx\n", (AES_ADDR + offset));
		break;
	}
	return 0;
}
static void aes_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)
{
	switch (offset) {
	default:
		printf("AES: unimp write on %08lx\n", (AES_ADDR + offset));
		break;

	}
}

/* ----------------------------------------------------------------------------
 * SHA controller MMIO
 */

static uint64_t sha_read(void *opaque, hwaddr offset, unsigned size)
{
	switch (offset) {
	default:
		printf("SHA: unimp read on %08lx\n", (SHA_ADDR + offset));
		break;
	}
	return 0;
}
static void sha_write(void *opaque, hwaddr offset, uint64_t value, unsigned size)
{
	switch (offset) {
	default:
		printf("SHA: unimp write on %08lx\n", (SHA_ADDR + offset));
		break;

	}
}


/* ----------------------------------------------------------------------------
 * MMIO Ops declarations
 */


static const MemoryRegionOps nand_ops = {
    .read =  nand_read,
    .write = nand_write,
    .endianness = DEVICE_BIG_ENDIAN,
};
static const MemoryRegionOps aes_ops = {
    .read =  aes_read,
    .write = aes_write,
    .endianness = DEVICE_BIG_ENDIAN,
};
static const MemoryRegionOps sha_ops = {
    .read =  sha_read,
    .write = sha_write,
    .endianness = DEVICE_BIG_ENDIAN,
};
static const MemoryRegionOps hlwd_ops = {
    .read =  hlwd_read,
    .write = hlwd_write,
    .endianness = DEVICE_BIG_ENDIAN,
};



static void starlet_init(MachineState *machine)
{
	StarletState *s = g_new0(StarletState, 1);

	// Think this is the top-level parent container for all memory
	MemoryRegion *sysmem = get_system_memory();

	// Initialize the CPU and set some relevant properties
	object_initialize_child(OBJECT(machine), "cpu", &s->cpu, 
		sizeof(s->cpu), ARM_CPU_TYPE_NAME("arm926"), 
		&error_abort, NULL);
	object_property_set_bool(OBJECT(&s->cpu), true, 
			"cfgend", &error_abort);
	object_property_set_bool(OBJECT(&s->cpu), true, 
			"reset-hivecs", &error_abort);
	object_property_set_bool(OBJECT(&s->cpu), true, 
			"realized", &error_abort);

	//DriveInfo *nandinfo;
	//nandinfo = drive_get(IF_MTD, 0, 0);
	//s->nand_dev = nand_init(nandinfo ? blk_by_legacy_dinfo(nandinfo) : NULL,
	//		NAND_MFR_HYNIX, 0xdc);


	// Boot ROM backing memory
	memory_region_init_rom(&s->rom, NULL,"starlet.rom", 
			ROM_SIZE, &error_abort);
	memory_region_add_subregion(sysmem, ROM_ADDR, &s->rom);

	// SRAM backing memory
	memory_region_init_ram(&s->sram, NULL,"starlet.sram", 
			SRAM_SIZE, &error_abort);
	memory_region_add_subregion(sysmem, SRAM_ADDR, &s->sram);

	// MEM1 backing memory
	memory_region_init_ram(&s->mem1, NULL,"starlet.mem1", 
			MEM1_SIZE, &error_abort);
	memory_region_add_subregion(sysmem, MEM1_ADDR, &s->mem1);

	// MEM2 backing memory
	memory_region_init_ram(&s->mem2, NULL,"starlet.mem2", 
			MEM2_SIZE, &error_abort);
	memory_region_add_subregion(sysmem, MEM2_ADDR, &s->mem2);

	// NAND MMIO backing
	memory_region_init_io(&s->nand_mem, NULL, &nand_ops, s,
                          "hollywood-nand", MMIO_SIZE);
	memory_region_add_subregion(sysmem, NAND_ADDR, &s->nand_mem);

	// AES MMIO backing
	memory_region_init_io(&s->aes_mem, NULL, &aes_ops, s,
                          "hollywood-aes", MMIO_SIZE);
	memory_region_add_subregion(sysmem, AES_ADDR, &s->aes_mem);

	// SHA MMIO backing
	memory_region_init_io(&s->sha_mem, NULL, &sha_ops, s,
                          "hollywood-sha", MMIO_SIZE);
	memory_region_add_subregion(sysmem, SHA_ADDR, &s->sha_mem);

	// HOLLYWOOD MMIO backing
	memory_region_init_io(&s->hlwd_mem, NULL, &hlwd_ops, s,
                          "hollywood-ctrl", HLWD_SIZE);
	memory_region_add_subregion(sysmem, HLWD_ADDR, &s->hlwd_mem);


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



