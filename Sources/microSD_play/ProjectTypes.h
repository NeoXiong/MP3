/**************************************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2010 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
***************************************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
***********************************************************************************************//*!
 *
 * @file ProjectTypes.h
 *
 * @author B04198
 *
 * @date 24/01/2012
 *
 * @brief
**************************************************************************************************/

#ifndef PROJECTTYPES_H_
#define PROJECTTYPES_H_

/*************************************************************************************************/
/*                                      Includes Section                                         */
/*************************************************************************************************/


/*************************************************************************************************/
/*                                  Defines & Macros Section                                     */
/*************************************************************************************************/

#ifndef FALSE
  #define  FALSE  0x00u
#endif
#ifndef TRUE
  #define  TRUE   0x01u
#endif

#ifndef ERROR
        #define ERROR 0x00u
#endif
#ifndef OK
        #define OK    0x01u
#endif

#ifndef NULL
  #define  NULL   0x00u
#endif

#define BUS_CLOCK                               (25000000)
#define BIG_ENDIAN_CORE                 (FALSE)

#ifndef ELEMENT_SIZE
        #define ELEMENT_SIZE(x)         (sizeof(x) / sizeof(&x))
#endif

/*#define BYTESWAP32(x)                                                           (u32)((((x) & 0xFF000000) >> 24) \
                                                                                                        | (((x) & 0x00FF0000) >> 8) \
                                                                                                        | (((x) & 0x0000FF00) << 8) \
                                                                                                        | (((x) & 0x000000FF) << 24))*/
#define BYTESWAP16(x)                                                           (u16)((((x) & 0xFF00) >> 0x08) \
                                                                                                        | (((x) & 0x00FF) << 0x8))

#define U32_TO_U8_SPLIT(x)                                                      (u8)(x >> 24),(u8)(x >> 16), \
                                                                                                        (u8)(x >> 8),(u8)(x)

/*************************************************************************************************/
/*                                      Typedef Section                                          */
/*************************************************************************************************/
typedef unsigned char                   u8;                             //!< UNSIGNED 8 BIT DEFINITION
typedef unsigned short                  u16;                    //!< UNSIGNED 16 BIT DEFINITION
typedef unsigned long                   u32;                    //!< UNSIGNED 32 BIT DEFINITION
typedef signed char                             s8;                             //!< SIGNED 8 BIT DEFINITION
typedef signed short                    s16;                    //!< SIGNED 16 BIT DEFINITION
typedef signed long                             s32;                    //!< SIGNED 32 BIT DEFINITION

/* State Machine */
  typedef struct
  {
    u8 ActualState;
    u8 PrevState;
    u8 NextState;
    u8 ErrorState;
  }sSM;
/*************************************************************************************************/
/*                                Function-like Macros Section                                   */
/*************************************************************************************************/


/*************************************************************************************************/
/*                                  Extern Constants Section                                     */
/*************************************************************************************************/


/*************************************************************************************************/
/*                                  Extern Variables Section                                     */
/*************************************************************************************************/


/*************************************************************************************************/
/*                                Function Prototypes Section                                    */
/*************************************************************************************************/


/*************************************************************************************************/

#endif /* PROJECTTYPES_H_ */



