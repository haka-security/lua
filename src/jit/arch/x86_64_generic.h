/*
** x86_64_generic.h - Remi Bauzac (rbauzac@arkoon.net)
** Lua JiT implementation for x86_64 processors (Intel compat) with common ABI
** See Copyright Notice in lua.h
*/

#ifndef __x86_64_generic_h__
#define __x86_64_generic_h__

/* Jumps constants */
#define X86_JB  0x72
#define X86_JAE 0x73
#define X86_JE  0x74
#define X86_JNE 0x75
#define X86_JBE 0x76
#define X86_JA  0x77
#define X86_NJ  0xeb /* near jump */
#define X86_LJ  0xe9 /* near jump */
/*
 * Registers :
 *  %rbp : frame pointer
 *  %rax : scratch register
 *  %r10 : scratch register
 *  %rbx : L (Lua state) callee saved register
 *  %r12 : &savedpc
 *  %r13 : ci (Call Info) callee saved register
 *  %r14 : k (constant base) callee saved register
 *  %r15 : base (stack base) callee saved register
 *
 * ABI: %rdi, %rsi, %rdx, %rcx, %r8, %r9
 *
 * Stack:
 *  -8(%rbp):saved %rax register
 *  -16(%rbp): saved %rbx callee saved register
 *  -24(%rbp): saved %r12 callee saved register
 *  -32(%rbp): saved %r13 callee saved register
 *  -40(%rbp): saved %r14 callee saved register
 *  -48(%rbp): saved %r15 callee saved register
 *  -56(%rbp): jit code pointer
 *  -64(%rbp): jitoffset address
 */

#define NOP APPEND1(0x90);

#define RABC_RDI(arg) \
	/* mov %r15, %rdi */\
	APPEND3(0x4c, 0x89, 0xff);\
	/* add (arg)*sizeof(TValue), %rdi */ \
	APPEND3(0x48, 0x81, 0xc7);\
	APPEND((arg)*sizeof(TValue), 4);

#define RKBC_RDI(arg) \
	if (ISK(arg)) { \
		/* k+INDEXK(arg)*sizeof(Tvalue) */\
		/* mov %r14, %rdi */ \
		APPEND3(0x4c, 0x89, 0xf7);\
		/* add INDEXK(arg)*sizeof(TValue), %rdi */\
		APPEND3(0x48, 0x81, 0xc7);\
		APPEND(INDEXK(arg)*sizeof(TValue), 4);\
	}\
	else { \
		RABC_RDI(arg); \
	}

#define RABC_RSI(arg) \
	/* mov %r15, %rsi */\
	APPEND3(0x4c, 0x89, 0xfe);\
	/* add (arg)*sizeof(TValue), %rsi */ \
	APPEND3(0x48, 0x81, 0xc6);\
	APPEND((arg)*sizeof(TValue), 4);

#define RKBC_RSI(arg) \
	if (ISK(arg)) { \
		/* k+INDEXK(arg)*sizeof(Tvalue) */\
		/* mov %r14, %rsi */ \
		APPEND3(0x4c, 0x89, 0xf6);\
		/* add INDEXK(arg)*sizeof(TValue), %rsi */\
		APPEND3(0x48, 0x81, 0xc6);\
		APPEND(INDEXK(arg)*sizeof(TValue), 4);\
	}\
	else { \
		RABC_RSI(arg); \
	}

#define RABC_RDX(arg) \
	/* mov %r15, %rdx */\
	APPEND3(0x4c, 0x89, 0xfa);\
	/* add (arg)*sizeof(TValue), %rdx */ \
	APPEND3(0x48, 0x81, 0xc2);\
	APPEND((arg)*sizeof(TValue), 4);

#define RKBC_RDX(arg) \
	if (ISK(arg)) { \
		/* k+INDEXK(arg)*sizeof(Tvalue) */\
		/* mov %r14, %rdx */ \
		APPEND3(0x4c, 0x89, 0xf2);\
		/* add INDEXK(arg)*sizeof(TValue), %rdx */\
		APPEND3(0x48, 0x81, 0xc2);\
		APPEND(INDEXK(arg)*sizeof(TValue), 4);\
	}\
	else { \
		RABC_RDX(arg); \
	}

#define RABC_RCX(arg) \
	/* mov %r15, %rcx */\
	APPEND3(0x4c, 0x89, 0xf9);\
	/* add (arg)*sizeof(TValue), %rcx */ \
	APPEND3(0x48, 0x81, 0xc1);\
	APPEND((arg)*sizeof(TValue), 4);

#define RKBC_RCX(arg) \
	if (ISK(arg)) { \
		/* k+INDEXK(arg)*sizeof(Tvalue) */\
		/* mov %r14, %rcx */ \
		APPEND3(0x4c, 0x89, 0xf1);\
		/* add INDEXK(arg)*sizeof(TValue), %rcx */\
		APPEND3(0x48, 0x81, 0xc1);\
		APPEND(INDEXK(arg)*sizeof(TValue), 4);\
	}\
	else { \
		RABC_RCX(arg); \
	}

