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


static int jit_opcodes[NUM_OPCODES] =
{
	40, /* OP_MOVE */
	41, /* OP_LOADK */
	43, /* OP_LOADKX */
	34, /* OP_LOADBOOL */
	35, /* OP_LOADNIL */
	38, /* OP_GETUPVAL */
	52, /* OP_GETTABUP */
	57, /* OP_GETTABLE */
	52, /* OP_SETTABUP */
	38, /* OP_SETUPVAL */
	57, /* OP_SETTABLE */
	51, /* OP_NEWTABLE */
	57, /* OP_SELF */
	57, /* OP_ADD */
	57,	/* OP_SUB */
	57, /* OP_MUL */
	57, /* OP_DIV */
	57, /* OP_MOD */
	57, /* OP_POW */
	47, /* OP_UNM */
	40, /* OP_NOT */
	47, /* OP_LEN */
	82, /* OP_CONCAT */
	36, /* OP_JMP */
	64, /* OP_EQ */
	64, /* OP_LT */
	64, /* OP_LE */
	50, /* OP_TEST */
	40, /* OP_TESTSET */
	60, /* OP_CALL */
	85, /* OP_TAILCALL */
	81, /* OP_RETURN */
	42, /* OP_FORLOOP */
	38, /* OP_FORPREP */
	45, /* OP_TFORCALL */
	42, /* OP_TFORLOOP */
	55, /* OP_SETLIST */
	49, /* OP_CLOSURE */
	44, /* OP_VARARG */
	1, /* OP_EXTRAARG */
};

#define PROLOGUE_LEN 89
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
	/* jmpq %r10 */ \
	APPEND3(0x41, 0xff, 0xe2);

#define JIT_OP_RETURN \
	LUA_ADD_SAVEDPC(1); \
	JIT_UPDATEOFFSET(addrs[pc+1]); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r15, %rsi */ \
	APPEND3(0x4c, 0x89, 0xfe); \
	RABC_RDX(GETARG_A(i)); \
	/* mov %r13, %rcx */ \
	APPEND3(0x4c, 0x89, 0xe9); \
	/* mov GETARG_B(i), %r8d */ \
	APPEND2(0x41, 0xb8); \
	APPEND(GETARG_B(i), 4); \
	VM_CALL(vm_return); \
	RESTORE_REGISTERS; \
	/* leaveq; retq */ \
	APPEND2(0xc9, 0xc3); \

#define JIT_OP_LOADK \
	LUA_ADD_SAVEDPC(1); \
	/* Get RA in rax */ \
	JIT_RABC(GETARG_A(i)); \
	/* Get RB */ \
	/* mov %r14, %rdx */ \
	APPEND3(0x4c, 0x89, 0xf2); \
	/* add GETARG_Bx(i)*sizeof(TValue), %rdx */ \
	APPEND3(0x48, 0x81, 0xc2); \
	APPEND(GETARG_Bx(i)*sizeof(TValue), 4); \
	/* mov (%rdx), %r10 */ \
	APPEND3(0x4c, 0x8b, 0x12); \
	/* mov %r10, (%rax) */ \
	APPEND3(0x4c, 0x89, 0x10); \
	/* mov 0x08(%rdx), %r10d */ \
	APPEND4(0x44, 0x8b, 0x52, 0x08); \
	/* mov %r10d, 0x8(%rax) */ \
	APPEND4(0x44, 0x89, 0x50, 0x08);

