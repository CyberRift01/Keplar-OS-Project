#include <stdint.h>
#include "acpi.h"

#define MULTIBOOT2_INFO_LIMIT 0x40000000u


typedef struct __attribute__((packed)) {
    uint32_t total_size;
    uint32_t reserved;
} multiboot_info_t;

typedef struct __attribute__((packed)) {
    uint32_t type;
    uint32_t size;
} multiboot_tag_t;

static const uint8_t *acpi_rsdp = 0;

static int checksum_valid(const uint8_t *data, uint32_t length)
{
    uint8_t sum = 0;

    for (uint32_t i = 0; i < length; i++)
        sum = (uint8_t)(sum + data[i]);

    return sum == 0;
}

int acpi_init(uint32_t mbi_addr)
{
    acpi_rsdp = 0;

    /*
     * Keplar currently identity-maps only the first 1 GiB.
     * Don't dereference boot information outside that range.
     */
    if (mbi_addr == 0 || mbi_addr >= MULTIBOOT2_INFO_LIMIT)
        return 0;

    const multiboot_info_t *info =
        (const multiboot_info_t *)(uintptr_t)mbi_addr;

    uint32_t total_size = info->total_size;

    if (total_size < sizeof(multiboot_info_t))
        return 0;

    if (total_size > MULTIBOOT2_INFO_LIMIT - mbi_addr)
        return 0;

    const uint8_t *base = (const uint8_t *)info;
    uint32_t offset = sizeof(multiboot_info_t);

    while (offset <= total_size - sizeof(multiboot_tag_t)) {
        const multiboot_tag_t *tag =
            (const multiboot_tag_t *)(base + offset);

        if (tag->size < sizeof(multiboot_tag_t))
            return 0;

        if (tag->size > total_size - offset)
            return 0;

        if (tag->type == 0)
            break;

        /*
         * Type 15: ACPI 2.0 RSDP.
         * Prefer this over the older type 14 tag.
         */
        if (tag->type == 15 && tag->size >= 44) {
            const uint8_t *rsdp = base + offset + 8;

            /* "RSD PTR " signature */
            static const char signature[] = "RSD PTR ";

            uint32_t i = 0;
            while (i < 8 && rsdp[i] == signature[i])
                i++;

            if (i != 8)
                goto next_tag;

            /* Original RSDP checksum covers 20 bytes. */
            if (!checksum_valid(rsdp, 20))
                goto next_tag;

            /* ACPI 2.0+ RSDP revision and length. */
            if (rsdp[15] < 2)
                goto next_tag;

            uint32_t rsdp_length =
                (uint32_t)rsdp[20] |
                ((uint32_t)rsdp[21] << 8) |
                ((uint32_t)rsdp[22] << 16) |
                ((uint32_t)rsdp[23] << 24);

            if (rsdp_length < 36 ||
                rsdp_length > tag->size - 8)
                goto next_tag;

            if (!checksum_valid(rsdp, rsdp_length))
                goto next_tag;

            acpi_rsdp = rsdp;
            return 1;
        }

next_tag:
        {
            uint32_t advance = (tag->size + 7u) & ~7u;

            if (advance < tag->size ||
                advance > total_size - offset)
                return 0;

            offset += advance;
        }
    }

    return 0;
}