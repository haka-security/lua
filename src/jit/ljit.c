/*
** ljit.c - Remi Bauzac (rbauzac@arkoon.net)
** Lua JiT skeleton
** See Copyright Notice in lua.h
*/

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ljit_c
#define LUA_CORE

#include "../lua.h"
#include "../ldebug.h"
#include "../lopcodes.h"
#include "../lstate.h"
#include "../lvm.h"
#include "../lauxlib.h"
#include "ljit.h"

#if defined LUA_USE_JIT_LINUX_X86_64
#include "arch/linux_x86_64.h"
#elif defined LUA_USE_JIT_MACOSX_X86_64
#include "arch/macosx_x86_64.h"
#elif defined LUA_USE_JIT_FREEBSD_X86_64
#include "arch/freebsd_x86_64.h"
#else
#error "Jit is not supported for this plateform"
#endif

static int get_jit_size(lua_State *L, Proto *p)
{
	int len = 0, pc, sz;
	const Instruction* code = p->code;

	for (len = PROLOGUE_LEN, pc = 0; pc < p->sizecode; pc++) {
    sz = 0;
    if (generator[GET_OPCODE(code[pc])].size != NULL) {
      sz = generator[GET_OPCODE(code[pc])].size(p, code, NULL, pc);
    }
		if (!sz) {
			luaG_runerror(L, "Unknown instruction size for %s\n", luaP_opnames[GET_OPCODE(code[pc])]);
			return sz;
		}
		len += sz;
	}
	return len;
}

static int get_jit(lua_State* L, Proto *p)
{
	uint8_t *prog;
	int pc, proglen;
	unsigned int *addrs; /* addresses of image addr offset */
	const Instruction* code = p->code;

	prog = p->jit;

	/* SetUp max addresses for Jumps */
	addrs = malloc(sizeof(*addrs)*(p->sizecode+1));
	if (!addrs) return 1;

	for (proglen = PROLOGUE_LEN, pc = 0; pc < p->sizecode; pc++) {
//		Instruction i = code[pc];
		addrs[pc]= proglen;
		proglen += generator[GET_OPCODE(code[pc])].size(p, code, NULL, pc);
	}
	/* offset addrss for the least instruction */
	addrs[pc] = proglen;

	if (p->sizecode > 0) {
		/* Prologue */
		uint8_t *tmp = prog;
		JIT_PROLOGUE;
		if (prog - tmp != PROLOGUE_LEN) {
			luaG_runerror(L, "Bad function prologue size for %d/%d\n", (prog - tmp), PROLOGUE_LEN);
		}
		/* End of prologue */
	}
	else {
		free(addrs);
		return -1;
	}

	for (pc = 0; pc < p->sizecode; pc++)  {
		uint8_t *tmp = prog;
		uint32_t clen = 0;
		Instruction i = code[pc];
    int jitcodesz = generator[GET_OPCODE(code[pc])].size(p, code, NULL, pc);

    if (generator[GET_OPCODE(code[pc])].create != NULL) {
      prog = generator[GET_OPCODE(i)].create(prog, p, code, addrs, pc);
    }

		clen = prog - tmp;
		if (clen != jitcodesz) {
			luaG_runerror(L, "Bad instruction size for instruction %s - %d/%d\n",
					luaP_opnames[GET_OPCODE(i)], clen, jitcodesz);
		}

	}
	free(addrs);
	return 0;
}

int luaJ_create(lua_State* L, Proto *p)
{
	if (!L || !p) return 1;

	if (p->sizejit && p->jit) {
		return 0;
	}

  p->called = 0;
	p->sizejit = get_jit_size(L, p);
	if (!p->sizejit) {
		luaG_runerror(L, "luaJ_create: cannot get jit size for %s (from %d to %d)\n",
				p->source ? getstr(p->source) : "?", p->linedefined,
        p->lastlinedefined);
		p->jit = NULL;
		return 1;
	}

	if (jit_alloc(p) == 1) {
		luaG_runerror(L, "luaJ_create: cannot alloc memory (%d bytes) for %s (from %d to %d)\n",
				p->sizejit, p->source ? getstr(p->source) : "?", p->linedefined,
        p->lastlinedefined);
		return 1;
	}

	if (get_jit(L, p) != 0) {
		jit_free(p);
		luaG_runerror(L, "luaJ_create: cannot create code (%d bytes) for %s (from %d to %d)\n",
				p->sizejit, p->source ? getstr(p->source) : "?", p->linedefined,
        p->lastlinedefined);
		return 1;
	}
	return 0;
}

void luaJ_init_offset(CallInfo *ci)
{
	/* Create initial offset for jmpq in prologu */
	ci->u.l.jitoffset = PROLOGUE_LEN;
}

/**
 * Jit Add/Remove functions
 */
static inline void jit_set_proto(lua_State *L, Proto *p, int state)
{
  int i;
  if (state) luaJ_create(L, p);
  else jit_free(p);
  for(i = 0; i < p->sizep; i++) {
    jit_set_proto(L, p->p[i], state);
  }
}

static inline int jit_add_remove(lua_State *L, int state)
{
  int i, n = lua_gettop(L);
  Proto *p;

  if (n == 0) {
    CallInfo *ci = L->ci->previous;
    if (ci && ttisLclosure(ci->func)) {
      Proto *p = clLvalue(ci->func)->p;
      for (i = 0; i < p->sizep; i++) {
        jit_set_proto(L, p->p[i], state);
      }
    }
  }
  else {
    for (i = 1; i <= n; i++) {
      p = lua_tolfunction(L, i);
      if (p) {
        jit_set_proto(L, p, state);
      }
    }
  }
  return 0;
}

static int jit_add(lua_State *L)
{
  return jit_add_remove(L, 1);
}

static int jit_remove(lua_State *L)
{
  return jit_add_remove(L, 0);
}

/**
 * Jit statistics functions
 */
static inline void jit_print_proto(lua_State *L, Proto *p, int level)
{
  int i;
  const char* s = p->source ? getstr(p->source) : "?";

  for (i = 0; i < level; i++) {
    fprintf(stderr, "  ");
  }
  if (p->jit != NULL) {
    fprintf(stderr, "\n%s <%s:%d,%d> at %p (%d) called %d times\n",
        p->linedefined == 0 ? "main":"function", s,
        p->linedefined,p->lastlinedefined, p->jit, p->sizejit, p->called);
  }

  for(i = 0; i < p->sizep; i++) {
    jit_print_proto(L, p->p[i], level+1);
  }
}

static int jit_stats(lua_State *L)
{
  int i, n = lua_gettop(L);
  Proto *p;

  if (n == 0) {
    CallInfo *ci = L->ci->previous;
    if (ci && ttisLclosure(ci->func)) {
      Proto *p = clLvalue(ci->func)->p;
      jit_print_proto(L, p, 0);
    }
  }
  else {
    for (i = 1; i <= n; i++) {
      p = lua_tolfunction(L, i);
      if (p) {
        jit_print_proto(L, p, 0);
      }
    }
  }
  return 0;
}

static const luaL_Reg jitlib[] = {
  {"add", jit_add},
  {"remove", jit_remove},
  {"stats", jit_stats},
  {NULL, NULL}
};

LUAMOD_API int luaopen_jit (lua_State *L) {
  luaL_newlib(L, jitlib);
  return 1;
}

