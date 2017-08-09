  #pragma once

#include "util/elf.h"
#include "util/ring3.h"
#include "util/bitpool.h"
#include "devices/mmu32.h"
#include "devices/gdt32.h"
#include <stddef.h>
#include <algorithm>


static inline void elf_load(addr_t from, size_t fromsize, process_t& proc, bitpool_t& pool4M){
  enum {PAGE_SIZE=4<<20};

  hoh_assert(elf_canread_prog_header(from,fromsize), "XXX");
  const Elf32_Ehdr& header      = *(Elf32_Ehdr*)(from);
  uint32_t prog_offset          = header.e_phoff;
  const Elf32_Phdr* prog_header = (Elf32_Phdr*)(from+prog_offset);
  uint32_t    prog_num          = header.e_phnum;

  elf_dump(from, fromsize);

  size_t numpages = elf_numpages(prog_header,prog_num);
  hoh_assert(numpages == 1, "XXX");

  addr_t to = alloc(pool4M); 

  int i, j;

  for(i = 0; i < prog_num; i++){
    if(prog_header[i].p_type == PT_LOAD){
      for(j = 0; j < prog_header[i].p_filesz; j++){
        *(to + prog_header[i].p_vaddr + j) = *(from + prog_header[i].p_offset + j);
      }
    }
  }

  for(i = 0; i < 1024; i++){
    proc.fpu_simd[i] = 0;
  }
  proc.eax = proc.ecx = proc.edx = proc.ebx = proc.ebp = proc.esi = proc.edi = 0;
  proc.startip = to + header.e_entry;
  proc.eip = (uint32_t)(to + header.e_entry);
  proc.iopl = 3;
  proc.eflags = (0x00003202) | (proc.eflags); 
  addr_t page = alloc(pool4M);
  proc.stackend = page;
  proc.esp = (uint32_t)(proc.stackend + (4 * 1024 * 1024) - (4 * 1024)); 
  proc.rank = 0;
  proc.masterrw = page;
  proc.mmu.map_identity();
  proc.mmu.map(to, to, 0x085);
  proc.mmu.map(page, page, 0x087);

  asm volatile(                                                \
   "  pushl %%ecx                                         \n\t"\
   "  pushl %%ebx                                         \n\t"\
   "  movl %%esp, %%ebx                                   \n\t"\
   "  movl " STR(process_offset_esp) "(%%eax), %%ecx      \n\t"\
   "  addl $16,%%ecx                                      \n\t"\
   "  movl %%ecx, %%esp                                   \n\t"\
   "  movl " STR(process_offset_sharedrw) "(%%eax), %%ecx      \n\t"\
   "  pushl %%ecx                                         \n\t"\
   "  movl " STR(process_offset_masterrw) "(%%eax), %%ecx  \n\t"\
   "  pushl %%ecx                                         \n\t"\
   "  movl " STR(process_offset_masterro) "(%%eax), %%ecx  \n\t"\
   "  pushl %%ecx                                         \n\t"\
   "  movl " STR(process_offset_rank) "(%%eax), %%ecx  \n\t"\
   "  pushl %%ecx                                         \n\t"\
   "  movl %%ebx, %%esp                                   \n\t"\
   "  popl %%ebx                                          \n\t"\
   "  popl %%ecx                                          \n\t"\
  :                                                            \
  :"a" (&proc)                                                 \
  :_ALL_REGISTERS, "memory", "flags"                           \
 );                                                            \

  for(i = 0; i < 64; i++){
    page[i] = 0; 
  } 

}


static inline void ring3_step(preempt_t& preempt, process_t& proc, dev_lapic_t& lapic){

  asm volatile(                                                       \
     "  movl %%cr3, %%ecx           \n\t"                               \
     "  movl %%ecx, 16(%%eax)       \n\t"                               \
    :                                                                   \
    :"a" (&preempt)                               \
    :_ALL_REGISTERS, "memory", "flags"                                  \
   );                                                                   \

  if(proc.state != 1){
    proc.mmu.reset();
    lapic.reset_timer_count(100000000);
    preempt.proc = (addr_t)&proc;
    preempt.emergency_stackp = proc.stackend + (4 * 1024 * 1024);
    proc.eflags = (proc.eflags & ~(3 << 12)) | (proc.iopl << 12);
    asm volatile(                                                       \
     "  fxrstor (%%eax)             \n\t"                               \
     "  movl 1024(%%eax),%%edi      \n\t"                               \
     "  movl 1028(%%eax),%%esi      \n\t"                               \
     "  movl 1032(%%eax),%%ebp      \n\t"                               \
     "  movl 1040(%%eax),%%ebx      \n\t"                               \
     "  movl 1044(%%eax),%%edx      \n\t"                               \
     "  movl 1048(%%eax),%%ecx      \n\t"                               \
     "  pushl $0x0023               \n\t"                               \
     "  pushl 1036(%%eax)           \n\t"                               \
     "  pushl 1060(%%eax)           \n\t"                               \
     "  pushl $0x001b               \n\t"                               \
     "  pushl 1056(%%eax)           \n\t"                               \
     "  movl 1052(%%eax),%%eax      \n\t"                               \
     "  iret                        \n\t"                               \
    :                                                                   \
    :"a" (&proc)                               \
    :_ALL_REGISTERS, "memory", "flags"                                  \
   );                                                                   \
  }
}

static inline void ring3_step_done(process_t& proc, dev_lapic_t& lapic){
  
  //
  //insert your code here
  //
  // proc.state = 1;
  // lapic.reset_timer_count(0);
}



