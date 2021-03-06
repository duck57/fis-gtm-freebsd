/****************************************************************
 *								*
 *	Copyright 2001, 2007 Fidelity Information Services, Inc	*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"
#include "gdsroot.h"
#include "gdsbt.h"
#include "gdsblk.h"
#include "gdsbml.h"

/* Include prototypes */
#include "bit_set.h"
#include "bit_clear.h"

GBLREF	boolean_t		dse_running;

uint4 bml_free(uint4 setfree, sm_uc_ptr_t map)
{
	uint4	ret, ret1;

	setfree *= BML_BITS_PER_BLK;
	ret = bit_set(setfree, map);
	ret1 = bit_clear(setfree + 1, map);
	/* Assert that only a BUSY or RECYCLED block gets marked as FREE (dse is an exception) */
	assert((!ret && !ret1) || (ret && ret1) || dse_running);
	return ret;
}
