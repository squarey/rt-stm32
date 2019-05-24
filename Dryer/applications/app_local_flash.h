


#ifndef _APP_LOCAL_FLASH_H
#define _APP_LOCAL_FLASH_H


#include "app_includes.h"


rt_uint8_t APP_LocalFlashWrite(rt_uint32_t Addrs, const rt_uint32_t *pData, rt_uint32_t Len);
rt_uint8_t APP_LocalFlashRead(rt_uint32_t Addrs, rt_uint32_t *pBuf, rt_uint32_t Len);

#endif  /* _APP_LOCAL_FLASH_H */