#define JIT_OP_LOADKX \
	LUA_ADD_SAVEDPC(2); \
	/* Get RA */ \
	JIT_RABC(GETARG_A(i)); \
	/* mov r14, %rdx */ \
	APPEND3(0x4c, 0x89, 0xf2); \
	/* add GETARG_Bx(i)*sizeof(TValue), %rdx */ \
	APPEND3(0x48, 0x81, 0xc2); \
	APPEND(GETARG_Ax(c[pc+1])*sizeof(TValue), 4); \
	/* mov (%rdx), %r10 */ \
	APPEND3(0x4c, 0x8b, 0x12); \
	/* mov %r10, (%rax) */ \
	APPEND3(0x4c, 0x89, 0x10); \
	/* mov 0x08(%rdx), %r10d */ \
	APPEND4(0x44, 0x8b, 0x52, 0x08); \
	/* mov %r10d, 0x8(%rax) */ \
	APPEND4(0x44, 0x89, 0x50, 0x08); \
	/* jump over the next instruction */ \
	APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]); \

#define JIT_OP_LOADBOOL \
	RABC_RDI(GETARG_A(i)); \
	/* mov GETARG_B(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_B(i), 4); \
	/* mov %esi, (%rdi) */ \
	APPEND2(0x89, 0x37); \
	/* movl   LUA_TBOOLEAN,0x8(%rdi) */ \
	APPEND3(0xc7, 0x47, 0x08); \
	APPEND(LUA_TBOOLEAN, 4); \
	if (GETARG_C(i)) { \
		LUA_ADD_SAVEDPC(2); \
		APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]); \
	} \
	else { \
		LUA_ADD_SAVEDPC(1); \
		APPEND2(0x90, 0x90); \
	}

#define JIT_OP_LOADNIL \
	LUA_ADD_SAVEDPC(1); \
	RABC_RDI(GETARG_A(i)); \
	/* mov GETARG_B(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_B(i), 4); \
	VM_CALL(vm_setnil);

#define JIT_OP_NOT \
	LUA_ADD_SAVEDPC(1); \
	RABC_RDI(GETARG_A(i)); \
	RABC_RSI(GETARG_B(i)); \
	VM_CALL(vm_not);

#define JIT_OP_ADD \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	/* Get RKB */ \
	RKBC_RDX(GETARG_B(i)); \
	/* Get RKC */ \
	RKBC_RCX(GETARG_C(i)); \
	VM_CALL(vm_add); \
	/* vm_add can realloc base, reset it */ \
	LUA_UPDATE_BASE;

#define JIT_OP_SUB \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	/* Get RKB */ \
	RKBC_RDX(GETARG_B(i)); \
	/* Get RKC */ \
	RKBC_RCX(GETARG_C(i)); \
	VM_CALL(vm_sub); \
	/* vm_sub can realloc base, reset it */ \
	LUA_UPDATE_BASE;

#define JIT_OP_MUL \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
    RABC_RSI(GETARG_A(i)); \
    /* Get RKB */ \
    RKBC_RDX(GETARG_B(i)); \
    /* Get RKC */ \
    RKBC_RCX(GETARG_C(i)); \
	VM_CALL(vm_mul); \
    /* vm_mul can realloc base, reset it */ \
    LUA_UPDATE_BASE;

#define JIT_OP_DIV \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
    RABC_RSI(GETARG_A(i)); \
    /* Get RKB */ \
    RKBC_RDX(GETARG_B(i)); \
    /* Get RKC */ \
    RKBC_RCX(GETARG_C(i)); \
	VM_CALL(vm_div); \
    /* vm_div can realloc base, reset it */ \
    LUA_UPDATE_BASE;

#define JIT_OP_MOD \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
    RABC_RSI(GETARG_A(i)); \
    /* Get RKB */ \
    RKBC_RDX(GETARG_B(i)); \
    /* Get RKC */ \
    RKBC_RCX(GETARG_C(i)); \
	VM_CALL(vm_mod); \
    /* vm_mod can realloc base, reset it */ \
    LUA_UPDATE_BASE;

#define JIT_OP_POW \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
    RABC_RSI(GETARG_A(i)); \
    /* Get RKB */ \
    RKBC_RDX(GETARG_B(i)); \
    /* Get RKC */ \
    RKBC_RCX(GETARG_C(i)); \
	VM_CALL(vm_pow); \
    /* vm_pow can realloc base, reset it */ \
    LUA_UPDATE_BASE;

