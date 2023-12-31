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

#include "other/tcisdr.h"
#include "support.h"

static const char TCI_SUNDX_name_[] = "SunSDR2-DX/TCI";
static const char TCI_SUNPRO_name_[] = "SunSDR2-Pro/TCI";

static std::vector<std::string>TCI_modes;
static const char *vTCI_modes[] = {
"AM", "SAM", "DSB", "LSB", "USB", "CW", "NFM", "DIGL", "DIGU", "WFM", "DRM" };

static const char TCI_mode_type[] = {
'U', 'U', 'U', 'L', 'U', 'U', 'U', 'L', 'U', 'U', 'U' };

// pairs set up for a nominal 700 Hz CW note
// default is 800 Hz bandwidth
static std::vector<std::string>TCI_CWwidths;
static const char *vTCI_CWwidths[] = {
 "50", "100",  "250",  "400", "500",
"650", "800", "1000", "1500", "2000",
"3000", "4000"};

static std::string TCI_CWpairs[] = {
  "-25,25;",   "-50,50;", "-125,125;", "-200,200;", "-250,250;",
"-325,325;", "-400,400;", "-500,500;", " -600,900;", "-600,1400;",
"-600,2400;", "-600, 3400;" };
static int tci_nbr_cw = 12;
static int tci_def_cw = 6;
static int tci_cw_tone = 0;

static std::vector<std::string>TCI_AMwidths;
static const char *vTCI_AMwidths[] = {
"3000", "3500", "4000", "5000", "6000",
"8000", "10000", "12000", "16000" };

static std::string TCI_AMpairs[] = {
"-1500,1500;", "-1750,1750;", "-2000,2000;", "-2500,2500;", "-3000,3000;",
"-4000,4000;", "-5000,5000;", "-6000,6000;", "-8000,8000;", "" };
static int tci_nbr_am = 9;
static int tci_def_am = 4;

static std::vector<std::string>TCI_DSBwidths;
static const char *vTCI_DSBwidths[] = {
"3000", "3500", "4000", "5000", "6000",
"8000", "10000", "12000", "16000" };

static std::string TCI_DSBpairs[] = {
"-1500,1500;", "-1750,1750;", "-2000,2000;", "-2500,2500;", "-3000,3000;",
"-4000,4000;", "-5000,5000;", "-6000,6000;", "-8000,8000;", "" };
static int tci_nbr_dsb = 9;
static int tci_def_dsb = 4;

static std::vector<std::string>TCI_LSBwidths;
static const char *vTCI_LSBwidths[] = {
 "100",  "200",  "400",  "800", "1000",
"1500", "1800", "2000", "2200", "2500",
"2700", "2900", "3000", "3300", "3500" };

static std::string TCI_LSBpairs[] = {
"-1550,-1450;", "-1600,-1400;", "-1700,-1300;", "-1900,-1100;", "-2000,-1000;",
 "-2250,-750;",   "-1800,-70;",   "-2000,-70;",   "-2200,-70;",   "-2500,-70;",
  "-2700,-70;",   "-2900,-70;",   "-3000,-70;",   "-3300,-70;",   "-3500,-70;", "" };
static int tci_nbr_lsb = 15;
static int tci_def_lsb = 12;

static int tci_centers[] = {
	500, 550, 600, 650, 700, 750, 800, 900, 1000,
	1250, 1500, 1750, 2000, 2250, 2500 };
std::vector<std::string>TCI_centers;
static const char *vTCI_centers[] = {
 "500", "550", "600", "650", "700", "750", "800", "900", "1000",
"1250", "1500", "1750", "2000", "2250", "2500" };
int tci_nbr_centers = 15;
int tci_center = -1; //1500 Hz default

static std::vector<std::string>TCI_USBwidths;
static const char *vTCI_USBwidths[] = {
 "100",  "200",  "400", "800", "1000", 
"1500", "1800", "2000", "2200", "2500",
"2700", "2900", "3000", "3300", "3500" };

static std::string TCI_USBpairs[] = {
"1450,1550;", "1400,1600;", "1300,1700;", "1100,1900;", "1000,2000;",
" 750,2250;", "  70,1800;", "  70,2000;", "  70,2200;", "  70,2500;",
"  70,2700;", "  70,2900;", "  70,3000;", "  70,3300;", "  70,3500;", "" };
static int tci_nbr_usb = 15;
static int tci_def_usb = 12;

