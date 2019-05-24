


#include "app_local_flash.h"


//芯片中FLASH的起始地址
#define DEF_LOCAL_FLASH_START_ADDR      0x8000000
//芯片中一页FLASH的大小
#define DEF_FLASH_ONE_PAGE_SIZE         1024
//芯片FLASH的总大小  单位KB
#define DEF_FLASH_TOTAL_SIZE            64


//向FLASH中写入len个字 即4个字节
rt_uint8_t APP_LocalFlashWrite(rt_uint32_t Addrs, const rt_uint32_t *pData, rt_uint32_t Len)
{
    FLASH_EraseInitTypeDef FlashType;
    rt_uint32_t PageError = 0;
    rt_uint32_t WriteAddr = 0;
    rt_uint32_t WriteData = 0;
    rt_uint32_t WriteLen = 0;
    if((Addrs << 2) >= DEF_FLASH_ONE_PAGE_SIZE){
        return 1;
    }
    if(HAL_FLASH_Unlock()){
         rt_kprintf("unock flash fail\n");
        return 1;
    }
    if(DEF_FLASH_ONE_PAGE_SIZE > 1024){
        WriteAddr = DEF_LOCAL_FLASH_START_ADDR + DEF_FLASH_ONE_PAGE_SIZE * (DEF_FLASH_TOTAL_SIZE/2 - 1);
    }else{
        WriteAddr = DEF_LOCAL_FLASH_START_ADDR + DEF_FLASH_ONE_PAGE_SIZE * (DEF_FLASH_TOTAL_SIZE - 1);
    }   
    WriteAddr += Addrs * 4;
    FlashType.TypeErase = FLASH_TYPEERASE_PAGES;
    FlashType.NbPages = 1;
    FlashType.PageAddress = WriteAddr;
    if(HAL_OK != HAL_FLASHEx_Erase(&FlashType, &PageError)){
        rt_kprintf("erase flash fail\n");
        return 1;
    }
    for(WriteLen = 0; WriteLen < Len; WriteLen++){
        WriteData = *(pData + WriteLen);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, WriteAddr + WriteLen * 4, WriteData);
    }
    HAL_FLASH_Lock();
    return 0;
}

rt_uint8_t APP_LocalFlashRead(rt_uint32_t Addrs, rt_uint32_t *pBuf, rt_uint32_t Len)
{
    rt_uint32_t ReadAddr = 0;
    rt_uint32_t ReadData = 0;
    rt_uint32_t ReadLen = 0;
    if((Addrs << 2) >= DEF_FLASH_ONE_PAGE_SIZE){
        return 1;
    } 
    if(DEF_FLASH_ONE_PAGE_SIZE > 1024){
        ReadAddr = DEF_LOCAL_FLASH_START_ADDR + DEF_FLASH_ONE_PAGE_SIZE * (DEF_FLASH_TOTAL_SIZE/2 - 1);
    }else{
        ReadAddr = DEF_LOCAL_FLASH_START_ADDR + DEF_FLASH_ONE_PAGE_SIZE * (DEF_FLASH_TOTAL_SIZE - 1);
    }
    ReadAddr += Addrs * 4;
    for(ReadLen = 0; ReadLen < Len; ReadLen++){
        ReadData = *(__IO uint32_t*)(ReadAddr + ReadLen * 4);
        pBuf[ReadLen] = ReadData;
    }
    return 0;
}

