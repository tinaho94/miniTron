#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

//#include <string.h>
#include "c_types.h"
#include "ets_sys.h"
#include "osapi.h"
#define EFAULT 14

//#define LWIP_PROVIDE_ERRNO

#if (1)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#define BYTE_ORDER BIG_ENDIAN
#endif


typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long    u32_t;
typedef signed     long    s32_t;
typedef unsigned long   mem_ptr_t;

#define S16_F "d"
#define U16_F "d"
#define X16_F "x"

#define S32_F "d"
#define U32_F "d"
#define X32_F "x"



//#define PACK_STRUCT_FIELD(x) x __attribute__((packed))
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

//#define LWIP_DEBUG

#ifdef LWIP_DEBUG
#define LWIP_PLATFORM_DIAG(x) os_printf x
#define LWIP_PLATFORM_ASSERT(x) ETS_ASSERT(x)
#else
#define LWIP_PLATFORM_DIAG(x)
#define LWIP_PLATFORM_ASSERT(x)
#endif

#define SYS_ARCH_DECL_PROTECT(x)
#define SYS_ARCH_PROTECT(x)
#define SYS_ARCH_UNPROTECT(x)

#define LWIP_PLATFORM_BYTESWAP 1
#define LWIP_PLATFORM_HTONS(_n)  ((u16_t)((((_n) & 0xff) << 8) | (((_n) >> 8) & 0xff)))
#define LWIP_PLATFORM_HTONL(_n)  ((u32_t)( (((_n) & 0xff) << 24) | (((_n) & 0xff00) << 8) | (((_n) >> 8)  & 0xff00) | (((_n) >> 24) & 0xff) ))

#if LWIP_RAW
extern u8_t memp_memory_RAW_PCB_base[];
#endif /* LWIP_RAW */

#if LWIP_UDP
extern u8_t memp_memory_UDP_PCB_base[];
#endif /* LWIP_UDP */

#if LWIP_TCP
extern u8_t memp_memory_TCP_PCB_base[];
extern u8_t memp_memory_TCP_PCB_LISTEN_base[];
extern u8_t memp_memory_TCP_SEG_base[] SHMEM_ATTR;
#endif /* LWIP_TCP */

#if (!NO_SYS || (NO_SYS && !NO_SYS_NO_TIMERS)) /* LWIP_TIMERS */
extern u8_t memp_memory_SYS_TIMEOUT_base[];
#endif /* LWIP_TIMERS */

extern u8_t memp_memory_PBUF_base[];
extern u8_t memp_memory_PBUF_POOL_base[];



#endif /* __ARCH_CC_H__ */