static std::vector<std::string>TCI_NFMwidths;
static const char *vTCI_NFMwidths[] = {
"3000", "3500", "4000", "5000", "6000",
"8000", "10000", "12000", "16000" };

static std::string TCI_NFMpairs[] = {
"-1500,1500;", "-1750,1750;", "-2000,2000;", "-2500,2500;", "-3000,3000;",
"-4000,4000;", "-5000,5000;", "-6000,6000;", "-8000,8000;",  "" };
static int tci_nbr_nfm = 9;
static int tci_def_nfm = 3;

static std::vector<std::string>TCI_WFMwidths;
static const char *vTCI_WFMwidths[] = {
"WIDE" };

static std::string TCI_WFMpairs[] = {
"-18000,18000;", "" };
static int tci_nbr_wfm = 1;
static int tci_def_wfm = 0;

static std::vector<std::string>TCI_DIGLwidths;
static const char *vTCI_DIGLwidths[] = {
"1800", "2000", "2200", "2500", "2700",
"2900", "3000", "3300", "3500" };

static std::string TCI_DIGLpairs[] = {
"-1800,-70;", "-2000,-70;", "-2200,-70;", "-2500,-70;", "-2700,-70;",
"-2900,-70;", "-3000,-70;", "-3300,-70;", "-3500,-70;", "" };
static int tci_nbr_digl = 9;
static int tci_def_digl = 1;

static std::vector<std::string>TCI_DIGUwidths;
static const char *vTCI_DIGUwidths[] = {
"1800", "2000", "2200", "2500", "2700",
"2900", "3000", "3300", "3500" };

static std::string TCI_DIGUpairs[] = {
"70,1800;", "70,2000;", "70,2200;", "70,2500;", "70,2700;",
"70,2900;", "70,3000;", "70,3300;", "70,3500;", "" };
static int tci_nbr_digu = 9;
static int tci_def_digu = 1;

static std::vector<std::string>TCI_DRMwidths;
static const char *vTCI_DRMwidths[] = {
"FIXED" };

static std::string TCI_DRMpairs[] = {
"-5000,5000;", "" };
static int tci_nbr_drm = 1;
static int tci_def_drm = 0;

static int agcval = 1;

void tci_adjust_widths()
{
	char szpairs[20];
	int  width;
	int  lo, hi;
	int  tci_freq;

	if (tci_center != progStatus.tci_center) {
		tci_center = progStatus.tci_center;
		tci_freq = tci_centers[tci_center];

		for (int i = 0; i < tci_nbr_usb; i++ ) {
			width = atol(TCI_USBwidths[i].c_str())/2;
			lo = tci_freq - width;
			hi = tci_freq + width;
			if (lo < 0) {
				hi -= lo;
				lo = 0;
			}
			snprintf(szpairs, sizeof(szpairs), "%d,%d;", lo, hi);
			TCI_USBpairs[i] = szpairs;
		}

		for (int i = 0; i < tci_nbr_lsb; i++ ) {
			width = atol(TCI_LSBwidths[i].c_str())/2;
			lo = -(tci_freq + width);
			hi = -(tci_freq - width);
			if (hi > 0) {
				lo -= hi;
				hi = 0;
			}
			snprintf(szpairs, sizeof(szpairs), "%d,%d;", lo, hi);
			TCI_LSBpairs[i] = szpairs;
		}
	}

	if (tci_cw_tone != progStatus.cw_spot_tone) {
		tci_cw_tone = progStatus.cw_spot_tone;
		for (int i = 0; i < 11; i++) {
			width = atol(TCI_CWwidths[i].c_str());
			lo = - width / 2;
			hi = width / 2;
			if (width/2 > tci_cw_tone) {
				lo = -(tci_cw_tone - 100);
				hi = width + 100 - tci_cw_tone;
			}
			snprintf(szpairs, sizeof(szpairs), "%d,%d;", lo, hi);
			TCI_CWpairs[i] = szpairs;
		}
	}
}

/*
int match( std::string &needle, const char **haystack)
{
	int n = 0;
	while (*haystack != NULL) {
		if (needle == *haystack) return n;
		if (needle < *haystack)  return n - 1;
		++n;
		++haystack;
	}
	return -1;
}
*/

