// ----------------------------------------------------------------------------
// Copyright (C) 2014, 2020
//              Michael Black W9MDB
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

#include "other/PowerSDR.h"
#include "support.h"
#include <sstream>

static const char PowerSDRname_[] = "PowerSDR";

static std::vector<std::string>PowerSDRmodes_;
static const char *vPowerSDRmodes_[] = {
	"LSB", "USB", "DSB", "CWL", "CWU", "FM", "AM", "DIGU", "SPEC", "DIGL", "SAM", "DRM"};

static std::vector<std::string>PowerSDR_mode_chr;
static const char *vPowerSDR_mode_chr[] =  { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11" };
static const char PowerSDR_mode_type[] = { 'L', 'U', 'U', 'L', 'U', 'U', 'U', 'U', 'U', 'L', 'U', 'U' };

static std::vector<std::string>PowerSDR_empty;
static const char *vPowerSDR_empty[] = { "NONE" };
//------------------------------------------------------------------------
static std::vector<std::string>PowerSDR_USBwidths;
static const char *vPowerSDR_USBwidths[] = {
" 5000", " 4400", " 3800", " 3300", " 2900",
" 2700", " 2400", " 2100", " 1800", " 1000",
" Var1", " Var2" };
static std::vector<std::string>PowerSDR_CAT_USB;
static const char *vPowerSDR_CAT_USB[] = {
"ZZFI00;", "ZZFI01;", "ZZFI02;", "ZZFI03;", "ZZFI04;",
"ZZFI05;", "ZZFI06;", "ZZFI07;", "ZZFI08;", "ZZFI09;",
"ZZFI10;", "ZZFI11;" };
//static const char *PowerSDR_SH_tooltip = "hi cut";
//static const char *PowerSDR_SSB_btn_SH_label = "H";
//------------------------------------------------------------------------
static std::vector<std::string>PowerSDR_WIDEwidths;
static const char *vPowerSDR_WIDEwidths[] = {
"Wideband" };
//static std::vector<std::string>PowerSDR_CAT_WIDE = {
//"ZZFI12;" };
//------------------------------------------------------------------------
static std::vector<std::string>PowerSDR_DIGwidths;
static const char *vPowerSDR_DIGwidths[] = {
" 3000", " 2500", " 2000", " 1500", " 1000",
"  800", "  600", "  300", "  150", "   75",
" Var1", " Var2" };
static std::vector<std::string>PowerSDR_CAT_DIG;
static const char *vPowerSDR_CAT_DIG[] = {
"ZZFI00;", "ZZFI01;", "ZZFI02;", "ZZFI03;", "ZZFI04;",
"ZZFI05;", "ZZFI06;", "ZZFI07;", "ZZFI08;", "ZZFI09;",
"ZZFI10;", "ZZFI11;" };
//------------------------------------------------------------------------------
static std::vector<std::string>PowerSDR_AMwidths;
static const char *vPowerSDR_AMwidths[] = {
"16000", "12000", "10000", " 8000", " 6600",
" 5200", " 4000", " 3100", " 2900", " 2400",
"Var1", "Var2" };
static std::vector<std::string>PowerSDR_CAT_AM;
static const char *vPowerSDR_CAT_AM[] = {
"ZZFI00;", "ZZFI01;", "ZZFI02;", "ZZFI03;", "ZZFI04;",
"ZZFI05;", "ZZFI06;", "ZZFI07;", "ZZFI08;", "ZZFI09;",
"ZZFI10;", "ZZFI11;" };
//------------------------------------------------------------------------------
static std::vector<std::string>PowerSDR_CWwidths;
static const char *vPowerSDR_CWwidths[] = {
" 1000", "  800", "  750", "  600", "  500",
"  400", "  250", "  100", "   50", "   25",
" Var1", " Var2"};
static std::vector<std::string>PowerSDR_CAT_CW;
static const char *vPowerSDR_CAT_CW[] = {
"ZZFI00;", "ZZFI01;", "ZZFI02;", "ZZFI03;", "ZZFI04;",
"ZZFI05;", "ZZFI06;", "ZZFI07;", "ZZFI08;", "ZZFI09;",
"ZZFI10;", "ZZFI11;" };
//------------------------------------------------------------------------------

static std::vector<std::string>varwidths;
static const char *vvarwidths[] = {
" 5000", " 4400", " 3800", " 3300", " 2900",
" 2700", " 2400", " 2100", " 1800", " 1000",
"  500", "  200" };

//------------------------------------------------------------------------------
static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 }, // 0
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 }, // 1
	{ (Fl_Widget *)btnAGC,        2, 145,  50 }, // 2
	{ (Fl_Widget *)sldrRFGAIN,   54, 145, 156 }, // 3
	{ (Fl_Widget *)btnIFsh,     214, 105,  50 }, // 4
	{ (Fl_Widget *)sldrIFSHIFT, 266, 105, 156 }, // 5
	{ (Fl_Widget *)btnNotch,    214, 125,  50 }, // 6
	{ (Fl_Widget *)sldrNOTCH,   266, 125, 156 }, // 7
	{ (Fl_Widget *)sldrSQUELCH, 266, 145, 156 }, // 8
	{ (Fl_Widget *)sldrMICGAIN, 266, 165, 156 }, // 9
	{ (Fl_Widget *)sldrPOWER,    54, 165, 368 }, // 10
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};