#define RABC_R8(arg) \
	/* mov %r15, %r8 */\
	APPEND3(0x4d, 0x89, 0xf8);\
	/* add (arg)*sizeof(TValue), %r8 */ \
	APPEND3(0x49, 0x81, 0xc0);\
	APPEND((arg)*sizeof(TValue), 4);

#define RKBC_R8(arg) \
	if (ISK(arg)) { \
		/* k+INDEXK(arg)*sizeof(Tvalue) */\
		/* mov %r14, %r8 */ \
		APPEND3(0x4d, 0x89, 0xf0);\
		/* add INDEXK(arg)*sizeof(TValue), %r8 */\
		APPEND3(0x49, 0x81, 0xc0);\
		APPEND(INDEXK(arg)*sizeof(TValue), 4);\
	}\
	else { \
		RABC_R8(arg); \
	}

#define RABC_R9(arg) \
	/* mov %r15, %r8 */\
	APPEND3(0x4c, 0x89, 0xf9);\
	/* add (arg)*sizeof(TValue), %r8 */ \
	APPEND3(0x49, 0x81, 0xc1);\
	APPEND((arg)*sizeof(TValue), 4);

#define RKBC_R9(arg) \
	if (ISK(arg)) { \
		/* k+INDEXK(arg)*sizeof(Tvalue) */\
		/* mov %r14, %r8 */ \
		APPEND3(0x4d, 0x89, 0xf1);\
		/* add INDEXK(arg)*sizeof(TValue), %r8 */\
		APPEND3(0x49, 0x81, 0xc1);\
		APPEND(INDEXK(arg)*sizeof(TValue), 4);\
	}\
	else { \
		RABC_R9(arg); \
	}

#define JIT_RABC(arg) { \
	/* mov %r15, %rax */\
	APPEND3(0x4c, 0x89, 0xf8);\
    /* add arg*sizeof(TValue), %rax*/\
    APPEND2(0x48, 0x05);\
    APPEND((arg)*sizeof(TValue), 4);\
}

#define LUA_UPDATE_BASE \
	/* mov offset8(%r13),%r15 */ \
	APPEND4(0x4d, 0x8b, 0x7d, offsetof(CallInfo, u.l.base));

#define LUA_ADD_SAVEDPC(arg) \
	/* addq arg*sizeof(Instrcution) (%r12) */ \
	APPEND4(0x49, 0x81, 0x04, 0x24); \
	APPEND((arg)*sizeof(Instruction), 4);

#define VM_CALL(func) \
	/* mov func, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&(func), 8); \
	/* call func */ \
	APPEND2(0xff, 0xd0); \

#define JIT_UPDATEOFFSET(off) \
	/* mov -64(%rbp), %rax*/ \
	APPEND4(0x48, 0x8b, 0x45, 0xc0); \
	/* movl off, (%rax) */ \
	APPEND2(0xc7, 0x00); \
	APPEND(off, 4);

#define SAVE_REGISTERS \
	/* mov %rax, -8(%rbp) */ \
	APPEND4(0x48, 0x89, 0x45, 0xf8); \
	/* mov %rbx, -16(%rbp) */ \
	APPEND4(0x48, 0x89, 0x5d, 0xf0); \
	/* mov %r12, -24(%rbp) */ \
	APPEND4(0x4c, 0x89, 0x65, 0xe8); \
	/* mov %r13, -32(%rbp) */ \
	APPEND4(0x4c, 0x89, 0x6d, 0xe0); \
	/* mov %r14, -40(%rbp) */ \
	APPEND4(0x4c, 0x89, 0x75, 0xd8); \
	/* mov %r15, -48(%rbp) */ \
	APPEND4(0x4c, 0x89, 0x7d, 0xd0);

#define RESTORE_REGISTERS \
	/* mov -8(%rbp), %rax */ \
	APPEND4(0x48, 0x8b, 0x45, 0xf8); \
	/* mov -16(%rbp), %rbx */ \
	APPEND4(0x48, 0x8b, 0x5d, 0xf0); \
	/* mov -24(%rbp), %r12 */ \
	APPEND4(0x4c, 0x8b, 0x65, 0xe8); \
	/* mov -32(%rbp), %r13 */ \
	APPEND4(0x4c, 0x8b, 0x6d, 0xe0); \
	/* mov -40(%rbp), %r14 */ \
	APPEND4(0x4c, 0x8b, 0x75, 0xd8); \
	/* mov -48(%rbp), %r15 */ \
	APPEND4(0x4c, 0x8b, 0x7d, 0xd0);


