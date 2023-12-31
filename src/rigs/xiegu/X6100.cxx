// ----------------------------------------------------------------------------
// Copyright (C) 2014
//              David Freese, W1HKJ
// Modified: January 2017
//              Andy Stewart, KB1OIQ
// Updated: June 2018
//              Cliff Scott, AE5ZA
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

#include <string>
#include <sstream>
#include <iostream>

#include "xiegu/X6100.h"
#include "status.h"
#include "trace.h"
#include "tod_clock.h"

#include "support.h"

//=============================================================================
// X6100

struct catpair {int cat; float val;};

static unsigned char bcd[] = {
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99
};

const char X6100name_[] = "X6100";

// these are only defined in this file
// undef'd at end of file
#define NUM_FILTERS 3
#define NUM_MODES  13

static int mode_filterA[NUM_MODES] = {1,1,1,1,1,1,1,1,1,1,1,1};
static int mode_filterB[NUM_MODES] = {1,1,1,1,1,1,1,1,1,1,1,1};

static int mode_bwA[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static int mode_bwB[NUM_MODES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

static const char *szfilter[NUM_FILTERS] = {"1", "2", "3"};

enum {
LSB705, USB705, AM705, FM705,
CW705, CWR705, RTTY705, RTTYR705,
LSBD705, USBD705, AMD705, FMD705, DV705
};

static std::vector<std::string>X6100modes_;
static const char *vX6100modes_[] =
{	"LSB", "USB", "AM", "FM",
	"CW", "CW-R", "RTTY", "RTTY-R",
	"LSB-D", "USB-D", "AM-D", "FM-D", "DV"};

char X6100_mode_type[] = {
	'L', 'U', 'U', 'U',
	'L', 'U', 'L', 'U',
	'L', 'U', 'U', 'U', 'U' };

const char X6100_mode_nbr[] = {
	0x00, 0x01, 0x02, 0x05,
	0x03, 0x07, 0x04, 0x08,
	0x00, 0x01, 0x02, 0x05, 0x17 };

static std::vector<std::string>X6100_ssb_bws;
static const char *vX6100_ssb_bws[] =
{
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700", "2800", "2900", "3000", "3100", "3200", "3300", "3400", "3500",
"3600" };
static int X6100_bw_vals_SSB[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40, WVALS_LIMIT};

static std::vector<std::string>X6100_rtty_bws;
static const char *vX6100_rtty_bws[] =
{
"50",    "100",  "150",  "200",  "250",  "300",  "350",  "400",  "450",  "500",
"600",   "700",  "800",  "900", "1000", "1100", "1200", "1300", "1400", "1500",
"1600", "1700", "1800", "1900", "2000", "2100", "2200", "2300", "2400", "2500",
"2600", "2700" };
static int X6100_bw_vals_RTTY[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31, WVALS_LIMIT};

static std::vector<std::string>X6100_am_bws;
static const char *vX6100_am_bws[] =
{
"200",   "400",  "600",  "800", "1000", "1200", "1400", "1600", "1800", "2000",
"2200", "2400", "2600", "2800", "3000", "3200", "3400", "3600", "3800", "4000",
"4200", "4400", "4600", "4800", "5000", "5200", "5400", "5600", "5800", "6000",
"6200", "6400", "6600", "6800", "7000", "705", "7400", "705", "7800", "8000",
"8200", "8400", "8600", "8800", "9000", "9200", "9400", "9600", "9800", "10000" };
static int X6100_bw_vals_AM[] = {
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49
WVALS_LIMIT};

static std::vector<std::string>X6100_fm_bws;
static const char *vX6100_fm_bws[] =
{ "FIXED" };
static int X6100_bw_vals_FM[] = { 1, WVALS_LIMIT};

static GUI X6100_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 },	//0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 },	//1
	{ (Fl_Widget *)btnAGC,        2, 145,  50 },	//2
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 },	//3
	{ (Fl_Widget *)sldrSQUELCH, 266, 125, 156 },	//4
	{ (Fl_Widget *)btnNR,       214, 145,  50 },	//5
	{ (Fl_Widget *)sldrNR,      266, 145, 156 },	//6
	{ (Fl_Widget *)sldrMICGAIN,  54, 165, 156 },	//7
	{ (Fl_Widget *)sldrPOWER,   266, 165, 156 },	//8
//	{ (Fl_Widget *)btnLOCK,     214, 105,  50 },	//7
//	{ (Fl_Widget *)sldrINNER,   266, 105, 156 },	//8
//	{ (Fl_Widget *)btnCLRPBT,   214, 125,  50 },	//9
//	{ (Fl_Widget *)sldrOUTER,   266, 125, 156 },	//10
//	{ (Fl_Widget *)btnNotch,    214, 145,  50 },	//11
//	{ (Fl_Widget *)sldrNOTCH,   266, 145, 156 },	//12
//	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 },	//13
//	{ (Fl_Widget *)sldrPOWER,   266, 185, 156 },	//14
	{ (Fl_Widget *)NULL, 0, 0, 0 }
};

void RIG_X6100::initialize()
{
	VECTOR (X6100modes_, vX6100modes_);
	VECTOR (X6100_ssb_bws, vX6100_ssb_bws);
	VECTOR (X6100_rtty_bws, vX6100_rtty_bws);
	VECTOR (X6100_am_bws, vX6100_am_bws);
	VECTOR (X6100_fm_bws, vX6100_fm_bws);

	modes_ = X6100modes_;
	bandwidths_ = X6100_ssb_bws;
	bw_vals_ = X6100_bw_vals_SSB;

	_mode_type = X6100_mode_type;

	X6100_widgets[0].W = btnVol;
	X6100_widgets[1].W = sldrVOLUME;
	X6100_widgets[2].W = btnAGC;
	X6100_widgets[3].W = sldrRFGAIN;
	X6100_widgets[4].W = sldrSQUELCH;
	X6100_widgets[5].W = btnNR;
	X6100_widgets[6].W = sldrNR;
//	X6100_widgets[7].W = btnLOCK;
//	X6100_widgets[8].W = sldrINNER;
//	X6100_widgets[9].W = btnCLRPBT;
//	X6100_widgets[10].W = sldrOUTER;
//	X6100_widgets[11].W = btnNotch;
//	X6100_widgets[12].W = sldrNOTCH;
//	X6100_widgets[13].W = sldrMICGAIN;
//	X6100_widgets[14].W = sldrPOWER;
	X6100_widgets[7].W = sldrMICGAIN;
	X6100_widgets[8].W = sldrPOWER;

	btn_icom_select_13->deactivate();

	choice_rTONE->activate();
	choice_tTONE->activate();
}