static GUI rig_widgets[]= {
	{ (Fl_Widget *)btnVol,        2, 125,  50 }, // 2
	{ (Fl_Widget *)sldrVOLUME,   54, 125, 156 }, // 2
	{ (Fl_Widget *)sldrPOWER,   264, 125, 156 }, // 0
	{ (Fl_Widget *)NULL,          0,   0,   0 }
};


void RIG_TCI_SDR::initialize()
{
	VECTOR (TCI_modes, vTCI_modes);
	VECTOR (TCI_CWwidths, vTCI_CWwidths);
	VECTOR (TCI_AMwidths, vTCI_AMwidths);
	VECTOR (TCI_DSBwidths, vTCI_DSBwidths);
	VECTOR (TCI_LSBwidths, vTCI_LSBwidths);
	VECTOR (TCI_centers, vTCI_centers);
	VECTOR (TCI_USBwidths, vTCI_USBwidths);
	VECTOR (TCI_NFMwidths, vTCI_NFMwidths);
	VECTOR (TCI_WFMwidths, vTCI_WFMwidths);
	VECTOR (TCI_DIGLwidths, vTCI_DIGLwidths);
	VECTOR (TCI_DIGUwidths, vTCI_DIGUwidths);
	VECTOR (TCI_DRMwidths, vTCI_DRMwidths);

	modes_ = TCI_modes;
	_mode_type = TCI_mode_type;
	bandwidths_ = TCI_USBwidths;
	bw_vals_ = NULL;

	tci_open(progStatus.tci_addr, progStatus.tci_port);

	rig_widgets[0].W = btnVol;
	rig_widgets[1].W = sldrVOLUME;
	rig_widgets[2].W = sldrPOWER;

	tci_center = progStatus.tci_center;
	tci_adjust_widths();

}

void RIG_TCI_SDR::shutdown()
{
	tci_trace(1, "RIG_TCI_SDR shutdown()");
	tci_close();
}

bool RIG_TCI_SDR::check()
{
	return tci_running();
}

RIG_TCI_SDR::RIG_TCI_SDR() {
// base class values
	name_ = TCI_SUNDX_name_;
	modes_ = TCI_modes;
	_mode_type = TCI_mode_type;
	bandwidths_ = TCI_USBwidths;
	bw_vals_ = NULL;

	io_class = TCI;

	widgets = rig_widgets;

	serial_baudrate = BR115200;
	stopbits = 1;
	serial_retries = 2;

//	serial_write_delay = 0;
//	serial_post_write_delay = 0;

	serial_timeout = 50;
	serial_rtscts = false;
	serial_rtsplus = false;
	serial_dtrplus = false;
	serial_catptt = true;
	serial_rtsptt = false;
	serial_dtrptt = false;

	A.freq  =  7070000ULL;
	A.imode = TCI_CW;
	A.iBW   = 6;

	B.freq  = 14070000ULL;
	B.imode = TCI_USB;
	B.iBW   = 10;

	can_change_alt_vfo = true;

	has_extras = true;

//	has_noise_reduction =
//	has_noise_reduction_control =
//	has_noise_control =
//	has_sql_control =

	has_split = true;
	has_split_AB = true;
//	has_micgain_control = true;
	has_swr_control = true;
	has_power_out = true;
	has_smeter = true;
	has_attenuator_control = true;
	has_preamp_control = true;
	has_mode_control = true;
	has_bandwidth_control = true;
	has_volume_control = true;
	has_power_control = true;
	has_tune_control = true;
	has_ptt_control = true;

	precision = 1;
	ndigits = 8;

	_noise_reduction_level = 0;
	_nrval1 = 2;
	_nrval2 = 4;
	preamp_level = atten_level = 0;

	sdrtype = UNK;
}