static inline int jit_opcode_size()
{
  return 0;
}
static int jit_opcodes[NUM_OPCODES] =
{
	40, /* OP_MOVE */
	40, /* OP_LOADK */
	48, /* OP_LOADKX */
	40, /* OP_LOADBOOL */
	40, /* OP_LOADNIL */
	40, /* OP_GETUPVAL */
	56, /* OP_GETTABUP */
	64, /* OP_GETTABLE */
	56, /* OP_SETTABUP */
	40, /* OP_SETUPVAL */
	64, /* OP_SETTABLE */
	56, /* OP_NEWTABLE */
	64, /* OP_SELF */
	64, /* OP_ADD */
	64,	/* OP_SUB */
	64, /* OP_MUL */
	64, /* OP_DIV */
	64, /* OP_MOD */
	64, /* OP_POW */
	48, /* OP_UNM */
	40, /* OP_NOT */
	48, /* OP_LEN */
	88, /* OP_CONCAT */
	40, /* OP_JMP */
	64, /* OP_EQ */
	64, /* OP_LT */
	64, /* OP_LE */
	56, /* OP_TEST */
	40, /* OP_TESTSET */
	64, /* OP_CALL */
	85, /* OP_TAILCALL */
	88, /* OP_RETURN */
	48, /* OP_FORLOOP */
	40, /* OP_FORPREP */
	48, /* OP_TFORCALL */
	48, /* OP_TFORLOOP */
	56, /* OP_SETLIST */
	56, /* OP_CLOSURE */
	48, /* OP_VARARG */
	1, /* OP_EXTRAARG */
};

#define PROLOGUE_LEN 96
#define JIT_PROLOGUE \
	APPEND4(0x55, 0x48, 0x89, 0xe5); /* push %rbp; mov %rsp,%rbp */ \
	APPEND4(0x48, 0x83, 0xec, 80);    /* subq  $72,%rsp       */ \
	/* First, save registers */ \
	SAVE_REGISTERS; \
	/* put L in %rbx */ \
	APPEND3(0x48, 0x89, 0xfb); /* mov %rdi, %rbx */ \
	/* put ci in r13 */ \
	APPEND3(0x49, 0x89, 0xf5); \
	/* put k in r14 */ \
	APPEND3(0x49, 0x89, 0xce); /* mov %rcx, %r14 */ \
	/* put base in r15 */ \
	LUA_UPDATE_BASE; \
	\
	/* put &savedpc %r12 */ \
	/* lea offset8(%r13),%rax */ \
	APPEND4(0x49, 0x8d, 0x45, offsetof(CallInfo, u.l.savedpc)); \
	/* mov %rax, %r12 */ \
	APPEND3(0x49, 0x89, 0xc4); \
	\
	/* put code in stack: -64(%rbp) */ \
	/* mov offset8(%rdx), %r10 */ \
	APPEND4(0x4c, 0x8b, 0x52, offsetof(LClosure, p)); \
	/* mov offset8(%r10), %r10 */ \
	APPEND4(0x4d, 0x8b, 0x52, offsetof(Proto, code)); \
	/* mov %r10, -56(%rbp) */  \
	APPEND4(0x4c, 0x89, 0x55, 0xc8); \
	\
	/* put &jitoffset in stack: -64(%rbp)  */ \
	/* lea offset8(%r13),%rax */ \
	APPEND4(0x49, 0x8d, 0x45, offsetof(CallInfo, u.l.jitoffset)); \
	/* mov %rax,-64(%rbp) */ \
	APPEND4(0x48, 0x89, 0x45, 0xc0); \
	\
	/* Prepare initial Jump for coroutine (%rax) is now jitoffset */ \
	/* mov offset8(%rdx), %r10 */ \
	APPEND4(0x4c, 0x8b, 0x52, offsetof(LClosure, p)); \
	/* mov offset32(%r10), %r10 */ \
	APPEND3(0x4d, 0x8b, 0x92); \
	APPEND(offsetof(Proto, jit), 4); \
	/* add (%rax), %r10 */ \
	APPEND3(0x4c, 0x03, 0x10); \
  NOP;NOP;NOP;NOP;NOP;NOP;NOP; \
	/* jmpq %r10 */ \
	APPEND3(0x41, 0xff, 0xe2);

/**
 * OP_MOVE opcode
 */
static inline int op_move_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 40;
}
static inline uint8_t *op_move_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	RABC_RSI(GETARG_A(code[pc]));
	RABC_RDX(GETARG_B(code[pc]));
	/* mov (%rdx), %rax */
	APPEND3(0x48, 0x8b, 0x02);
	/* mov %rax, (%rsi) */
	APPEND3(0x48, 0x89, 0x06);
	/* mov 0x08(%rdx), %eax */
	APPEND3(0x8b, 0x42, 0x08);
	/* mov %eax, 0x08(%rsi) */
	APPEND3(0x89, 0x46, 0x08);
  return prog;
}

/**
 * OP_LOADK opcode
 */
static int op_loadk_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 40;
}
static uint8_t *op_loadk_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
  TValue *rb = p->k + GETARG_Bx(code[pc]);
  LUA_ADD_SAVEDPC(1);
	/* Get RA in rax */ \
  JIT_RABC(GETARG_A(code[pc]));
  /* mov rb->_value, %r10 */ \
  APPEND2(0x49, 0xba);\
  APPEND((uint64_t)val_(rb).p, 8); \
	/* mov %r10, (%rax) */ \
	APPEND3(0x4c, 0x89, 0x10); \
  /* movl rb->_tt, 0x8(%rax) */ \
  APPEND3(0xc7, 0x40, 0x8);\
  APPEND(rttype(rb), 4);
  NOP;NOP;NOP;
  return prog;
}