RIG_X6100::RIG_X6100() {
	defaultCIV = 0xA4;
	adjustCIV(defaultCIV);

	name_ = X6100name_;
	modes_ = X6100modes_;
	bandwidths_ = X6100_ssb_bws;
	bw_vals_ = X6100_bw_vals_SSB;

	_mode_type = X6100_mode_type;

	serial_baudrate = BR19200;
	stopbits = 1;
	serial_retries = 2;
	serial_write_delay = 0;
	serial_post_write_delay = 0;
	serial_timeout = 50;
	serial_echo = true;
	serial_rtscts = false;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_catptt = true;
	serial_rtsptt = false;
	serial_dtrptt = false;

	widgets = X6100_widgets;

	def_freq = A.freq = 14070000ULL;
	def_mode = A.imode = 9;
	def_bw = A.iBW = 34;

	B.freq = 7070000ULL;
	B.imode = 9;
	B.iBW = 34;

	has_extras = true;

	has_cw_wpm = true;
	has_cw_spot_tone = true;
//	has_cw_qsk = true;
//	has_cw_break_in = true;

	has_vox_onoff = true;
	has_vox_gain = true;
	has_vox_anti = true;
	has_vox_hang = true;

	has_compON = true;
	has_compression = true;

	has_split = true;
	has_split_AB = true;

	has_micgain_control = true;
	has_bandwidth_control = true;

	has_smeter = true;

//	has_voltmeter = true;

	has_power_out = true;
	has_swr_control = true;
	has_alc_control = true;
	has_sql_control = true;
	has_agc_control = true;

	has_power_control = true;
	has_volume_control = true;
	has_mode_control = true;

	has_attenuator_control = true;
	has_preamp_control = true;

	has_noise_control = true;
	has_nb_level = true;

	has_noise_reduction = true;
	has_noise_reduction_control = true;

//	has_auto_notch = true;
//	has_notch_control = true;

//	has_pbt_controls = true;
	has_FILTER = true;

	has_rf_control = true;

	has_ptt_control = true;
//	has_tune_control = true;

	has_band_selection = true;

//	has_xcvr_auto_on_off = true;

	precision = 1;
	ndigits = 9;

	has_vfo_adj = true;

	can_change_alt_vfo = true;
	has_a2b = true;

	CW_sense = 0; // CW is LSB
};

static inline void minmax(int min, int max, int &val)
{
	if (val > max) val = max;
	if (val < min) val = min;
}

void RIG_X6100::selectA()
{
	cmd.assign(pre_to).append("\x07");
	cmd += '\x00';
	cmd.append(post);
	waitFB("select A");
	isett("selectA");
	inuse = onA;
}

void RIG_X6100::selectB()
{
	cmd.assign(pre_to).append("\x07");
	cmd += '\x01';
	cmd.append(post);
	waitFB("select B");
	isett("selectB");
	inuse = onB;
}

//======================================================================
// X6100 unique commands
//======================================================================

void RIG_X6100::swapAB()
{
	cmd = pre_to;
	cmd += 0x07; cmd += 0xB0;
	cmd.append(post);
	waitFB("Exchange vfos");
	get_modeA(); // get mode to update the filter A / B usage
	get_modeB();
}

void RIG_X6100::set_xcvr_auto_on()
{
	cmd = pre_to;
	cmd += '\x19'; cmd += '\x00';

	get_trace(1, "getID()");

	cmd.append(post);
	RigSerial->failed(0);

	if (waitFOR(8, "get ID") == false) {
		cmd.clear();
		int fes[] = { 2, 2, 2, 3, 7, 13, 25, 50, 75, 150, 150, 150 };
		if (progStatus.serial_baudrate >= 0 && progStatus.serial_baudrate <= 11) {
			cmd.append( fes[progStatus.serial_baudrate], '\xFE');
		}
		RigSerial->WriteBuffer(cmd.c_str(), cmd.length());

		cmd.assign(pre_to);
		cmd += '\x18'; cmd += '\x01';
		set_trace(1, "power_on()");
		cmd.append(post);
		RigSerial->failed(0);

		if (waitFB("Power ON")) {
			isett("power_on()");
			cmd = pre_to; cmd += '\x19'; cmd += '\x00';
			get_trace(1, "getID()");
			cmd.append(post);
			int i = 0;
			for (i = 0; i < 150; i++) { // 15 second total timeout
				if (waitFOR(8, "get ID", 100) == true) {
					RigSerial->failed(0);
					return;
				}
				update_progress(i / 2);
				Fl::awake();
			}
			RigSerial->failed(0);
			return;
		}

		isett("power_on()");
		RigSerial->failed(1);
		return;
	}
}

void RIG_X6100::set_xcvr_auto_off()
{
	cmd.clear();
	cmd.append(pre_to);
	cmd += '\x18'; cmd += '\x00';
	cmd.append(post);
	waitFB("Power OFF", 200);
}

bool RIG_X6100::check ()
{
	std::string resp = pre_fm;
	resp += '\x03';
	cmd = pre_to;
	cmd += '\x03';
	cmd.append( post );
	bool ok = waitFOR(11, "check vfo");
	isett("check vfo");
	return ok;
}

unsigned long long RIG_X6100::get_vfoA ()
{
	std::string resp;

	cmd.assign(pre_to).append("\x25");
	resp.assign(pre_fm).append("\x25");

	if (inuse == onB) {
		cmd  += '\x01';
		resp += '\x01';
	} else {
		cmd  += '\x00';
		resp += '\x00';
	}

	cmd.append(post);
	if (waitFOR(12, "get vfo A")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+6] == -1)
				A.freq = 0;
			else
				A.freq = fm_bcd_be(replystr.substr(p+6), 10);
		}
	}

	igett("get_vfoA");

	return A.freq;
}

void RIG_X6100::set_vfoA (unsigned long long freq)
{
	A.freq = freq;

	cmd.assign(pre_to).append("\x25");
	if (inuse == onB) cmd += '\x01';
	else      cmd += '\x00';

	cmd.append( to_bcd_be( freq, 10) );
	cmd.append( post );
	waitFB("set vfo A");

	isett("set_vfoA");

}

unsigned long long RIG_X6100::get_vfoB ()
{
	std::string resp;

	cmd.assign(pre_to).append("\x25");
	resp.assign(pre_fm).append("\x25");

	if (inuse == onB) {
		cmd  += '\x00';
		resp += '\x00';
	} else {
		cmd  += '\x01';
		resp += '\x01';
	}

	cmd.append(post);
	if (waitFOR(12, "get vfo B")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+6] == -1)
				A.freq = 0;
			else
				B.freq = fm_bcd_be(replystr.substr(p+6), 10);
		}
	}

	igett("get_vfoB");

	return B.freq;
}

void RIG_X6100::set_vfoB (unsigned long long freq)
{
	B.freq = freq;

	cmd.assign(pre_to).append("\x25");
	if (inuse == onB) cmd += '\x00';
	else      cmd += '\x01';

	cmd.append( to_bcd_be( freq, 10 ) );
	cmd.append( post );
	waitFB("set vfo B");

	isett("set_vfoB");
}

// expecting
//  0  1  2  3  4  5  6  7  8  9
// FE FE E0 94 26 NN NN NN NN FD
//                |  |  |  |
//                |  |  |  |__filter setting, 01, 02, 03
//                |  |  |_____data mode, 00 - off, 01 - on
//                |  |________Mode 00 - LSB
//                |                01 - USB
//                |                02 - AM
//                |                03 - CW
//                |                04 - RTTY
//                |                05 - FM
//                |                07 - CW-R
//                |                08 - RTTY-R
//                |                17 - DV
//                |___________selected vfo, 00 - active, 01 - inactive

