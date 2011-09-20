#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <Ivy/ivy.h>
#include <Ivy/ivyglibloop.h>

#define ASTECH 1

#include "tuning.h"

//#include "sliding_plot.h"

#define MB_MODES_IDLE   0
#define MB_MODES_MANUAL 1
#define MB_MODES_RAMP   2
#define MB_MODES_STEP   3
#define MB_MODES_PRBS   4
#define MB_MODES_SINE       5
#define MB_MODES_FIXED_RPM  6

#define AS_MOT_FRONT 0
#define AS_MOT_BACK  1
#define AS_MOT_LEFT  2
#define AS_MOT_RIGHT 3

#define AS_CMD_TEST_ADDR 1
#define AS_CMD_REVERSE   2
#define AS_CMD_SET_ADDR  3




const guint mb_id = 158;

struct motor_bench_state {
  guint mode;
  double time;
  double throttle;
  double rpms;
  double amps;
  double thrust;
  double torque;
  double av_rpm;
  double av_throttle;
  double av_thrust;
  double av_amps;
  GIOChannel* log_channel;
  GIOChannel* log_channel_static;
};

struct motor_bench_gui {
  GtkWidget* lab_time;
  GtkWidget* lab_throttle;
  GtkWidget* lab_rpms;
  GtkWidget* lab_amps;
  GtkWidget* lab_thrust;
  GtkWidget* lab_torque;
  GtkWidget* entry_log;
};

static void on_mode_changed (GtkRadioButton  *radiobutton, gpointer user_data);
static void on_MOTOR_BENCH_STATUS(IvyClientPtr app, void *user_data, int argc, char *argv[]);
static gboolean timeout_callback(gpointer data);
static void on_log_button_toggled (GtkWidget *widget, gpointer data);

static void on_as_test_button_clicked (GtkWidget *widget, gpointer data);
static void on_as_reverse_button_clicked (GtkWidget *widget, gpointer data);
static void on_as_addr_changed (GtkRadioButton  *radiobutton, gpointer user_data);


static GtkWidget* build_gui ( void );

static struct motor_bench_state mb_state;
static struct motor_bench_gui mb_gui;

static void on_mode_changed (GtkRadioButton  *radiobutton, gpointer user_data) {
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton)))
    return;
  guint mode = (guint)user_data;
  IvySendMsg("dl DL_SETTING %d %d %d", mb_id, PPRZ_MB_MODES_MODE, mode);
}

#if 0
void on_scale_value_changed (GtkScale  *scale, gpointer user_data) {
  gfloat cf = gtk_range_get_value(GTK_RANGE(scale));
  gint c = round(cf);
  g_message("foo %d %f", user_data, c);
  IvySendMsg("ME RAW_DATALINK 16 SETTING;%d;0;%d", (gint)user_data, c);
}
#endif


static void on_MOTOR_BENCH_STATUS(IvyClientPtr app, void *user_data, int argc, char *argv[]){
  guint time_ticks =  atoi(argv[0]);
  double throttle =  atof(argv[1]);
  double rpm =  atof(argv[2]);
  double amp =  atof(argv[3]);
  double thrust =  atof(argv[4]);
  double torque =  atof(argv[5]);
  guint time_sec = atoi(argv[6]);
  guint mode = atoi(argv[7]);
  mb_state.mode = mode;
  mb_state.time = (double)time_sec + (double)time_ticks/15000000.;
  mb_state.throttle = throttle;
  mb_state.rpms = rpm;
  mb_state.amps = amp;
  mb_state.thrust = thrust;
  mb_state.torque = torque;
  if (mb_state.log_channel) {
    GString* str = g_string_sized_new(256);
    g_string_printf(str, "%.4f %.3f %.0f %.1f %.1f %.1f\n", mb_state.time, mb_state.throttle, mb_state.rpms, mb_state.amps, mb_state.thrust, mb_state.torque);
    gsize b_writen;
    GError* my_err = NULL;
    GIOStatus stat = g_io_channel_write_chars(mb_state.log_channel,str->str, str->len, &b_writen, &my_err); 
    g_string_free(str, TRUE);
  }
  //  g_message("foo %f %f %f %f %d", mb_state.time, throttle, rpm, amp, mode);
}

