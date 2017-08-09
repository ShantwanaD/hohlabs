#include "labs/fiber_scheduler.h"

void array_init(char* a);

//Additional long tasks implemented through fiber
 void factorial(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t& stateinout){

      addr_t& main_stack = *pmain_stack; // boilerplate: to ease the transition from existing code
      addr_t& f_stack    = *pf_stack;
      int i;
      int fact = 1;
      for(i=1; i<1000; i++){
        hoh_debug("fact: " << i);
        fact *= i; 
        stack_saverestore(f_stack, main_stack);
      }
      array_init(stateinout.result);
      stateinout.row += 2;
      stateinout.result[0] = '#'; // helps in rendering   
      stateinout.result[1] = i;
      stateinout.mult_result[stateinout.top] = 0;
      for(;;){ 
        stack_saverestore(f_stack,main_stack);
      }
    }


    void fibonacci(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t& stateinout){
      addr_t& main_stack = *pmain_stack; 
      addr_t& f_stack    = *pf_stack;
      int i, a = 0, b = 1, c;
      for(i=3; i<1000; i++){
        hoh_debug("fibo: " << i);
        c = a + b;
        a = b;
        b = c; 
        stack_saverestore(f_stack, main_stack);
      }
      array_init(stateinout.result);
      stateinout.row += 2;
      stateinout.result[0] = '#'; 
      stateinout.result[1] = c;
      stateinout.mult_result[stateinout.top] = 0;
      for(;;){ 
        stack_saverestore(f_stack,main_stack);
      }
    }

//
// stackptrs:      Type: addr_t[stackptrs_size].  array of stack pointers (generalizing: main_stack and f_stack)
// stackptrs_size: number of elements in 'stacks'.
//
// arrays:      Type: uint8_t [arrays_size]. array of memory region for stacks (generalizing: f_array)
// arrays_size: size of 'arrays'. equal to stackptrs_size*STACK_SIZE.
//
// Tip: divide arrays into stackptrs_size parts.
// Tip: you may implement a circular buffer using arrays inside shellstate_t
//      if you choose linked lists, create linked linked using arrays in
//      shellstate_t. (use array indexes as next pointers)
// Note: malloc/new is not available at this point.
//
void shell_step_fiber_scheduler(shellstate_t& shellstate, addr_t& main_stack, preempt_t& preempt, addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size, dev_lapic_t lapic){
		size_t f_arraysize = arrays_size / stackptrs_size;
  		addr_t f_array = addr_t(arrays) + (shellstate.top * f_arraysize);
		if(shellstate.fibonacci == true && shellstate.mult_result[shellstate.rear] == '#' && shellstate.rear == shellstate.top){
  			stack_init3(stackptrs[shellstate.rear], f_array, f_arraysize, &fibonacci, &main_stack, &stackptrs[shellstate.rear], &shellstate);
    		shellstate.mult_result[shellstate.rear] = '*';
    		
    		stack_saverestore(main_stack, stackptrs[shellstate.rear]);
	  		shellstate.fibonacci = false;
  		} 
  		else if(shellstate.factorial == true && shellstate.mult_result[shellstate.rear] == '$' && shellstate.rear == shellstate.top){
			stack_init3(stackptrs[shellstate.rear], f_array, f_arraysize, &factorial, &main_stack, &stackptrs[shellstate.rear], &shellstate);
    		shellstate.mult_result[shellstate.rear] = '@';
    		stack_saverestore(main_stack, stackptrs[shellstate.rear]);
	  		shellstate.factorial = false;
	  	}
  		else if(shellstate.mult_result[shellstate.top] != 0){
  	 		stack_saverestore(main_stack, stackptrs[shellstate.top]);
  		}
  		int i;
  		for(i=0;i<10;i++){
  			shellstate.top = (shellstate.top + 1) % 10;
  			if(shellstate.mult_result[shellstate.top] != 0){
  				break;
  			}
  		}
}

//     void fibonacci(addr_t* pmain_stack, addr_t* pf_stack, shellstate_t& stateinout){
//       addr_t& main_stack = *pmain_stack;
//       addr_t& f_stack    = *pf_stack;
//       int i, a = 0, b = 1, c;
//       for(i=3; i<10000; i++){
//         hoh_debug("fibo: " << i);
//         c = a + b;
//         a = b;
//         b = c; 
//       }
//       array_init(stateinout.result);
//       stateinout.row += 2;
//       stateinout.result[0] = '#'; 
//       stateinout.result[1] = c;
//       stateinout.mult_result[stateinout.top] = 0;
//       for(;;){ 
//         stack_saverestore(f_stack,main_stack);
//       }
//     }

//     void shell_step_fiber_scheduler(shellstate_t& shellstate, addr_t& main_stack, preempt_t& preempt, addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size, dev_lapic_t lapic){
// 		size_t f_arraysize = arrays_size / stackptrs_size;
//   		addr_t f_array = addr_t(arrays) + (shellstate.top * f_arraysize);
// 		if(shellstate.fibonacci == true && shellstate.mult_result[shellstate.rear] == '#' && shellstate.rear == shellstate.top){
//   			stack_init3(stackptrs[shellstate.rear], f_array, f_arraysize, &fibonacci, &main_stack, &stackptrs[shellstate.rear], &shellstate);
//     		shellstate.mult_result[shellstate.rear] = '*';
//     		shellstate.fibonacci = false;
	  		
//   		} 
//   		else if(shellstate.factorial == true && shellstate.mult_result[shellstate.rear] == '$' && shellstate.rear == shellstate.top){
// 			stack_init3(stackptrs[shellstate.rear], f_array, f_arraysize, &factorial, &main_stack, &stackptrs[shellstate.rear], &shellstate);
//     		shellstate.mult_result[shellstate.rear] = '@';
//     		shellstate.factorial = false;
	  		
// 	  	}
//   		else if(shellstate.mult_result[shellstate.top] == '*'){
//   			lapic.reset_timer_count(10000);
//   			preempt.flag = 0;
//   	 		stack_saverestore(main_stack, stackptrs[shellstate.top]);
//   	 		if(preempt.flag == 1){
//     			stackptrs[shellstate.top] = preempt.saved_stack;
//   	 		}
//   		}
//   		else if(shellstate.mult_result[shellstate.top] == '@'){
//   	 		lapic.reset_timer_count(10000);
//   			preempt.flag = 0;
//   	 		stack_saverestore(main_stack, stackptrs[shellstate.top]);
//   	 		if(preempt.flag == 1){
//     			stackptrs[shellstate.top] = preempt.saved_stack;
//   	 		}
//   		}
//   		int i;
//   		for(i=0;i<10;i++){
//   			shellstate.top = (shellstate.top + 1) % 10;
//   			if(shellstate.mult_result[shellstate.top] != 0){
//   				break;
//   			}
//   		}
// }