// mid range on loudness
static std::string menu012 = "EX01200004";

void RIG_PowerSDR::initialize()
{
	VECTOR (PowerSDRmodes_, vPowerSDRmodes_);
	VECTOR (PowerSDR_mode_chr, vPowerSDR_mode_chr);
	VECTOR (PowerSDR_empty, vPowerSDR_empty);
	VECTOR (PowerSDR_USBwidths, vPowerSDR_USBwidths);
	VECTOR (PowerSDR_CAT_USB, vPowerSDR_CAT_USB);
	VECTOR (PowerSDR_WIDEwidths, vPowerSDR_WIDEwidths);
	VECTOR (PowerSDR_DIGwidths, vPowerSDR_DIGwidths);
	VECTOR (PowerSDR_CAT_DIG, vPowerSDR_CAT_DIG);
	VECTOR (PowerSDR_AMwidths, vPowerSDR_AMwidths);
	VECTOR (PowerSDR_CAT_AM, vPowerSDR_CAT_AM);
	VECTOR (PowerSDR_CWwidths, vPowerSDR_CWwidths);
	VECTOR (PowerSDR_CAT_CW, vPowerSDR_CAT_CW);
	VECTOR (varwidths, vvarwidths);

	modes_ = PowerSDRmodes_;
	bandwidths_ = PowerSDR_empty;

	std::stringstream str;
	str << "PowerSDR";
	trace(2, __func__, str.str().c_str());

	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrRFGAIN;
	rig_widgets[3].W = btnIFsh;
	rig_widgets[4].W = sldrIFSHIFT;
	rig_widgets[5].W = btnNotch;
	rig_widgets[6].W = sldrNOTCH;
	rig_widgets[7].W = sldrSQUELCH;
	rig_widgets[8].W = sldrMICGAIN;
	rig_widgets[9].W = sldrPOWER;

//	varwidths.clear();
//	for (int i = 0; i < 12; i++) {
//		varwidths.push_back(PowerSDR_USBwidths.at(i));
//	}

// get current noise reduction values for NR1 and NR2
	std::string current_nr;
	cmd = "ZZNR;";
	wait_char(';', 6, 100, "read current NR", ASC);
	gett("get ZZNR");
	size_t p = replystr.rfind("RL");
	if (p != std::string::npos)
		_nrval1 = atoi(&replystr[p+2]);
	cmd = "ZZNS;";
	wait_char(';', 6, 100, "read current NR", ASC);
	int nrval2=0;
	if (p != std::string::npos)
		nrval2 = atoi(&replystr[p+2]);
	if (nrval2 == 1) _nrval1 = 2;
}

void RIG_PowerSDR::shutdown()
{
}

static bool is_tuning = false;

RIG_PowerSDR::RIG_PowerSDR() {
// base class values
	name_ = PowerSDRname_;
	modes_ = PowerSDRmodes_;
	bandwidths_ = PowerSDR_empty;

	//dsp_SL     = PowerSDR_SL;
	//SL_tooltip = PowerSDR_SL_tooltip;
	//SL_label   = PowerSDR_SSB_btn_SL_label;

	//dsp_SH     = PowerSDR_SH;
	//SH_tooltip = PowerSDR_SH_tooltip;
	//SH_label   = PowerSDR_SSB_btn_SH_label;

	widgets = rig_widgets;

	serial_baudrate = BR4800;
	stopbits = 2;
	serial_retries = 2;

//	serial_write_delay = 0;
//	serial_post_write_delay = 0;

	serial_timeout = 50;
	serial_rtscts = true;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_catptt = true;
	serial_rtsptt = false;
	serial_dtrptt = false;
	B.imode = A.imode = 1;
	B.iBW = A.iBW = 0x8803;
	B.freq = A.freq = 14070000ULL;
	can_change_alt_vfo = true;

	//has_dsp_controls = true;
    has_voltmeter = true;
	has_power_out = true;
	has_swr_control = true;
	has_alc_control =
	has_idd_control =
	has_split =
	has_split_AB =
	has_rf_control =
	has_auto_notch =
	has_ifshift_control =
	has_smeter =
	has_noise_reduction = true;
	has_noise_reduction_control =
	has_noise_control =
	has_micgain_control =
	has_volume_control =
	has_power_control =
	has_tune_control =
//	has_preamp_control =
	has_mode_control =
	has_bandwidth_control =
	has_sql_control =
	has_ptt_control =
    has_agc_control =
	has_extras = true;

	rxona = true;

	precision = 1;
	ndigits = 9;

	att_level = 0;
//	preamp_level = 0;
	_noise_reduction_level = 0;
	_nrval1 = 2;
	_nrval2 = 4;

	is_tuning = false;
}

static int ret = 0;