/**
 * OP_LOADKX opcode
 */
static int op_loadkx_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 48;
}

static uint8_t *op_loadkx_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(2);
	/* Get RA */
	JIT_RABC(GETARG_A(code[pc]));
	/* mov r14, %rdx */
	APPEND3(0x4c, 0x89, 0xf2);
	/* add GETARG_Bx(i)*sizeof(TValue), %rdx */
	APPEND3(0x48, 0x81, 0xc2);
	APPEND(GETARG_Ax(code[pc+1])*sizeof(TValue), 4);
	/* mov (%rdx), %r10 */
	APPEND3(0x4c, 0x8b, 0x12);
	/* mov %r10, (%rax) */
	APPEND3(0x4c, 0x89, 0x10);
	/* mov 0x08(%rdx), %r10d */
	APPEND4(0x44, 0x8b, 0x52, 0x08);
	/* mov %r10d, 0x8(%rax) */
	APPEND4(0x44, 0x89, 0x50, 0x08);
  NOP;NOP;NOP;NOP;NOP;
	/* jump over the next instruction */
	APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);
  return prog;
}

/**
 * OP_LOADBOOL opcode
 */
static inline int op_loadbool_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 40;
}
static inline uint8_t *op_loadbool_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	RABC_RDI(GETARG_A(code[pc]));
	/* mov GETARG_B(i), %esi */
	APPEND1(0xbe);
	APPEND(GETARG_B(code[pc]), 4);
	/* mov %esi, (%rdi) */
	APPEND2(0x89, 0x37);
	/* movl   LUA_TBOOLEAN,0x8(%rdi) */
	APPEND3(0xc7, 0x47, 0x08);
	APPEND(LUA_TBOOLEAN, 4);
	if (GETARG_C(code[pc])) {
		LUA_ADD_SAVEDPC(2);
		APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);
	}
	else {
		LUA_ADD_SAVEDPC(1);
    NOP;NOP
	}
  NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_LOADNIL opcode
 */
static int op_loadnil_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 40;
}
static uint8_t *op_loadnil_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1); \
	RABC_RDI(GETARG_A(code[pc])); \
	/* mov GETARG_B(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_B(code[pc]), 4); \
  NOP;NOP;NOP;NOP;NOP;
	VM_CALL(vm_setnil);
  return prog;
}

/**
 * OP_GETUPVAL opcode
 */
static int op_getupval_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 40;
}
static uint8_t *op_getupval_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	/* mov GETARG_B(i), %edx */
	APPEND1(0xba);
	APPEND(GETARG_B(code[pc]), 4);
	VM_CALL(vm_getupval);
  NOP;NOP;
  return prog;
}

/**
 * OP_GETTABUP opcode
 */
static int op_gettabup_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 56;
}
static uint8_t *op_gettabup_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov GETARG_B(i), %esi */
	APPEND1(0xbe);
	APPEND(GETARG_B(code[pc]), 4);
	RKBC_RDX(GETARG_C(code[pc]));
	RABC_RCX(GETARG_A(code[pc]));
  NOP;NOP;NOP;NOP;
	VM_CALL(vm_gettabup);
	/* vm_gettabup can realloc base, reset it */
	LUA_UPDATE_BASE;
  return prog;
}

/**
 * OP_GETTABLE opcode
 */
static int op_gettable_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_gettable_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_B(code[pc]));
	RKBC_RDX(GETARG_C(code[pc]));
	RABC_RCX(GETARG_A(code[pc]));
	VM_CALL(luaV_gettable);
	/* luaV_gettable can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_SETTABUP opcode
 */
static int op_settabup_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 56;
}
static uint8_t *op_settabup_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov GETARG_A(i), %esi */
	APPEND1(0xbe);
	APPEND(GETARG_A(code[pc]), 4);
	RKBC_RDX(GETARG_B(code[pc]));
	RKBC_RCX(GETARG_C(code[pc]));
	VM_CALL(vm_settabup);
	/* vm_settabup can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_SETUPVAL opcode
 */
static int op_setupval_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 40;
}
static uint8_t *op_setupval_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	/* mov GETARG_B(i), %edx */
	APPEND1(0xba);
	APPEND(GETARG_B(code[pc]), 4);
	VM_CALL(vm_setupval);
  NOP;NOP;
  return prog;
}

/**
 * OP_SETTABLE opcode
 */
static int op_settable_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_settable_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	RKBC_RDX(GETARG_B(code[pc]));
	RKBC_RCX(GETARG_C(code[pc]));
	VM_CALL(luaV_settable);
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_NEWTABLE opcode
 */
static int op_newtable_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 56;
}
static uint8_t *op_newtable_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov %r13, %rsi */
	APPEND3(0x4c, 0x89, 0xee);
	RABC_RDX(GETARG_A(code[pc]));
	/* mov GETARG_B(i), %ecx */
	APPEND1(0xb9);
	APPEND(GETARG_B(code[pc]), 4);
	/* mov GETARG_C(i), %r8d */
	APPEND2(0x41, 0xb8);
	APPEND(GETARG_C(code[pc]), 4);
	VM_CALL(vm_newtable);
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_SELF
 */
