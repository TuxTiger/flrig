#ifndef _TT538_H
#define _TT538_H

#include "rigbase.h"

class RIG_TT538 : public rigbase {
private:
	double fwdpwr;
	double refpwr;
	double fwdv;
	double refv;
	double VfoAdj;

public:
	RIG_TT538();
	~RIG_TT538(){}

	void initialize();

	long get_vfoA();
	void set_vfoA(long);

	long get_vfoB();
	void set_vfoB(long);

	void set_modeA(int val);
	int  get_modeA();
//	void set_modeB(int val);
//	int  get_modeB();
	int  get_modetype(int n);

	void set_bwA(int val);
	int  get_bwA();
//	void set_bwB(int val);
//	int  get_bwB();

	bool twovfos() { return false; }
//	void set_split(bool);
	bool can_split() {return false;}

	int  get_smeter();
	void set_volume_control(int val);
	int  get_volume_control();
	void set_attenuator(int val);
	int  get_attenuator();
	void set_rf_gain(int val);
	int  get_rf_gain();
	void set_if_shift(int val);
	bool get_if_shift(int &val);
	void get_if_min_max_step(int &min, int &max, int &step);
	void setVfoAdj(double v);
	int  get_swr();
	int  get_power_out();
//	int  get_power_control();
//	void set_power_control(double val);
	void set_PTT_control(int val);
//	void tune_rig();
//	void set_preamp(int val);
//	int  get_preamp();
//	void set_notch(bool on, int val);
//	bool get_notch(int &val);
//	void get_notch_min_max_step(int &min, int &max, int &step);
//	void set_noise(bool b);
//	void set_mic_gain(int val);
//	int  get_mic_gain();
//	void get_mic_min_max_step(int &min, int &max, int &step);
	void checkresponse(string s);
	void shutdown();

	int  adjust_bandwidth(int m);

};


#endif