#define JIT_OP_UNM \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	/* Get RKB */ \
	RKBC_RDX(GETARG_B(i)); \
	VM_CALL(vm_unm); \
	/* vm_unm can realloc base, reset it */ \
	LUA_UPDATE_BASE;

#define JIT_OP_GETTABUP \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov GETARG_B(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_B(i), 4); \
	RKBC_RDX(GETARG_C(i)); \
	RABC_RCX(GETARG_A(i)); \
	VM_CALL(vm_gettabup); \
	/* vm_gettabup can realloc base, reset it */ \
	LUA_UPDATE_BASE;

#define JIT_OP_SETTABUP \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov GETARG_A(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_A(i), 4); \
	RKBC_RDX(GETARG_B(i)); \
	RKBC_RCX(GETARG_C(i)); \
	VM_CALL(vm_settabup); \
	/* vm_settabup can realloc base, reset it */ \
	LUA_UPDATE_BASE;

#define JIT_OP_MOVE \
	LUA_ADD_SAVEDPC(1); \
	RABC_RSI(GETARG_A(i)); \
	RABC_RDX(GETARG_B(i)); \
	/* mov (%rdx), %rax */ \
	APPEND3(0x48, 0x8b, 0x02); \
	/* mov %rax, (%rsi) */ \
	APPEND3(0x48, 0x89, 0x06); \
	/* mov 0x08(%rdx), %eax */ \
	APPEND3(0x8b, 0x42, 0x08); \
	/* mov %eax, 0x08(%rsi) */ \
	APPEND3(0x89, 0x46, 0x08);

#define JIT_OP_CALL \
	LUA_ADD_SAVEDPC(1); \
	JIT_UPDATEOFFSET(addrs[pc+1]); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	/* mov GETARG_B(i), %edx */ \
	APPEND1(0xba); \
	APPEND(GETARG_B(i), 4); \
	/* mov GETARG_C(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_C(i), 4); \
	/* mov %r13, %r8 */ \
	APPEND3(0x4d, 0x89, 0xe8); \
	VM_CALL(vm_call); \
	/* vm_call can realloc base, reset it */ \
	LUA_UPDATE_BASE;

#define JIT_OP_CLOSURE \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r15, %rsi */ \
	APPEND3(0x4c, 0x89, 0xfe); \
	RABC_RDX(GETARG_A(i)); \
	/* mov %r13, %rcx */ \
	APPEND3(0x4c, 0x89, 0xe9); \
	/* movl GETARG_Bx(i), %r8d */ \
	APPEND2(0x41, 0xb8); \
	APPEND(GETARG_Bx(i), 4); \
	VM_CALL(vm_closure); \
	/* vm_closure can realloc base, reset it */ \
	LUA_UPDATE_BASE;

#define JIT_OP_GETTABLE \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_B(i)); \
	RKBC_RDX(GETARG_C(i)); \
	RABC_RCX(GETARG_A(i)); \
	VM_CALL(luaV_gettable); \
	/* luaV_gettable can realloc base, reset it */ \
	LUA_UPDATE_BASE;

#define JIT_OP_LEN \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	RABC_RDX(GETARG_B(i)); \
	VM_CALL(luaV_objlen); \
	/* luaV_objlen can realloc base, reset it */ \
	LUA_UPDATE_BASE;

#define JIT_OP_JMP \
	LUA_ADD_SAVEDPC(1+GETARG_sBx(i)); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	/* mov GETARG_A(i), %edx */ \
	APPEND1(0xba); \
	APPEND(GETARG_A(i), 4); \
	VM_CALL(vm_jumpclose); \
	APPEND1(X86_LJ); \
	APPEND(addrs[pc+1+GETARG_sBx(i)] - addrs[pc+1], 4);