static int op_self_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_self_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	RKBC_RDX(GETARG_B(code[pc]));
	RKBC_RCX(GETARG_C(code[pc]));
	VM_CALL(vm_self);
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_ADD opcode
 */
static int op_add_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_add_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	/* Get RKB */
	RKBC_RDX(GETARG_B(code[pc]));
	/* Get RKC */
	RKBC_RCX(GETARG_C(code[pc]));
	VM_CALL(vm_add);
	/* vm_add can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_SUB opcode
 */
static int op_sub_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_sub_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	/* Get RKB */
	RKBC_RDX(GETARG_B(code[pc]));
	/* Get RKC */
	RKBC_RCX(GETARG_C(code[pc]));
	VM_CALL(vm_sub);
	/* vm_sub can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_MUL opcode
 */
static int op_mul_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_mul_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	/* Get RKB */
	RKBC_RDX(GETARG_B(code[pc]));
	/* Get RKC */
	RKBC_RCX(GETARG_C(code[pc]));
	VM_CALL(vm_mul);
	/* vm_mul can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_DIV opcode
 */
static int op_div_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_div_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	/* Get RKB */
	RKBC_RDX(GETARG_B(code[pc]));
	/* Get RKC */
	RKBC_RCX(GETARG_C(code[pc]));
	VM_CALL(vm_div);
	/* vm_div can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_MOD opcode
 */
static int op_mod_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_mod_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	/* Get RKB */
	RKBC_RDX(GETARG_B(code[pc]));
	/* Get RKC */
	RKBC_RCX(GETARG_C(code[pc]));
	VM_CALL(vm_mod);
	/* vm_mod can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_POW opcode
 */
static int op_pow_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_pow_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	/* Get RKB */
	RKBC_RDX(GETARG_B(code[pc]));
	/* Get RKC */
	RKBC_RCX(GETARG_C(code[pc]));
	VM_CALL(vm_pow);
	/* vm_mod can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_UNM
 */
static int op_unm_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 48;
}
static uint8_t *op_unm_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(code[pc])); \
	/* Get RKB */ \
	RKBC_RDX(GETARG_B(code[pc])); \
	VM_CALL(vm_unm); \
	/* vm_unm can realloc base, reset it */ \
	LUA_UPDATE_BASE;
  NOP;
  return prog;
}

/**
 * OP_NOT opcode
 */
static int op_not_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 40;
}
static uint8_t *op_not_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1); \
	RABC_RDI(GETARG_A(code[pc])); \
	RABC_RSI(GETARG_B(code[pc])); \
	VM_CALL(vm_not);
  return prog;
}

/**
 * OP_LEN opcode
 */
static int op_len_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 48;
}
static uint8_t *op_len_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	RABC_RDX(GETARG_B(code[pc]));
	VM_CALL(luaV_objlen);
	/* luaV_objlen can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;
  return prog;
}

/**
 * OP_CONCAT opcode
 */
static int op_concat_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 88;
}
static uint8_t *op_concat_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov %r15, %rsi */
	APPEND3(0x4c, 0x89, 0xfe);
	/* mov GETARG_B(i), %edx */
	APPEND1(0xba);
	APPEND(GETARG_B(code[pc]), 4);
	/* mov GETARG_C(i), %ecx */
	APPEND1(0xb9);
	APPEND(GETARG_C(code[pc]), 4);
	VM_CALL(vm_concat);
	LUA_UPDATE_BASE;
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov %r13, %rsi */
	APPEND3(0x4c, 0x89, 0xee);
	RABC_RDX(GETARG_A(code[pc]));
	RABC_RCX(GETARG_B(code[pc]));
	VM_CALL(vm_setconcat);
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_JMP opcode
 */
static int op_jmp_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 40;
}
static uint8_t *op_jmp_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
  NOP;NOP;NOP;NOP;
	LUA_ADD_SAVEDPC(1+GETARG_sBx(code[pc]));
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov %r13, %rsi */
	APPEND3(0x4c, 0x89, 0xee);
	/* mov GETARG_A(i), %edx */
	APPEND1(0xba);
	APPEND(GETARG_A(code[pc]), 4);
	VM_CALL(vm_jumpclose);
	APPEND1(X86_LJ);
	APPEND(addrs[pc+1+GETARG_sBx(code[pc])] - addrs[pc+1], 4);
  return prog;
}

/**
 * OP_EQ opcode
 */
static int op_eq_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_eq_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RKBC_RSI(GETARG_B(code[pc]));
	RKBC_RDX(GETARG_C(code[pc]));
	VM_CALL(vm_eq);
	LUA_UPDATE_BASE;
	/* cmp GETARG_A(i), %eax */
	APPEND1(0x3d);
	APPEND(GETARG_A(code[pc]), 4);
	APPEND2(X86_JE, 10);
	LUA_ADD_SAVEDPC(1);
	APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);
  return prog;
}

