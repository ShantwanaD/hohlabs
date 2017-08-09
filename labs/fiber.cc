#include "labs/fiber.h"

void array_init(char* a);

//Long task implemented through fiber
 void fiblong(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t& stateinout){
      addr_t& main_stack = *pmain_stack; // boilerplate: to ease the transition from existing code
      addr_t& f_stack    = *pf_stack;
      int i;
      int sum = 0;
      for(i=1; i<100000; i++){
        hoh_debug(i);
        sum += i; 
        stack_saverestore(f_stack, main_stack);
      }
      array_init(stateinout.result);
      stateinout.fiblong = false;    //to stop this function from executing throughout
      stateinout.row += 2;
      stateinout.result[0] = '#'; // helps in rendering "Task completed" since we cannot check by the value of stateinout.fiblong  
      stateinout.result[1] = sum;

      for(;;){ 
        stack_saverestore(f_stack,main_stack);
      }
    }

// void fiblong(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t& stateinout){
//      // addr_t& main_stack = *pmain_stack; // boilerplate: to ease the transition from existing code
//       //addr_t& f_stack    = *pf_stack;
//   //hoh_debug("lol");
//       int i;
//       int sum = 0;
//       for(i=1; i<100000; i++){
//         hoh_debug(i);
//         sum += i; 
//         //stack_saverestore(f_stack, main_stack);
//       }
//       array_init(stateinout.result);
//       stateinout.fiblong = false;    //to stop this function from executing throughout
//       stateinout.row += 2;
//       stateinout.result[0] = '#'; // helps in rendering "Task completed" since we cannot check by the value of stateinout.fiblong  
//       stateinout.result[1] = sum;

//       // for(;;){ 
//        //  stack_saverestore(f_stack,main_stack);
//       // }
//     }



// void shell_step_fiber(shellstate_t& shellstate, addr_t& main_stack, preempt_t& preempt, addr_t& f_stack, addr_t f_array, uint32_t f_arraysize, dev_lapic_t lapic){
//   	if(shellstate.fiblong == true){
//       //hoh_debug("init ho gya");
//       lapic.reset_timer_count(1000);
//     	stack_saverestore(main_stack,preempt.saved_stack);
//     }
//   	else
//   	{
//     	stack_init3(preempt.saved_stack, f_array, f_arraysize, &fiblong, &main_stack, &preempt.saved_stack, &shellstate);
//     }
// }

void shell_step_fiber(shellstate_t& shellstate, addr_t& main_stack, preempt_t& preempt, addr_t& f_stack, addr_t f_array, uint32_t f_arraysize, dev_lapic_t lapic){
    if(shellstate.fiblong == true)
      stack_saverestore(main_stack,f_stack);
    else
      stack_init3(f_stack, f_array, f_arraysize, &fiblong, &main_stack, &f_stack, &shellstate);
}