const char * RIG_TCI_SDR::get_bwname_(int n, int md)
{
	try {
		switch (md) {
			case TCI_AM:	return TCI_AMwidths.at(n).c_str();
			case TCI_SAM:	return TCI_AMwidths.at(n).c_str();
			case TCI_DSB:	return TCI_DSBwidths.at(n).c_str();
			case TCI_LSB:	return TCI_LSBwidths.at(n).c_str();
			case TCI_USB:	return TCI_USBwidths.at(n).c_str();
			case TCI_CW:	return TCI_CWwidths.at(n).c_str();
			case TCI_NFM:	return TCI_NFMwidths.at(n).c_str();
			case TCI_DIGL:	return TCI_DIGLwidths.at(n).c_str();
			case TCI_DIGU:	return TCI_DIGUwidths.at(n).c_str();
			case TCI_WFM:	return TCI_WFMwidths.at(n).c_str();
			case TCI_DRM:	return TCI_DRMwidths.at(n).c_str();
			default:		return TCI_USBwidths.at(n).c_str();
		}
	} catch (const std::exception& e) {
		std::cout << e.what() << '\n';
	}
	return "UNKNOWN";
}

// SM cmd 0 ... 100 (rig values 0 ... 15)
int RIG_TCI_SDR::get_smeter()
{
	std::string tcicmd = "rx_smeter:0,0;";
	tci_send(tcicmd);
	MilliSleep(10);
// responds in dBm, +80; -125
//                  100, 0
	int mtr = slice_0.smeter;
	mtr += 150; mtr *= 100; mtr /= 160;
	return mtr;
}

int RIG_TCI_SDR::get_power_out()
{
	int mtr = slice_0.tx_power;
	return mtr;
}

int RIG_TCI_SDR::get_swr()
{
	int mtr = slice_0.tx_swr;
	return (mtr - 1) * 25;
}

int RIG_TCI_SDR::set_widths(int mode)
{
	int bw = 0;
	switch (mode) {
		case TCI_AM:	bandwidths_ = TCI_AMwidths;		bw = tci_def_am;	break;
		case TCI_SAM:	bandwidths_ = TCI_AMwidths;		bw = tci_def_am;	break;
		case TCI_DSB:	bandwidths_ = TCI_DSBwidths;	bw = tci_def_dsb;	break;
		case TCI_LSB:	bandwidths_ = TCI_LSBwidths;	bw = tci_def_lsb;	break;
		case TCI_USB:	bandwidths_ = TCI_USBwidths;	bw = tci_def_usb;	break;
		case TCI_CW:	bandwidths_ = TCI_CWwidths;		bw = tci_def_cw;	break;
		case TCI_NFM:	bandwidths_ = TCI_NFMwidths;	bw = tci_def_nfm;	break;
		case TCI_DIGL:	bandwidths_ = TCI_DIGLwidths;	bw = tci_def_digl;	break;
		case TCI_DIGU:	bandwidths_ = TCI_DIGUwidths;	bw = tci_def_digu;	break;
		case TCI_WFM:	bandwidths_ = TCI_WFMwidths;	bw = tci_def_wfm;	break;
		case TCI_DRM:	bandwidths_ = TCI_DRMwidths;	bw = tci_def_drm;	break;
		default:		bandwidths_ = TCI_USBwidths;	bw = tci_def_usb;	break;
	}
	return bw;
}

std::vector<std::string>& RIG_TCI_SDR::bwtable(int md)
{
	switch (md) {
		case TCI_AM:   bandwidths_ = TCI_AMwidths;   break;
		case TCI_SAM:  bandwidths_ = TCI_AMwidths;   break;
		case TCI_DSB:  bandwidths_ = TCI_DSBwidths;  break;
		case TCI_LSB:  bandwidths_ = TCI_LSBwidths;  break;
		case TCI_USB:  bandwidths_ = TCI_USBwidths;  break;
		case TCI_CW:   bandwidths_ = TCI_CWwidths;   break;
		case TCI_NFM:  bandwidths_ = TCI_NFMwidths;  break;
		case TCI_DIGL: bandwidths_ = TCI_DIGLwidths; break;
		case TCI_DIGU: bandwidths_ = TCI_DIGUwidths; break;
		case TCI_WFM:  bandwidths_ = TCI_WFMwidths;  break;
		case TCI_DRM:  bandwidths_ = TCI_DRMwidths;  break;
		default:
			bandwidths_ = TCI_USBwidths; break;
	}
	return bandwidths_;
}

#define WAIT_ON_MODE 10
int wait_on_mode = 0;

void RIG_TCI_SDR::set_modeA(int mode)
{
	try {
		std::string tcicmd = "modulation:0,";
		tcicmd.append(TCI_modes.at(mode)).append(";");
		tci_send(tcicmd);

		A.imode = mode;

		bwtable(A.imode);
		wait_on_mode = WAIT_ON_MODE;
	} catch (const std::exception& e) {
		std::cout << e.what() << '\n';
	}

}