int RIG_X6100::get_modeA()
{
	int md = 0;
	size_t p;

	std::string resp;
	cmd.assign(pre_to).append("\x26");
	resp.assign(pre_fm).append("\x26");

	if (inuse == onB)
		cmd += '\x01';
	else
		cmd += '\x00';

	cmd.append(post);

	if (waitFOR(10, "get mode A")) {
		p = replystr.rfind(resp);
		if (p == std::string::npos)
			goto end_wait_modeA;

		if (replystr[p+6] == -1) { md = A.imode = 0; }
		else {
			for (md = 0; md < DV705; md++) {
				if (replystr[p+6] == X6100_mode_nbr[md]) {
					A.imode = md;
					if (replystr[p+7] == 0x01 && A.imode < CW705)
						A.imode += 8;
					if (A.imode > DV705)
						A.imode = 1;
					break;
				}
			}
			A.filter = replystr[p+8];
			if (A.filter > 0 && A.filter < 4)
				mode_filterA[A.imode] = A.filter;
		}
	}

end_wait_modeA:

	get_trace(4, "get mode A[", X6100modes_[A.imode].c_str(), "] ", str2hex(replystr.c_str(), replystr.length()));

	if (A.imode == CW705 || A.imode == CWR705) {
		cmd.assign(pre_to).append("\x1A\x05");
		cmd += '\x00'; cmd += '\x53';
		cmd.append(post);
		resp.assign(pre_fm).append("\x1A\x05");
		resp += '\x00'; resp += '\x53';
		if (waitFOR(10, "get CW sideband")) {
			p = replystr.rfind(resp);
			CW_sense = replystr[p+8];
			if (CW_sense) {
				X6100_mode_type[CW705] = 'U';
				X6100_mode_type[CWR705] = 'L';
			} else {
				X6100_mode_type[CW705] = 'L';
				X6100_mode_type[CWR705] = 'U';
			}
		}
		get_trace(2, "get CW sideband ", str2hex(replystr.c_str(), replystr.length()));
	}

	return A.imode;
}

// LSB  USB  AM   CW  RTTY  FM  CW-R  RTTY-R  LSB-D  USB-D
//  0    1    2    3   4     5    6     7      8      9