/**
 * OP_LT opcode
 */
static int op_lt_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_lt_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RKBC_RSI(GETARG_B(code[pc]));
	RKBC_RDX(GETARG_C(code[pc]));
	VM_CALL(luaV_lessthan);
	LUA_UPDATE_BASE;
	/* cmp GETARG_A(i), %eax */
	APPEND1(0x3d);
	APPEND(GETARG_A(code[pc]), 4);
	APPEND2(X86_JE, 10);
	LUA_ADD_SAVEDPC(1);
	APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);
  return prog;
}

/**
 * OP_LE opcode
 */
static int op_le_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_le_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RKBC_RSI(GETARG_B(code[pc]));
	RKBC_RDX(GETARG_C(code[pc]));
	VM_CALL(luaV_lessequal);
	LUA_UPDATE_BASE;
	/* cmp GETARG_A(i), %eax */
	APPEND1(0x3d);
	APPEND(GETARG_A(code[pc]), 4);
	APPEND2(X86_JE, 10);
	LUA_ADD_SAVEDPC(1);
	APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);
  return prog;
}

/**
 * OP_TEST opcode
 */
static int op_test_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 56;
}
static uint8_t *op_test_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	RABC_RDI(GETARG_A(code[pc]));
	/* mov GETARG_C(i), %esi */
	APPEND1(0xbe);
	APPEND(GETARG_C(code[pc]), 4);
	VM_CALL(vm_test);
	/* cmpl %$0x1, %eax */
	APPEND3(0x83, 0xf8, 0X01);
	APPEND2(X86_JNE, 10);
	LUA_ADD_SAVEDPC(1);
	APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);
  NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_TESTSET opcode
 */
static int op_testset_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 40;
}
static uint8_t *op_testset_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	RABC_RDX(GETARG_B(code[pc]));
	/* movl GETARG_C(i), %ecx */
	APPEND1(0xb9);
	APPEND(GETARG_C(code[pc]), 4);
	VM_CALL(vm_testset);
  return prog;
}

/**
 * OP_CALL opcode
 */
static int op_call_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 64;
}
static uint8_t *op_call_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	JIT_UPDATEOFFSET(addrs[pc+1]);
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	/* mov GETARG_B(i), %edx */
	APPEND1(0xba);
	APPEND(GETARG_B(code[pc]), 4);
	/* mov GETARG_C(i), %ecx */
	APPEND1(0xb9);
	APPEND(GETARG_C(code[pc]), 4);
	/* mov %r13, %r8 */
	APPEND3(0x4d, 0x89, 0xe8);
	VM_CALL(vm_call);
	/* vm_call can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_TAILCALL opcaode
 */
static int op_tailcall_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 88;
}
static uint8_t *op_tailcall_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	JIT_UPDATEOFFSET(addrs[pc+1]);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov %r13, %rsi */
	APPEND3(0x4c, 0x89, 0xee);
	/*  mov %r15, %rdx */
	APPEND3(0x4c, 0x89, 0xfa);
	/* movl GETARG_A(i), %ecx */
	APPEND1(0xb9);
	APPEND(GETARG_A(code[pc]), 4);
	/* movl GETARG_B(i), %r8d */
	APPEND2(0x41, 0xb8);
	APPEND(GETARG_B(code[pc]), 4);
	VM_CALL(vm_tailcall);
	LUA_UPDATE_BASE;
	/* if C function we have to Jump to next return */
	/* cmp $0x1, %eax */
	APPEND3(0x83, 0xf8, 0x01);
	/* jeq +offset8 */
	APPEND2(X86_JE, 26);
	RESTORE_REGISTERS;
  NOP;NOP;NOP;
	/* leaveq; retq */
	APPEND2(0xc9, 0xc3);
  return prog;
}

/**
 * OP_RETURN opcode
 */
static int op_return_size(Proto *p, const Instruction *code, unsigned int *addrs, int pc)
{
  return 88;
}
static uint8_t *op_return_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	JIT_UPDATEOFFSET(addrs[pc+1]);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov %r15, %rsi */
	APPEND3(0x4c, 0x89, 0xfe);
	RABC_RDX(GETARG_A(code[pc]));
	/* mov %r13, %rcx */
	APPEND3(0x4c, 0x89, 0xe9);
	/* mov GETARG_B(i), %r8d */
	APPEND2(0x41, 0xb8);
	APPEND(GETARG_B(code[pc]), 4);
	VM_CALL(vm_return);
	RESTORE_REGISTERS;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
	/* leaveq; retq */
	APPEND2(0xc9, 0xc3);
  return prog;
}

/**
 * OP_FORLOOP opcode
 */
static int op_forloop_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 48;
}
static uint8_t *op_forloop_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1+GETARG_sBx(code[pc]));
  NOP;NOP;NOP;NOP;NOP;NOP;
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	VM_CALL(vm_forloop);
	/* cmpl %$0x1, %eax */
	APPEND3(0x83, 0xf8, 0x01);
	/* je +offset */
	APPEND2(0x0f, X86_JE+0x10);
	APPEND(addrs[pc+1+GETARG_sBx(code[pc])] - addrs[pc+1], 4);
  return prog;
}

