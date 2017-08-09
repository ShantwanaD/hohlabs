#pragma once


#include "util/ring3.h"
#include "devices/lapic.h"
#include "util/bitpool.h"

static inline void ring3_downcall(process_t& proc, dev_lapic_t& lapic, bitpool_t& pool4M){

  uint32_t* systemcall_mmio = reinterpret_cast<uint32_t*>(proc.masterrw);
  uint32_t fnum             = systemcall_mmio[1];  //read fnum first.

  if(fnum==0){ //make sure you check fnum.
    return;
  }

  uint32_t farg1=systemcall_mmio[2];
  uint32_t farg2=systemcall_mmio[3];
  uint32_t farg3=systemcall_mmio[4];


  hoh_debug("Systemcall: "<<fnum<<": "<<farg1<<","<<farg2<<","<<farg3);

  uint32_t fret1=0;
  uint32_t fret2=0;
  uint32_t fret3=0;

  switch(fnum){
  default:{
           hoh_debug("Invalid system call. Killing the application");
           //proc->state=PROC_DONE;
           //free resources
          }break;
  case 0: { //nop: don't do anything
            asm volatile ("nop ");
          }break;
  case 1: { //done
            //proc->state=PROC_DONE;
            proc.state = 1;
            lapic.reset_timer_count(0);
            //free resources
          }break;
  case 2: {  //mmio_read
            if(farg1 == 8){
              fret1 = mmio::read8(addr_t(farg2), 0);
            }
            else if(farg1 == 16){
              fret1 = mmio::read16(addr_t(farg2), 0);
            }
            else if(farg1 == 32){
              fret1 = mmio::read32(addr_t(farg2), 0);
            }
          }break;
  case 3: {  //mmio_write
            if(farg1 == 8){
              mmio::write8(addr_t(farg2), 0, farg3);
            }
            else if(farg1 == 16){
              mmio::write16(addr_t(farg2), 0, farg3);
            }
            else if(farg1 == 32){
              mmio::write32(addr_t(farg2), 0, farg3);
            }
          }break;
  case 4: {  //io_read
          if(farg1 == 8){
              fret1 = io::read8(io_t(farg2), 0);
            }
            else if(farg1 == 16){
              fret1 = io::read16(io_t(farg2), 0);
            }
            else if(farg1 == 32){
              fret1 = io::read32(io_t(farg2), 0);
           }
          }break;
  case 5: {  //io_write
            if(farg1 == 8){
              io::write8(io_t(farg2), 0, farg3);
            }
            else if(farg1 == 16){
              io::write16(io_t(farg2), 0, farg3);
            }
            else if(farg1 == 32){
              io::write32(io_t(farg2), 0, farg3);
            }
          }break;
  case 6: {  //mmu_swapva
             //align va1 and va2 using prev_align
             //swap using: proc.mmu.get/set()
             enum { PAGE_SIZE = 4<<20 } ;
             if((farg1 >= (2 << 30)) && (farg1 < (3 << 30)) && (farg1 >= (2 << 30)) && (farg2 < (3 << 30))){
               farg1 = (uint32_t)prevalign(addr_t(farg1), PAGE_SIZE);
               farg2 = (uint32_t)prevalign(addr_t(farg2), PAGE_SIZE);
               proc.mmu.swap(get_bits<31,22>(farg1), get_bits<31,22>(farg2));
             }
             
          }break;
  case 7: {  //mmu_mapmmio
             //align va using prev_align
             //proc.mmu.map
            enum { PAGE_SIZE = 4<<20 } ;
            if((farg1 < (2 << 30)) || (farg1 >= (3 << 30))){
              farg1 = (uint32_t)prevalign(addr_t(farg1), PAGE_SIZE);
              proc.mmu.map(addr_t(farg1), addr_t(farg1), 0x87);
            }

          }break;
  case 8: {  //mmu_mappmio
             //proc.iopl=3; and make sure in your ring3_step you respect this variable
              proc.iopl = 3;
          }break;
  case 9: {  //addr_t x=alloc(pool4M);
             //unused_page = 0;
             //unused_page = find a page in VA_RANGE. ie: [ (0x2<<30) , (0x3<<30) )
             //if you're able to find a page: 
             //   proc.mmu.map_large(unused_page,x,0x87);

             if(!canalloc(pool4M))
              fret1 = 0;
             else{
              uint32_t unused_page = 0;
              addr_t x = alloc(pool4M);
              for(int i = 512; i < 768; i++){
                if((proc.mmu.get(i) & 0x4) == 0){
                  unused_page = i << 22;
                  proc.mmu.map_large(addr_t(unused_page), x, 0x87, 1);
                  fret1 = unused_page;
                  break;
                }
              }
             }
          }break;
  }


  if(fnum!=0){
    // do not modify the arguments if fnum is zero.
    systemcall_mmio[2]=fret1;
    systemcall_mmio[3]=fret2;
    systemcall_mmio[4]=fret3;
    systemcall_mmio[1]=0; //modify this last. ZERO means systemcall done.
  }

  hoh_debug("Systemcall: "<<fnum<<": "<<farg1<<","<<farg2<<","<<farg3<<" RESULT: "<<fret1<<","<<fret2<<","<<fret3);

}

