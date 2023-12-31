// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
//
// This file is part of flrig.
//
// flrig is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// flrig is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <vector>
#include <queue>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

#include "support.h"
#include "debug.h"
#include "gettext.h"
#include "rig_io.h"
#include "dialogs.h"
#include "rigbase.h"
#include "ptt.h"

#include "rigs.h"
#include "KX3_ui.h"

extern std::queue<XCVR_STATE> queA;
extern std::queue<XCVR_STATE> queB;

void read_KX3_vfo()
{
	unsigned long long freq;
	freq = selrig->get_vfoA();
	if (freq != vfoA.freq) {
		vfoA.freq = freq;
		Fl::awake(setFreqDispA);
		vfo = &vfoA;
	}
	freq = selrig->get_vfoB();
	if (freq != vfoB.freq) {
		vfoB.freq = freq;
		Fl::awake(setFreqDispB);
	}
}

void read_KX3_mode()
{
	int nu_mode;
	nu_mode = selrig->get_modeA();
	if (nu_mode != vfoA.imode) {
		vfoA.imode = vfo->imode = nu_mode;
		selrig->set_bwA(vfo->iBW = selrig->adjust_bandwidth(nu_mode));
		Fl::awake(setModeControl);
		Fl::awake(updateBandwidthControl);
	}
	nu_mode = selrig->get_modeB();
	if (nu_mode != vfoB.imode) {
		vfoB.imode = nu_mode;
	}
}

void read_KX3_bw()
{
	int nu_BW;
	nu_BW = selrig->get_bwA();
	if (nu_BW != vfoA.iBW) {
		vfoA.iBW = vfo->iBW = nu_BW;
		Fl::awake(setBWControl);
	}
	nu_BW = selrig->get_bwB();
	if (nu_BW != vfoB.iBW) {
		vfoB.iBW = nu_BW;
	}
}


void KX3_set_split(int val)
{
	guard_lock serial_lock(&mutex_serial);
	selrig->set_split(val);
}

void cb_KX3_A2B()
{
	guard_lock serial_lock(&mutex_serial);
	vfoB = vfoA;
	selrig->set_vfoB(vfoB.freq);
	selrig->set_bwB(vfoB.iBW);
	selrig->set_modeB(vfoB.imode);
	FreqDispB->value(vfoB.freq);
}

void cb_KX3_swapAB()
{
	guard_lock serial_lock(&mutex_serial);

	XCVR_STATE nuB = vfoA, nuA = vfoB;

	selrig->set_vfoB(nuB.freq);
	selrig->set_bwB(nuB.iBW);
	selrig->set_modeB(nuB.imode);

	selrig->set_vfoA(nuA.freq);
	selrig->set_bwA(nuA.iBW);
	selrig->set_modeA(nuA.imode);

	vfoA = nuA;
	vfoB = nuB;
	vfo = &vfoA;

	FreqDispA->value(vfoA.freq);
	opBW->index(vfoA.iBW);
	opMODE->index(vfoA.imode);

	FreqDispB->value(vfoB.freq);

}

void cb_KX3_IFsh()
{
	guard_lock serial_lock(&mutex_serial);
	selrig->set_if_shift(9999);
}
