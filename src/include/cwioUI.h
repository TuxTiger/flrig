// generated by Fast Light User Interface Designer (fluid) version 1.0308

#ifndef cwioUI_h
#define cwioUI_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
extern Fl_Double_Window *cwlog_viewer; 
#include <FL/Fl_Double_Window.H>
#include "status.h"
#include "cwio.h"
#include "ValueSlider.h"
#include "flinput2.h"
extern Fl_Input2 *txt_to_send;
#include <FL/Fl_Group.H>
#include "flslider2.h"
extern Fl_Value_Slider2 *sldr_cwioWPM;
#include <FL/Fl_Button.H>
extern Fl_Button *btn_cwio_config;
extern Fl_Button *btn_cwio_clear;
#include <FL/Fl_Light_Button.H>
extern Fl_Light_Button *btn_cwioKEY;
extern Fl_Light_Button *btn_cwioSEND;
extern Fl_Button *btn_msg[12];
extern Fl_Button *btn_view_cwlog;
Fl_Double_Window* new_cwio_dialog();
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Box.H>
extern Fl_Input2 *edit_label[12];
extern Fl_Input2 *edit_msg[12];
#include "combo.h"
extern Fl_ListBox *listbox_BT;
extern Fl_ListBox *listbox_AA;
extern Fl_ListBox *listbox_AS;
extern Fl_ListBox *listbox_AR;
extern Fl_ListBox *listbox_SK;
extern Fl_ListBox *listbox_KN;
extern Fl_ListBox *listbox_INT;
extern Fl_ListBox *listbox_HM;
extern Fl_ListBox *listbox_VE;
extern Fl_Button *btn_start_xmt;
extern Fl_Button *btn_end_xmt;
extern Fl_Button *btn_append_call;
extern Fl_Button *btn_append_name;
extern Fl_Button *btn_append_rst;
extern Fl_Button *btn_cancel_edit;
extern Fl_Button *btn_apply_edit;
extern Fl_Button *btn_done_edit;
extern Fl_Button *btn_append_contest_nbr;
extern Fl_Button *btn_append_decr;
extern Fl_Button *btn_append_incr;
Fl_Double_Window* new_message_editor();
extern Fl_ComboBox *select_cwioPORT;
extern Fl_Light_Button *btn_cwioCONNECT;
#include <FL/Fl_Check_Button.H>
extern Fl_Check_Button *btn_cwioCAT;
extern Fl_Check_Button *btn_cwioAUX;
extern Fl_Check_Button *btn_cwioSEP;
extern Fl_ListBox *listbox_cwioKEYLINE;
#include <FL/Fl_Spinner.H>
extern Fl_Spinner *cntr_cwioPTT;
extern Fl_Light_Button *btn_cw_dtr_calibrate;
#include <FL/Fl_Counter.H>
extern Fl_Counter *cnt_cwio_comp;
extern Fl_Check_Button *btn_cwioINVERTED;
extern Fl_Counter *cnt_cwio_keycorr;
Fl_Double_Window* new_cwio_config_dialog();
#include <FL/Fl_Menu_Bar.H>
extern Fl_Menu_Bar *CWlog_menubar;
#include <FL/Fl_Output.H>
extern Fl_Output *txt_cwlog_file;
extern Fl_Group *cw_grp_qso_data;
extern Fl_Input2 *cw_freq;
extern Fl_Button *btn_cwlog_clear_qso;
extern Fl_Button *btn_cwlog_save_qso;
extern Fl_Button *btn_cwlog_edit_entry;
extern Fl_Button *btn_cwlog_delete_entry;
extern Fl_Input2 *cw_qso_date;
extern Fl_Input2 *cw_qso_time;
extern Fl_Input2 *cw_rst_out;
extern Fl_Input2 *cw_rst_in;
extern Fl_Input2 *cw_xchg_in;
extern Fl_Counter *cw_log_nbr;
extern Fl_Input2 *cw_op_name;
extern Fl_Input2 *cw_op_call;
extern Fl_Button *btn_cw_datetime;
extern Fl_Button *btn_cw_time;
extern Fl_Button *btn_cw_freq;
extern Fl_Button *btn_cw_call;
extern Fl_Button *btn_cw_name;
extern Fl_Button *btn_cw_in;
extern Fl_Button *btn_cw_out;
extern Fl_Button *btn_cw_sent_nbr;
extern Fl_Button *btn_cw_xchg;
#include <FL/Fl_Browser.H>
extern Fl_Browser *brwsr_cwlog_entries;
Fl_Double_Window* new_cwlogbook_dialog();
extern unsigned char menu_CWlog_menubar_i18n_done;
extern Fl_Menu_Item menu_CWlog_menubar[];
#define CWlog_files (menu_CWlog_menubar+0)
#define cwlog_menu_open (menu_CWlog_menubar+1)
#define cwlog_menu_new (menu_CWlog_menubar+2)
#define cwlog_menu_save (menu_CWlog_menubar+3)
#define cwlog_menu_save_as (menu_CWlog_menubar+4)
#define cwlog_menu_export_adif (menu_CWlog_menubar+5)
#define cwlog_menu_import_adif (menu_CWlog_menubar+6)
#define cwlog_menu_dupcheck (menu_CWlog_menubar+9)
#define cwlog_menu_leading_zeros (menu_CWlog_menubar+10)
#define cwlog_menu_cut_numbers (menu_CWlog_menubar+11)
#endif
