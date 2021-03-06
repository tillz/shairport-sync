#ifndef _COMMON_H
#define _COMMON_H

#include <libconfig.h>
#include <signal.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "audio.h"
#include "config.h"
#include "definitions.h"
#include "mdns.h"

// struct sockaddr_in6 is bigger than struct sockaddr. derp
#ifdef AF_INET6
#define SOCKADDR struct sockaddr_storage
#define SAFAMILY ss_family
#else
#define SOCKADDR struct sockaddr
#define SAFAMILY sa_family
#endif

#if defined(HAVE_DBUS) || defined(HAVE_MPRIS)
enum dbus_session_type {
  DBT_system = 0, // use the session bus
  DBT_session,    // use the system bus
} dbt_type;
#endif

enum endian_type {
  SS_LITTLE_ENDIAN = 0,
  SS_PDP_ENDIAN,
  SS_BIG_ENDIAN,
} endian_type;

enum stuffing_type {
  ST_basic = 0, // straight deletion or insertion of a frame in a 352-frame packet
  ST_soxr,      // use libsoxr to make a 352 frame packet one frame longer or shorter
} s_type;

enum playback_mode_type {
  ST_stereo = 0,
  ST_mono,
  ST_reverse_stereo,
  ST_left_only,
  ST_right_only,
} playback_mode_type;

enum volume_control_profile_type {
  VCP_standard = 0,
  VCP_flat,
} volume_control_profile_type;

enum decoders_supported_type {
  decoder_hammerton = 0,
  decoder_apple_alac,
} decoders_supported_type;

// the following enum is for the formats recognised -- currently only S16LE is recognised for input,
// so these are output only for the present

enum sps_format_t {
  SPS_FORMAT_UNKNOWN = 0,
  SPS_FORMAT_S8,
  SPS_FORMAT_U8,
  SPS_FORMAT_S16,
  SPS_FORMAT_S24,
  SPS_FORMAT_S24_3LE,
  SPS_FORMAT_S24_3BE,
  SPS_FORMAT_S32,
} sps_format_t;

typedef struct {
  config_t *cfg;
  double airplay_volume; // stored here for reloading when necessary
  char *appName;         // normally the app is called shairport-syn, but it may be symlinked
  char *password;
  char *service_name; // the name for the shairport service, e.g. "Shairport Sync Version %v running
                      // on host %h"
#ifdef CONFIG_PA
  char *pa_application_name; // the name under which Shairport Sync shows up as an "Application" in
                             // the Sound Preferences in most desktop Linuxes.
// Defaults to "Shairport Sync". Shairport Sync must be playing to see it.
#endif
#ifdef CONFIG_METADATA
  int metadata_enabled;
  char *metadata_pipename;
  char *metadata_sockaddr;
  int metadata_sockport;
  size_t metadata_sockmsglength;
  int get_coverart;
#endif
  uint8_t hw_addr[6];
  int port;
  int udp_port_base;
  int udp_port_range;
  int ignore_volume_control;
  int volume_max_db_set; // set to 1 if a maximum volume db has been set
  int volume_max_db;
  int no_sync;            // disable synchronisation, even if it's available
  int no_mmap;            // disable use of mmap-based output, even if it's available
  double resyncthreshold; // if it get's out of whack my more than this number of seconds, resync.
                          // Zero means never
                          // resync.
  int allow_session_interruption;
  int timeout; // while in play mode, exit if no packets of audio come in for more than this number
               // of seconds . Zero means never exit.
  int dont_check_timeout; // this is used to maintain backward compatability with the old -t option
                          // behaviour; only set by -t 0, cleared by everything else
  char *output_name;
  audio_output *output;
  char *mdns_name;
  mdns_backend *mdns;
  int buffer_start_fill;
  int64_t userSuppliedLatency; // overrides all other latencies -- use with caution
  int64_t fixedLatencyOffset;  // add this to all automatic latencies supplied to get the actual
                               // total latency
  // the total latency will be limited to the min and max-latency values, if supplied
  int daemonise;
  int daemonise_store_pid; // don't try to save a PID file
  char *piddir;
  char *computed_piddir; // the actual pid directory to create, if any

  int logOutputLevel;              // log output level
  int debugger_show_elapsed_time;  // in the debug message, display the time since startup
  int debugger_show_relative_time; // in the debug message, display the time since the last one
  int statistics_requested, use_negotiated_latencies;
  enum playback_mode_type playback_mode;
  char *cmd_start, *cmd_stop, *cmd_set_volume;
  int cmd_blocking, cmd_start_returns_output;
  double tolerance; // allow this much drift before attempting to correct it
  enum stuffing_type packet_stuffing;
  int decoders_supported;
  int use_apple_decoder; // set to 1 if you want to use the apple decoder instead of the original by
                         // David Hammerton
  char *pidfile;
  // char *logfile;
  // char *errfile;
  char *configfile;
  char *regtype; // The regtype is the service type followed by the protocol, separated by a dot, by
                 // default “_raop._tcp.”.
  char *interface;     // a string containg the interface name, or NULL if nothing specified
  int interface_index; // only valid if the interface string is non-NULL
  double audio_backend_buffer_desired_length; // this will be the length in seconds of the
                                              // audio backend buffer -- the DAC buffer for ALSA
  double audio_backend_latency_offset; // this will be the offset in seconds to compensate for any
                                       // fixed latency there might be in the audio path
  double audio_backend_silent_lead_in_time; // the length of the silence that should precede a play.
  uint32_t volume_range_db; // the range, in dB, from max dB to min dB. Zero means use the mixer's
                            // native range.
  enum sps_format_t output_format;
  enum volume_control_profile_type volume_control_profile;
  int output_rate;

#ifdef CONFIG_CONVOLUTION
  int convolution;
  const char *convolution_ir_file;
  float convolution_gain;
  int convolution_max_length;
#endif

  int loudness;
  float loudness_reference_volume_db;
  int alsa_use_hardware_mute;
#if defined(HAVE_DBUS)
  enum dbus_session_type dbus_service_bus_type;
#endif
#if defined(HAVE_MPRIS)
  enum dbus_session_type mpris_service_bus_type;
#endif

#ifdef HAVE_METADATA_HUB
  char *cover_art_cache_dir;
  int scan_interval_when_active;   // number of seconds between DACP server scans when playing
                                   // something (1)
  int scan_interval_when_inactive; // number of seconds between DACP server scans playing nothing
                                   // (3)
  int scan_max_bad_response_count; // number of successive bad results to ignore before giving up
                                   // (10)
  int scan_max_inactive_count;     // number of scans to do before stopping if not made active again
                                   // (about 15 minutes worth)
#endif
  int disable_resend_requests; // set this to stop resend request being made for missing packets
  double diagnostic_drop_packet_fraction; // pseudo randomly drop this fraction of packets, for
                                          // debugging. Currently audio packets only...
} shairport_cfg;

