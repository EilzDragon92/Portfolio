/**
 * @file	Entry.cpp
 * @brief	Implementation of Entry struct
 * @author	EilzDragon92
 */

#include "Core/Entry.h"

size_t Entry::size() const {
    return sizeof(uint32_t) + site.size() + sizeof(uint32_t) + acc.size() + sizeof(uint32_t) + pw.getSize();
}

size_t Entry::ser(uint8_t *dst) const {
    size_t cur = 0;
    uint32_t tmp;


    /* Write site */

    tmp = static_cast<uint32_t>(site.size());

    memcpy(dst + cur, &tmp, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    memcpy(dst + cur, site.data(), tmp);
    cur += tmp;


    /* Write account */

    tmp = static_cast<uint32_t>(acc.size());

    memcpy(dst + cur, &tmp, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    memcpy(dst + cur, acc.data(), tmp);
    cur += tmp;


    /* Write password */

    tmp = static_cast<uint32_t>(pw.getSize());

    memcpy(dst + cur, &tmp, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    memcpy(dst + cur, pw.getData(), tmp);
    cur += tmp;


    return cur;
}

size_t Entry::deser(const uint8_t *src) {
    size_t cur = 0;
    uint32_t tmp;


    /* Read site */

    memcpy(&tmp, src + cur, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    site.assign(reinterpret_cast<const char *>(src + cur), tmp);
    cur += tmp;


    /* Read account */

    memcpy(&tmp, src + cur, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    acc.assign(reinterpret_cast<const char *>(src + cur), tmp);
    cur += tmp;


    /* Read password */

    memcpy(&tmp, src + cur, sizeof(uint32_t));
    cur += sizeof(uint32_t);

    if (pw.setData(reinterpret_cast<const char *>(src + cur), tmp)) return 0;
    cur += tmp;


    return cur;
}