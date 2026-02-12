/**
 * @file	Entry.h
 * @brief	Implementation of Entry struct
 * @author	EilzDragon92
 */

#include "Core/Entry.h"

size_t Entry::size() const {
    return sizeof(uint32_t) + site.size() + sizeof(uint32_t) + acc.size() + sizeof(uint32_t) + pw.getSize();
}

size_t Entry::ser(uint8_t *dst) const {
    size_t cur = 0;
    uint32_t size;


    /* Write site */

    size = static_cast<uint32_t>(site.size());

    memcpy(dst + cur, &size, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    memcpy(dst + cur, site.data(), size);
    cur += size;


    /* Write account */

    size = static_cast<uint32_t>(acc.size());

    memcpy(dst + cur, &size, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    memcpy(dst + cur, acc.data(), size);
    cur += size;


    /* Write password */

    size = static_cast<uint32_t>(pw.getSize());

    memcpy(dst + cur, &size, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    memcpy(dst + cur, pw.getData(), size);
    cur += size;


    return cur;
}

size_t Entry::deser(const uint8_t *src) {
    size_t cur = 0;
    uint32_t size;


    /* Read site */

    memcpy(&size, src + cur, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    site.assign(reinterpret_cast<const char *>(src + cur), size);
    cur += size;


    /* Read account */

    memcpy(&size, src + cur, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    acc.assign(reinterpret_cast<const char *>(src + cur), size);
    cur += size;


    /* Read password */

    memcpy(&size, src + cur, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    pw.setData(reinterpret_cast<const char *>(src + cur), size);
    cur += size;


    return cur;
}