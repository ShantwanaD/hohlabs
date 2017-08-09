#pragma once
#include "util/config.h"
#include "util/debug.h"

struct shellstate_t{
	uint8_t row;
	uint8_t column;
	char ch;
	uint8_t nkeys;
	bool echo;
	bool longtask;
	bool fiblong;
	bool corlong;
	bool factorial;
	bool fibonacci;
	char command[50];
	char result[50];
	char mult_result[10];
	uint8_t top;
	uint8_t rear;
	uint8_t offset;
};

struct renderstate_t{
	uint8_t row;
	uint8_t column;
	char ch;
	uint8_t nkeys;
	bool echo;
	bool longtask;
	bool corlong;
	bool fiblong;
	char command[50];
	char result[50];
	uint8_t offset;
};

void shell_init(shellstate_t& state);
void shell_update(uint8_t scankey, shellstate_t& stateinout);
void shell_step(shellstate_t& stateinout);
void shell_render(const shellstate_t& shell, renderstate_t& render);

bool render_eq(const renderstate_t& a, const renderstate_t& b);
void render(const renderstate_t& state, int w, int h, addr_t display_base);