const char * RIG_PowerSDR::get_bwname_(int n, int md)
{
	std::stringstream str;
	str << "n=" << n << ", md=" << md;
	trace(2, __func__, str.str().c_str());
	try {
		if (md == USB || md == LSB) 
			return PowerSDR_USBwidths.at(n).c_str();
		if (md == FM || md == DRM || md == SPEC) {
			return PowerSDR_WIDEwidths.at(n).c_str();
		if (md == DIGU || md == DIGL)
			return PowerSDR_DIGwidths.at(n).c_str();
		if (md == CWU || md == CWL)
			return PowerSDR_CWwidths.at(n).c_str();
		else
			return PowerSDR_AMwidths.at(n).c_str();
		}
	} catch (const std::exception& e) {
		std::cout << e.what() << '\n';
	}
	return "UNKNOWN";
}

int RIG_PowerSDR::get_smeter()
{
	int smtr = 0;
	if (rxona)
		cmd = "ZZSM0;";
	else
		cmd = "ZZSM1;";
	get_trace(1, "get_smeter");
	ret = wait_char(';', 9, 100, "get smeter", ASC);
	gett("");
	if (ret == 9) {
		size_t p = replystr.rfind("SM");
		if (p != std::string::npos) {
			smtr = fm_decimal(replystr.substr(p+3),3);
			smtr = -54 + smtr/(256.0/100.0); // in S-Units
			smtr = (smtr + 54);
		}
	}
	return smtr;
}