void RIG_X6100::set_modeA(int val)
{
	A.imode = val;
	cmd.assign(pre_to);
	cmd += '\x26';
	if (inuse == onB)
		cmd += '\x01';					// unselected vfo
	else
		cmd += '\x00';					// selected vfo
	cmd += X6100_mode_nbr[A.imode];		// operating mode
	if (A.imode >= LSBD705 && A.imode < DV705)
		cmd += '\x01';					// data mode
	else
		cmd += '\x00';
	cmd += mode_filterA[A.imode];		// filter
	cmd.append( post );
	waitFB("set mode A");

	set_trace(4, "set mode A[", X6100modes_[A.imode].c_str(), "] ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_X6100::get_modeB()
{
	int md = 0;
	size_t p;

	std::string resp;
	cmd.assign(pre_to).append("\x26");
	resp.assign(pre_fm).append("\x26");

	if (inuse == onB)
		cmd += '\x00';   // active vfo
	else
		cmd += '\x01';   // inactive vfo
	cmd.append(post);

	if (waitFOR(10, "get mode B")) {
		p = replystr.rfind(resp);
		if (p == std::string::npos)
			goto end_wait_modeB;

		if (replystr[p+6] == -1) { md = filA = 0; }
		else {
			for (md = 0; md < LSBD705; md++) {
				if (replystr[p+6] == X6100_mode_nbr[md]) {
					B.imode = md;
					if (replystr[p+7] == 0x01 && B.imode < CW705)
						B.imode += 8;
					if (B.imode > DV705)
						B.imode = 1;
					break;
				}
			}
			B.filter = replystr[p+8];
		}
	}

end_wait_modeB:

	get_trace(4, "get mode B[", X6100modes_[B.imode].c_str(), "] ", str2hex(replystr.c_str(), replystr.length()));

	if (B.filter > 0 && B.filter < 4)
		mode_filterB[B.imode] = B.filter;

	get_trace(4, "get mode A[", X6100modes_[B.imode].c_str(), "] ", str2hex(replystr.c_str(), replystr.length()));
	if (B.imode == CW705 || B.imode == CWR705) {
		cmd.assign(pre_to).append("\x1A\x05");
		cmd += '\x00'; cmd += '\x53';
		cmd.append(post);
		resp.assign(pre_fm).append("\x1A\x05");
		resp += '\x00'; resp += '\x53';
		if (waitFOR(10, "get CW sideband")) {
			p = replystr.rfind(resp);
			CW_sense = replystr[p+8];
			if (CW_sense) {
				X6100_mode_type[CW705] = 'U';
				X6100_mode_type[CWR705] = 'L';
			} else {
				X6100_mode_type[CW705] = 'L';
				X6100_mode_type[CWR705] = 'U';
			}
		}
		get_trace(2, "get CW sideband ", str2hex(replystr.c_str(), replystr.length()));
	}

	return B.imode;
}

void RIG_X6100::set_modeB(int val)
{
	B.imode = val;
	cmd.assign(pre_to);
	cmd += '\x26';
	if (inuse == onB)
		cmd += '\x00';					// selected vfo
	else
		cmd += '\x01';					// unselected vfo
	cmd += X6100_mode_nbr[B.imode];		// operating mode
	if (B.imode >= LSBD705 && B.imode < DV705)
		cmd += '\x01';					// data mode
	else
		cmd += '\x00';
	cmd += mode_filterB[B.imode];		// filter
	cmd.append( post );
	waitFB("set mode B");

	set_trace(4, "set mode B[", X6100modes_[B.imode].c_str(), "] ", str2hex(replystr.c_str(), replystr.length()));
}

int RIG_X6100::get_FILT(int mode)
{
	if (inuse == onB) return mode_filterB[mode];
	return mode_filterA[mode];
}

void RIG_X6100::set_FILT(int filter)
{
	if (filter < 1 || filter > 3)
		return;

	if (inuse == onB) {
		B.filter = filter;
		mode_filterB[B.imode] = filter;
		cmd.assign(pre_to);
		cmd += '\x26';
		cmd += '\x00';							// selected vfo
		cmd += X6100_mode_nbr[B.imode];			// operating mode
		if (B.imode >= LSBD705) cmd += '\x01';	// data mode
		else cmd += '\x00';
		cmd += filter;							// filter
		cmd.append( post );
		waitFB("set mode/filter B");

		set_trace(4, "set mode/filter B[", X6100modes_[B.imode].c_str(), "] ", str2hex(replystr.c_str(), replystr.length()));
	} else {
		A.filter = filter;
		mode_filterA[A.imode] = filter;
		cmd.assign(pre_to);
		cmd += '\x26';
		cmd += '\x00';						// selected vfo
		cmd += X6100_mode_nbr[A.imode];	// operating mode
		if (A.imode >= LSBD705) cmd += '\x01';	// data mode
		else cmd += '\x00';
		cmd += filter;						// filter
		cmd.append( post );
		waitFB("set mode/filter A");

		set_trace(4, "set mode/filter A[", X6100modes_[A.imode].c_str(), "] ", str2hex(replystr.c_str(), replystr.length()));
	}
}

const char *RIG_X6100::FILT(int val)
{
	if (val < 1) val = 1;
	if (val > 3) val = 3;
	return(szfilter[val - 1]);
}

const char * RIG_X6100::nextFILT()
{
	int val = A.filter;
	if (inuse == onB) val = B.filter;
	val++;
	if (val > 3) val = 1;
	set_FILT(val);
	return szfilter[val - 1];
}

void RIG_X6100::set_FILTERS(std::string s)
{
	std::stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_filterA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_filterB[i];
	for (int i = 0; i < NUM_MODES; i++) {
		if (mode_filterA[i] < 1) mode_filterA[i] = 1;
		if (mode_filterA[i] > 3) mode_filterA[i] = 3;
		if (mode_filterB[i] < 1) mode_filterB[i] = 1;
		if (mode_filterB[i] > 3) mode_filterB[i] = 3;
	}
}

std::string RIG_X6100::get_FILTERS()
{
	std::stringstream s;
	for (int i = 0; i < NUM_MODES; i++) {
		if (mode_filterA[i] < 1) mode_filterA[i] = 1;
		if (mode_filterA[i] > 3) mode_filterA[i] = 3;
		if (mode_filterB[i] < 1) mode_filterB[i] = 1;
		if (mode_filterB[i] > 3) mode_filterB[i] = 3;
	}
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_filterA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_filterB[i] << " ";
	return s.str();
}

std::string RIG_X6100::get_BANDWIDTHS()
{
	std::stringstream s;
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwA[i] << " ";
	for (int i = 0; i < NUM_MODES; i++)
		s << mode_bwB[i] << " ";
	return s.str();
}

void RIG_X6100::set_BANDWIDTHS(std::string s)
{
	std::stringstream strm;
	strm << s;
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwA[i];
	for (int i = 0; i < NUM_MODES; i++)
		strm >> mode_bwB[i];
}

bool RIG_X6100::can_split()
{
	return true;
}

void RIG_X6100::set_split(bool val)
{
	split = val;
	cmd = pre_to;
	cmd += 0x0F;
	cmd += val ? 0x01 : 0x00;
	cmd.append(post);
	waitFB(val ? "set split ON" : "set split OFF");

	isett("set_split");
}

int RIG_X6100::get_split()
{
	int read_split = 0;
	cmd.assign(pre_to);
	cmd.append("\x0F");
	cmd.append( post );
	if (waitFOR(7, "get split")) {
		std::string resp = pre_fm;
		resp.append("\x0F");
		size_t p = replystr.find(resp);
		if (p != std::string::npos)
			read_split = replystr[p+5];
		if (read_split != 0xFA) // fail byte
			split = read_split;
	}

	igett("get_split");

	return split;
}

int RIG_X6100::get_bwA()
{
	if (A.imode == 3 || A.imode == 11) return 0; // FM, FM-D

	int current_vfo = inuse;
	if (current_vfo == onB) selectA();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	int bwval = A.iBW;
	if (waitFOR(8, "get_bwA")) {
		std::string resp = pre_fm;
		resp.append("\x1A\x03");
		size_t p = replystr.find(resp);
		if (p != std::string::npos)
			bwval = fm_bcd(replystr.substr(p+6), 2);
	}
	if (bwval != A.iBW) {
		A.iBW = bwval;
		mode_bwA[A.imode] = bwval;
	}

	if (current_vfo == onB) selectB();

	igett("get_bwA");

	return A.iBW;
}

void RIG_X6100::set_bwA(int val)
{

	if (A.imode == 3 || A.imode == 11) return; // FM, FM-D

	A.iBW = val;

	int current_vfo = inuse;
	if (current_vfo == onB) selectA();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(A.iBW, 2));
	cmd.append(post);
	waitFB("set bwA");

	mode_bwA[A.imode] = val;
	isett("set_bwA");

	if (current_vfo == onB) selectB();
}

int RIG_X6100::get_bwB()
{
	if (B.imode == 3 || B.imode == 11) return 0; // FM, FM-D

	int current_vfo = inuse;
	if (current_vfo == onA) selectB();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(post);
	int bwval = B.iBW;
	if (waitFOR(8, "get_bwB")) {
		std::string resp = pre_fm;
		resp.append("\x1A\x03");
		size_t p = replystr.find(resp);
		if (p != std::string::npos)
			bwval = fm_bcd(replystr.substr(p+6), 2);
	}
	if (bwval != B.iBW) {
		B.iBW = bwval;
		mode_bwB[B.imode] = bwval;
	}

	if (current_vfo == onA) selectA();

	igett("get_bwB");

	return B.iBW;
}

void RIG_X6100::set_bwB(int val)
{
	if (B.imode == 3 || B.imode == 11) return; // FM, FM-D
	B.iBW = val;

	int current_vfo = inuse;
	if (current_vfo == onA) selectB();

	cmd = pre_to;
	cmd.append("\x1a\x03");
	cmd.append(to_bcd(B.iBW, 2));
	cmd.append(post);
	waitFB("set bwB");

	mode_bwB[B.imode] = val;
	isett("set_bwB");

	if (current_vfo == onA) selectA();
}

// LSB  USB  AM   FM   CW  CW-R  RTTY  RTTY-R  LSB-D  USB-D  AM-D  FM-D
//  0    1    2    3   4     5    6     7      8      9       10    11

int RIG_X6100::adjust_bandwidth(int m)
{
	int bw = 0;
	switch (m) {
		case 2: case 10: // AM, AM-D
			bandwidths_ = X6100_am_bws;
			bw_vals_ = X6100_bw_vals_AM;
			bw = 19;
			break;
		case 3: case 11: // FM, FM-D
			bandwidths_ = X6100_fm_bws;
			bw_vals_ = X6100_bw_vals_FM;
			bw = 0;
			break;
		case 6: case 7: // RTTY, RTTY-R
			bandwidths_ = X6100_rtty_bws;
			bw_vals_ = X6100_bw_vals_RTTY;
			bw = 12;
			break;
		case 4: case 5: // CW, CW -R
			bandwidths_ = X6100_ssb_bws;
			bw_vals_ = X6100_bw_vals_SSB;
			bw = 12;
			break;
		case 0: case 1: // LSB, USB
		case 8: case 9: // LSB-D, USB-D
		default:
			bandwidths_ = X6100_ssb_bws;
			bw_vals_ = X6100_bw_vals_SSB;
			bw = 34;
	}
	return bw;
}

std::vector<std::string>& RIG_X6100::bwtable(int m)
{
	switch (m) {
		case 2: case 10: // AM, AM-D
			return X6100_am_bws;
			break;
		case 3: case 11: // FM, FM-D
			return X6100_fm_bws;
			break;
		case 6: case 7: // RTTY, RTTY-R
			return X6100_rtty_bws;
			break;
		case 4: case 5: // CW, CW -R
		case 0: case 1: // LSB, USB
		case 8: case 9: // LSB-D, USB-D
		default:
			return X6100_ssb_bws;
	}
	return X6100_ssb_bws;
}

int RIG_X6100::def_bandwidth(int m)
{
	int bw = adjust_bandwidth(m);
	if (inuse == onB) {
		if (mode_bwB[m] == -1)
			mode_bwB[m] = bw;
		return mode_bwB[m];
	}
	if (mode_bwA[m] == -1)
		mode_bwA[m] = bw;
	return mode_bwA[m];
}

int RIG_X6100::get_mic_gain()
{
	int val = 0;
	std::string cstr = "\x14\x0B";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get mic")) {
		igett("get mic gain");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p + 6));
	}
	return val;
}

void RIG_X6100::set_mic_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x0B");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set mic gain");
	isett("set mic gain");
}