/**
 * OP_FORPREP opcode
 */
static int op_forprep_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 40;
}
static uint8_t *op_forprep_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1+GETARG_sBx(code[pc]));
  NOP;NOP;
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	VM_CALL(vm_forprep);
	APPEND1(X86_LJ);
	APPEND(addrs[pc+1+GETARG_sBx(code[pc])] - addrs[pc+1], 4);
  return prog;
}

/**
 * OP_TFORCALL opcode
 */
static int op_tforcall_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 48;
}
static uint8_t *op_tforcall_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov %r13, %rsi */
	APPEND3(0x4c, 0x89, 0xee);
	RABC_RDX(GETARG_A(code[pc]));
	/* mov GETARG_C(i), %ecx */
	APPEND1(0xb9);
	APPEND(GETARG_C(code[pc]), 4);
	VM_CALL(vm_tforcall);
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;
  return prog;
}

/**
 * OP_TFORLOOP opcode
 */
static int op_tforloop_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 48;
}
static uint8_t *op_tforloop_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1+GETARG_sBx(code[pc]));
  NOP;NOP;NOP;NOP;NOP;NOP;
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	RABC_RSI(GETARG_A(code[pc]));
	VM_CALL(vm_tforloop);
	/* cmpl %$0x1, %eax */
	APPEND3(0x83, 0xf8, 0x01);
	APPEND2(0x0f, X86_JE+0x10);
	APPEND(addrs[pc+1+GETARG_sBx(code[pc])] - addrs[pc+1], 4);
  return prog;
}

/**
 * OP_SETLIST opcode
 */
static int op_setlist_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 56;
}
static uint8_t *op_setlist_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov %r13, %rsi */
	APPEND3(0x4c, 0x89, 0xee);
	RABC_RDX(GETARG_A(code[pc]));
	/* mov GETARG_B(i), %ecx */
	APPEND1(0xb9);
	APPEND(GETARG_B(code[pc]), 4);
	/* mov GETARG_C(i), %r8d */
	APPEND2(0x41, 0xb8);
	APPEND(GETARG_C(code[pc]), 4);
	/* mov GETARG_Ax(i+1), %r9d */
	APPEND2(0x41, 0xb9);
	APPEND(GETARG_Ax(code[pc+1]), 4);
	VM_CALL(vm_setlist);
  NOP;
	if (GET_OPCODE(code[pc+1]) == OP_EXTRAARG) {
		/* jump over OP_EXTRAARG */
		LUA_ADD_SAVEDPC(2);
		APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);
	}
	else {
		LUA_ADD_SAVEDPC(1);
		APPEND2(0x90, 0x90);
	}
  return prog;
}

/**
 * OP_CLOSURE opcode
 */
static int op_closure_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 56;
}
static uint8_t *op_closure_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov %r15, %rsi */
	APPEND3(0x4c, 0x89, 0xfe);
	RABC_RDX(GETARG_A(code[pc]));
	/* mov %r13, %rcx */
	APPEND3(0x4c, 0x89, 0xe9);
	/* movl GETARG_Bx(i), %r8d */
	APPEND2(0x41, 0xb8);
	APPEND(GETARG_Bx(code[pc]), 4);
	VM_CALL(vm_closure);
	/* vm_closure can realloc base, reset it */
	LUA_UPDATE_BASE;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

/**
 * OP_VARARG opcode
 */
static int op_vararg_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 48;
}
static uint8_t *op_vararg_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
	LUA_ADD_SAVEDPC(1);
  NOP;NOP;NOP;NOP;
	/* mov %rbx, %rdi */
	APPEND3(0x48, 0x89, 0xdf);
	/* mov %r13, %rsi */
	APPEND3(0x4c, 0x89, 0xee);
	/* mov %r15, %rdx */
	APPEND3(0x4c, 0x89, 0xfa);
	/* movl GETARG_A(i), %ecx */
	APPEND1(0xb9);
	APPEND(GETARG_A(code[pc]), 4);
	/* movl GETARG_B(i), %r8d */
	APPEND2(0x41, 0xb8);
	APPEND(GETARG_B(code[pc]), 4);
	VM_CALL(vm_vararg);
	LUA_UPDATE_BASE;
  return prog;
}

/**
 * OP_EXTRAARG opcode
 */

static int op_extraarg_size(Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  return 8;
}
static uint8_t *op_extraarg_create(uint8_t *bin, Proto *p, const Instruction *code,
    unsigned int *addrs, int pc)
{
  uint8_t *prog = bin;
  NOP;NOP;NOP;NOP;NOP;NOP;NOP;NOP;
  return prog;
}

