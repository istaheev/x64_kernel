// Defines GRUB header

struct grub_signature {
	unsigned int magic;
    unsigned int flags;
    unsigned int checksum;
};
 
#define GRUB_MAGIC 0x1BADB002
#define GRUB_FLAGS 0x0
#define GRUB_CHECKSUM (-1 * (GRUB_MAGIC + GRUB_FLAGS))
 
struct grub_signature gs __attribute__ ((section (".grub_sig"))) =
        { GRUB_MAGIC, GRUB_FLAGS, GRUB_CHECKSUM };