void RIG_X6100::get_mic_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0;
	max = 100;
	step = 1;
}

static int comp_level[] = {11,34,58,81,104,128,151,174,197,221,244};
void RIG_X6100::set_compression(int on, int val)
{
	cmd = pre_to;
	cmd.append("\x16\x44");
	if (on) cmd += '\x01';
	else cmd += '\x00';
	cmd.append(post);
	waitFB("set Comp ON/OFF");

	if (val < 0) return;
	if (val > 10) return;

	cmd.assign(pre_to).append("\x14\x0E");
	cmd.append(to_bcd(comp_level[val], 3));
	cmd.append( post );
	waitFB("set comp");
	isett("set comp");
}

void RIG_X6100::get_compression(int &on, int &val)
{
	std::string resp;

	cmd.assign(pre_to).append("\x16\x44").append(post);

	resp.assign(pre_fm).append("\x16\x44");

	if (waitFOR(8, "get comp on/off")) {
		size_t p = replystr.find(resp);
		if (p != std::string::npos)
			on = (replystr[p+6] == 0x01);
	}

	cmd.assign(pre_to).append("\x14\x0E").append(post);
	resp.assign(pre_fm).append("\x14\x0E");

	if (waitFOR(9, "get comp level")) {
		igett("get comp level");
		size_t p = replystr.find(resp);
		int level = 0;
		if (p != std::string::npos) {
			level = fm_bcd(replystr.substr(p+6), 3);
			for (val = 0; val < 11; val++)
				if (level <= comp_level[val]) break;
		}
	}
}

void RIG_X6100::set_vox_onoff()
{
	if (progStatus.vox_onoff) {
		cmd.assign(pre_to).append("\x16\x46\x01");
		cmd.append( post );
		waitFB("set vox ON");
	} else {
		cmd.assign(pre_to).append("\x16\x46");
		cmd += '\x00';
		cmd.append( post );
		waitFB("set vox OFF");
	}
	isett("set vox on/off");
}

// Xcvr values range 0...255 step 1
void RIG_X6100::get_vox_gain_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_X6100::set_vox_gain()
{
	int vox_gain = round((progStatus.vox_gain * 255 / 100 + 0.5));
	minmax(0, 255, vox_gain);
	cmd.assign(pre_to).append("\x14\x16");
	cmd.append(to_bcd(vox_gain, 3));
	cmd.append( post );
	waitFB("SET vox gain");
	isett("set vox gain");
}

// Xcvr values range 0...255 step 1
void RIG_X6100::get_vox_anti_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_X6100::set_vox_anti()
{
	int vox_anti = round((progStatus.vox_anti * 255 / 100 + 0.5));
	minmax(0, 255, vox_anti);
	cmd.assign(pre_to).append("\x14\x17");
	cmd.append(to_bcd(vox_anti, 3));
	cmd.append( post );
	waitFB("SET anti-vox");
	isett("set anti vox");
}

// VOX hang 0.0 - 2.0, step 0.1
// Xcvr values 0..20 step 1
void RIG_X6100::get_vox_hang_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 20; step = 1;
}

void RIG_X6100::set_vox_hang()
{
	cmd.assign(pre_to).append("\x1A\x05\x01\x91");
	cmd.append(to_bcd(progStatus.vox_hang, 2));
	cmd.append( post );
	waitFB("SET vox hang");
	isett("set vox hang");
}

//----------------------------------------------------------------------
// CW controls

void RIG_X6100::get_cw_wpm_min_max(int &min, int &max)
{
	min = 6; max = 48;
}

void RIG_X6100::set_cw_wpm()
{
	int iwpm = round((progStatus.cw_wpm - 6) * 255 / 42 + 0.5);
	minmax(0, 255, iwpm);

	cmd.assign(pre_to).append("\x14\x0C");
	cmd.append(to_bcd(iwpm, 3));
	cmd.append( post );
	waitFB("SET cw wpm");
	isett("set_cw_wpm");
}

void RIG_X6100::set_break_in()
{
// 16 47 00 break-in off
// 16 47 01 break-in semi
// 16 47 02 break-in full

	cmd.assign(pre_to).append("\x16\x47");

	switch (progStatus.break_in) {
		case 2: cmd += '\x02'; break_in_label("FULL"); break;
		case 1: cmd += '\x01'; break_in_label("SEMI");  break;
		case 0:
		default: cmd += '\x00'; break_in_label("BK-IN");
	}
	cmd.append(post);
	waitFB("SET break-in");
	isett("set break in");
}

int RIG_X6100::get_break_in()
{
	cmd.assign(pre_to).append("\x16\x47").append(post);
	std::string resp;
	resp.assign(pre_fm);
	if (waitFOR(8, "get break in")) {
		igett("get break in");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			progStatus.break_in = replystr[p+6];
			if (progStatus.break_in == 0) break_in_label("qsk");
			else  if (progStatus.break_in == 1) break_in_label("SEMI");
			else  break_in_label("FULL");
		}
	}
	return progStatus.break_in;
}

void RIG_X6100::get_cw_qsk_min_max_step(double &min, double &max, double &step)
{
	min = 2.0; max = 13.0; step = 0.1;
}

void RIG_X6100::set_cw_qsk()
{
	int qsk = round ((progStatus.cw_qsk - 2.0) * 255.0 / 11.0 + 0.5);
	minmax(0, 255, qsk);

	cmd.assign(pre_to).append("\x14\x0F");
	cmd.append(to_bcd(qsk, 3));
	cmd.append(post);
	waitFB("Set cw qsk delay");
	isett("set cw qsk delay");
}

void RIG_X6100::get_cw_spot_tone_min_max_step(int &min, int &max, int &step)
{
	min = 300; max = 900; step = 5;
}

void RIG_X6100::set_cw_spot_tone()
{
	cmd.assign(pre_to).append("\x14\x09"); // values 0=300Hz 255=900Hz
	int n = round((progStatus.cw_spot_tone - 300) * 255.0 / 600.0 + 0.5);
	minmax(0, 255, n);

	cmd.append(to_bcd(n, 3));
	cmd.append( post );
	waitFB("SET cw spot tone");
	isett("set cw spot tone");
}

void RIG_X6100::set_cw_vol()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x24';
	cmd.append(to_bcd((int)(progStatus.cw_vol * 2.55), 3));
	cmd.append( post );
	waitFB("SET cw sidetone volume");
	isett("set cw sidetone vol");
}

// Tranceiver PTT on/off
void RIG_X6100::set_PTT_control(int val)
{
	cmd = pre_to;
	cmd += '\x1c';
	cmd += '\x00';
	cmd += (unsigned char) val;
	cmd.append( post );
	waitFB("set ptt");
	isett("set PTT");
	ptt_ = val;
}

int RIG_X6100::get_PTT()
{
	cmd = pre_to;
	cmd += '\x1c'; cmd += '\x00';
	std::string resp = pre_fm;
	resp += '\x1c'; resp += '\x00';
	cmd.append(post);
	if (waitFOR(8, "get PTT")) {
		igett("get PTT");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			ptt_ = replystr[p + 6];
	}
	return ptt_;
}

