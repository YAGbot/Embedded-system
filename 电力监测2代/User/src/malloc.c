/**
  **************************************************************************************
  * @File      malloc.c
  * @Author    ChuanYong Wan
  * @brief   	 Managing memory.
	* @Version   1.0
  **************************************************************************************
  * @COPYRIGHT (c) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.
  *								 Without permission, shall not be reproduced and copied, shall not be 
	*							used for commercial purposes.
  **************************************************************************************
 **/
/* Include ----------------------------------------------------------------------------*/
#include "malloc.h"

/* Variable ---------------------------------------------------------------------------*/  
__align(4) uint8_t gMEM_Base[MEM_SIZE];
uint16_t gMEM_Table[MEM_TABLE_SIZE];
uint8_t  gMEM_OK = 0;

/* Function ---------------------------------------------------------------------------*/
/**
  * @brief  Setting up memory.
	* @param  *add: Head address of memory
	* @param  value: The value you want to set
	* @param  count: Memory size(bytes)
	* @retval None
 **/
void My_memset(void *addr, uint8_t value, uint32_t count)  
{  
    uint8_t *address = addr;  
    while(count--)
		{
			*address++ = value;
		}			
}	   
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Memory copy.
	* @param  *des: Destinate address of memory
	* @param  *src: Source address of memory
	* @param  cnt:  Memory size(bytes)
	* @retval None
 **/
void My_memcpy(void *des, void *src, uint32_t cnt)  
{  
  uint8_t *des_temp=des;
	uint8_t *src_temp=src; 
   while(cnt--) *des_temp++ = *src_temp++;  
} 
/*-------------------------------------------------------------------------------------*/
void MEM_Init(void)  
{  
  My_memset(gMEM_Table, 0, MEM_TABLE_SIZE);
	My_memset(gMEM_Base, 0, MEM_SIZE);
	gMEM_OK = 1; 
}  
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Memory allocation (internal call).
	* @param  size: The size of the memory to be allocated(bytes)
	* @retval 0XFFFFFFFF: Error
	*         Others: Memory offset address 
 **/
uint32_t mem_malloc(uint32_t size)  
{   
  uint32_t Block_Need  = 0; //The number of memory blocks required to allocate memory
	uint32_t Block_Empty = 0; //Continuous empty memory blocks
  uint32_t i=0;
	int j=0;  
  if(!gMEM_OK)
	{
		MEM_Init(); 
	}
  if(size==0)	return 0;
  Block_Need = size/MEM_BLOCK_SIZE;
  if(size%MEM_BLOCK_SIZE)
	{
		Block_Need++;  
	}
  for(j=(MEM_TABLE_SIZE-1);j>=0;j--) //Search the entire memory control area 
  {     
		if(!gMEM_Table[j]) Block_Empty++;
		else               Block_Empty = 0;	
		if(Block_Empty==Block_Need)      //Continuous empty memory block was found
		{
			for(i=0;i<Block_Need;i++)      //Annotated memory block is not empty 
			{  
					gMEM_Table[j+i] = Block_Need;  
			}  
			return (j*MEM_BLOCK_SIZE);//Return the offset address 
		}
   }  
   return 0;
}
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Release memory(internal call).
	* @param  offset: Memory address offset
	* @retval	0: Succeed
	*					Others: Failed
 **/ 
uint8_t mem_free(uint32_t offset)  
{  
	uint16_t i=0, index=0, Block_Need=0;
	if(!gMEM_OK)
	{
		MEM_Init();    
    return 1;  
  }  
  if(offset<MEM_SIZE)
  {  
		index = offset/MEM_BLOCK_SIZE;  
		Block_Need = gMEM_Table[index];
		for(i=0;i<Block_Need;i++)
		{  
				gMEM_Table[index+i]=0;  
		}
		return 0;  
  }
	else return 2;
}  
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Memory allocation (external call).
	* @param  size: The size of the memory to be allocated(bytes)
	* @retval NULL: Error
	*         Adress: Memory offset address 
 **/
void *My_mem_malloc(uint32_t size)  
{  
  uint32_t offset = 0;  									      
	offset = mem_malloc(size);  	   				   
  if(offset==0)	return NULL; 		
  else return (void*)((uint32_t)gMEM_Base+offset);  
} 
/*-------------------------------------------------------------------------------------*/
/**
  * @brief  Release memory(external call).
	* @param  add: Head address of memory
	* @retval	None
 **/ 
void My_mem_free(void *addr)  
{  
	uint32_t offset = 0;  
  if(addr==NULL)
	{
	}
	else
	{	
		offset = (uint32_t)addr-(uint32_t)gMEM_Base;  
		mem_free(offset);  
	}	
}  
/*-------------------------------------------------------------------------------------*/
void *My_mem_realloc(void *addr, uint32_t size)  
{  
	uint32_t offset;  
	offset = mem_malloc(size);  
	if(offset==0) return NULL;     
	else  
	{  									   
		My_memcpy((void*)((uint32_t)gMEM_Base+offset),addr,size);   
		My_mem_free(addr);  											  	
		return (void*)((uint32_t)gMEM_Base+offset);  			
	}  
}
/*-------------------------------------------------------------------------------------*/
/***COPYRIGHT (C) Taihua Hongye(Tianjin) Robot Technology Research Institute Co.,Ltd.***/










