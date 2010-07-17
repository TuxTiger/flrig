/*
 * Icom IC-746, 746PRO, 756, 756PRO
 * 7000, 7200, 7700 ... drivers
 *
 * a part of flrig
 *
 * Copyright 2009, Dave Freese, W1HKJ
 *
 */

#include "IC7000.h"

//=============================================================================
// IC-7000

const char IC7000name_[] = "IC-7000";

const char *IC7000modes_[] = {
		"LSB", "USB", "AM", "CW", "RTTY", "FM", "CW-R", "RTTY-R", NULL};

const char IC7000_mode_type[] =
	{ 'L', 'U', 'U', 'U', 'L', 'U', 'L', 'U' };

RIG_IC7000::RIG_IC7000() {
	defaultCIV = 0x70;
	name_ = IC7000name_;
	modes_ = IC7000modes_;
	_mode_type = IC7000_mode_type;
	adjustCIV(defaultCIV);
	restore_mbw = false;
};

//======================================================================
// IC7000 unique commands
//======================================================================

void RIG_IC7000::set_mode(int val)
{
	mode_ = val;
	switch (val) {
		case 7  : val = 8; break;
		case 6  : val = 7; break;
		default: break;
	}
	cmd = pre_to;
	cmd += '\x06';
	cmd += val;
	cmd += filter_nbr;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC7000::get_mode()
{
	int md;
	cmd = pre_to;
	cmd += '\x04';
	cmd.append(post);
	if (sendICcommand (cmd, 8 )) {
		md = replystr[5];
		if (md > 6) md--;
		filter_nbr = replystr[6];
	} else {
		checkresponse(8);
	}
	return mode_;
}

int RIG_IC7000::adjust_bandwidth(int m)
{
	filter_nbr = 1;
	if (m == 0 || m == 1) { //SSB
		bandwidths_ = IC746PRO_SSBwidths;
		return (bw_ = 32);
	}
	if (m == 3 || m == 6) { //CW
		bandwidths_ = IC746PRO_SSBwidths;
		return (bw_ = 14);
	}
	if (m == 4 || m == 7) { //RTTY
		bandwidths_ = IC746PRO_RTTYwidths;
		return (bw_ = 28);
	}
	bandwidths_ = IC746PRO_AMFMwidths;
	return (bw_ = 0);
}


int RIG_IC7000::get_attenuator()
{
	cmd = pre_to;
	cmd += '\x11';
	cmd.append( post );
	if (sendICcommand(cmd,7))
		return (replystr[5] ? 1 : 0);
	return 0;
}

void RIG_IC7000::set_attenuator(int val)
{
	cmd = pre_to;
	cmd += '\x11';
	cmd += val ? '\x12' : '\x00';
	cmd.append( post );
	sendICcommand(cmd,6);
	checkresponse(6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

void RIG_IC7000::set_preamp(int val)
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd += val ? 0x01 : 0x00;
	cmd.append( post );
	sendICcommand (cmd, 6);
	checkresponse(6);
	if (RIG_DEBUG)
		LOG_INFO("%s", str2hex(cmd.data(), cmd.length()));
}

int RIG_IC7000::get_preamp()
{
	cmd = pre_to;
	cmd += '\x16';
	cmd += '\x02';
	cmd.append( post );
	if (sendICcommand (cmd, 8))
		return replystr[6] ? 1 : 0;
	return 0;
}
