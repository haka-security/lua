#ifndef __macos_x86_64_h__
#define __macos_x86_64_h__

#include <stdlib.h>
#include <machine/param.h>
#include <sys/mman.h>
#include "../ljit.h"

#define X86_JB  0x72
#define X86_JAE 0x73
#define X86_JE  0x74
#define X86_JNE 0x75
#define X86_JBE 0x76
#define X86_JA  0x77


#define JIT_REG_RDI 0
#define JIT_REG_RSI 1
#define JIT_REG_RDX 2
#define JIT_REG_RCX 3
#define JIT_REG_R8  4
#define JIT_REG_R9  5

/*
 * Conventions :
 *  RBP : frame pointer
 * -8(RBP): Lua state
 * -32(RBP) : saved R14
 * -40(RBP): save R15
 * -48(RBP): &savedpc
 * -56(RBP): &jitoffset
 * -64(RBP): cl->p->code
 * -72(RBP): saved RAX
 * -80(RBP): saved RBX
 * -88(RBP): saved R13
 */


#define JIT_RKBC(arg) \
	if (ISK(arg)) { \
		/* k+INDEXK(arg)*sizeof(Tvalue) */\
		/* mov %r14, %rax */ \
		APPEND3(0x4c, 0x89, 0xf0);\
		/* add INDEXK(arg)*sizeof(TValue), %rax */\
		APPEND2(0x48, 0x05);\
		APPEND(INDEXK(arg)*sizeof(TValue), 4);\
	}\
	else { \
		/* base+arg */ \
		/* mov %r15, %rax */\
		APPEND3(0x4c, 0x89, 0xf8);\
		/* add (arg)*sizeof(TValue), %rax */ \
		APPEND2(0x48, 0x05);\
		APPEND((arg)*sizeof(TValue), 4); \
	}

#define JIT_RABC(arg) { \
	/* mov %r15, %rax */\
	APPEND3(0x4c, 0x89, 0xf8);\
    /* add arg*sizeof(TValue), %rax*/\
    APPEND2(0x48, 0x05);\
    APPEND((arg)*sizeof(TValue), 4);\
}

#define JIT_RESETBASE \
	/* mov offset8(%r13),%r15 */ \
	APPEND4(0x4d, 0x8b, 0x7d, offsetof(CallInfo, u.l.base));

#define JIT_UPDATEPC(off) \
	/* mov -48(%rbp), %rax */ \
	APPEND4(0x48, 0x8b, 0x45, 0xd0); \
	/* mov -64(%rbp), %r10 */ \
	APPEND4(0x4c, 0x8b, 0x55, 0xc0); \
	/* mov %r10, (%rax) */ \
	APPEND3(0x4c, 0x89, 0x10); \
	/* addq sizeof(Instrcution), (%rax) */ \
	APPEND3(0x48, 0x81, 0x00); \
	APPEND((off)*sizeof(Instruction), 4);

#define JIT_UPDATEOFFSET(off) \
	/* mov -56(%rbp), %rax*/ \
	APPEND4(0x48, 0x8b, 0x45, 0xc8); \
	/* movl off, (%rax) */ \
	APPEND2(0xc7, 0x00); \
	APPEND(off, 4);

#define JIT_SAVE_REGISTERS \
	/* mov %rax, -72(%rbp) */ \
	APPEND4(0x48, 0x89, 0x45, 0xb8); \
	/* mov %rbx, -80(%rbp) */ \
	APPEND4(0x48, 0x89, 0x5d, 0xb0); \
	/* mov %r13, -88(%rbp) */ \
	APPEND4(0x4c, 0x89, 0x6d, 0xa8); \
	/* mov %r14, -32(%rbp) */ \
	APPEND4(0x4c, 0x89, 0x75, 0xe0); \
	/* mov %r15, -40(%rbp) */ \
	APPEND4(0x4c, 0x89, 0x7d, 0xd8);

