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

int luaJ_create(lua_State* L, Proto *p)
{
	uint8_t *prog, *tmp, *orig;
	int pc;
	unsigned int *addrs; /* addresses of image addr offset */
	const Instruction* code = p->code;

	if (!L || !p) return 1;

	if (p->sizejit && p->jit) {
		return 0;
	}

  p->called = 0;

  /**
   * Alloc temporary code buffer
   */
  prog = luaM_malloc(L, (p->sizecode+1)*OP_SZ_MAX);
  if (!prog) {
    luaG_runerror(L, "No enougth memory to alloc Jit temporary code\n");
    return 1;
  }

	/* SetUp max addresses for Jumps and initialize it */
  addrs = luaM_newvector(L, p->sizecode+1, unsigned int);
	if (!addrs) {
    luaG_runerror(L, "Not enougth memory to alloc Jit addresses\n");
    luaM_free(L, prog);
    return 1;
  }
  for (pc = 0; pc <= p->sizecode; pc++) addrs[pc] = JITADDR_NONE;

  /**
   * Fist pass - prologue, then code
   */
  tmp = prog;
  orig= prog;
  JIT_PROLOGUE;
  addrs[0] = prog - tmp;
  for (pc = 0; pc < p->sizecode; pc++)  {
    Instruction i = code[pc];

    tmp = prog;
    prog = op_generic(prog, p, code, addrs, pc);
    prog = jit_create_funcs[GET_OPCODE(i)](prog, p, code, addrs, pc);
    addrs[pc+1] = addrs[pc] + prog - tmp;
  }

  /**
   * Verify addrs and get final size
   */
  for (pc = 0; pc <= p->sizecode; pc++) {
    if (addrs[pc] == JITADDR_NONE) {
      luaG_runerror(L, "Error on Jit generation\n");
      luaM_free(L, prog);
      luaM_free(L, addrs);
      return 1;
    }
  }
  p->sizejit = addrs[p->sizecode];

  /**
   * Alloc real buffer and free temporary one
   */
	if (jit_alloc(p)) {
		luaG_runerror(L, "luaJ_create: cannot alloc memory (%d bytes) for %s (from %d to %d)\n",
				p->sizejit, p->source ? getstr(p->source) : "?", p->linedefined,
        p->lastlinedefined);
		return 1;
	}
  luaM_free(L, orig);

  /**
   * Second pass
   */
  prog = p->jit;
  tmp = prog;
  JIT_PROLOGUE;
  addrs[0] = prog - tmp;
  for (pc = 0; pc < p->sizecode; pc++)  {
    Instruction i = code[pc];

    tmp = prog;
    prog = op_generic(prog, p, code, addrs, pc);
    prog = jit_create_funcs[GET_OPCODE(i)](prog, p, code, addrs, pc);
    addrs[pc+1] = addrs[pc] + prog - tmp;
  }
  p->addrs = addrs;
  /**
   * Clen opcodes stats
   */
  for(pc = 0; pc < NUM_OPCODES; pc++) {
    p->opcodes[pc] = 0;
  }
  return 0;
}

void luaJ_free(lua_State* L, Proto *p)
{
  p->sizejit = 0;
  jit_free(p);
  luaM_free(L, p->addrs);
}

/**
 * Jit Add/Remove functions
 */
static inline void jit_set_proto(lua_State *L, Proto *p, int state)
{
  int i;
  if (state) luaJ_create(L, p);
  else luaJ_free(L, p);
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
      jit_set_proto(L, clLvalue(ci->func)->p, state);
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
static inline void jit_print_proto(lua_State *L, Proto *p, luaL_Buffer *b)
{
  int i, sz;
  unsigned long total = 0;
  const char* s = p->source ? getstr(p->source) : "?";
  char *r = luaL_prepbuffer(b);

  if (p->jit != NULL) {
    sz = sprintf(r, "\n%s <%s:%d,%d> at %p (%d bytes) called %d times\n",
        p->linedefined == 0 ? "main":"function", s,
        p->linedefined,p->lastlinedefined, p->jit, p->sizejit, p->called);
    luaL_addsize(b, sz);
    for (i = 0; i < NUM_OPCODES; i++) {
      if (p->opcodes[i]) {
        r = luaL_prepbuffer(b);
        sz = sprintf(r, "  %s: %d\n", luaP_opnames[i], p->opcodes[i]);
        luaL_addsize(b, sz);
        total+= p->opcodes[i];
      }
    }
    r = luaL_prepbuffer(b);
    sz = sprintf(r, "  Total: %lu\n", total);
    luaL_addsize(b, sz);
  }

  for(i = 0; i < p->sizep; i++) {
    jit_print_proto(L, p->p[i], b);
  }
}

static int jit_stats(lua_State *L)
{
  int i, n = lua_gettop(L);
  Proto *p;
  luaL_Buffer b;
  luaL_buffinit(L,&b);

  if (n == 0) {
    CallInfo *ci = L->ci->previous;
    if (ci && ttisLclosure(ci->func)) {
      Proto *p = clLvalue(ci->func)->p;
      jit_print_proto(L, p, &b);
    }
  }
  else {
    for (i = 1; i <= n; i++) {
      p = lua_tolfunction(L, i);
      if (p) {
        jit_print_proto(L, p, &b);
      }
    }
  }
  luaL_pushresult(&b);
  return 1;
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

