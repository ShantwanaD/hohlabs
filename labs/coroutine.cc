#include "labs/shell.h"
#include "labs/coroutine.h"

void array_init(char* a);

//Long task implemented through stackless coroutine
 void corlong(coroutine_t& pf_coro, f_t& pf_locals, shellstate_t& stateinout){
      coroutine_t& f_coro = pf_coro;

      int& i              = pf_locals.i;
      int& sum            = pf_locals.sum;
      h_begin(f_coro);
      sum = 0;
      for(i=1; i<100000; i++){
        hoh_debug(i);
        sum += i;
        h_yield(f_coro);
      }
      //itoa(sum,stateinout.result[1],10);
      array_init(stateinout.result);
      stateinout.corlong = false;    //to stop this function from executing throughout
      stateinout.row += 2;
      stateinout.result[0] = '#'; // helps in rendering "Task completed, result" since we cannot check by the value of stateinout.corlong  
      stateinout.result[1] = sum; 
      h_end(f_coro); // yield (0,true)
    }


void shell_step_coroutine(shellstate_t& shellstate, coroutine_t& f_coro, f_t& f_locals){
  if(shellstate.corlong == true)
    corlong(f_coro, f_locals, shellstate);
  else{
  	coroutine_reset(f_coro);
  }
}


