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
	const Instruction* c = p->code;

	for (len = PROLOGUE_LEN, pc = 0; pc < p->sizecode; pc++) {
		Instruction i = c[pc];
		sz = jit_opcodes[GET_OPCODE(i)];
		if (!sz) {
			luaG_runerror(L, "Unknown instruction size for %s\n", luaP_opnames[GET_OPCODE(i)]);
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
	const Instruction* c = p->code;

	prog = p->jit;

	/* SetUp max addresses for Jumps */
	addrs = malloc(sizeof(*addrs)*(p->sizecode+1));
	if (!addrs) return 1;

	for (proglen = PROLOGUE_LEN, pc = 0; pc < p->sizecode; pc++) {
		Instruction i = c[pc];
		addrs[pc]= proglen;
		proglen += jit_opcodes[GET_OPCODE(i)];
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
		Instruction i = c[pc];

		switch(GET_OPCODE(i)) {
			/**
			 * Return case
			 */
			CASE_OP(OP_RETURN)
			CASE_OP(OP_LOADK)
			CASE_OP(OP_LOADKX)
			CASE_OP(OP_EXTRAARG)
			CASE_OP(OP_LOADBOOL)
			CASE_OP(OP_LOADNIL)
			CASE_OP(OP_NOT)
			CASE_OP(OP_ADD)
			CASE_OP(OP_SUB)
			CASE_OP(OP_MUL)
			CASE_OP(OP_DIV)
			CASE_OP(OP_MOD)
			CASE_OP(OP_POW)
			CASE_OP(OP_UNM)
			CASE_OP(OP_GETTABUP)
			CASE_OP(OP_SETTABUP)
			CASE_OP(OP_MOVE)
			CASE_OP(OP_CALL)
			CASE_OP(OP_CLOSURE)
			CASE_OP(OP_GETTABLE)
			CASE_OP(OP_LEN)
			CASE_OP(OP_JMP)
			CASE_OP(OP_LT)
			CASE_OP(OP_LE)
			CASE_OP(OP_EQ)
			CASE_OP(OP_NEWTABLE)
			CASE_OP(OP_SETUPVAL)
			CASE_OP(OP_GETUPVAL)
			CASE_OP(OP_FORPREP)
			CASE_OP(OP_FORLOOP)
			CASE_OP(OP_SETTABLE)
			CASE_OP(OP_SELF)
			CASE_OP(OP_SETLIST)
			CASE_OP(OP_TFORCALL)
			CASE_OP(OP_TFORLOOP)
			CASE_OP(OP_TEST)
			CASE_OP(OP_TESTSET)
			CASE_OP(OP_CONCAT)
			CASE_OP(OP_VARARG)
			CASE_OP(OP_TAILCALL)

			default:
				luaG_runerror(L, "Unknown instruction %s - please investigate\n", luaP_opnames[GET_OPCODE(i)]);
				free(addrs);
				return -1;
		}
		clen = prog - tmp;
		if (clen != jit_opcodes[GET_OPCODE(i)]) {
			luaG_runerror(L, "Bad instruction size for instruction %s - %d/%d\n",
					luaP_opnames[GET_OPCODE(i)], clen, jit_opcodes[GET_OPCODE(i)]);
		}

	}
	free(addrs);
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