// Transceiver power level
void RIG_PowerSDR::set_power_control(double val)
{
	int ival = (int)val;
	cmd = "PC";
	cmd.append(to_decimal(ival, 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set pwr ctrl", cmd, "");
	sett("pwr control");
}

int RIG_PowerSDR::get_power_out()
{
	int mtr = 0;

	cmd = "ZZRM5;";
	get_trace(1, "get_power_out");
	ret = wait_char(';', 11, 100, "get power", ASC);
	gett("");
	if (ret < 9) return 0;

	sscanf(&replystr[0],"ZZRM5%d", &mtr);

	return mtr;
}


double RIG_PowerSDR::get_power_control()
{
	int pctrl = 0;
	cmd = "PC;";
	get_trace(1, "get_power_control");
	ret = wait_char(';', 6, 100, "get pout", ASC);
	gett("");
	if (ret == 6) {
		size_t p = replystr.rfind("PC");
		if (p != std::string::npos) {
			pctrl = fm_decimal(replystr.substr(p+2), 3);
		}
	}
	return pctrl;
}

struct meterpair {float mtr; float val;};

// Table entries below correspond to SWR readings of 1.1, 1.5, 2.0, 2.5, 3.0 and infinity.
// Values are also tweaked to fit the display of the SWR meter.


static meterpair swr_tbl[] = {
    {1.0, 0},
    {1.5, 10.5 },
    {2.0, 23.0 },
    {2.5, 35.0 },
    {3.0, 48.0 },
    {10.0, 100.0 } 
};

int RIG_PowerSDR::get_swr()
{
	double mtr = 0;

	cmd = "ZZRM8;";
	get_trace(1, "get_swr");
	ret = wait_char(';', 13, 100, "get SWR", ASC);
	gett("");
    // Response is ZZRM86.1 : 1; 
    // What's the 2nd part?
	if(sscanf(&replystr[0], "ZZRM8%lf", &mtr)!=1)
	{
		return 3;
	}
    size_t i = 0;
    for (i = 0; i < sizeof(swr_tbl) / sizeof(meterpair) - 1; i++)
            if (mtr >= swr_tbl[i].mtr && mtr < swr_tbl[i+1].mtr)
                    break;
    if (mtr > 19) mtr = 19;
    mtr = (int)round(swr_tbl[i].val +
            (swr_tbl[i+1].val - swr_tbl[i].val)*(mtr - swr_tbl[i].mtr)/(swr_tbl[i+1].mtr - swr_tbl[i].mtr));
    if (mtr > 100) mtr = 100;

	std::stringstream str;
	str << "swr mtr=" << mtr;
	trace(2, __func__, str.str().c_str());
	return mtr;
}

int RIG_PowerSDR::get_alc()
{
	double alc = 0;
	cmd = "ZZRM4;";
	get_trace(1, "get_alc");
	ret = wait_char(';', 12, 100, "get ALC", ASC);
    gett("alc");
	showresp(WARN, ASC, "get alc", cmd, replystr);
	if (sscanf(&replystr[0], "ZZRM4%lf", &alc) != 1) alc=0;
    alc=fabs(alc)*2;
    if (alc > 50) alc = 50;
	return alc;
}

#if 0
/* disabling this as Thetis ZZPA command is screwy and won't work on all the models
Using the selector
0dB = ZZPA1
-10dB = ZZPA2 = -140
-20dB = ZZPA0 = -130
-30dB = ZZPA8 = -120 yet ZZPA8; does nothing

If I look at the signal level for what the level is now.
ZZPA0 = -130
ZZPA1 = -150
ZZPA2 = -140 -- but ZZPA then reports ZZPA7;
ZZPA3 = -130
ZZPA4 = -120
ZZPA5 = -110
ZZPA6 = -100
ZZPA7 does nothing
*/

int  RIG_PowerSDR::next_preamp()
{
	// strange sequence for ANAN 7000DLE MKII
	switch(preamp_level)
	{
		case 0: preamp_level = 1;break;
		case 1: preamp_level = 2;break;
		case 7: preamp_level = 3;break;
		case 3: preamp_level = 4;break;
		case 4: preamp_level = 5;break;
		case 5: preamp_level = 6;break;
		default:
		case 6: preamp_level = 0;break;
	}
	return preamp_level;
}

void RIG_PowerSDR::set_preamp(int val)
{
	preamp_level = val;
	cmd = "ZZPA";
	cmd.append(to_decimal(val, 1)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set PRE", cmd, "");
	sett("preamp");
		case 7: preamp_level = 0;break;
	if (val == 0) {
		preamp_level = 0;
	} else if (val == 1) {
		preamp_level = 1;
	} else if (val == 7) {
		preamp_level = 2;
	}

}

const char *RIG_PowerSDR::PRE_label()
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

int RIG_PowerSDR::get_preamp()
{
	int preamp_level;
	cmd = "ZZPA;";
	std::stringstream str;
	sendCommand(cmd);
	get_trace(1, "get_preamp");
	ret = wait_char(';', 6, 100, "get PRE", ASC);
	gett("");
	if (ret == 6) {
		size_t p = replystr.rfind("PA");
		str << "ZZPA #2 replystr=" << replystr << ", p=" << p;
		trace(2, "get_preamp", replystr.c_str());
		if (p != std::string::npos && (p+2 < replystr.length())) {
			preamp_level = fm_decimal(replystr.substr(p+2),1);
			// need to map 7 to 2 to keep FLRig cbPreamp happy
			// when ZZPA2; is sent ZZPA7; comes back
			if (preamp_level == 7) preamp_level = 2;
		}
	}
	else preamp_level = 0;
	return preamp_level;
}
#endif

int RIG_PowerSDR::set_widths(int val)
{
	int bw = get_bwA();

	cmd = "ZZMN";
	cmd += PowerSDR_mode_chr[val];
	cmd += ';';

	set_trace(1, "bandwidths:");
	wait_char(';', 187, 1000, "bandwidths", 187);
	sett("");

	bool fill_widths = false;
	switch (val) {
	case LSB: case USB:
		bandwidths_ = PowerSDR_USBwidths;
		dsp_SL = PowerSDR_empty;
		dsp_SH = PowerSDR_empty;
		fill_widths = true;
		break;
	case DIGU: case DIGL:
		bandwidths_ = PowerSDR_DIGwidths;
		dsp_SL = PowerSDR_empty;
		dsp_SH = PowerSDR_empty;
		fill_widths = true;
		break;
	case FM: case SPEC: case DRM:
		bandwidths_ = PowerSDR_WIDEwidths;
		dsp_SL = PowerSDR_empty;
		dsp_SH = PowerSDR_empty;
		break;
	case CWL: case CWU:
		bandwidths_ = PowerSDR_CWwidths;
		dsp_SL = PowerSDR_empty;
		dsp_SH = PowerSDR_empty;
		fill_widths = true;
		break;
	case AM: case SAM: case DSB: default:
		bandwidths_ = PowerSDR_AMwidths;
		dsp_SL = PowerSDR_empty;
		dsp_SH = PowerSDR_empty;
		fill_widths = true;
		break;
	}

	if (!fill_widths)
		return bw;

	size_t p = replystr.rfind("ZZMN");

	if (p == std::string::npos)
		return bw;

	if (replystr.length() < 187)
		return bw;
	varwidths.clear();
	static std::string widths;
	widths = replystr.substr(p + 6);

	for (int i = 0; i < 12; i++) {
		widths[i*15 + 6] = 0;
		varwidths.push_back(&widths[i*15]);
	}
	bandwidths_ = varwidths;

	return bw;
}

std::vector<std::string>& RIG_PowerSDR::bwtable(int val)
{
	return bandwidths_;
}

#if 0
std::vector<std::string>& RIG_PowerSDR::lotable(int val)
{
	if (val == LSB || val == USB || val == FM || val == DRM || val == SPEC)
		return PowerSDR_USBwidths;
	else if (val == DIGU || val == DIGL)
		return PowerSDR_DIGwidths;
	else if (val == AM || val == SAM || val == DSB)
		return PowerSDR_AMwidths;
	return PowerSDR_USBwidths;
}

std::vector<std::string>& RIG_PowerSDR::hitable(int val)
{
	if (val == LSB || val == USB || val == FM || val == DRM || val == SPEC)
		return PowerSDR_USBwidths;
	else if (val == DIGU || val == DIGL)
		return PowerSDR_DIGwidths;
	else if (val == CWU || val == CWL)
		return PowerSDR_CWwidths;
	else if (val == AM || val == SAM || val == DSB)
		return PowerSDR_AMwidths;
	return PowerSDR_USBwidths;
}
#endif

void RIG_PowerSDR::set_modeA(int val)
{
	try  {
		_currmode = A.imode = val;
		cmd = "ZZMD";
		cmd += PowerSDR_mode_chr.at(val);
		cmd += ';';
		sendCommand(cmd);
		showresp(WARN, ASC, "set mode", cmd, "");
		sett("modeA");
		A.iBW = set_widths(val);
		vfoA.iBW = A.iBW;
	} catch (const std::exception& e) {
		std::cout << e.what() << '\n';
	}
}

int RIG_PowerSDR::get_modeA()
{
	if (tuning()) return A.imode;
	cmd = "ZZMD;";
	get_trace(1, "get_modeA");
	ret = wait_char(';', 7, 100, "get mode A", 7);
	gett("");
	if (ret == 7) {
		size_t p = replystr.rfind("MD");
		if (p != std::string::npos) {
			int md;
			sscanf(&replystr[p+2],"%d",&md);
			if (A.imode != md) {
				A.imode = md;
				A.iBW = set_widths(A.imode);
			}
		}
	}
	_currmode = A.imode;
	return A.imode;
}


void RIG_PowerSDR::set_modeB(int val)
{
	try {
		_currmode = B.imode = val;
		cmd = "MD";
		cmd += PowerSDR_mode_chr.at(val);
		cmd += ';';
		sendCommand(cmd);
		showresp(WARN, ASC, "set mode B", cmd, "");
		sett("modeB");
		MilliSleep(100); // give rig a chance to change width
		B.iBW = set_widths(val);
		vfoB.iBW = B.iBW;
	} catch (const std::exception& e) {
		std::cout << e.what() << '\n';
	}
}

int RIG_PowerSDR::get_modeB()
{
	if (tuning()) return B.imode;
	cmd = "ZZMD;";
	get_trace(1, "get_modeB");
	ret = wait_char(';', 7, 100, "get mode B", ASC);
	gett("");
	if (ret == 7) {
		size_t p = replystr.rfind("MD");
		if (p != std::string::npos) {
			int md=0;
			sscanf(&replystr[p+2],"%d",&md);
			if (B.imode != md) {
				B.imode = md;
				B.iBW = set_widths(B.imode);
			}
		}
	}
	_currmode = B.imode;
	return B.imode;
}

int RIG_PowerSDR::adjust_bandwidth(int val)
{
	int bw = 0;
	bw = 0;
	if (val == CWU || val == CWL)
		bw = 7;
	return bw;
}

int RIG_PowerSDR::def_bandwidth(int val)
{
	return adjust_bandwidth(val);
}

void RIG_PowerSDR::set_bwA(int val)
{
	try {
		if (A.imode == FM || A.imode == DRM || A.imode == SPEC) return; // mode is fixed
		else if (A.imode == LSB || A.imode == USB) {
			cmd = PowerSDR_CAT_USB.at(val);
			sendCommand(cmd);
			showresp(WARN, ASC, "set_bwA USB", cmd, "");
			sett("bwA USB");
			A.iBW = val;
		}
		else if (A.imode == DIGU || A.imode == DIGL) {
			cmd = PowerSDR_CAT_DIG.at(val);
			sendCommand(cmd);
			showresp(WARN, ASC, "set_bwA DIG", cmd, "");
			sett("bwA DIG");
			A.iBW = val;
		}
		else if (A.imode == CWU || A.imode == CWL) {
			cmd = PowerSDR_CAT_CW.at(val);
			sendCommand(cmd);
			showresp(WARN, ASC, "set_bwA CW", cmd, "");
			sett("bwA CW");
			A.iBW = val;
		}
		else if (A.imode == AM || A.imode == SAM || A.imode == DSB) {
			cmd = PowerSDR_CAT_AM.at(val);
			sendCommand(cmd);
			showresp(WARN, ASC, "set_bwA AM", cmd, "");
			sett("bwA AM");
			A.iBW = val;
		}
	} catch (const std::exception& e) {
		std::cout << e.what() << '\n';
	}
}

int RIG_PowerSDR::get_bwA()
{
	size_t i = 0;
	size_t p;
	std::stringstream str;
	str << "get_bwA" ;
	A.iBW = 0;
	trace(2, __func__, str.str().c_str());
	if (A.imode == FM || A.imode == DRM || A.imode == SPEC) {
		A.iBW = 0;
		gett("get_bwA Wideband");
	}
	else if (A.imode == LSB || A.imode == USB) {
		cmd = "ZZFI;";
		get_trace(1, "get_bw SSB");
		ret = wait_char(';', 7, 100, "get ZZFI", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("ZZFI");
			if (p != std::string::npos) {
				for (i = 0; i < PowerSDR_CAT_USB.size(); i++) {
					if (replystr.find(PowerSDR_CAT_USB[i]) == p) {
						A.iBW = i;
						break;
					}
				}
			}
		}
	}
	else if (A.imode == CWL || A.imode == CWU) {
		cmd = "ZZFI;";
		get_trace(1, "get_bw CW");
		ret = wait_char(';', 7, 100, "get ZZFI", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("ZZFI");
			if (p != std::string::npos) {
				for (i = 0; i < PowerSDR_CAT_CW.size(); i++) {
					if (replystr.find(PowerSDR_CAT_CW[i]) == p) {
						A.iBW = i;
						break;
					}
				}
			}
		}
	}
	else if (A.imode == DIGU || A.imode == DIGL) {
		cmd = "ZZFI;";
		get_trace(1, "get_bw DIGI");
		ret = wait_char(';', 7, 100, "get ZZFI", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("ZZFI");
			if (p != std::string::npos) {
				for (i = 0; i < PowerSDR_CAT_DIG.size(); i++) {
					if (replystr.find(PowerSDR_CAT_DIG[i]) == p) {
						A.iBW = i;
						break;
					}
				}
			}
		}
	}
	else if (A.imode == AM || A.imode == SAM || A.imode == DSB) {
		cmd = "ZZFI;";
		get_trace(1, "get_bw AM");
		ret = wait_char(';', 7, 100, "get ZZFI", ASC);
		gett("");
		if (ret == 7) {
			p = replystr.rfind("ZZFI");
			if (p != std::string::npos) {
				for (i = 0; i < PowerSDR_CAT_AM.size(); i++) {
					if (replystr.find(PowerSDR_CAT_AM[i]) == p) {
						A.iBW = i;
						break;
					}
				}
			}
		}
	}
	vfoA.iBW = A.iBW;
	progStatus.iBW_A = A.iBW;
	return A.iBW;
}

void RIG_PowerSDR::set_bwB(int val)
{
	set_bwA(val);
	B.iBW = val;
	std::stringstream str;
	str << "B.iBW = " << B.iBW;
	trace(2, __func__, str.str().c_str());
	return;
}

int RIG_PowerSDR::get_bwB() // same as A
{
	B.iBW =  get_bwA();
	std::stringstream str;
	str << "B.iBW = " << B.iBW;
	trace(2, __func__, str.str().c_str());
	vfoB.iBW = B.iBW;
	progStatus.iBW_B = B.iBW;
	return B.iBW;
}

int RIG_PowerSDR::get_modetype(int n)
{
	if (n >= (int)(sizeof(PowerSDR_mode_type)/sizeof(*PowerSDR_mode_type))) return 0;
	return PowerSDR_mode_type[n];
}

void RIG_PowerSDR::get_if_min_max_step(int &min, int &max, int &step)
{
	if_shift_min = min = 400;
	if_shift_max = max = 1000;
	if_shift_step = step = 50;
	if_shift_mid = 700;
}

void RIG_PowerSDR::set_notch(bool on, int val)
{
	if (on) {
		cmd = "ZZBC1;";
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch on", cmd, "");
		sett("notch ON");
	} else {
		cmd = "ZZBC0;"; // no notch action
		sendCommand(cmd);
		showresp(WARN, ASC, "set notch off", cmd, "");
		sett("notch OFF");
	}
}

bool  RIG_PowerSDR::get_notch(int &val)
{
	bool ison = false;
	cmd = "BC;";
	get_trace(1, "get_notch");
	ret = wait_char(';', 4, 100, "get notch on/off", ASC);
	gett("");
	if (ret == 4) {
		size_t p = replystr.rfind("BC");
		if (p != std::string::npos) {
			if (replystr[p+2] == '2') {
				ison = true;
				cmd = "BP;";
				get_trace(1, "get_notch_val");
				ret = wait_char(';', 6, 100, "get notch val", ASC);
				gett("");
				if (ret == 6) {
					p = replystr.rfind("BP");
					if (p != std::string::npos)
						val = 200 + 50 * fm_decimal(replystr.substr(p+2),3);
				}
			}
		}
	}
	return (ison);
}

void RIG_PowerSDR::get_notch_min_max_step(int &min, int &max, int &step)
{
	min = 200;
	max = 3350;
	step = 50;
}

void RIG_PowerSDR::set_auto_notch(int v)
{
	cmd = v ? "NT1;" : "NT0;";
	sendCommand(cmd);
	showresp(WARN, ASC, "set auto notch", cmd, "");
	sett("auto notch");
}

int  RIG_PowerSDR::get_auto_notch()
{
	int anotch = 0;
	cmd = "NT;";
	get_trace(1, "get_auto_notch");
	ret = wait_char(';', 4, 100, "get auto notch", ASC);
	gett("");
	if (ret == 4) {
		size_t p = replystr.rfind("NT");
		if (p != std::string::npos) {
			anotch = (replystr[p+2] == '1');
		}
	}
	return anotch;
}

void RIG_PowerSDR::set_noise(bool on)
{
    if (on)
       cmd = "ZZNA1;";
    else
       cmd = "ZZNA0;";
	sendCommand (cmd);
}

int RIG_PowerSDR::get_noise()
{
    cmd = "ZZNA;";
    int ret = wait_char(';', 6, 100, "get Noise Blanker", ASC);
    if (ret < 6) return progStatus.noise;
    size_t p = replystr.rfind("NA");
    if (p == std::string::npos) return progStatus.noise;
    return (replystr[p+2] == '1' ? 1 : 0);
}

void RIG_PowerSDR::set_noise_reduction(int val)
{
	if (val == -1) {
		return;
	}
	_noise_reduction_level = val;
	if (_noise_reduction_level == 0) {
		nr_label("ZZNR0", 0);
	} else if (_noise_reduction_level == 1) {
		nr_label("ZZNR1", 1);
	} else {
		nr_label("???", 2);
		return;
	}
	cmd.assign("ZZNR");
	cmd += '0' + _noise_reduction_level;
	cmd += ';';
	sendCommand (cmd);
	showresp(WARN, ASC, "SET noise reduction", cmd, "");
	sett("noise reduction");
}

int  RIG_PowerSDR::get_noise_reduction()
{
	cmd = rsp = "ZZNR";
	cmd.append(";");
	get_trace(1, "get_noise_reduction");
	ret = wait_char(';', 6, 100, "GET noise reduction", ASC);
	gett("");
	if (ret == 6) {
		size_t p = replystr.rfind(rsp);
		if (p == std::string::npos) return _noise_reduction_level;
		_noise_reduction_level = replystr[p+4] - '0';
	}

	if (_noise_reduction_level == 1) {
		nr_label("NR", 1);
	} else if (_noise_reduction_level == 2) {
		nr_label("NR2", 2);
	} else {
		nr_label("NR", 0);
	}
	return _noise_reduction_level;
}

void RIG_PowerSDR::set_noise_reduction_val(int val)
{
	if (_noise_reduction_level == 0) return;
	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	cmd.assign("NR").append(to_decimal(val, 2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "SET_noise_reduction_val", cmd, "");
	sett("noise reduction val");
}

int  RIG_PowerSDR::get_noise_reduction_val()
{
	int nrval = 0;
	if (_noise_reduction_level == 0) return 0;
	int val = progStatus.noise_reduction_val;
	cmd = rsp = "ZZNR";
	cmd.append(";");
	get_trace(1, "get_noise_reduction_val");
	ret = wait_char(';', 5, 100, "GET noise reduction val", ASC);
	gett("");
	if (ret == 5) {
		size_t p = replystr.rfind(rsp);
		if (p == std::string::npos) {
			nrval = (_noise_reduction_level == 1 ? _nrval1 : _nrval2);
			return nrval;
		}
		val = atoi(&replystr[p+2]);
	}
	if (_noise_reduction_level == 1) _nrval1 = val;
	else _nrval2 = val;

	return val;
}

void RIG_PowerSDR::tune_rig(int val)
{

	std::stringstream str;
	str << "val=" << val;
	trace(2, __func__, str.str().c_str());
	switch (val) {
		case 0:
			cmd = "ZZTU0;";
			break;
		case 1:
		case 2:
			cmd = "ZZTU1;";
			break;
	}
	sendCommand(cmd);
	showresp(WARN, ASC, "tune rig", cmd, replystr);
	sett("tune_rig");
}

int RIG_PowerSDR::get_tune()
{
	cmd = rsp = "ZZTU";
	cmd += ';';
	get_trace(1, "get_tune");
	ret = wait_char(';', 6, 100, "get tune", ASC);
	gett("");

	size_t p = replystr.rfind(rsp);
	if (p == std::string::npos) return 0;
	int val = replystr[p+4] - '0';
	return (tune_ = val);
}

void RIG_PowerSDR::set_rf_gain(int val)
{
	cmd = "ZZAR";
	if (val < 0) cmd += '-';
	else cmd += '+';
	cmd.append(to_decimal(abs(val), 3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set rf gain", cmd, "");
	sett("RFgain");
}

int  RIG_PowerSDR::get_rf_gain()
{
	cmd = "ZZAR;";
	int rfg = 120;
	char sign;
	get_trace(1, "get_rf_gain");
	ret = wait_char(';', 9, 100, "get rf gain", ASC);
	gett("");
	if (ret == 9) {
		size_t p = replystr.rfind("ZZAR");
		if (p != std::string::npos) {
			sscanf(replystr.c_str(),"ZZAR%c%d", &sign, &rfg);
			if (sign == '-') rfg *= -1;
		}
	}
	return rfg;
}

void RIG_PowerSDR::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = -20;
	max = 120;
	step = 1;
}

void RIG_PowerSDR::set_mic_gain(int val)
{
	cmd = "ZZMG";
	if (val < 0) cmd += "-";
	else cmd += "+";
	cmd.append(to_decimal(abs(val),2)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic", cmd, "");
	sett("MICgain");
}

int RIG_PowerSDR::get_mic_gain()
{
	int mgain = 0;
	cmd = "ZZMG;";
	get_trace(1, "get_mic_gain");
	ret = wait_char(';', 8, 100, "get mic", ASC);
	gett("");
	if (ret == 8) {
		size_t p = replystr.rfind("MG");
		if (p != std::string::npos) {
			sscanf(replystr.c_str(),"ZZMG%d",&mgain);
		}
	}
	return mgain;
}

void RIG_PowerSDR::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = -40;
	max = 10;
	step = 1;
}


// Transceiver PTT on/off
void RIG_PowerSDR::set_PTT_control(int val)
{
	if (val) sendCommand("ZZTX1;");
	else     sendCommand("ZZTX0;");
	ptt_ = val;
}

int RIG_PowerSDR::get_PTT()
{
	cmd = "ZZTX;";
	get_trace(1, "get_PTT");
	ret = wait_char(';', 6, 100, "get PTT", ASC);
	gett("");
	if (ret < 6) return ptt_;
	ptt_ = (replystr[4] == '1');
	return ptt_;
}

bool RIG_PowerSDR::tuning()
{
	cmd = "ZZTU;";
	if (wait_char(';', 6, 100, "tuning?", ASC) == 6) {
		if (replystr[4] == '1') return true;
	}
	return false;
}

void RIG_PowerSDR::set_split(bool val)
{
	if (val) {
		cmd = "ZZSP1;";
	} else {
		cmd = "ZZSP0;";
	}
	sendCommand(cmd);
}

int RIG_PowerSDR::get_split()
{
	cmd = "ZZSP;";
	get_trace(1, "get_split");
	ret = wait_char(';', 6, 100, "get split", ASC);
	gett("");
	if (ret < 6) return split;
	split = (replystr[4] == '1');
	return split;
}

void RIG_PowerSDR::set_squelch(int val)
{
	cmd = "ZZSQ";
	cmd.append(to_decimal(abs(val),3)).append(";");
	sendCommand(cmd);
	showresp(WARN, ASC, "set mic", cmd, "");
	sett("Squelch");
}

int  RIG_PowerSDR::get_squelch()
{
	int sq = 0;
	cmd = "ZZSQ;";
	get_trace(1, "get_squelch");
	ret = wait_char(';', 8, 100, "get squelch", ASC);
	gett("");
	if (ret == 8) {
		size_t p = replystr.rfind("ZZSQ");
		if (p != std::string::npos) {
			sscanf(replystr.c_str(),"ZZSQ%d",&sq);
		}
	}
	return (-sq);
}


void RIG_PowerSDR::get_squelch_min_max_step(int &min, int &max, int &step)
{
	min = -160;
	max = 0;
	step = 4;
}

static int agcval = 1;

int  RIG_PowerSDR::get_agc()
{
	cmd = "GT;";
	get_trace(1, "get_agc");
	ret = wait_char(';', 6, 100, "GET agc val", ASC);
	gett("");
	size_t p = replystr.rfind("GT");
	if (p == std::string::npos) return agcval;
	if (replystr[4] == ' ') return 0;
	agcval = replystr[4] - '0' + 1; // '0' == off, '1' = fast, '2' = slow
	return agcval;
}

int RIG_PowerSDR::incr_agc()
{
	agcval++;
	if (agcval > 6) agcval = 1;
	cmd.assign("GT00");
	cmd += (agcval + '0' - 1);
	cmd += ";";
	sendCommand(cmd);
	showresp(WARN, ASC, "SET agc", cmd, replystr);
	return agcval;
}


static const char *agcstrs[] = {"AGC", "FIX", "LNG", "SLW", "MED", "FST", "CST"};

const char *RIG_PowerSDR::agc_label()
{
	return agcstrs[agcval];
}

int RIG_PowerSDR::agc_val()
{
	return agcval;
}

double RIG_PowerSDR::get_voltmeter()
{
    static int timedout = 0;
    double val;
    trace(1, "get_voltmeter");
    ret = 0;
    if (!timedout)
    {
        cmd.assign("ZZRV;");
        sendCommand(cmd);
    	get_trace(1, "get_voltmeter");
    	ret = wait_char(';', 8, 100, "GET agc val", ASC);
    }
    if (ret == 8)
    {
        sscanf(&replystr[0],"ZZRV%lf", &val);
    }
    else
    {
        timedout = 1;
        val = 0;
        replystr = "No Reply";
        mtr_VOLTS->labelcolor(fl_rgb_color(200,200,200));
        mtr_VOLTS->redraw();
    }
	showresp(WARN, ASC, "get voltmeter", cmd, replystr);
    return val;
}

double RIG_PowerSDR::get_idd()
{
    static int timedout = 0;
    double val;
    trace(1, "get_idd");
	std::stringstream str;
	str << "get_idd#2";
	trace(2, __func__, str.str().c_str());
    ret = 0;
    if (!timedout)
    {
        cmd.assign("ZZRV;");
        sendCommand(cmd);
	    get_trace(1, "get_voltmeter");
	    ret = wait_char(';', 8, 100, "GET agc val", ASC);
        gett("idd");
    }
    if (ret == 8)
    {
        sscanf(&replystr[0],"ZZRV%lf", &val);
    }
    else
    {
        timedout = 1;
        val = 0;
        mtr_IDD->labelcolor(fl_rgb_color(200,200,200));
        mtr_IDD->redraw();
    }
	showresp(WARN, ASC, "get idd", cmd, replystr);
    return val;
}