int RIG_TCI_SDR::get_modeA()
{
	if (--wait_on_mode > 0) return A.imode;

	std::string tcicmd = slice_0.A.mod;

	try {
		size_t n = 0;
		for (n = 0; n < TCI_modes.size(); n++) {
			if (tcicmd.find(TCI_modes.at(n)) == 0) {
				if (n != (size_t)A.imode) {
					A.imode = n;
					A.iBW = set_widths(n);
				}
				return A.imode;
			}
			n++;
		}
	} catch (const std::exception& e) {
		std::cout << e.what() << '\n';
	}
	return A.imode;
}

void RIG_TCI_SDR::set_modeB(int mode)
{
	try {
		std::string tcicmd = "MODULATION:1,";
		tcicmd.append(TCI_modes.at(mode)).append(";");
		tci_send(tcicmd);

		B.imode = mode;

		bwtable(B.imode);

		wait_on_mode = WAIT_ON_MODE;
	} catch (const std::exception& e) {
		std::cout << e.what() << '\n';
	}
}

int RIG_TCI_SDR::get_modeB()
{
	if (--wait_on_mode > 0) return B.imode;

	std::string tcicmd = slice_0.B.mod;
	try {
		size_t n = 0;
		for (n = 0; n < TCI_modes.size(); n++) {
			if (tcicmd.find(TCI_modes.at(n)) == 0) {
				if (n != (size_t)B.imode) {
					B.imode = n;
					B.iBW = set_widths(n);
				}
				return B.imode;
			}
			n++;
		}
	} catch (const std::exception& e) {
		std::cout << e.what() << '\n';
	}
	return B.imode;
}

int RIG_TCI_SDR::get_modetype(int n)
{
	return _mode_type[n];
}

void RIG_TCI_SDR::set_bwA(int val)
{
	tci_adjust_widths();
	std::string tcicmd = "rx_filter_band:0,";
	std::string pairs;

	switch (A.imode) {
		case TCI_AM:	pairs = TCI_AMpairs[val < tci_nbr_am ? val : tci_def_am];		break;
		case TCI_SAM:	pairs = TCI_AMpairs[val < tci_nbr_am ? val : tci_def_am];		break;
		case TCI_DSB:	pairs = TCI_DSBpairs[val < tci_nbr_dsb ? val : tci_def_dsb];	break;
		case TCI_LSB:	pairs = TCI_LSBpairs[val < tci_nbr_lsb ? val : tci_def_lsb];	break;
		case TCI_USB:	pairs = TCI_USBpairs[val < tci_nbr_usb ? val : tci_def_usb];	break;
		case TCI_CW:	pairs = TCI_CWpairs[val < tci_nbr_cw ? val : tci_def_cw];		break;
		case TCI_NFM:	pairs = TCI_NFMpairs[val < tci_nbr_nfm ? val : tci_def_nfm];	break;
		case TCI_DIGL:	pairs = TCI_DIGLpairs[val < tci_nbr_digl ? val : tci_def_digl];	break;
		case TCI_DIGU:	pairs = TCI_DIGUpairs[val < tci_nbr_digu ? val : tci_def_digu];	break;
		case TCI_WFM:	pairs = TCI_WFMpairs[val < tci_nbr_wfm ? val : tci_def_wfm];	break;
		case TCI_DRM:	pairs = TCI_DRMpairs[val < tci_nbr_drm ? val : tci_def_drm];	break;
		default:		pairs = TCI_USBpairs[val < tci_nbr_usb ? val : tci_def_usb];	break;
	}

	tcicmd.append(pairs);
	A.iBW = val;
	slice_0.A.bw = pairs;

	tci_send(tcicmd);

	FilterInner_A = atoi(pairs.c_str());
	size_t pos = pairs.find(",");
	if (pos != std::string::npos) pairs.erase(0,pos);
	FilterOuter_A = atoi(pairs.c_str());
}

