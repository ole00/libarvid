/**
 * \file crc.h
 * Functions and types for CRC checks.
 *
 * Generated on Sat Feb  4 11:41:22 2017,
 * by pycrc v0.9, https://pycrc.org
 * using the configuration:
 *    Width         = 32
 *    Poly          = 0x04c11db7
 *    Xor_In        = 0xffffffff
 *    ReflectIn     = True
 *    Xor_Out       = 0xffffffff
 *    ReflectOut    = True
 *    Algorithm     = table-driven
 *****************************************************************************/
#ifndef __CRC_H__
#define __CRC_H__

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef unsigned int crc_t;


crc_t crc_calc(const void *data, size_t data_len);



#ifdef __cplusplus
}           /* closing brace for extern "C" */
#endif

#endif      /* __CRC_H__ */