static void on_MOTOR_BENCH_STATIC(IvyClientPtr app, void *user_data, int argc, char *argv[]){
  mb_state.av_rpm =  atof(argv[0]);
  mb_state.av_thrust =  atof(argv[1]);
  mb_state.av_amps =  atof(argv[2]);
  mb_state.av_throttle =  atof(argv[3]);

  if (mb_state.log_channel_static) {
    GString* str = g_string_sized_new(256);
    g_string_printf(str, "%0f %.3f %.2f %.1f\n", mb_state.av_throttle, mb_state.av_rpm, mb_state.av_amps, mb_state.av_thrust);
    gsize b_writen;
    GError* my_err = NULL;
    GIOStatus stat = g_io_channel_write_chars(mb_state.log_channel_static,str->str, str->len, &b_writen, &my_err); 
    g_string_free(str, TRUE);
  }
  g_message("in_static %f %f %f %f", mb_state.av_throttle, mb_state.av_rpm, mb_state.av_amps, mb_state.av_thrust);
}


static gboolean timeout_callback(gpointer data) {
  GString* str = g_string_sized_new(64);
  g_string_printf(str, "%.2f s", mb_state.time);
  gtk_label_set_text(GTK_LABEL(mb_gui.lab_time), str->str);
  g_string_printf(str, "%.2f %%", mb_state.throttle*100.);
  gtk_label_set_text(GTK_LABEL(mb_gui.lab_throttle), str->str);
  g_string_printf(str, "%.0f rpms", mb_state.rpms);
  gtk_label_set_text(GTK_LABEL(mb_gui.lab_rpms), str->str);
  g_string_printf(str, "%.1f a", mb_state.amps);
  gtk_label_set_text(GTK_LABEL(mb_gui.lab_amps), str->str);
  g_string_printf(str, "%.1f g", mb_state.thrust);
  gtk_label_set_text(GTK_LABEL(mb_gui.lab_thrust), str->str);
  g_string_printf(str, "%.1f g", mb_state.torque);
  gtk_label_set_text(GTK_LABEL(mb_gui.lab_torque), str->str);
  g_string_free(str, TRUE);
  return TRUE;
}

static void on_log_button_toggled (GtkWidget *widget, gpointer data) {
   if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
     gtk_editable_set_editable( GTK_EDITABLE(mb_gui.entry_log), FALSE );
     const gchar *log_file_name = gtk_entry_get_text (GTK_ENTRY (mb_gui.entry_log));
     GError* my_err = NULL;
     mb_state.log_channel = g_io_channel_new_file (log_file_name, "w", &my_err);
     GString* static_name = g_string_sized_new(128);
     g_string_printf(static_name,"%s%s", log_file_name, "_static"); 
     mb_state.log_channel_static = g_io_channel_new_file (static_name->str, "w", &my_err);
     g_string_free(static_name, TRUE);
   }
   else {
     gtk_editable_set_editable( GTK_EDITABLE(mb_gui.entry_log), TRUE );
     if (mb_state.log_channel) {
       g_io_channel_close(mb_state.log_channel);
       g_io_channel_close(mb_state.log_channel_static);
       mb_state.log_channel = NULL;
       mb_state.log_channel_static = NULL;
     }
   }
}


static void on_as_test_button_clicked (GtkWidget *widget, gpointer data) {
#ifdef ASTECH
  IvySendMsg("dl DL_SETTING %d %d %d", mb_id, PPRZ_MB_TWI_CONTROLLER_ASCTECH_COMMAND_TYPE, AS_CMD_TEST_ADDR);
#endif
}