static catpair vol_pairs[] = {
	{  0,  0}, {  4,  1}, {  9,  2}, { 13,  3}, { 18,  4}, { 23,  5}, { 27,  6}, { 32,  7}, { 37,  8}, { 41,  9},
	{ 46, 10}, { 51, 11}, { 55, 12}, { 60, 13}, { 64, 14}, { 69, 15}, { 74, 16}, { 78, 17}, { 83, 18}, { 88, 19},
	{ 92, 20}, { 97, 21}, {102, 22}, {106, 23}, {111, 24}, {115, 25}, {120, 26}, {125, 27}, {129, 28}, {134, 29},
	{139, 30}, {143, 31}, {148, 32}, {153, 33}, {157, 34}, {162, 35}, {166, 36}, {171, 37}, {176, 38}, {180, 39},
	{185, 40}, {190, 41}, {194, 42}, {199, 43}, {204, 44}, {208, 45}, {213, 46}, {217, 47}, {222, 48}, {227, 49},
	{231, 50}, {236, 51}, {241, 52}, {245, 53}, {250, 54}, {255, 55}
};

// Volume control val 0 ... 100
void RIG_X6100::set_volume_control(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x01");
	int cat = 0;
	if (val) ++val;
	if (val > 255) val = 255;
	for (size_t n = 0; n < sizeof(vol_pairs) / sizeof(catpair); n++)
		if (vol_pairs[n].val >= val) {
			cat = vol_pairs[n].cat;
			break;
		} 
	cmd += bcd[cat / 100];
	cmd += bcd[cat % 100];
	cmd.append( post );
	waitFB("set vol");
	isett("set volume_control()");
}

int RIG_X6100::get_volume_control()
{
	int val = 0;
	std::string cstr = "\x14\x01";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	int mtr = 0;
	if (waitFOR(9, "get vol")) {
		igett("get volume control");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			size_t i = 0;
			for (i = 0; i < sizeof(vol_pairs) / sizeof(catpair) - 1; i++)
				if (mtr >= vol_pairs[i].cat && mtr < vol_pairs[i+1].cat)
					break;
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			val = (int)ceil(vol_pairs[i].val +
				(vol_pairs[i+1].val - vol_pairs[i].val)*(mtr - vol_pairs[i].cat)/(vol_pairs[i+1].cat - vol_pairs[i].cat));

			if (val > 55) val = 55;
		}
	}
	return val;
}

void RIG_X6100::get_vol_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 55; step = 1;
}

void RIG_X6100::set_power_control(double val)
{
	cmd = pre_to;
	cmd.append("\x14\x0A");
	cmd.append(to_bcd((unsigned long long int)((val + 1) * 25), 4));
	cmd.append( post );
	isett("set_power_control()");
	waitFB("set power");
}

double RIG_X6100::get_power_control()
{
	int val = progStatus.power_level;
	std::string cstr = "\x14\x0A";
	std::string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get power")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = fm_bcd(replystr.substr(p+6), 3);
	}
	igett("get_power_control()");

	if (val <= 25) return 1.0;
	if (val <= 51) return 2.0;
	if (val <= 76) return 3.0;
	if (val <= 102) return 4.0;
	if (val <= 127) return 5.0;
	if (val <= 153) return 6.0;
	if (val <= 178) return 7.0;
	if (val <= 204) return 8.0;
	if (val <= 229) return 9.0;
	return 10.0;
}

void RIG_X6100::get_pc_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 10.0; step = 1.0;
}

int RIG_X6100::get_smeter()
{
	std::string cstr = "\x15\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get smeter")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr * 1.2);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

double RIG_X6100::get_voltmeter()
{
	std::string cstr = "\x15\x15";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );

	int mtr = 0;
	double val = 0;

	int ret = waitFOR(9, "get voltmeter");
	igett( "get_voltmeter()");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			val = 16.0 * mtr / 241.0;
			return val;
		}
	}
	return -1;
}

int RIG_X6100::get_power_out(void)
{
	std::string cstr = "\x15\x11";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= 0;

	int ret = waitFOR(9, "get power out");
	igett("get power out");

	if (ret) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			if (mtr > 0) ++mtr;
		}
	}
	return mtr;
}

struct swrpair {int mtr; float swr;};

// Table entries below correspond to SWR readings of 1.1, 1.5, 2.0, 2.5, 3.0 and infinity.
// Values are also tweaked to fit the display of the SWR meter.


static catpair swr_pairs[] = {
	{0, 0.0},
	{48, 10.5},
	{80, 23.0},
	{103, 35.0},
	{120, 48.0},
	{255, 100.0 } };



int RIG_X6100::get_swr(void)
{
	std::string cstr = "\x15\x12";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get swr")) {
		igett("get swr");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			size_t i = 0;
			for (i = 0; i < sizeof(swr_pairs) / sizeof(catpair) - 1; i++)
				if (mtr >= swr_pairs[i].cat && mtr < swr_pairs[i+1].cat)
					break;
			if (mtr < 0) mtr = 0;
			if (mtr > 255) mtr = 255;
			mtr = (int)ceil(swr_pairs[i].val +
				(swr_pairs[i+1].val - swr_pairs[i].val)*(mtr - swr_pairs[i].cat)/(swr_pairs[i+1].cat - swr_pairs[i].cat));

			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

int RIG_X6100::get_alc(void)
{
	std::string cstr = "\x15\x13";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	int mtr= -1;
	if (waitFOR(9, "get alc")) {
		igett("get alc");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			mtr = fm_bcd(replystr.substr(p+6), 3);
			mtr = (int)ceil(mtr /1.2);
			if (mtr > 100) mtr = 100;
		}
	}
	return mtr;
}

void RIG_X6100::set_rf_gain(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x02");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set RF");
	isett("set rf gain");
}

int RIG_X6100::get_rf_gain()
{
	int val = progStatus.rfgain;
	std::string cstr = "\x14\x02";
	std::string resp = pre_fm;
	cmd = pre_to;
	cmd.append(cstr).append(post);
	resp.append(cstr);
	if (waitFOR(9, "get RF")) {
		igett("get rf gain");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p + 6));
	}
	return val;
}

void RIG_X6100::get_rf_min_max_step(double &min, double &max, double &step)
{
	min = 0; max = 100; step = 1;
}

int RIG_X6100::next_preamp()
{
	if (atten_level == 1)
		return preamp_level;
	switch (preamp_level) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 0;
	}
	return 0;
}

void RIG_X6100::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';

	preamp_level = val;

	cmd += (unsigned char)preamp_level;
	cmd.append( post );
	waitFB(	(preamp_level == 0) ? "set Preamp OFF" :
			(preamp_level == 1) ? "set Preamp Level 1" :
			"set Preamp Level 2");
	isett("set preamp on/off");
}

int RIG_X6100::get_preamp()
{
	std::string cstr = "\x16\x02";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get Preamp Level")) {
		igett("get preamp level");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			preamp_level = replystr[p+6];
		}
	}
	return preamp_level;
}

void RIG_X6100::set_attenuator(int val)
{
	if (val) {
		atten_level = 1;
	} else {
		atten_level = 0;
	}

	cmd = pre_to;
	cmd += '\x11';
	cmd += atten_level ? '\x20' : '\x00';
	cmd.append( post );
	waitFB("set att");
	isett("set attenuator");
}

int RIG_X6100::next_attenuator()
{
	if (atten_level) return 0;
	return 1;
}