int RIG_TCI_SDR::get_bwA()
{
	std::string *tbl = TCI_USBpairs;
	std::string sbw = slice_0.A.bw;
	switch (A.imode) {
		case TCI_AM:   tbl = TCI_AMpairs;	break;
		case TCI_SAM:  tbl = TCI_AMpairs;	break;
		case TCI_DSB:  tbl = TCI_DSBpairs;	break;
		case TCI_LSB:  tbl = TCI_LSBpairs;	break;
		case TCI_USB:  tbl = TCI_USBpairs;	break;
		case TCI_CW:   tbl = TCI_CWpairs;	break;
		case TCI_NFM:  tbl = TCI_NFMpairs;	break;
		case TCI_DIGL: tbl = TCI_DIGLpairs;	break;
		case TCI_DIGU: tbl = TCI_DIGUpairs;	break;
		case TCI_WFM:  tbl = TCI_WFMpairs;	break;
		case TCI_DRM:  tbl = TCI_DRMpairs;	break;
		default:       tbl = TCI_USBpairs;	break;
	}
	int n = 0;
	while (!tbl[n].empty()) {
		if (sbw.find(tbl[n]) != std::string::npos)
			break;
		n++;
	}
	if (!tbl[n].empty()) A.iBW = n;

	FilterInner_A = atoi(sbw.c_str());
	size_t pos = sbw.find(",");
	if (pos != std::string::npos) {
		sbw.erase(0, pos + 1);
		FilterOuter_A = atoi(sbw.c_str());
	}
	return A.iBW;
}

void RIG_TCI_SDR::set_bwB(int val)
{
	tci_adjust_widths();
	std::string tcicmd = "rx_filter_band:0,";
	std::string pairs;
	switch (B.imode) {
		case TCI_AM:   pairs = TCI_AMpairs[val < tci_nbr_am ? val : tci_def_am];		break;
		case TCI_SAM:  pairs = TCI_AMpairs[val < tci_nbr_am ? val : tci_def_am];		break;
		case TCI_DSB:  pairs = TCI_DSBpairs[val < tci_nbr_dsb ? val : tci_def_dsb];		break;
		case TCI_LSB:  pairs = TCI_LSBpairs[val < tci_nbr_lsb ? val : tci_def_lsb];		break;
		case TCI_USB:  pairs = TCI_USBpairs[val < tci_nbr_usb ? val : tci_def_usb];		break;
		case TCI_CW:   pairs = TCI_CWpairs[val < tci_nbr_cw ? val : tci_def_cw];		break;
		case TCI_NFM:  pairs = TCI_NFMpairs[val < tci_nbr_nfm ? val : tci_def_nfm];		break;
		case TCI_DIGL: pairs = TCI_DIGLpairs[val < tci_nbr_digl ? val : tci_def_digl];	break;
		case TCI_DIGU: pairs = TCI_DIGUpairs[val < tci_nbr_digu ? val : tci_def_digu];	break;
		case TCI_WFM:  pairs = TCI_WFMpairs[val < tci_nbr_wfm ? val : tci_def_wfm];		break;
		case TCI_DRM:  pairs = TCI_DRMpairs[val < tci_nbr_drm ? val : tci_def_drm];		break;
		default:       pairs = TCI_USBpairs[val < tci_nbr_usb ? val : tci_def_usb];		break;
	}
	tcicmd.append(pairs);
	B.iBW = val;
	slice_0.B.bw = pairs;
	tci_send(tcicmd);

	FilterInner_B = atoi(pairs.c_str());
	size_t pos = pairs.find(",");
	if (pos != std::string::npos) pairs.erase(0,pos);
	FilterOuter_B = atoi(pairs.c_str());
}

int RIG_TCI_SDR::get_bwB()
{
	std::string *tbl = TCI_USBpairs;
	std::string sbw = slice_0.B.bw;
	switch (B.imode) {
		case TCI_AM:   tbl = TCI_AMpairs; break;
		case TCI_SAM:  tbl = TCI_AMpairs; break;
		case TCI_DSB:  tbl = TCI_DSBpairs; break;
		case TCI_LSB:  tbl = TCI_LSBpairs; break;
		case TCI_USB:  tbl = TCI_USBpairs; break;
		case TCI_CW:   tbl = TCI_CWpairs; break;
		case TCI_NFM:  tbl = TCI_NFMpairs; break;
		case TCI_DIGL: tbl = TCI_DIGLpairs; break;
		case TCI_DIGU: tbl = TCI_DIGUpairs; break;
		case TCI_WFM:  tbl = TCI_WFMpairs; break;
		case TCI_DRM:  tbl = TCI_DRMpairs; break;
		default:       tbl = TCI_USBpairs; break;
	}
	int n = 0;
	while (!tbl[n].empty()) {
		if (sbw.find(tbl[n]) != std::string::npos)
			break;
		n++;
	}
	if (!tbl[n].empty()) B.iBW = n;

	FilterInner_B = atoi(sbw.c_str());
	size_t pos = sbw.find(",");
	if (pos != std::string::npos) sbw.erase(0,pos + 1);
	FilterOuter_B = atoi(sbw.c_str());

	return B.iBW;
}