#define JIT_OP_LT \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RKBC_RSI(GETARG_B(i)); \
	RKBC_RDX(GETARG_C(i)); \
	VM_CALL(luaV_lessthan); \
	LUA_UPDATE_BASE; \
	/* cmp GETARG_A(i), %eax */ \
	APPEND1(0x3d); \
	APPEND(GETARG_A(i), 4); \
	APPEND2(X86_JE, 10); \
	LUA_ADD_SAVEDPC(1); \
	APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);

#define JIT_OP_LE \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RKBC_RSI(GETARG_B(i)); \
	RKBC_RDX(GETARG_C(i)); \
	VM_CALL(luaV_lessequal); \
	LUA_UPDATE_BASE; \
	/* cmp GETARG_A(i), %eax */ \
	APPEND1(0x3d); \
	APPEND(GETARG_A(i), 4); \
	APPEND2(X86_JE, 10); \
	LUA_ADD_SAVEDPC(1); \
	APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);

#define JIT_OP_EQ \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RKBC_RSI(GETARG_B(i)); \
	RKBC_RDX(GETARG_C(i)); \
	VM_CALL(vm_eq); \
	LUA_UPDATE_BASE; \
	/* cmp GETARG_A(i), %eax */ \
	APPEND1(0x3d); \
	APPEND(GETARG_A(i), 4); \
	APPEND2(X86_JE, 10); \
	LUA_ADD_SAVEDPC(1); \
	APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);

#define JIT_OP_NEWTABLE \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	RABC_RDX(GETARG_A(i)); \
	/* mov GETARG_B(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_B(i), 4); \
	/* mov GETARG_C(i), %r8d */ \
	APPEND2(0x41, 0xb8); \
	APPEND(GETARG_C(i), 4); \
	VM_CALL(vm_newtable); \
	LUA_UPDATE_BASE;

#define JIT_OP_SETUPVAL \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	/* mov GETARG_B(i), %edx */ \
	APPEND1(0xba); \
	APPEND(GETARG_B(i), 4); \
	VM_CALL(vm_setupval);

#define JIT_OP_GETUPVAL \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	/* mov GETARG_B(i), %edx */ \
	APPEND1(0xba); \
	APPEND(GETARG_B(i), 4); \
	VM_CALL(vm_getupval);

#define JIT_OP_FORPREP \
	LUA_ADD_SAVEDPC(1+GETARG_sBx(i)); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	VM_CALL(vm_forprep); \
	APPEND1(X86_LJ); \
	APPEND(addrs[pc+1+GETARG_sBx(i)] - addrs[pc+1], 4); \

#define JIT_OP_FORLOOP \
	LUA_ADD_SAVEDPC(1+GETARG_sBx(i)); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	VM_CALL(vm_forloop); \
	/* cmpl %$0x1, %eax */ \
	APPEND3(0x83, 0xf8, 0x01); \
	/* je +offset */ \
	APPEND2(0x0f, X86_JE+0x10); \
	APPEND(addrs[pc+1+GETARG_sBx(i)] - addrs[pc+1], 4);

#define JIT_OP_SETTABLE \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	RKBC_RDX(GETARG_B(i)); \
	RKBC_RCX(GETARG_C(i)); \
	VM_CALL(luaV_settable); \
	LUA_UPDATE_BASE;

#define JIT_OP_SELF \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	RKBC_RDX(GETARG_B(i)); \
	RKBC_RCX(GETARG_C(i)); \
	VM_CALL(vm_self); \
	LUA_UPDATE_BASE;

#define JIT_OP_SETLIST \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	RABC_RDX(GETARG_A(i)); \
	/* mov GETARG_B(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_B(i), 4); \
	/* mov GETARG_C(i), %r8d */ \
	APPEND2(0x41, 0xb8); \
	APPEND(GETARG_C(i), 4); \
	/* mov GETARG_Ax(i+1), %r9d */ \
	APPEND2(0x41, 0xb9); \
	APPEND(GETARG_Ax(c[pc+1]), 4); \
	VM_CALL(vm_setlist); \
	if (GET_OPCODE(c[pc+1]) == OP_EXTRAARG) { \
		/* jump over OP_EXTRAARG */ \
		LUA_ADD_SAVEDPC(2); \
		APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]); \
	} \
	else { \
		LUA_ADD_SAVEDPC(1); \
		APPEND2(0x90, 0x90); \
	}

