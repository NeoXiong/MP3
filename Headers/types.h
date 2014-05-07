///////////////////////////////////////////////////////////////////////////////
//! \addtogroup Standard data types Data_Types_File
//! @{
//
// Copyright (c) 2013-2014 Freescale, Inc.
//
//! \file types.h
//! \brief Contains standard data types.
//! \todo [PUBS] Add definitions for TBDs in this file
///////////////////////////////////////////////////////////////////////////////


#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "assert.h"

#ifndef __ICCARM__   // bool is the key word of the IAR toolchain 
typedef char bool;
#endif
typedef unsigned short wchar_t;

//! \brief 
//! \todo [PUBS] Add definition(s)...
//! \todo Where does this really go?
typedef struct
{
    //! \brief 
    uint32_t val[4];
} uint128_t;

//! \brief 
#ifndef RETCODE
#define RETCODE int
#endif

//------------------------------------------------------------------------------
// All of the following defs are included for compatability.  Please use the
// ANSI C99 defs for all new work.
//------------------------------------------------------------------------------

//! \brief 
#if !defined(FALSE)
#define FALSE false
#endif

//! \brief 
#if !defined(TRUE)
#define TRUE  true
#endif

//! \brief 
#if !defined(NULL)
#define NULL 0
#endif

//! \brief 
typedef bool        BOOL;

//! \brief 
typedef uint8_t     UINT8;
//! \brief 
typedef uint8_t     BYTE;
//! \brief 
typedef uint8_t     PACKED_BYTE;

//! \brief 
typedef uint16_t    UINT16;
//! \brief 
typedef uint16_t    USHORT;
//! \brief 
typedef uint16_t    WCHAR;
//! \brief 
typedef uint16_t    UCS3;
//! \brief 
typedef int16_t     SHORT;

//! \brief 
typedef uint32_t    UINT32;
//! \brief 
typedef uint32_t    WORD;
//! \brief 
typedef uint32_t    SECTOR_BUFFER;
//! \brief 
typedef uint32_t *  P_SECTOR_BUFFER;

//! \brief 
typedef uint64_t    DWORD;
//! \brief 
typedef int64_t     INT64;
//! \brief 
typedef int64_t     UINT64;

//! \brief 
typedef uint128_t   UINT128;

//! \brief 
typedef float       FLOAT;

//! \brief 
#define FRACT       _fract
//! \brief 
#define CIRC        _circ

//! \brief Provides a default of 16 bytes (128 bits / 8 bits per byte)
#ifndef MAX_NUM_RAW_SERIAL_NUMBER_BYTES
    #define MAX_NUM_RAW_SERIAL_NUMBER_BYTES (16)
#endif

//! \brief Provides a default value that allows each nibble of raw to be converted to its
//! ASCII hex character (1 extra for NULL termination)
#ifndef MAX_NUM_ASCII_SERIAL_NUMBER_CHARS
    #define MAX_NUM_ASCII_SERIAL_NUMBER_CHARS   (2*MAX_NUM_RAW_SERIAL_NUMBER_BYTES)
#endif


//------------------------------------------------------------------------------
// Huh?
//------------------------------------------------------------------------------

//! \brief 
#define REENTRANT

//------------------------------------------------------------------------------
// Debug macros in types.h?
//------------------------------------------------------------------------------

//! \brief 
#ifdef DEBUG
#define IFDEBUG(x) x
#else
#define IFDEBUG(x)
#endif

//------------------------------------------------------------------------------
// This sets the default build of the target
//------------------------------------------------------------------------------

//! \brief 
#if !defined(HW_TARGET_ASIC) && !defined(HW_TARGET_SIMULATOR)
#define HW_TARGET_BRAZOS 1
#endif

//------------------------------------------------------------------------------
// Win32 compatibility?
//------------------------------------------------------------------------------

//! \brief 
#ifdef _WIN32
#define inline __inline
#endif

#endif // #ifndef _TYPES_H

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
//! @}
