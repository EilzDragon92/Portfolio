/**
 * @file	Entry.h
 * @brief	Password entry with serialization support
 * @author	EilzDragon92
 */

#pragma once

#include "Common/header.h"
#include "Utils/Password.h"

 /**
  * @struct  Entry
  * @brief   Single password entry containing site, account, and password
  */
struct Entry {
    std::string site;
    std::string acc;
    Password pw;

    /**
     * @brief   Calculate serialized size in bytes
     * @return  Total bytes needed for serialization
     */
    size_t size() const;

    /**
     * @brief   Serialize entry to buffer
     * @param   dst     Destination buffer (must have enough space)
     * @return  Number of bytes written
     */
    size_t ser(uint8_t *dst) const;

    /**
     * @brief   Deserialize entry from buffer
     * @param   src     Source buffer
     * @return  Number of bytes read
     */
    size_t deser(const uint8_t *src);
};

/**
 * @struct  EntryCmp
 * @brief   Comparator for Entry set ordering
 */
struct EntryCmp {
    bool operator()(const Entry &a, const Entry &b) const {
        if (a.site != b.site) return a.site < b.site;
        return a.acc < b.acc;
    }
};