#define JIT_OP_TFORCALL \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	RABC_RDX(GETARG_A(i)); \
	/* mov GETARG_C(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_C(i), 4); \
	VM_CALL(vm_tforcall); \
	LUA_UPDATE_BASE;

#define JIT_OP_TFORLOOP \
	LUA_ADD_SAVEDPC(1+GETARG_sBx(i)); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	VM_CALL(vm_tforloop); \
	/* cmpl %$0x1, %eax */ \
	APPEND3(0x83, 0xf8, 0x01); \
	APPEND2(0x0f, X86_JE+0x10); \
	APPEND(addrs[pc+1+GETARG_sBx(i)] - addrs[pc+1], 4);

#define JIT_OP_TEST \
	LUA_ADD_SAVEDPC(1); \
	RABC_RDI(GETARG_A(i)); \
	/* mov GETARG_C(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_C(i), 4); \
	VM_CALL(vm_test); \
	/* cmpl %$0x1, %eax */ \
	APPEND3(0x83, 0xf8, 0X01); \
	APPEND2(X86_JNE, 10); \
	LUA_ADD_SAVEDPC(1); \
	APPEND2(X86_NJ, addrs[pc+2] - addrs[pc+1]);

#define JIT_OP_CONCAT \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r15, %rsi */ \
	APPEND3(0x4c, 0x89, 0xfe); \
	/* mov GETARG_B(i), %edx */ \
	APPEND1(0xba); \
	APPEND(GETARG_B(i), 4); \
	/* mov GETARG_C(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_C(i), 4); \
	VM_CALL(vm_concat); \
	LUA_UPDATE_BASE; \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	RABC_RDX(GETARG_A(i)); \
	RABC_RCX(GETARG_B(i)); \
	VM_CALL(vm_setconcat); \
	LUA_UPDATE_BASE;

#define JIT_OP_VARARG \
	LUA_ADD_SAVEDPC(1); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	/* mov %r15, %rdx */ \
	APPEND3(0x4c, 0x89, 0xfa); \
	/* movl GETARG_A(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_A(i), 4); \
	/* movl GETARG_B(i), %r8d */ \
	APPEND2(0x41, 0xb8); \
	APPEND(GETARG_B(i), 4); \
	VM_CALL(vm_vararg); \
	LUA_UPDATE_BASE;

#define JIT_OP_EXTRAARG \
	APPEND1(0x90);

#define JIT_OP_TAILCALL \
	LUA_ADD_SAVEDPC(1); \
	JIT_UPDATEOFFSET(addrs[pc+1]); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	/*  mov %r15, %rdx */ \
	APPEND3(0x4c, 0x89, 0xfa); \
	/* movl GETARG_A(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_A(i), 4); \
	/* movl GETARG_B(i), %r8d */ \
	APPEND2(0x41, 0xb8); \
	APPEND(GETARG_B(i), 4); \
	VM_CALL(vm_tailcall); \
	LUA_UPDATE_BASE; \
	/* if C function we have to Jump to next return */ \
	/* cmp $0x1, %eax */ \
	APPEND3(0x83, 0xf8, 0x01); \
	/* jeq +offset8 */ \
	APPEND2(X86_JE, 26); \
	RESTORE_REGISTERS; \
	/* leaveq; retq */ \
	APPEND2(0xc9, 0xc3);

#define JIT_OP_TESTSET \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	RABC_RSI(GETARG_A(i)); \
	RABC_RDX(GETARG_B(i)); \
	/* movl GETARG_C(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_C(i), 4); \
	VM_CALL(vm_testset);

#endif
