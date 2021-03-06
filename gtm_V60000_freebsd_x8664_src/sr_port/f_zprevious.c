/****************************************************************
 *								*
 *	Copyright 2001, 2012 Fidelity Information Services, Inc	*
 *								*
 *	This source code contains the intellectual property	*
 *	of its copyright holder(s), and is made available	*
 *	under a license.  If you do not know the terms of	*
 *	the license, please stop and do not read further.	*
 *								*
 ****************************************************************/

#include "mdef.h"
#include "compiler.h"
#include "opcode.h"
#include "indir_enum.h"
#include "toktyp.h"
#include "mdq.h"
#include "advancewindow.h"
#include "fullbool.h"

error_def(ERR_VAREXPECTED);

int f_zprevious(oprtype *a, opctype op)
{
	triple *oldchain, *r, tmpchain, *triptr;
	DCL_THREADGBL_ACCESS;

	SETUP_THREADGBL_ACCESS;
	r = maketriple(op);
	switch (TREF(window_token))
	{
	case TK_IDENT:
		if (TK_LPAREN != TREF(director_token))
		{
			r->opcode = OC_FNLVPRVNAME;
			r->operand[0] = put_str((TREF(window_ident)).addr, (TREF(window_ident)).len);
			ins_triple(r);
			advancewindow();
			break;
		}
		if (!lvn(&(r->operand[0]), OC_SRCHINDX, r))
			return FALSE;
		ins_triple(r);
		break;
	case TK_CIRCUMFLEX:
		if (!gvn())
			return FALSE;
		r->opcode = OC_ZPREVIOUS;
		ins_triple(r);
		break;
	case TK_ATSIGN:
		TREF(saw_side_effect) = TREF(shift_side_effects);
		if (TREF(shift_side_effects) && (GTM_BOOL == TREF(gtm_fullbool)))
		{
			dqinit(&tmpchain, exorder);
			oldchain = setcurtchain(&tmpchain);
			if (!indirection(&(r->operand[0])))
			{
				setcurtchain(oldchain);
				return FALSE;
			}
			r->operand[1] = put_ilit((mint)indir_fnzprevious);
			ins_triple(r);
			newtriple(OC_GVSAVTARG);
			setcurtchain(oldchain);
			dqadd(TREF(expr_start), &tmpchain, exorder);
			TREF(expr_start) = tmpchain.exorder.bl;
			triptr = newtriple(OC_GVRECTARG);
			triptr->operand[0] = put_tref(TREF(expr_start));
		} else
		{
			if (!indirection(&(r->operand[0])))
				return FALSE;
			r->operand[1] = put_ilit((mint)indir_fnzprevious);
			ins_triple(r);
		}
		r->opcode = OC_INDFUN;
		break;
	default:
		stx_error(ERR_VAREXPECTED);
		return FALSE;
	}
	*a = put_tref(r);
	return TRUE;
}