int RIG_TCI_SDR::def_bandwidth(int val)
{
	int defbw = 0;
	switch (A.imode) {
		case TCI_AM:   defbw = tci_def_am; break;
		case TCI_SAM:  defbw = tci_def_am; break;
		case TCI_DSB:  defbw = tci_def_dsb; break;
		case TCI_LSB:  defbw = tci_def_lsb; break;
		case TCI_USB:  defbw = tci_def_usb; break;
		case TCI_CW:   defbw = tci_def_cw; break;
		case TCI_NFM:  defbw = tci_def_nfm; break;
		case TCI_DIGL: defbw = tci_def_digl; break;
		case TCI_DIGU: defbw = tci_def_digu; break;
		case TCI_WFM:  defbw = tci_def_wfm; break;
		case TCI_DRM:  defbw = tci_def_drm; break;
		default:       defbw = 0; break;
	}
	return defbw;
}

int RIG_TCI_SDR::adjust_bandwidth(int val)
{
	return def_bandwidth(val);
}

void RIG_TCI_SDR::set_pbt(int inner, int outer)
{
	char cmdstr[50];
	snprintf(cmdstr, sizeof(cmdstr), "rx_filter_band:%c,%d,%d;",
		onA ? '0' : '1', inner, outer);
	tci_send(cmdstr);
}

int RIG_TCI_SDR::get_pbt_inner()
{
	if (onA)
		return FilterInner_A;
	else
		return FilterInner_B;
}

int RIG_TCI_SDR::get_pbt_outer()
{
	if (onA)
		return FilterOuter_A;
	else
		return FilterOuter_B;
}

void RIG_TCI_SDR::set_attenuator(int val)
{
}

int RIG_TCI_SDR::get_attenuator()
{
	return 0;
}

void RIG_TCI_SDR::set_preamp(int val)
{
}

int RIG_TCI_SDR::get_preamp()
{
	return 0;
}

// Noise Reduction (TS2000.cxx) NR1 only works; no NR2 and don' no why
void RIG_TCI_SDR::set_noise_reduction(int val)
{
}

int  RIG_TCI_SDR::get_noise_reduction()
{
	return 0;
}

void RIG_TCI_SDR::set_noise_reduction_val(int val)
{
}

int  RIG_TCI_SDR::get_noise_reduction_val()
{
	return 0;
}

int  RIG_TCI_SDR::get_agc()
{
	return 0;
}

int RIG_TCI_SDR::incr_agc()
{
	return agcval;
}


static const char *agcstrs[] = {"FM", "AGC", "FST", "SLO"};
const char *RIG_TCI_SDR::agc_label()
{
	return agcstrs[agcval];
}

int  RIG_TCI_SDR::agc_val()
{
	return agcval;
}

// Noise Blanker (TS2000.cxx)
void RIG_TCI_SDR::set_noise(bool b)
{
}

int RIG_TCI_SDR::get_noise()
{
	return 0;
}

// Tranceiver PTT on/off
void RIG_TCI_SDR::set_PTT_control(int val)
{
	std::string tcicmd;
	if (val) {
		tcicmd = "TRX:0,true;";
	} else
		tcicmd = "TRX:0,false;";
	tci_send(tcicmd);
	ptt_ = slice_0.ptt = val;
}

int RIG_TCI_SDR::get_PTT()
{
	ptt_ = slice_0.ptt;
	return ptt_;
}

void RIG_TCI_SDR::set_rf_gain(int val)
{
}

int  RIG_TCI_SDR::get_rf_gain()
{
	return 100;
}

void RIG_TCI_SDR::get_rf_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}

void RIG_TCI_SDR::selectA()
{
	inuse = onA;
}

void RIG_TCI_SDR::selectB()
{
	inuse = onB;
}