uint32_t nctohl(const uint8_t *p); // read 4 characters from *p and do ntohl on them
uint16_t nctohs(const uint8_t *p); // read 2 characters from *p and do ntohs on them

void memory_barrier();

// true if Shairport Sync is supposed to be sending output to the output device, false otherwise

int get_requested_connection_state_to_output();

void set_requested_connection_state_to_output(int v);

ssize_t non_blocking_write(int fd, const void *buf, size_t count); // used in a few places

/* from
 * http://coding.debuntu.org/c-implementing-str_replace-replace-all-occurrences-substring#comment-722
 */
char *str_replace(const char *string, const char *substr, const char *replacement);

// based on http://burtleburtle.net/bob/rand/smallprng.html

void r64init(uint64_t seed);
uint64_t r64u();
int64_t r64i();

void r64arrayinit();
uint64_t ranarray64u();
int64_t ranarray64i();

extern int debuglev;
void die(const char *format, ...);
void warn(const char *format, ...);
void inform(const char *format, ...);
void debug(int level, const char *format, ...);

uint8_t *base64_dec(char *input, int *outlen);
char *base64_enc(uint8_t *input, int length);

#define RSA_MODE_AUTH (0)
#define RSA_MODE_KEY (1)
uint8_t *rsa_apply(uint8_t *input, int inlen, int *outlen, int mode);

// given a volume (0 to -30) and high and low attenuations in dB*100 (e.g. 0 to -6000 for 0 to -60
// dB), return an attenuation depending on a linear interpolation along along the range
double flat_vol2attn(double vol, long max_db, long min_db);

// given a volume (0 to -30) and high and low attenuations in dB*100 (e.g. 0 to -6000 for 0 to -60
// dB), return an attenuation depending on the transfer function
double vol2attn(double vol, long max_db, long min_db);

// return a monolithic (always increasing) time in nanoseconds
uint64_t get_absolute_time_in_fp(void);

// time at startup for debugging timing
uint64_t fp_time_at_startup, fp_time_at_last_debug_message;

// this is for reading an unsigned 32 bit number, such as an RTP timestamp

long endianness;
uint32_t uatoi(const char *nptr);

shairport_cfg config;
config_t config_file_stuff;

void command_start(void);
void command_stop(void);
void command_set_volume(double volume);

int mkpath(const char *path, mode_t mode);

void shairport_shutdown();
// void shairport_startup_complete(void);

extern sigset_t pselect_sigset;

// wait for the specified time in microseconds -- it checks every 20 milliseconds
int sps_pthread_mutex_timedlock(pthread_mutex_t *mutex, useconds_t dally_time,
                                const char *debugmessage, int debuglevel);
// wait for the specified time, checking every 20 milliseconds, and block if it can't acquire the
// lock
int _debug_mutex_lock(pthread_mutex_t *mutex, useconds_t dally_time, const char *filename,
                      const int line, int debuglevel);

#define debug_mutex_lock(mu, t, d) _debug_mutex_lock(mu, t, __FILE__, __LINE__, d)

int _debug_mutex_unlock(pthread_mutex_t *mutex, const char *filename, const int line,
                        int debuglevel);

#define debug_mutex_unlock(mu, d) _debug_mutex_unlock(mu, __FILE__, __LINE__, d)

char *get_version_string(); // mallocs a string space -- remember to free it afterwards

void sps_nanosleep(const time_t sec,
                   const long nanosec); // waits for this time, even through interruptions

#endif // _COMMON_H