int RIG_X6100::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	std::string resp = pre_fm;
	resp += '\x11';
	if (waitFOR(7, "get ATT")) {
		igett("get attenuator");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+5] == 0x20) {
				atten_level = 1;
				return 1;
			} else {
				atten_level = 0;
				return 0;
			}
		}
	}
	return 0;
}

const char *RIG_X6100::PRE_label()
{
	switch (preamp_level) {
		case 0: default:
			return "PRE"; break;
		case 1:
			return "Pre 1"; break;
		case 2:
			return "Pre 2"; break;
	}
	return "PRE";
}

const char *RIG_X6100::ATT_label()
{
	if (atten_level == 1) return "20 dB";
	return "ATT";
}

void RIG_X6100::set_noise(bool val)
{
	cmd = pre_to;
	cmd.append("\x16\x22");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set noise");
	isett("set noise");
}

int RIG_X6100::get_noise()
{
	int val = progStatus.noise;
	std::string cstr = "\x16\x22";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get noise")) {
		igett("get noise");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = replystr[p+6];
		}
	}
	return val;
}

void RIG_X6100::set_nb_level(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x12");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set NB level");
	isett("set nb level");
}

int  RIG_X6100::get_nb_level()
{
	int val = progStatus.nb_level;
	std::string cstr = "\x14\x12";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get NB level")) {
		igett("get nb level");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_X6100::set_noise_reduction(int val)
{
	cmd = pre_to;
	cmd.append("\x16\x40");
	cmd += val ? 1 : 0;
	cmd.append(post);
	waitFB("set NR");
	isett("set NR");
}

int RIG_X6100::get_noise_reduction()
{
	std::string cstr = "\x16\x40";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(8, "get NR")) {
		igett("get NR");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			return (replystr[p+6] ? 1 : 0);
	}
	return progStatus.noise_reduction;
}

/*

I:12:06:50: get NR ans in 0 ms, OK
cmd FE FE 7A E0 16 40 FD
ans FE FE 7A E0 16 40 FD
FE FE E0 7A 16 40 01 FD
 0  1  2  3  4  5  6  7

I:12:06:50: get NRval ans in 0 ms, OK
cmd FE FE 7A E0 14 06 FD
ans FE FE 7A E0 14 06 FD
FE FE E0 7A 14 06 00 24 FD
 0  1  2  3  4  5  6  7  8

*/

void RIG_X6100::set_noise_reduction_val(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x06");
	val *= 16;
	val += 8;
	cmd.append(to_bcd(val, 3));
	cmd.append(post);
	waitFB("set NRval");
	isett("set NR val");
}

int RIG_X6100::get_noise_reduction_val()
{
	int val = progStatus.noise_reduction_val;
	std::string cstr = "\x14\x06";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get NRval")) {
		igett("get NR val");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = fm_bcd(replystr.substr(p+6),3);
			val -= 8;
			val /= 16;
		}
	}
	return val;
}

void RIG_X6100::set_squelch(int val)
{
	cmd = pre_to;
	cmd.append("\x14\x03");
	cmd.append(bcd255(val));
	cmd.append( post );
	waitFB("set Sqlch");
	isett("set squelch");
}

int  RIG_X6100::get_squelch()
{
	int val = progStatus.squelch;
	std::string cstr = "\x14\x03";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append(post);
	if (waitFOR(9, "get squelch")) {
		igett("get squelch");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			val = num100(replystr.substr(p+6));
	}
	return val;
}

void RIG_X6100::set_auto_notch(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x41';
	cmd += (unsigned char)val;
	cmd.append( post );
	waitFB("set AN");
	isett("set auto notch on/off");
}

int RIG_X6100::get_auto_notch()
{
	std::string cstr = "\x16\x41";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get AN")) {
		igett("get autonotch");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			if (replystr[p+6] == 0x01) {
				auto_notch_label("AN", true);
				return true;
			} else {
				auto_notch_label("AN", false);
				return false;
			}
		}
	}
	return progStatus.auto_notch;
}

void RIG_X6100::set_notch(bool on, int freq)
{
	int hexval;
	switch (vfo->imode) {
		default:
		case USB705: case USBD705: case RTTYR705:
		case DV705:
			hexval = freq - 1500;
			break;
		case LSB705: case LSBD705: case RTTY705:
			hexval = 1500 - freq;
			break;
		case CW705:
			if (CW_sense)
				hexval = freq - progStatus.cw_spot_tone;
			else
				hexval = progStatus.cw_spot_tone - freq;
			break;
		case CWR705:
			if (CW_sense)
				hexval = progStatus.cw_spot_tone - freq;
			else
				hexval = freq - progStatus.cw_spot_tone;
			break;
	}

	hexval /= 20;
	hexval += 128;
	if (hexval < 0) hexval = 0;
	if (hexval > 255) hexval = 255;

	cmd = pre_to;
	cmd.append("\x16\x48");
	cmd += on ? '\x01' : '\x00';
	cmd.append(post);
	waitFB("set notch");
	set_trace(2, "set_notch() ", str2hex(cmd.c_str(), cmd.length()));

	cmd = pre_to;
	cmd.append("\x14\x0D");
	cmd.append(to_bcd(hexval,3));
	cmd.append(post);
	waitFB("set notch val");
	set_trace(2, "set_notch_val() ", str2hex(cmd.c_str(), cmd.length()));
}

bool RIG_X6100::get_notch(int &val)
{
	bool on = false;
	val = 1500;

	std::string cstr = "\x16\x48";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(8, "get notch")) {
		igett("get_notch()");
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos)
			on = replystr[p + 6];
		cmd = pre_to;
		resp = pre_fm;
		cstr = "\x14\x0D";
		cmd.append(cstr);
		resp.append(cstr);
		cmd.append(post);
		if (waitFOR(9, "notch val")) {
			igett("get notch val");
			size_t p = replystr.rfind(resp);
			if (p != std::string::npos) {
				val = (int)ceil(fm_bcd(replystr.substr(p+6),3));
				val -= 128;
				val *= 20;
				switch (vfo->imode) {
					default:
					case USB705: case USBD705: case RTTYR705:
					case DV705:
						val = 1500 + val;
						break;
					case LSB: case LSBD705: case RTTY705:
						val = 1500 - val;
						break;
					case CW705:
						if (CW_sense)
							val = progStatus.cw_spot_tone + val;
						else
							val = progStatus.cw_spot_tone - val;
						break;
					case CWR705:
						if (CW_sense)
							val = progStatus.cw_spot_tone - val;
						else
							val = progStatus.cw_spot_tone + val;
						break;
				}
			}
			get_trace(2, "get_notch_val() ", str2hex(replystr.c_str(), replystr.length()));
		}
	}
	return on;
}

void RIG_X6100::get_notch_min_max_step(int &min, int &max, int &step)
{
	switch (vfo->imode) {
		default:
		case USB705: case USBD705: case RTTYR705:
		case LSB705: case LSBD705: case RTTY705:
		case DV705:
			min = 0; max = 3000; step = 20; break;
		case CW705: case CWR705:
			min = progStatus.cw_spot_tone - 500;
			max = progStatus.cw_spot_tone + 500;
			step = 20;
			break;
	}
}
static int agcval = 3;
int  RIG_X6100::get_agc()
{
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd.append(post);
	if (waitFOR(8, "get AGC")) {
		igett("get agc");
		size_t p = replystr.find(pre_fm);
		if (p != std::string::npos)
			agcval = replystr[p+6]; // 1 == off, 2 = FAST, 3 = MED, 4 = SLOW
	}
	return agcval;
}