#define JIT_RESTORE_REGISTERS \
	/* restore accumulators */ \
	/* mov -72(%rbp), %rax */ \
	APPEND4(0x48, 0x8b, 0x45, 0xb8); \
	/* mov -80(%rbp), %rbx */ \
	APPEND4(0x48, 0x8b, 0x5d, 0xb0); \
	/* mov -88(%rbp), %r13 */ \
	APPEND4(0x4c, 0x8b, 0x6d, 0xa8); \
	/* mov -32(%rbp), %r14 */ \
	APPEND4(0x4c, 0x8b, 0x75, 0xe0); \
	/* mov -40(%rbp), %r15 */ \
	APPEND4(0x4c, 0x8b, 0x7d, 0xd8);


static int jit_opcodes[NUM_OPCODES] =
{
	39, /* OP_MOVE */
	33, /* OP_LOADK */
	38, /* OP_LOADKX */
	34, /* OP_LOADBOOL */
	29, /* OP_LOADNIL */
	32, /* OP_GETUPVAL */
	48, /* OP_GETTABUP */
	55, /* OP_GETTABLE */
	48, /* OP_SETTABUP */
	32, /* OP_SETUPVAL */
	55, /* OP_SETTABLE */
	41, /* OP_NEWTABLE */
	55, /* OP_SELF */
	55, /* OP_ADD */
	55,	/* OP_SUB */
	55, /* OP_MUL */
	55, /* OP_DIV */
	55, /* OP_MOD */
	55, /* OP_POW */
	43, /* OP_UNM */
	36, /* OP_NOT */
	43, /* OP_LEN */
	78, /* OP_CONCAT */
	28, /* OP_JMP */
	54, /* OP_EQ */
	54, /* OP_LT */
	54, /* OP_LE */
	38, /* OP_TEST */
	44, /* OP_TESTSET */
	72, /* OP_CALL */
	91, /* OP_TAILCALL */
	89, /* OP_RETURN */
	36, /* OP_FORLOOP */
	32, /* OP_FORPREP */
	39, /* OP_TFORCALL */
	36, /* OP_TFORLOOP */
	52, /* OP_SETLIST */
	43, /* OP_CLOSURE */
	36, /* OP_VARARG */
	1, /* OP_EXTRAARG */
};

#define PROLOGUE_LEN 86
#define JIT_PROLOGUE \
	APPEND4(0x55, 0x48, 0x89, 0xe5); /* push %rbp; mov %rsp,%rbp */ \
	APPEND4(0x48, 0x83, 0xec, 96);    /* subq  $96,%rsp       */ \
	/* First, save registers */ \
	JIT_SAVE_REGISTERS; \
	/* put L in stack */ \
	APPEND3(0x48, 0x89, 0xfb); /* mov %rdi, %rbx */ \
	/* put ci in r13 */ \
	APPEND3(0x49, 0x89, 0xf5); \
	/* put k in r14 */ \
	APPEND3(0x49, 0x89, 0xce); /* mov %rcx, %r14 */ \
	/* put base in stack */ \
	JIT_RESETBASE; \
	/* put &savedpc in stack */ \
	/* lea offset8(%r13),%rax */ \
	APPEND4(0x49, 0x8d, 0x45, offsetof(CallInfo, u.l.savedpc)); \
	/* mov %rax,-48(%rbp) */ \
	APPEND4(0x48, 0x89, 0x45, 0xd0); \
	/* mov offset8(%rdx), %r10 */ \
	APPEND4(0x4c, 0x8b, 0x52, offsetof(LClosure, p)); \
	/* mov offset8(%r10), %r10 */ \
	APPEND4(0x4d, 0x8b, 0x52, offsetof(Proto, code)); \
	/* mov %r10, -64(%rbp) */  \
	APPEND4(0x4c, 0x89, 0x55, 0xc0); \
	/* put &jitoffset in stack */ \
	/* lea offset8(%r13),%rax */ \
	APPEND4(0x49, 0x8d, 0x45, offsetof(CallInfo, u.l.jitoffset)); \
	/* mov %rax,-56(%rbp) */ \
	APPEND4(0x48, 0x89, 0x45, 0xc8); \
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
	JIT_UPDATEPC(pc+1); \
	JIT_UPDATEOFFSET(addrs[pc+1]); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r15, %rsi */ \
	APPEND3(0x4c, 0x89, 0xfe); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov %r13, %rcx */ \
	APPEND3(0x4c, 0x89, 0xe9); \
	/* mov GETARG_B(i), %r8d */ \
	APPEND2(0x41, 0xb8); \
	APPEND(GETARG_B(i), 4); \
	/* mov vm_return, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_return, 8); \
	/* call vm_return*/ \
	APPEND2(0xff, 0xd0); \
	JIT_RESTORE_REGISTERS; \
	/* leaveq; retq */ \
	APPEND2(0xc9, 0xc3); \