static void on_as_reverse_button_clicked (GtkWidget *widget, gpointer data) {
#ifdef ASTECH
  IvySendMsg("dl DL_SETTING %d %d %d", mb_id, PPRZ_MB_TWI_CONTROLLER_ASCTECH_COMMAND_TYPE, AS_CMD_REVERSE);
#endif
}


static void on_as_addr_changed (GtkRadioButton  *radiobutton, gpointer user_data) {
#ifdef ASTECH
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton)))
    return;
  guint new_addr = (guint)user_data;
  IvySendMsg("dl DL_SETTING %d %d %d", mb_id, PPRZ_MB_TWI_CONTROLLER_ASCTECH_ADDR, new_addr);
#endif
}





int main (int argc, char** argv) {

  gtk_init(&argc, &argv);

  GtkWidget* window = build_gui();
  gtk_widget_show_all(window);

  IvyInit ("MotorBench", "MotorBench READY", NULL, NULL, NULL, NULL);
  IvyBindMsg(on_MOTOR_BENCH_STATUS, NULL, "^\\S* MOTOR_BENCH_STATUS (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");
  IvyBindMsg(on_MOTOR_BENCH_STATIC, NULL, "^\\S* MOTOR_BENCH_STATIC (\\S*) (\\S*) (\\S*) (\\S*)");
  IvyStart("127.255.255.255");

  g_timeout_add(40, timeout_callback, NULL);
  
  mb_state.log_channel = NULL;

  gtk_main();
  return 0;
}