void RIG_TCI_SDR::set_split(bool val)
{
	std::string tcicmd =  "split_enable:0,";
	if (val) tcicmd.append("true;");
	else     tcicmd.append("false;");
	tci_send(tcicmd);
}

bool RIG_TCI_SDR::can_split()
{
	return true;
}

int RIG_TCI_SDR::get_split()
{
	return slice_0.split;
}

unsigned long long RIG_TCI_SDR::get_vfoA ()
{
	A.freq = slice_0.A.freq;
	return A.freq;
}

void RIG_TCI_SDR::set_vfoA (unsigned long long freq)
{
	A.freq = slice_0.A.freq = freq;
	char vfostr[20];
	snprintf(vfostr, sizeof(vfostr), "vfo:0,0,%llu;", freq);
	tci_send(vfostr);
}

unsigned long long RIG_TCI_SDR::get_vfoB ()
{
	B.freq = slice_0.B.freq;
	return B.freq;
}

void RIG_TCI_SDR::set_vfoB (unsigned long long freq)
{
	B.freq = slice_0.B.freq = freq;
	char vfostr[20];
	snprintf(vfostr, sizeof(vfostr), "vfo:0,1,%llu;", freq);
	tci_send(vfostr);
}

// Squelch (TS990.cxx)
void RIG_TCI_SDR::set_squelch(int val)
{
}

int  RIG_TCI_SDR::get_squelch()
{
	int val = 0;
	return val;
}

void RIG_TCI_SDR::get_squelch_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 255; step = 1;
}

/*
void RIG_TCI_SDR::set_mic_gain(int val)
{
	std::string tcicmd =  "MG";
	tcicmd.append(to_decimal(val,3)).append(";");
	sendCommand(tcicmd);
	showresp(WARN, ASC, "set mic gain", tcicmd, "");
}

int  RIG_TCI_SDR::get_mic_gain()
{
	int val = progStatus.mic_gain;
	std::string tcicmd =  "MG;";
	if (wait_char(';', 6, 100, "get mic gain", ASC) < 6) return val;

	size_t p = replystr.rfind("MG");
	if (p != std::string::npos)
		val = fm_decimal(replystr.substr(p+2), 3);
	return val;
}

void RIG_TCI_SDR::get_mic_min_max_step(int &min, int &max, int &step)
{
	min = 0; max = 100; step = 1;
}
*/

void RIG_TCI_SDR::set_volume_min_max_step(double &min, double &max, double &step)
{
	min = -60; max = 0; step = 1; // in dBm
}

void RIG_TCI_SDR::set_volume_control(int val)
{
	char szcmd[20];
	val = ((val * 60)/100) - 60;
	snprintf(szcmd, sizeof(szcmd), "volume:%d;", val);
	tci_send(szcmd);
	slice_0.vol = val;
}

int RIG_TCI_SDR::get_volume_control()
{
	int vol = (slice_0.vol + 60) * 100 / 60;
	return vol;
}

void RIG_TCI_SDR::get_pc_min_max_step(double &min, double &max, double &step)
{
	if (sdrtype == DX) {
		min = 0; max = 100; step = 1; 
	} else {
		min = 0; max = 20; step = 0.1; 
	}
}

void RIG_TCI_SDR::set_power_control(double val)
{
	char szcmd[20];
	if (sdrtype == PRO)
		val *= 5;
	snprintf(szcmd, sizeof(szcmd), "drive:%d;", (int)(val));
	tci_send(szcmd);
	slice_0.pwr = val;
}

double RIG_TCI_SDR::get_power_control()
{
	double pwr = slice_0.pwr;
	if (sdrtype == PRO) pwr *= 0.2;
	return pwr;
}

static bool tune_on = false;
void RIG_TCI_SDR::tune_rig()
{
	tune_on = !tune_on;
	char szcmd[20];
	snprintf(szcmd, sizeof(szcmd), "tune:0,%s;",
		(tune_on ? "true" : "false"));
	tci_send(szcmd);
}

//======================================================================

RIG_TCI_SUNPRO::RIG_TCI_SUNPRO() {
	sdrtype = PRO;
	name_ = TCI_SUNPRO_name_;
};

//======================================================================

RIG_TCI_SUNDX::RIG_TCI_SUNDX() {
	sdrtype = DX;
	name_ = TCI_SUNDX_name_;
};



