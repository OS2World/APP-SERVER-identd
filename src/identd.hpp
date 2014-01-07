/****************************************************************************
 *
 *                 I D E N T D / 2    An IDENTD server for OS/2
 *
 *  ========================================================================
 *
 *    Version 1.3       Michael K Greene <greenemk@cox.net>
 *                      October 2004
 *
 *
 *              IDENTD/2 compiles with OpenWatom (www.openwatcom.org).
 *
 *  ========================================================================
 *
 * Description:  This is a small identd (RFC 1413) server; it returns the
 *               USER environment variable in response to any well-formed
 *               identd request.
 *
 ***************************************************************************/

#define  ERROR_1      1
#define  ERROR_2      2
#define  ERROR_3      3
#define  ERROR_4      4
#define  ERROR_5      5
#define  ERROR_6      6
#define  ERROR_7      7
#define  ERROR_8      8
#define  ERROR_9      9
#define  ERROR_10    10

#define  INFO_1      20

#define  SYSNAM_1    21
#define  SYSNAM_2    22
#define  SYSNAM_3    23
#define  SYSNAM_4    24

#define  CHAR_1      30
#define  CHAR_2      31
#define  CHAR_3      32
#define  CHAR_4      33
#define  CHAR_5      34
#define  CHAR_6      35
#define  CHAR_7      36
#define  CHAR_8      37
#define  CHAR_9      38
#define  CHAR_10     39

#define  STERROR_1   40

// Unless passed on the command line these are the defaults
#define  DEFAULT_PORT             113
#define  DEFAULT_TIMEOUT          30
#define  DEFAULT_CONNECTS         10
#define  DEFAULT_SYSLOG_ENABLE    0              // default syslogd disabled
#define  DEFAULT_SYSLOG_FAC       LOG_AUTH       // default syslogd facility
#define  DEFAULT_CHARSET          0              // default char set US-ASCII
#define  DEFAULT_SYSNAME          SYSNAM_1
#define  DEFAULT_RESPONSE_STRING  "os2-user"

#define BUFFER_SIZE 1024

struct OPTIONS
{
   int   useport;
   int   timeout;
   int   connects;
   int   syslog;
   int   syslogfac;
   int   charset;
   int   systemname;
   char  *respvar;
};


