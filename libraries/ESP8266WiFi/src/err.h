#ifndef __LWIP_ERR_H__
#define __LWIP_ERR_H__

#include "opt.h"
#include "arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Define LWIP_ERR_T in cc.h if you want to use
 *  a different type for your platform (must be signed). */
#ifdef LWIP_ERR_T
typedef LWIP_ERR_T err_t;
#else /* LWIP_ERR_T */
typedef s8_t err_t;
#endif /* LWIP_ERR_T*/

/* Definitions for error constants. */

#define ERR_OK          0    /* No error, everything OK. */
#define ERR_MEM        -1    /* Out of memory error.     */
#define ERR_BUF        -2    /* Buffer error.            */
#define ERR_TIMEOUT    -3    /* Timeout.                 */
#define ERR_RTE        -4    /* Routing problem.         */
#define ERR_INPROGRESS -5    /* Operation in progress    */
#define ERR_VAL        -6    /* Illegal value.           */
#define ERR_WOULDBLOCK -7    /* Operation would block.   */

#define ERR_IS_FATAL(e) ((e) < ERR_WOULDBLOCK)

#define ERR_ABRT       -8    /* Connection aborted.      */
#define ERR_RST        -9    /* Connection reset.        */
#define ERR_CLSD       -10   /* Connection closed.       */
#define ERR_CONN       -11   /* Not connected.           */

#define ERR_ARG        -12   /* Illegal argument.        */

#define ERR_USE        -13   /* Address in use.          */

#define ERR_IF         -14   /* Low-level netif error    */
#define ERR_ISCONN     -15   /* Already connected.       */


#ifdef LWIP_DEBUG
extern const char *lwip_strerr(err_t err)ICACHE_FLASH_ATTR;
#else
#define lwip_strerr(x) ""
#endif /* LWIP_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_ERR_H__ */