static GtkWidget* build_gui ( void ) {

  GtkWidget *window1;
  GtkWidget *vbox1;


  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), "motor_bench");

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  GtkWidget* hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  //
  // Modes
  //
  GtkWidget *mode_frame = gtk_frame_new ("Mode");
  gtk_container_set_border_width (GTK_CONTAINER (mode_frame), 10);
  gtk_box_pack_start (GTK_BOX (hbox1), mode_frame, TRUE, TRUE, 0);

  GtkWidget* vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (mode_frame), vbox2);

  GSList *rb_mode_group = NULL;
  GtkWidget* rb_idle = gtk_radio_button_new_with_mnemonic (NULL, "idle");
  gtk_box_pack_start (GTK_BOX (vbox2), rb_idle, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_idle), rb_mode_group);
  rb_mode_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_idle));
  GtkWidget* rb_manual = gtk_radio_button_new_with_mnemonic (NULL, "manual");
  gtk_box_pack_start (GTK_BOX (vbox2), rb_manual, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_manual), rb_mode_group);
  rb_mode_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_manual));
  GtkWidget* rb_ramp = gtk_radio_button_new_with_mnemonic (NULL, "ramp");
  gtk_box_pack_start (GTK_BOX (vbox2), rb_ramp, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_ramp), rb_mode_group);
  rb_mode_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_ramp));
  GtkWidget* rb_step = gtk_radio_button_new_with_mnemonic (NULL, "step");
  gtk_box_pack_start (GTK_BOX (vbox2), rb_step, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_step), rb_mode_group);
  rb_mode_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_step));
  GtkWidget* rb_prbs = gtk_radio_button_new_with_mnemonic (NULL, "prbs");
  gtk_box_pack_start (GTK_BOX (vbox2), rb_prbs, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_prbs), rb_mode_group);
  rb_mode_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_prbs));
  GtkWidget* rb_sine = gtk_radio_button_new_with_mnemonic (NULL, "sine");
  gtk_box_pack_start (GTK_BOX (vbox2), rb_sine, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_sine), rb_mode_group);
  rb_mode_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_sine));
  GtkWidget* rb_fixed_rpm = gtk_radio_button_new_with_mnemonic (NULL, "fixed");
  gtk_box_pack_start (GTK_BOX (vbox2), rb_fixed_rpm, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_fixed_rpm), rb_mode_group);
  rb_mode_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_fixed_rpm));



  g_signal_connect ((gpointer) rb_idle, "toggled", G_CALLBACK (on_mode_changed), (gpointer)MB_MODES_IDLE);
  g_signal_connect ((gpointer) rb_manual, "toggled", G_CALLBACK (on_mode_changed), (gpointer)MB_MODES_MANUAL);
  g_signal_connect ((gpointer) rb_ramp, "toggled", G_CALLBACK (on_mode_changed), (gpointer)MB_MODES_RAMP);
  g_signal_connect ((gpointer) rb_step, "toggled", G_CALLBACK (on_mode_changed), (gpointer)MB_MODES_STEP);
  g_signal_connect ((gpointer) rb_prbs, "toggled", G_CALLBACK (on_mode_changed), (gpointer)MB_MODES_PRBS);
  g_signal_connect ((gpointer) rb_sine, "toggled", G_CALLBACK (on_mode_changed), (gpointer)MB_MODES_SINE);
  g_signal_connect ((gpointer) rb_fixed_rpm, "toggled", G_CALLBACK (on_mode_changed), (gpointer)MB_MODES_FIXED_RPM);


  //
  // Measures
  //
  GtkWidget *measure_frame = gtk_frame_new ("Measures");
  gtk_container_set_border_width (GTK_CONTAINER (measure_frame), 10);
  gtk_box_pack_start (GTK_BOX (hbox1), measure_frame, TRUE, TRUE, 0);

  GtkWidget* table1 = gtk_table_new (2, 3, FALSE);
  gtk_container_add (GTK_CONTAINER (measure_frame), table1);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 5);
  
  GtkWidget* t_time = gtk_label_new ("time");
  gtk_table_attach (GTK_TABLE (table1), t_time, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (t_time), 0, 0.5);
  mb_gui.lab_time = gtk_label_new ("XXXX");
  gtk_table_attach (GTK_TABLE (table1), mb_gui.lab_time, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (mb_gui.lab_time), 0, 0.5);

  GtkWidget* t_throttle = gtk_label_new ("throttle");
  gtk_table_attach (GTK_TABLE (table1), t_throttle, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (t_throttle), 0, 0.5);
  mb_gui.lab_throttle = gtk_label_new ("XXXX");
  gtk_table_attach (GTK_TABLE (table1), mb_gui.lab_throttle, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (mb_gui.lab_throttle), 0, 0.5);

  GtkWidget* t_rpms = gtk_label_new ("rpms");
  gtk_table_attach (GTK_TABLE (table1), t_rpms, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (t_rpms), 0, 0.5);
  mb_gui.lab_rpms = gtk_label_new ("XXXX");
  gtk_table_attach (GTK_TABLE (table1), mb_gui.lab_rpms, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (mb_gui.lab_rpms), 0, 0.5);

  GtkWidget* t_amps = gtk_label_new ("amps");
  gtk_table_attach (GTK_TABLE (table1), t_amps, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (t_amps), 0, 0.5);
  mb_gui.lab_amps = gtk_label_new ("XXXX");
  gtk_table_attach (GTK_TABLE (table1), mb_gui.lab_amps, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (mb_gui.lab_amps), 0, 0.5);

  GtkWidget* t_thrust = gtk_label_new ("Thrust");
  gtk_table_attach (GTK_TABLE (table1), t_thrust, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (t_thrust), 0, 0.5);
  mb_gui.lab_thrust = gtk_label_new ("XXXX");
  gtk_table_attach (GTK_TABLE (table1), mb_gui.lab_thrust, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (mb_gui.lab_thrust), 0, 0.5);

  GtkWidget* t_torque = gtk_label_new ("Torque");
  gtk_table_attach (GTK_TABLE (table1), t_torque, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (t_torque), 0, 0.5);
  mb_gui.lab_torque = gtk_label_new ("XXXX");
  gtk_table_attach (GTK_TABLE (table1), mb_gui.lab_torque, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (mb_gui.lab_torque), 0, 0.5);


  //
  // Log
  //
  GtkWidget *log_frame = gtk_frame_new ("Log");
  gtk_container_set_border_width (GTK_CONTAINER (log_frame), 10);
  gtk_box_pack_start (GTK_BOX (vbox1), log_frame, TRUE, TRUE, 0);
  

  GtkWidget* bbox = gtk_hbutton_box_new (); 
  gtk_container_add (GTK_CONTAINER (log_frame), bbox);

  GtkWidget* log_button = gtk_toggle_button_new_with_label( "Log" );
  gtk_container_add (GTK_CONTAINER (bbox), log_button);
  g_signal_connect (G_OBJECT (log_button), "toggled", G_CALLBACK (on_log_button_toggled), NULL);

  mb_gui.entry_log = gtk_entry_new( );
  gtk_container_add (GTK_CONTAINER (bbox), mb_gui.entry_log);
  gtk_entry_set_text( GTK_ENTRY(mb_gui.entry_log), "mb_log.txt" );



  //
  // Asctech
  //
  
  GtkWidget *asctech_frame = gtk_frame_new ("Asctech");
  gtk_container_set_border_width (GTK_CONTAINER (asctech_frame), 10);
  gtk_box_pack_start (GTK_BOX (vbox1), asctech_frame, TRUE, TRUE, 0);


  GtkWidget* as_vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (asctech_frame), as_vbox2);

  GSList *rb_addr_group = NULL;
  GtkWidget* rb_front = gtk_radio_button_new_with_mnemonic (NULL, "front");
  gtk_box_pack_start (GTK_BOX (as_vbox2), rb_front, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_front), rb_addr_group);
  rb_addr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_front));
  GtkWidget* rb_back = gtk_radio_button_new_with_mnemonic (NULL, "back");
  gtk_box_pack_start (GTK_BOX (as_vbox2), rb_back, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_back), rb_addr_group);
  rb_addr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_back));
  GtkWidget* rb_left = gtk_radio_button_new_with_mnemonic (NULL, "left");
  gtk_box_pack_start (GTK_BOX (as_vbox2), rb_left, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_left), rb_addr_group);
  rb_addr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_left));
  GtkWidget* rb_right = gtk_radio_button_new_with_mnemonic (NULL, "right");
  gtk_box_pack_start (GTK_BOX (as_vbox2), rb_right, TRUE, TRUE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (rb_right), rb_addr_group);
  rb_addr_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (rb_right));

  g_signal_connect ((gpointer) rb_front, "toggled", G_CALLBACK (on_as_addr_changed), (gpointer)AS_MOT_FRONT);
  g_signal_connect ((gpointer) rb_back,  "toggled", G_CALLBACK (on_as_addr_changed), (gpointer)AS_MOT_BACK);
  g_signal_connect ((gpointer) rb_left,  "toggled", G_CALLBACK (on_as_addr_changed), (gpointer)AS_MOT_LEFT);
  g_signal_connect ((gpointer) rb_right, "toggled", G_CALLBACK (on_as_addr_changed), (gpointer)AS_MOT_RIGHT);



  GtkWidget* as_bbox = gtk_hbutton_box_new (); 
  gtk_box_pack_start (GTK_BOX (as_vbox2), as_bbox, TRUE, TRUE, 0);
  //gtk_container_add (GTK_CONTAINER (asctech_frame), as_bbox);

  GtkWidget* test_button = gtk_toggle_button_new_with_label( "Test" );
  gtk_container_add (GTK_CONTAINER (as_bbox), test_button);
  g_signal_connect (G_OBJECT (test_button), "clicked", G_CALLBACK (on_as_test_button_clicked), NULL);

  GtkWidget* reverse_button = gtk_toggle_button_new_with_label( "Reverse" );
  gtk_container_add (GTK_CONTAINER (as_bbox), reverse_button);
  g_signal_connect (G_OBJECT (reverse_button), "clicked", G_CALLBACK (on_as_reverse_button_clicked), NULL);


  return window1;
}
