#pragma once

#include "generated/lpc_kbd.dev.h"

namespace lpc_kbd{
	static inline bool has_key(lpc_kbd_t& dev){
		//lpc_kbd_status_t register_value = lpc_kbd_status_rd(&dev);
		if(lpc_kbd_status_obf_extract(lpc_kbd_status_rd(&dev)) == 0)
			return false;
		else
			return true;
	}
	static inline uint8_t get_key(lpc_kbd_t& dev){
		return lpc_kbd_input_rd(&dev);
	}
}// namespace lpc_kbd
