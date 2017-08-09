#pragma once

#include "util/config.h"


//
// preempt_t
//
//
//

#define _ring3_preempt(_name, _f) \
         __asm(                                   \
         "  .text                            \n\t"\
         " " STR(_name) ":                   \n\t"\
         "  pushl %edx                       \n\t"\
         "  pushl %ecx                       \n\t"\
         "  pushl %eax                       \n\t"\
         "  call " STR(_f) "                 \n\t"\
         "  popl  %eax                       \n\t"\
         "  popl  %ecx                       \n\t"\
         "  popl  %edx                       \n\t"\
         "                                   \n\t"\
         "  #insert your code here           \n\t"\
         "                                   \n\t"\
         "  pushl %eax                       \n\t"\
         "  pushl %ecx                       \n\t"\
         "  movl %gs:" STR(core_offset_preempt+8) ", %eax   \n\t"\
         "  movl %gs:" STR(core_offset_preempt+16) ", %ecx  \n\t"\
         "  movl %ecx, %cr3                                 \n\t"\
         "  fxsave (%eax)                    \n\t"\
         "  movl %edi," STR(process_offset_edi) "(%eax)     \n\t"\
         "  movl %esi," STR(process_offset_esi) "(%eax)     \n\t"\
         "  movl %ebp," STR(process_offset_ebp) "(%eax)     \n\t"\
         "  movl 28(%esp), %ecx              \n\t"\
         "  movl %ecx," STR(process_offset_esp) "(%eax)     \n\t"\
         "  movl %ebx," STR(process_offset_ebx) "(%eax)     \n\t"\
         "  movl %edx," STR(process_offset_edx) "(%eax)     \n\t"\
         "  movl (%esp), %ecx              \n\t"\
         "  movl %ecx," STR(process_offset_ecx) "(%eax)     \n\t"\
         "  movl 4(%esp), %ecx              \n\t"\
         "  movl %ecx," STR(process_offset_eax) "(%eax)     \n\t"\
         "  movl 16(%esp), %ecx              \n\t"\
         "  movl %ecx," STR(process_offset_eip) "(%eax)     \n\t"\
         "  popl %ecx                        \n\t"\
         "  popl %eax                        \n\t"\
         "                                   \n\t"\
         "  movl $0, %ebp                    \n\t"\
         "  pushl %gs:0                      \n\t"\
         "  pushl %gs:4                      \n\t"\
         "  call core_loop                   \n\t"\
         "                                   \n\t"\
         )                                        \



