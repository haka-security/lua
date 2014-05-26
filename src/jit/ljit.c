/*
** ljit.c - Remi Bauzac (rbauzac@arkoon.net)
** Lua JiT skeleton
** See Copyright Notice in lua.h
*/

#include <stdlib.h>
#include <string.h>

#define ljit_c
#define LUA_CORE

#include "../lua.h"
#include "../ldebug.h"
#include "../lopcodes.h"
#include "../lstate.h"
#include "../lvm.h"
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

static int get_jit(lua_State* L, CallInfo *ci, Proto *p)
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

static int type_propagation(lua_State* L, CallInfo *ci)
{
  LClosure *cl = clLvalue(ci->func);
  Proto *p = cl->p;
  const Instruction* code = p->code;
  int pc;
  TValue *k = p->k;

  p->stypes = luaM_malloc(L, p->maxstacksize*sizeof(StackTypes));
  for (pc = 0; pc < p->maxstacksize; pc++) {
    p->stypes[pc].types = 0;
    p->stypes[pc].sizetypes = 0;
  }

  for (pc = 0; pc < p->sizecode; pc++)  {
    Instruction i = code[pc];
    OpCode o=GET_OPCODE(i);
    int a=GETARG_A(i);
    int b=GETARG_B(i);
    //int c=GETARG_C(i);
    int ax=GETARG_Ax(i);
    int bx=GETARG_Bx(i);
    //int sbx=GETARG_sBx(i);

    switch(o) {
      TValue* v;
      case OP_LOADK:
        v = k + bx;
        if (!(p->stypes[a].types & (1 << ttypenv(v)))) {
          p->stypes[a].sizetypes++;
          p->stypes[a].types |= (1 << ttypenv(v));
        }
        break;
      case OP_LOADKX:
        i = code[++pc]; ax = GETARG_Ax(i);
        v = k + ax;
        if (!(p->stypes[a].types & (1 << ttypenv(v)))) {
          p->stypes[a].sizetypes++;
          p->stypes[a].types |= (1 << ttypenv(v));
        }
        break;
      case OP_MOVE:
        if (!(p->stypes[a].types & p->stypes[b].types)) {
          p->stypes[a].sizetypes++;
          p->stypes[a].types |= p->stypes[b].types;
        }
        break;
      case OP_GETUPVAL:
        v = cl->upvals[b]->v;
        if (!(p->stypes[a].types & (1 << ttypenv(v)))) {
          p->stypes[a].sizetypes++;
          p->stypes[a].types |= (1 << ttypenv(v));
        }
        break;
      default:
        break;
    }
  }
  return 0;
}

int luaJ_create(lua_State* L, CallInfo *ci)
{
	Proto *p;
	const char* s;

	if (!L) return 1;
	if (!lua_getjit(L)) return 0;


	p = clLvalue(ci->func)->p;
	if (!p) {
		luaG_runerror(L, "luaJ_create: cannot get function to jit\n");
		return 1;
	}

	s = p->source ? getstr(p->source) : "=?";
	if (*s=='@' || *s=='=')
		s++;
	else if (*s==LUA_SIGNATURE[0])
		s="(bstring)";
	else
		s="(string)";

	if (p->sizejit && p->jit) {
		luaJ_init_offset(ci);
		return 0;
	}

  /**
   * Make optimisations before compiling LUA
   */
  type_propagation(L, ci);

	p->sizejit = get_jit_size(L, p);
	if (!p->sizejit) {
		luaG_runerror(L, "luaJ_create: cannot get jit size for %s (from %d to %d)\n",
				s, p->linedefined, p->lastlinedefined);
		p->jit = NULL;
		return 1;
	}

	if (jit_alloc(p) == 1) {
		luaG_runerror(L, "luaJ_create: cannot alloc memory (%d bytes) for %s (from %d to %d)\n",
				p->sizejit, s, p->linedefined, p->lastlinedefined);
		return 1;
	}

	if (get_jit(L, ci, p) != 0) {
		jit_free(p);
		luaG_runerror(L, "luaJ_create: cannot create code (%d bytes) for %s (from %d to %d)\n",
				p->sizejit, s, p->linedefined, p->lastlinedefined);
		return 1;
	}


#ifdef JIT_DEBUG
	{
		const char* s=p->source ? getstr(p->source) : "=?";
		if (*s=='@' || *s=='=')
			s++;
		else if (*s==LUA_SIGNATURE[0])
			s="(bstring)";
		else
			s="(string)";
		fprintf(stderr, "\n%s <%s:%d,%d> at %p (%d)\n",
			p->linedefined==0?"main":"function", s,
			p->linedefined,p->lastlinedefined, p->jit, p->sizejit);
	}
#endif
	luaJ_init_offset(ci);
	return 0;
}

void luaJ_init_offset(CallInfo *ci)
{
	/* Create initial offset for jmpq in prologu */
	ci->u.l.jitoffset = PROLOGUE_LEN;
}
