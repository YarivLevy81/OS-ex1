#include "os.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/mman.h>

void vpn_to_vpn5(uint64_t vpn, unsigned short *vpn1, unsigned short *vpn2,
                 unsigned short *vpn3, unsigned short *vpn4, unsigned short *vpn5, unsigned short *offset);

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn)
{
    
    unsigned short vpn1, vpn2, vpn3, vpn4, vpn5, offset;

    vpn_to_vpn5(vpn, &vpn1, &vpn2, &vpn3, &vpn4, &vpn5, &offset);
    
    unsigned short vpns[5] = {vpn1, vpn2, vpn3, vpn4, vpn5};
    
    uint64_t *level_k = (uint64_t *)(phys_to_virt(pt << 12));
    uint64_t current;

    for (int i = 0; i < 4; i++) {
        
        current = level_k[vpns[i]];

        if ((current & 1) == 0){
            uint64_t next = alloc_page_frame();
            next = next << 12;
            next = next | 1;
            level_k[vpns[i]] = next; 
            level_k = (uint64_t*)(phys_to_virt(next)-1);
        
        } else {
            level_k = (uint64_t*)(phys_to_virt(current)-1);

        }
    
    }
    
    if (ppn == NO_MAPPING){
        //Current entry is invalid
        
        level_k[vpns[4]] = 0x0;
        return;
    }
    ppn = ppn << 12;
    ppn = ppn | 1;
    level_k[vpns[4]] = ppn; 
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn)
{
    unsigned short vpn1, vpn2, vpn3, vpn4, vpn5, offset;

    vpn_to_vpn5(vpn, &vpn1, &vpn2, &vpn3, &vpn4, &vpn5, &offset);
    
    unsigned short vpns[5] = {vpn1, vpn2, vpn3, vpn4, vpn5};
    uint64_t *level_k = (uint64_t *)(phys_to_virt(pt << 12 ));
    uint64_t current;

    for (int i = 0; i < 4; i++) {
        current  = level_k[vpns[i]];

        if ((current & 1) == 0){
            return NO_MAPPING; 
        }

        level_k  = (uint64_t *)(phys_to_virt(current)-1);
    }
    
    if ((level_k[vpns[4]] & 1) == 0){
        return NO_MAPPING; 
    }

    return level_k[vpns[4]] >> 12;
}

void vpn_to_vpn5(uint64_t vpn, unsigned short *vpn1, unsigned short *vpn2,
                 unsigned short *vpn3, unsigned short *vpn4, unsigned short *vpn5, unsigned short *offset)
{

    uint64_t temp = vpn;

    //Offest is the 12 lsbs
    //*offset = temp & 0xfff;
    
    //temp = temp >> 12;
    //vpn5 is vpn[12:21]
    *vpn5 =  temp & 0x1ff;

    temp = temp >> 9;
    //vpn4 is vpn[21:30]
    *vpn4 =  temp & 0x1ff;

     temp = temp >> 9;
    //vpn3 is vpn[30:39]
    *vpn3 =  temp & 0x1ff;

     temp = temp >> 9;
    //vpn2 is vpn[39:48]
    *vpn2 =  temp & 0x1ff;

     temp = temp >> 9;
    //vpn1 is vpn[48:57]
    *vpn1 =  temp & 0x1ff;
    
}