#define JIT_OP_LOADK \
	/* Get RA in rax */ \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
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
	/* Get RA */ \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
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
	APPEND1(0xe9); \
	APPEND(addrs[pc+2] - addrs[pc+1], 4);

#define JIT_OP_LOADBOOL \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rdi */ \
	APPEND3(0x48, 0x89, 0xc7); \
	/* mov GETARG_B(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_B(i), 4); \
	/* mov vm_setbool, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_setbool, 8); \
	/* call vm_setbool */ \
	APPEND2(0xff, 0xd0); \
	if (GETARG_C(i)) { \
		APPEND1(0xe9); \
		APPEND(addrs[pc+2] - addrs[pc+1], 4); \
	} \
	else { \
		APPEND1(0x90); \
		APPEND4(0x90, 0x90, 0x90, 0x90); \
	}

#define JIT_OP_LOADNIL \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rdi */ \
	APPEND3(0x48, 0x89, 0xc7); \
	/* mov GETARG_B(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_B(i), 4); \
	/* mov vm_setnil, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_setnil, 8); \
	/* call vm_setnil */ \
	APPEND2(0xff, 0xd0);

#define JIT_OP_NOT \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rdi */ \
	APPEND3(0x48, 0x89, 0xc7); \
	JIT_RABC(GETARG_B(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	/* mov vm_not, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_not, 8); \
	/* call vm_not */ \
	APPEND2(0xff, 0xd0);

#define JIT_OP_ADD \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	/* Get RKB */ \
	JIT_RKBC(GETARG_B(i)); /* size 10 */ \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* Get RKC */ \
	JIT_RKBC(GETARG_C(i)); /* size 10 */ \
	/* mov %rax, %rcx */ \
	APPEND3(0x48, 0x89, 0xc1); \
	/* mov vm_add, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_add, 8); \
	/* call vm_add */ \
	APPEND2(0xff, 0xd0); \
	/* jit_XXX can realloc base, reset it */ \
	JIT_RESETBASE;

#define JIT_OP_SUB \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	/* Get RKB */ \
	JIT_RKBC(GETARG_B(i)); /* size 10 */ \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* Get RKC */ \
	JIT_RKBC(GETARG_C(i)); /* size 10 */ \
	/* mov %rax, %rcx */ \
	APPEND3(0x48, 0x89, 0xc1); \
	/* mov vm_sub, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_sub, 8); \
	/* call vm_sub */ \
	APPEND2(0xff, 0xd0); \
	/* jit_XXX can realloc base, reset it */ \
	JIT_RESETBASE;

#define JIT_OP_MUL \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
    JIT_RABC(GETARG_A(i)); /* size 10 */ \
    /* mov %rax, %rsi */ \
    APPEND3(0x48, 0x89, 0xc6); \
    /* Get RKB */ \
    JIT_RKBC(GETARG_B(i)); /* size 10 */ \
    /* mov %rax, %rdx */ \
    APPEND3(0x48, 0x89, 0xc2); \
    /* Get RKC */ \
    JIT_RKBC(GETARG_C(i)); /* size 10 */ \
    /* mov %rax, %rcx */ \
    APPEND3(0x48, 0x89, 0xc1); \
	/* mov vm_mul, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_mul, 8); \
	/* call vm_mul */ \
	APPEND2(0xff, 0xd0); \
    /* jit_XXX can realloc base, reset it */ \
    JIT_RESETBASE;

#define JIT_OP_DIV \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
    JIT_RABC(GETARG_A(i)); /* size 10 */ \
    /* mov %rax, %rsi */ \
    APPEND3(0x48, 0x89, 0xc6); \
    /* Get RKB */ \
    JIT_RKBC(GETARG_B(i)); /* size 10 */ \
    /* mov %rax, %rdx */ \
    APPEND3(0x48, 0x89, 0xc2); \
    /* Get RKC */ \
    JIT_RKBC(GETARG_C(i)); /* size 10 */ \
    /* mov %rax, %rcx */ \
    APPEND3(0x48, 0x89, 0xc1); \
	/* mov vm_div, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_div, 8); \
	/* call vm_div */ \
	APPEND2(0xff, 0xd0); \
    /* jit_XXX can realloc base, reset it */ \
    JIT_RESETBASE;

#define JIT_OP_MOD \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
    JIT_RABC(GETARG_A(i)); /* size 10 */ \
    /* mov %rax, %rsi */ \
    APPEND3(0x48, 0x89, 0xc6); \
    /* Get RKB */ \
    JIT_RKBC(GETARG_B(i)); /* size 10 */ \
    /* mov %rax, %rdx */ \
    APPEND3(0x48, 0x89, 0xc2); \
    /* Get RKC */ \
    JIT_RKBC(GETARG_C(i)); /* size 10 */ \
    /* mov %rax, %rcx */ \
    APPEND3(0x48, 0x89, 0xc1); \
	/* mov vm_mod, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_mod, 8); \
	/* call vm_mod */ \
	APPEND2(0xff, 0xd0); \
    /* jit_XXX can realloc base, reset it */ \
    JIT_RESETBASE;

#define JIT_OP_POW \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
    JIT_RABC(GETARG_A(i)); /* size 10 */ \
    /* mov %rax, %rsi */ \
    APPEND3(0x48, 0x89, 0xc6); \
    /* Get RKB */ \
    JIT_RKBC(GETARG_B(i)); /* size 10 */ \
    /* mov %rax, %rdx */ \
    APPEND3(0x48, 0x89, 0xc2); \
    /* Get RKC */ \
    JIT_RKBC(GETARG_C(i)); /* size 10 */ \
    /* mov %rax, %rcx */ \
    APPEND3(0x48, 0x89, 0xc1); \
	/* mov vm_pow, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_pow, 8); \
	/* call vm_pow */ \
	APPEND2(0xff, 0xd0); \
    /* jit_XXX can realloc base, reset it */ \
    JIT_RESETBASE;

#define JIT_OP_UNM \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); /* size 9 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	/* Get RKB */ \
	JIT_RKBC(GETARG_B(i)); /* size 9 */ \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov vm_unm, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_unm, 8); \
	/* call vm_unm */ \
	APPEND2(0xff, 0xd0); \
	/* jit_XXX can realloc base, reset it */ \
	JIT_RESETBASE;

#define JIT_OP_GETTABUP \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov GETARG_B(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_B(i), 4); \
	JIT_RKBC(GETARG_C(i)); \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rcx */ \
	APPEND3(0x48,0x89, 0xc1); \
	/* mov vm_gettabup, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_gettabup, 8); \
	/* call vm_gettabup */ \
	APPEND2(0xff, 0xd0); \
	/* jit_gettabup can realloc base, reset it */ \
	JIT_RESETBASE;

#define JIT_OP_SETTABUP \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov GETARG_A(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_A(i), 4); \
	JIT_RKBC(GETARG_B(i)); \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	JIT_RKBC(GETARG_C(i)); \
	/* mov %rax, %rcx */ \
	APPEND3(0x48, 0x89, 0xc1); \
	/* mov vm_settabup, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_settabup, 8); \
	/* call vm_settabup */ \
	APPEND2(0xff, 0xd0); \
	/* jit_settabup can realloc base, reset it */ \
	JIT_RESETBASE;

#define JIT_OP_MOVE \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	JIT_RABC(GETARG_B(i)); /* size 10 */ \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov setobj, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_setobj, 8); \
	/* call setobj */ \
	APPEND2(0xff, 0xd0);

#define JIT_OP_CALL \
	JIT_UPDATEPC(pc+1); \
	JIT_UPDATEOFFSET(addrs[pc+1]); \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	/* mov GETARG_B(i), %edx */ \
	APPEND1(0xba); \
	APPEND(GETARG_B(i), 4); \
	/* mov GETARG_C(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_C(i), 4); \
	/* mov %r13, %r8 */ \
	APPEND3(0x4d, 0x89, 0xe8); \
	/* mov vm_call, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_call, 8); \
	/* call vm_call */ \
	APPEND2(0xff, 0xd0); \
	/* jit_call can realloc base, reset it */ \
	JIT_RESETBASE;

#define JIT_OP_CLOSURE \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r15, %rsi */ \
	APPEND3(0x4c, 0x89, 0xfe); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov %r13, %rcx */ \
	APPEND3(0x4c, 0x89, 0xe9); \
	/* movl GETARG_Bx(i), %r8d */ \
	APPEND2(0x41, 0xb8); \
	APPEND(GETARG_Bx(i), 4); \
	/* mov vm_closure, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_closure, 8); \
	/* call vm_closure */ \
	APPEND2(0xff, 0xd0); \
	/* jit_call can realloc base, reset it */ \
	JIT_RESETBASE;

#define JIT_OP_GETTABLE \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_B(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	JIT_RKBC(GETARG_C(i)); /* size 10 */ \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rcx */ \
	APPEND3(0x48, 0x89, 0xc1); \
	/* mov luaV_gettable, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&luaV_gettable, 8); \
	/* call luaV_gettable */ \
	APPEND2(0xff, 0xd0); \
	/* jit_call can realloc base, reset it */ \
	JIT_RESETBASE;

#define JIT_OP_LEN \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	JIT_RABC(GETARG_B(i)); /* size 10 */ \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov luaV_objlen, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&luaV_objlen, 8); \
	/* call luaV_objlen */ \
	APPEND2(0xff, 0xd0); \
	/* jit_call can realloc base, reset it */ \
	JIT_RESETBASE;

#define JIT_OP_JMP \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	/* mov GETARG_A(i), %edx */ \
	APPEND1(0xba); \
	APPEND(GETARG_A(i), 4); \
	/* mov vm_jumpclose, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_jumpclose, 8); \
	/* call vm_jumpclose */ \
	APPEND2(0xff, 0xd0); \
	APPEND1(0xe9); \
	APPEND(addrs[pc+1+GETARG_sBx(i)] - addrs[pc+1], 4);

#define JIT_OP_LT \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RKBC(GETARG_B(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	JIT_RKBC(GETARG_C(i)); /* size 10 */ \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov luaV_lessthan, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&luaV_lessthan, 8); \
	/* call luaV_lessthan */ \
	APPEND2(0xff, 0xd0); \
	JIT_RESETBASE; /* size 15 */ \
	/* cmp GETARG_A(i), %eax */ \
	APPEND1(0x3d); \
	APPEND(GETARG_A(i), 4); \
	APPEND2(0x0f, X86_JNE+0x10); \
	APPEND(addrs[pc+2] - addrs[pc+1], 4);

#define JIT_OP_LE \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RKBC(GETARG_B(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	JIT_RKBC(GETARG_C(i)); /* size 10 */ \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov luaV_lessequal, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&luaV_lessequal, 8); \
	/* call luaV_lessequal */ \
	APPEND2(0xff, 0xd0); \
	JIT_RESETBASE; /* size 15 */ \
	/* cmp GETARG_A(i), %eax */ \
	APPEND1(0x3d); \
	APPEND(GETARG_A(i), 4); \
	APPEND2(0x0f, X86_JNE+0x10); \
	APPEND(addrs[pc+2] - addrs[pc+1], 4); \

#define JIT_OP_EQ \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RKBC(GETARG_B(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	JIT_RKBC(GETARG_C(i)); /* size 10 */ \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov vm_eq, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_eq, 8); \
	/* call vm_eq */ \
	APPEND2(0xff, 0xd0); \
	JIT_RESETBASE; /* size 15 */ \
	/* cmp GETARG_A(i), %eax */ \
	APPEND1(0x3d); \
	APPEND(GETARG_A(i), 4); \
	APPEND2(0x0f, X86_JNE+0x10); \
	APPEND(addrs[pc+2] - addrs[pc+1], 4);

#define JIT_OP_NEWTABLE \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov GETARG_B(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_B(i), 4); \
	/* mov GETARG_C(i), %r8d */ \
	APPEND2(0x41, 0xb8); \
	APPEND(GETARG_C(i), 4); \
	/* mov vm_newtable, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_newtable, 8); \
	/* call vm_newtable */ \
	APPEND2(0xff, 0xd0);

#define JIT_OP_SETUPVAL \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	/* mov GETARG_B(i), %edx */ \
	APPEND1(0xba); \
	APPEND(GETARG_B(i), 4); \
	/* mov vm_setupval, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_setupval, 8); \
	/* call vm_setupval */ \
	APPEND2(0xff, 0xd0);

#define JIT_OP_GETUPVAL \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); /* size 10 */ \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	/* mov GETARG_B(i), %edx */ \
	APPEND1(0xba); \
	APPEND(GETARG_B(i), 4); \
	/* mov vm_getupval, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_getupval, 8); \
	/* call vm_getupval */ \
	APPEND2(0xff, 0xd0);

#define JIT_OP_FORPREP \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	/* mov vm_forprep, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_forprep, 8); \
	/* call vm_forprep */ \
	APPEND2(0xff, 0xd0); \
	APPEND1(0xe9); \
	APPEND(addrs[pc+1+GETARG_sBx(i)] - addrs[pc+1], 4); \

#define JIT_OP_FORLOOP \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	/* mov vm_forloop, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_forloop, 8); \
	/* call vm_forloop */ \
	APPEND2(0xff, 0xd0); \
	/* cmpl %$0x1, %eax */ \
	APPEND3(0x83, 0xf8, 0x01); \
	/* je +offset */ \
	APPEND2(0x0f, X86_JE+0x10); \
	APPEND(addrs[pc+1+GETARG_sBx(i)] - addrs[pc+1], 4);

#define JIT_OP_SETTABLE \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	JIT_RKBC(GETARG_B(i)); \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	JIT_RKBC(GETARG_C(i)); \
	/* mov %rax, %rcx */ \
	APPEND3(0x48, 0x89, 0xc1); \
	/* mov luaV_settable, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&luaV_settable, 8); \
	/* call luaV_settable */ \
	APPEND2(0xff, 0xd0); \
	JIT_RESETBASE; /* size 15 */

#define JIT_OP_SELF \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	JIT_RKBC(GETARG_B(i)); \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	JIT_RKBC(GETARG_C(i)); \
	/* mov %rax, %rcx */ \
	APPEND3(0x48, 0x89, 0xc1); \
	/* mov vm_self, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_self, 8); \
	/* call vm_self */ \
	APPEND2(0xff, 0xd0); \
	JIT_RESETBASE; /* size 15 */

#define JIT_OP_SETLIST \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov GETARG_B(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_B(i), 4); \
	/* mov GETARG_C(i), %r8d */ \
	APPEND2(0x41, 0xb8); \
	APPEND(GETARG_C(i), 4); \
	/* mov GETARG_Ax(i+1), %r9d */ \
	APPEND2(0x41, 0xb9); \
	APPEND(GETARG_Ax(c[pc+1]), 4); \
	/* mov vm_setlist, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_setlist, 8); \
	/* call vm_setlist */ \
	APPEND2(0xff, 0xd0); \
	if (GET_OPCODE(c[pc+1]) == OP_EXTRAARG) { \
		/* jump over OP_EXTRAARG */ \
		APPEND1(0xe9); \
		APPEND(addrs[pc+2] - addrs[pc+1], 4); \
	} \
	else { \
		APPEND4(0x90, 0x90, 0x90, 0x90); \
		APPEND1(0x90); \
	}

#define JIT_OP_TFORCALL \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* mov GETARG_C(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_C(i), 4); \
	/* mov vm_tforcall, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_tforcall, 8); \
	/* call vm_tforcall */ \
	APPEND2(0xff, 0xd0); \
	JIT_RESETBASE; /* size 15 */

#define JIT_OP_TFORLOOP \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	/* mov vm_tforloop, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_tforloop, 8); \
	/* call vm_tforloop */ \
	APPEND2(0xff, 0xd0); \
	/* cmpl %$0x1, %eax */ \
	APPEND3(0x83, 0xf8, 0x01); \
	APPEND2(0x0f, X86_JE+0x10); \
	APPEND(addrs[pc+1+GETARG_sBx(i)] - addrs[pc+1], 4);

#define JIT_OP_TEST \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rdi */ \
	APPEND3(0x48, 0x89, 0xc7); \
	/* mov GETARG_C(i), %esi */ \
	APPEND1(0xbe); \
	APPEND(GETARG_C(i), 4); \
	/* mov vm_test, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_test, 8); \
	/* call vm_test */ \
	APPEND2(0xff, 0xd0); \
	/* cmpl %$0x1, %eax */ \
	APPEND3(0x83, 0xf8, 0X01); \
	APPEND2(0x0f, X86_JE+0x10); \
	APPEND(addrs[pc+2] - addrs[pc+1], 4);

#define JIT_OP_CONCAT \
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
	/* mov vm_concat, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_concat, 8); \
	/* call vm_concat */ \
	APPEND2(0xff, 0xd0); \
	JIT_RESETBASE; \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	/* mov %r13, %rsi */ \
	APPEND3(0x4c, 0x89, 0xee); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	JIT_RABC(GETARG_B(i)); \
	/* mov %rax, %rcx */ \
	APPEND3(0x48, 0x89, 0xc1); \
	/* mov vm_setconcat, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_setconcat, 8); \
	/* call vm_setconcat */ \
	APPEND2(0xff, 0xd0); \
	JIT_RESETBASE;

#define JIT_OP_VARARG \
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
	/* mov vm_vararg, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_vararg, 8); \
	/* call vm_vararg */ \
	APPEND2(0xff, 0xd0); \
	JIT_RESETBASE;

#define JIT_OP_EXTRAARG \
	APPEND1(0x90);

#define JIT_OP_TAILCALL \
	JIT_UPDATEPC(pc+1); \
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
	/* mov vm_tailcall, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_tailcall, 8); \
	/* call vm_tailcall */ \
	APPEND2(0xff, 0xd0); \
	JIT_RESETBASE; \
	/* if C function we have to Jump to next return */ \
	/* cmp $0x1, %eax */ \
	APPEND3(0x83, 0xf8, 0x01); \
	/* jeq +offset8 */ \
	APPEND2(X86_JE, 22); \
	JIT_RESTORE_REGISTERS; \
	/* leaveq; retq */ \
	APPEND2(0xc9, 0xc3);

#define JIT_OP_TESTSET \
	/* mov %rbx, %rdi */ \
	APPEND3(0x48, 0x89, 0xdf); \
	JIT_RABC(GETARG_A(i)); \
	/* mov %rax, %rsi */ \
	APPEND3(0x48, 0x89, 0xc6); \
	JIT_RABC(GETARG_B(i)); \
	/* mov %rax, %rdx */ \
	APPEND3(0x48, 0x89, 0xc2); \
	/* movl GETARG_C(i), %ecx */ \
	APPEND1(0xb9); \
	APPEND(GETARG_C(i), 4); \
	/* mov vm_testset, %rax */ \
	APPEND2(0x48, 0xb8); \
	APPEND((uint64_t)&vm_testset, 8); \
	/* call vm_testset */ \
	APPEND2(0xff, 0xd0); \

static inline void jit_free(Proto *p)
{
	if (!p->jit) {
		return;
	}
	free(p->jit);
	p->jit = NULL;
}


static inline int jit_alloc(Proto *p)
{
	if (posix_memalign(&p->jit, PAGE_SIZE, p->sizejit) != 0) return 1;

	if (mprotect(p->jit, p->sizejit, PROT_EXEC|PROT_READ|PROT_WRITE) == -1) {
		jit_free(p);
		return 1;
	}
	return 0;
}

#endif
