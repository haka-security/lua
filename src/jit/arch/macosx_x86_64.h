#ifndef __macos_x86_64_h__
#define __macos_x86_64_h__

#include <stdlib.h>
#include <sys/mman.h>
#include <sys/user.h>
#include "../ljit.h"
#include "x86_64_generic.h"

static inline void jit_free(Proto *p)
{
	if (!p->jit) {
		return;
	}
	free(*((void **)p->jit - 1));
	p->jit = NULL;
}

static inline int jit_alloc(Proto *p)
{
	if (posix_memalign((void **)(&p->jit), PAGE_SIZE, p->sizejit) != 0) return 1;

	if (mprotect(p->jit, p->sizejit, PROT_EXEC|PROT_READ|PROT_WRITE) == -1) {
		jit_free(p);
		return 1;
	}
	return 0;
}

#endif
