#pragma once

#include "util/config.h"


//
// upcall
//
//
//




#define _ring3_upcall(_name, _f)                  \
         __asm(                                   \
         "  .text                            \n\t"\
         " " STR(_name) ":                   \n\t"\
         "                                   \n\t"\
         "  #insert your code here           \n\t"\
         "                                   \n\t"\
         "  pushl %eax                       \n\t"\
         "  pushl %ecx                       \n\t"\
         "  pushl %ebx                       \n\t"\
         "  movl %esp, %ebx                  \n\t"\
         "  movl %gs:" STR(core_offset_preempt+8) ", %eax   \n\t"\
         "  movl %gs:" STR(core_offset_preempt+12) ", %ecx  \n\t"\
         "  movl %ecx, %esp                  \n\t"\
         "  movl %cr2, %ecx                  \n\t"\
         "  pushl %ecx                       \n\t"\
         "  movl 20(%ebx), %ecx              \n\t"\
         "  pushl %ecx                       \n\t"\
         "  movl 32(%ebx), %ecx              \n\t"\
         "  pushl %ecx                       \n\t"\
         "  movl 16(%ebx), %ecx              \n\t"\
         "  pushl %ecx                       \n\t"\
         "  movl 12(%ebx), %ecx              \n\t"\
         "  pushl %ecx                       \n\t"\
         "  movl " STR(process_offset_sharedrw) "(%eax), %ecx      \n\t"\
         "  pushl %ecx                       \n\t"\
         "  movl " STR(process_offset_masterrw) "(%eax), %ecx      \n\t"\
         "  pushl %ecx                       \n\t"\
         "  movl " STR(process_offset_masterro) "(%eax), %ecx      \n\t"\
         "  pushl %ecx                       \n\t"\
         "  movl " STR(process_offset_rank) "(%eax), %ecx      \n\t"\
         "  pushl %ecx                       \n\t"\
         "  movl %esp, 32(%ebx)              \n\t"\
         "  movl " STR(process_offset_startip)"(%eax), %ecx  \n\t"\
         "  addl $0x4, %ecx                  \n\t"\
         "  movl %ecx, 20(%ebx)              \n\t"\
         "  movl %ebx, %esp                  \n\t"\
         "  popl %ebx                       \n\t"\
         "  popl %ecx                        \n\t"\
         "  popl %eax                        \n\t"\
         "                                   \n\t"\
         "  pushl %edx                       \n\t"\
         "  pushl %ecx                       \n\t"\
         "  pushl %eax                       \n\t"\
         "  call " STR(_f) "                 \n\t"\
         "  popl  %eax                       \n\t"\
         "  popl  %ecx                       \n\t"\
         "  popl  %edx                       \n\t"\
         "                                   \n\t"\
         "  jmp iret_toring3                 \n\t"\
         )                                        \