int RIG_X6100::incr_agc()
{
	agcval++;
	if (agcval == 4) agcval = 1;
	cmd = pre_to;
	cmd.append("\x16\x12");
	cmd += agcval;
	cmd.append(post);
	waitFB("set AGC");
	isett("set agc");
	return agcval;
}


static const char *agcstrs[] = {"AGC", "FST", "MED", "SLO"};
const char *RIG_X6100::agc_label()
{
	return agcstrs[agcval];
}

int  RIG_X6100::agc_val()
{
	return (agcval);
}

void RIG_X6100::set_if_shift(int val)
{
	int shift;
	sh_ = val;
	if (val == 0) sh_on_ = false;
	else sh_on_ = true;

	shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF on/off");
	isett("set IF on/off");

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set IF val");
	isett("set IF val");
}

bool RIG_X6100::get_if_shift(int &val) {
	val = sh_;
	return sh_on_;
}

void RIG_X6100::get_if_min_max_step(int &min, int &max, int &step)
{
	min = -50;
	max = +50;
	step = 1;
}

void RIG_X6100::set_pbt_inner(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x07");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT inner");
	isett("set_pbt_inner()");
}

void RIG_X6100::set_pbt_outer(int val)
{
	int shift = 128 + val * 128 / 50;
	if (shift < 0) shift = 0;
	if (shift > 255) shift = 255;

	cmd = pre_to;
	cmd.append("\x14\x08");
	cmd.append(to_bcd(shift, 3));
	cmd.append(post);
	waitFB("set PBT outer");
	isett("set_pbt_outer()");
}

int RIG_X6100::get_pbt_inner()
{
	int val = 0;
	std::string cstr = "\x14\x07";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get pbt inner")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	igett("get_pbt_inner");
	return val;
}

int RIG_X6100::get_pbt_outer()
{
	int val = 0;
	std::string cstr = "\x14\x08";
	std::string resp = pre_fm;
	resp.append(cstr);
	cmd = pre_to;
	cmd.append(cstr);
	cmd.append( post );
	if (waitFOR(9, "get pbt outer")) {
		size_t p = replystr.rfind(resp);
		if (p != std::string::npos) {
			val = num100(replystr.substr(p+6));
			val -= 50;
		}
	}
	igett("get_pbt_outer");
	return val;
}

void RIG_X6100::setVfoAdj(double v)
{
	vfo_ = v;
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x89';
	cmd.append(bcd255(int(v)));
	cmd.append(post);
	waitFB("SET vfo adjust");
}

double RIG_X6100::getVfoAdj()
{
	cmd.assign(pre_to);
	cmd.append("\x1A\x05");
	cmd += '\x00';
	cmd += '\x89';
	cmd.append(post);

	if (waitFOR(11, "get vfo adj")) {
		igett("get vfo adj");
		size_t p = replystr.find(pre_fm);
		if (p != std::string::npos) {
			vfo_ = num100(replystr.substr(p+8));
		}
	}
	return vfo_;
}

/*
Received from transceiver:
Rx 56 bytes
00 | FE FE E0 A4 
04 | 1A 01             band stack register command
06 | 03                register # 3
07 | 01                left side of display
08 | 00 00 05 07 00    frequency
13 | 00                mode 
14 | 02                filter #
15 | 00                data mode == 0x01
16 | 00                duplex tone
17 | 00                digital squelch
18 | 00 08 85          repeater tone freq
21 | 00 08 85          tone squelch freq
25 | 00 00 23          dtcs code
27 | 00                digital squelch code
28 | 00 50 00          duplex offset freq
31 | 43 51 43 51 43 51 20 20                          destination call 8 chars
39 | 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20  register description 16 chars
55 | FD
*/
// test std::string

void RIG_X6100::get_band_selection(int v)
{
	int v2 = v;
	switch (v) {
		case 11: v2 = 13; break;
		case 12: v2 = 14; break;
		default: break;
	}
	cmd.assign(pre_to);
	cmd.append("\x1A\x01");
	cmd += to_bcd_be( v2, 2 );
	cmd += '\x01';
	cmd.append( post );

	if (waitFOR(56, "get band stack")) {
		igett("get band stack");
		size_t p = replystr.rfind(pre_fm);
		if (p != std::string::npos) {
			int bandfreq = fm_bcd_be(replystr.substr(p + 8, 5), 10);
//			int bandmode = fm_bcd(replystr.substr(p + 13, 1) ,2);
			int mode = 0;
			for (int md = LSB705; md <= DV705; md++) {
				if (replystr[p + 13] == X6100_mode_nbr[md]) {
					mode = md;
					if (replystr[p+15] == 0x01 && mode < CW705)
						mode += 8;
					break;
				}
			}
			if (mode > DV705) mode = USB705;
			int bandfilter = replystr[p+14];
			int tone = fm_bcd(replystr.substr(p + 18, 3), 6);
			tTONE = 0;
			for (size_t n = 0; n < sizeof(PL_tones) / sizeof(*PL_tones); n++) {
				if (tone == PL_tones[n]) {
					tTONE = n;
					break;
				}
			}
			tone = fm_bcd(replystr.substr(p + 21, 3), 6);
			rTONE = 0;
			for (size_t n = 0; n < sizeof(PL_tones) / sizeof(*PL_tones); n++) {
				if (tone == PL_tones[n]) {
					rTONE = n;
					break;
				}
			}
			if (inuse == onB) {
				set_vfoB(bandfreq);
				set_modeB(mode);
				set_FILT(bandfilter);
			} else {
				set_vfoA(bandfreq);
				set_modeA(mode);
				set_FILT(bandfilter);
			}
		}
		return;
	}
}

void RIG_X6100::set_band_selection(int v)
{
	switch (v) {
		case 11: v = 13; break;
		case 12: v = 14; break;
		default: break;
	}
	unsigned long long freq = (inuse == onB ? B.freq : A.freq);
	int fil = (inuse == onB ? B.filter : A.filter);
	int mode = (inuse == onB ? B.imode : A.imode);

	cmd.assign(pre_to);
	cmd += '\x1A'; cmd += '\x01';
	cmd += to_bcd_be( v, 2 );
	cmd += '\x01';
	cmd.append( to_bcd_be( freq, 10 ) );					// freq
	cmd += X6100_mode_nbr[mode];							// mode
	cmd += fil;												// filter #
	if (mode >= LSBD705 && mode < DV705)
		cmd += '\x01';		// data mode
	else
		cmd += '\x00';
	cmd += '\x00'; cmd += '\x00';							// duplex tone, digital squelch
	cmd.append(to_bcd(PL_tones[tTONE], 6));					// transmit tone
	cmd.append(to_bcd(PL_tones[rTONE], 6));					// receive tone
	cmd += '\x00'; cmd += '\x00'; cmd += '\x23';			// dtcs tone
	cmd += '\x00';											//digital squelch code
	cmd += '\x00'; cmd += '\x60'; cmd += '\x00';			// duplex offset
	cmd.append("        ");
	cmd.append("                ");
	cmd.append(post);

	waitFB("set_band_selection");

	isett("set_band_selection");
}

