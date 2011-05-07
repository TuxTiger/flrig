#ifndef _TS570_H
#define _TS570_H

#include "rigbase.h"

class RIG_TS570 : public rigbase {
private:
	bool beatcancel_on;
	bool preamp_on;
	bool att_on;
	bool is_TS570S;
public:
	RIG_TS570();
	~RIG_TS570(){}
	
	void initialize();

	bool get_ts570id();

	void selectA();
	void selectB();
	void set_split(bool val);
	bool get_split();

	long get_vfoA();
	void set_vfoA(long);
	long get_vfoB();
	void set_vfoB (long);

	void set_modeA(int val);
	int  get_modeA();
	void set_modeB(int val);
	int  get_modeB();
	int  get_modetype(int n);

	void set_bwA(int val);
	int  get_bwA();
	void set_bwB(int val);
	int  get_bwB();
	int  adjust_bandwidth(int val);
	int  def_bandwidth(int val);
	void set_widths();

	int  get_smeter();
	int  get_swr();
	int  get_power_out();
	int  get_power_control();
	void set_volume_control(int val);
	int  get_volume_control();
	void set_power_control(double val);
	void set_PTT_control(int val);
	void tune_rig();
	void set_attenuator(int val);
	int  get_attenuator();
	void set_preamp(int val);
	int  get_preamp();

	//	void set_if_shift(int val);
	//	bool get_if_shift(int &val);
	//	void get_if_min_max_step(int &min, int &max, int &step);
	//	void set_notch(bool on, int val);
	//	bool get_notch(int &val);
	//	void get_notch_min_max_step(int &min, int &max, int &step);

	void set_noise(bool b);
	int  get_noise();

	void set_mic_gain(int val);
	int  get_mic_gain();
	void get_mic_min_max_step(int &min, int &max, int &step);

	bool sendTScommand(string, int, bool);
	const char **bwtable(int);

};


#endif
