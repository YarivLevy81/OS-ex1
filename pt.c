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
    
    uint64_t current = pt;
    unsigned short vpns[5] = {vpn1, vpn2, vpn3, vpn4, vpn5};
    uint64_t *level_k;

    for (int i = 0; i < 4; i++) {
        
        level_k  = phys_to_virt(current);
        current  = level_k[vpns[i]];
        printf("current(loop-update)=%lu\n",current);

        if ((current & 1) == 0){
            //NO MAPPING HERE
            printf("NO MAPPING - UPDATE\n");
            uint64_t next = alloc_page_frame();
            next = next << 12;
            next = next | 1;
            level_k[vpns[i]] = next; 
             
        } 
                
        current  = level_k[vpns[i]];
    }
    
    level_k = phys_to_virt(current);
    
    level_k[vpns[4]] = ppn; 

}

uint64_t page_table_query(uint64_t pt, uint64_t vpn)
{
    unsigned short vpn1, vpn2, vpn3, vpn4, vpn5, offset;

    vpn_to_vpn5(vpn, &vpn1, &vpn2, &vpn3, &vpn4, &vpn5, &offset);
    
    uint64_t current = pt;
    unsigned short vpns[5] = {vpn1, vpn2, vpn3, vpn4, vpn5};
    uint64_t *level_k;

    for (int i = 0; i < 5; i++) {
        
        level_k  = phys_to_virt(current);
        current  = level_k[vpns[i]];

        printf("current(loop-query)=%lu\n", current);
        if ((current & 1) == 0){
            //Current entry is invalid
            printf("NO MAPPING - query\n");
            return NO_MAPPING; 
        }

        //current  = current >> 12;
    }
    
    printf("current(out)=%lu\n", current);
    //Want to 'mute' lsb
    //current = current & (~1);
    
    //We return ppn concatenated with offset
    //return (current | offset);
    return current;
}

void vpn_to_vpn5(uint64_t vpn, unsigned short *vpn1, unsigned short *vpn2,
                 unsigned short *vpn3, unsigned short *vpn4, unsigned short *vpn5, unsigned short *offset)
{

    uint64_t temp = vpn;
    printf("vpn=%lu\n", vpn);

    //Offest is the 12 lsbs
    *offset = temp & 0xfff;
    printf("offset=%d\n", *offset);
    
    temp = temp >> 12;
    //vpn5 is vpn[12:21]
    *vpn5 =  temp & 0x1ff;
    printf("vpn5=%d\n", *vpn5);

    temp = temp >> 9;
    //vpn4 is vpn[21:30]
    *vpn4 =  temp & 0x1ff;
    printf("vpn4=%d\n", *vpn4);

     temp = temp >> 9;
    //vpn3 is vpn[30:39]
    *vpn3 =  temp & 0x1ff;
    printf("vpn3=%d\n", *vpn3);

     temp = temp >> 9;
    //vpn2 is vpn[39:48]
    *vpn2 =  temp & 0x1ff;
    printf("vpn2=%d\n", *vpn2);

     temp = temp >> 9;
    //vpn1 is vpn[48:57]
    *vpn1 =  temp & 0x1ff;
    printf("vpn1=%d\n", *vpn1);
    
}