static struct jit_generator generator[NUM_OPCODES] =
{
  { op_move_size, op_move_create },/* OP_MOVE,	A B	R(A) := R(B)					*/
  { op_loadk_size, op_loadk_create },/* OP_LOADK,	A Bx	R(A) := Kst(Bx)					*/
  { op_loadkx_size, op_loadkx_create },/* OP_LOADKX,	A	R(A) := Kst(extra arg)				*/
  { op_loadbool_size, op_loadbool_create },/* OP_LOADBOOL,	A B C	R(A) := (Bool)B; if (C) pc++			*/
  { op_loadnil_size, op_loadnil_create },/* OP_LOADNIL,	A B	R(A), R(A+1), ..., R(A+B) := nil		*/
  { op_getupval_size, op_getupval_create },/* OP_GETUPVAL,	A B	R(A) := UpValue[B]				*/
  { op_gettabup_size, op_gettabup_create },/* OP_GETTABUP,	A B C	R(A) := UpValue[B][RK(C)]			*/
  { op_gettable_size, op_gettable_create },/* OP_GETTABLE,	A B C	R(A) := R(B)[RK(C)]				*/
  { op_settabup_size, op_settabup_create },/* OP_SETTABUP,A B C	UpValue[A][RK(B)] := RK(C)			*/
  { op_setupval_size, op_setupval_create },/* OP_SETUPVAL,	A B	UpValue[B] := R(A)				*/
  { op_settable_size, op_settable_create },/* OP_SETTABLE,	A B C	R(A)[RK(B)] := RK(C)				*/
  { op_newtable_size, op_newtable_create },/* OP_NEWTABLE,	A B C	R(A) := {} (size = B,C)				*/
  { op_self_size, op_self_create },/* OP_SELF,	A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)]		*/
  { op_add_size, op_add_create },/* OP_ADD,	A B C	R(A) := RK(B) + RK(C)				*/
  { op_sub_size, op_sub_create },/* OP_SUB,	A B C	R(A) := RK(B) - RK(C)				*/
  { op_mul_size, op_mul_create },/* OP_MUL,	A B C	R(A) := RK(B) * RK(C)				*/
  { op_div_size, op_div_create },/* OP_DIV,	A B C	R(A) := RK(B) / RK(C)				*/
  { op_mod_size, op_mod_create },/* OP_MOD,	A B C	R(A) := RK(B) % RK(C)				*/
  { op_pow_size, op_pow_create },/* OP_POW,	A B C	R(A) := RK(B) ^ RK(C)				*/
  { op_unm_size, op_unm_create },/* OP_UNM,	A B	R(A) := -R(B)					*/
  { op_not_size, op_not_create },/* OP_NOT,	A B	R(A) := not R(B)				*/
  { op_len_size, op_len_create },/* OP_LEN,	A B	R(A) := length of R(B)				*/
  { op_concat_size, op_concat_create },/* OP_CONCAT,	A B C	R(A) := R(B).. ... ..R(C)			*/
  { op_jmp_size, op_jmp_create },/* OP_JMP,	A sBx	pc+=sBx; if (A) close all upvalues >= R(A) + 1	*/
  { op_eq_size, op_eq_create },/* OP_EQ,	A B C	if ((RK(B) == RK(C)) ~= A) then pc++		*/
  { op_lt_size, op_lt_create },/* OP_LT,	A B C	if ((RK(B) <  RK(C)) ~= A) then pc++		*/
  { op_le_size, op_le_create },/* OP_LE,	A B C	if ((RK(B) <= RK(C)) ~= A) then pc++		*/
  { op_test_size, op_test_create },/* OP_TEST,	A C	if not (R(A) <=> C) then pc++			*/
  { op_testset_size, op_testset_create },/* OP_TESTSET,	A B C	if (R(B) <=> C) then R(A) := R(B) else pc++	*/
  { op_call_size, op_call_create },/* OP_CALL,	A B C	R(A), ... ,R(A+C-2) := R(A)(R(A+1), ... ,R(A+B-1)) */
  { op_tailcall_size, op_tailcall_create },/* OP_TAILCALL,	A B C	return R(A)(R(A+1), ... ,R(A+B-1))		*/
  { op_return_size, op_return_create },/* OP_RETURN,	A B	return R(A), ... ,R(A+B-2)	(see note)	*/
  { op_forloop_size, op_forloop_create },/* OP_FORLOOP,	A sBx	R(A)+=R(A+2);
                       if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) }*/
  { op_forprep_size, op_forprep_create },/* OP_FORPREP,	A sBx	R(A)-=R(A+2); pc+=sBx				*/
  { op_tforcall_size, op_tforcall_create },/* OP_TFORCALL,	A C	R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2));	*/
  { op_tforloop_size, op_tforloop_create },/* OP_TFORLOOP,	A sBx	if R(A+1) ~= nil then { R(A)=R(A+1); pc += sBx }*/
  { op_setlist_size, op_setlist_create },/* OP_SETLIST,	A B C	R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B	*/
  { op_closure_size, op_closure_create },/* OP_CLOSURE,	A Bx	R(A) := closure(KPROTO[Bx])			*/
  { op_vararg_size, op_vararg_create },/* OP_VARARG,	A B	R(A), R(A+1), ..., R(A+B-2) = vararg		*/
  { op_extraarg_size, op_extraarg_create }/* OP_EXTRAARG	Ax	extra (larger) argument for previous opcode	*/
};

#endif
