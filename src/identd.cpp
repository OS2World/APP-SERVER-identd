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
 *               accepts rfc1413 (identd) requests, and returns either
 *               "ERROR: INVALID-PORT" if it could not understand the request,
 *               or "OS/2: FOOBAR" where 'foobar' is whatever your USER
 *               environment variable or is set to 'os2-user' if it is not
 *               set or the option passed by -u.
 *
 *   Original: 1.1   Roddy's wee identd/2  - 31 October 1997
 *   Version   1.2   Michael Greene <greenemk@cox.net> - 08 August 2004
 *                   Compiles with Watcom 1.3
 *   Version   1.2a  Changed to use getopt() from glibc 2.3.3 -12 August 2004
 *   Version   1.3   Number of changes, add syslogd logging, and corrected
 *                   some RFC message format problems.
 *
 ***************************************************************************/

#define  INCL_WINWINDOWMGR

#include  <os2>
#include  <iostream>
#include  <cstdio>
#include  <cstdarg>
#include  <types>
#include  <sys\time>
#include  <sys/socket>
#include  <netinet/in>
#include  <arpa\inet>
#include  <netdb>
#include  <unistd>
#include  <ctime>
#include  <signal>
#include  "getopt.h"
#include  "syslog.h"
#include  "identd.hpp"

using std::cout;
using std::endl;
using std::sprintf;
using std::snprintf;
using std::vsnprintf;
using std::sscanf;
using std::time;
using std::localtime;
using std::strftime;

static int  setup_server(int port);
static int  accept_connection(int sock);
static void handle_connection(int sock);
static void setupresponse( void );
static char *gettimestamp( void );
static void handleinternalmsg(int severity, int msgnum);
static void logerror(int severity, int msgnum, const char *fmt, ...);
static void usage( void );
static void cleanup(int signo);

OPTIONS prg_options = { DEFAULT_PORT,
                        DEFAULT_TIMEOUT,
                        DEFAULT_CONNECTS,
                        DEFAULT_SYSLOG_ENABLE,
                        DEFAULT_SYSLOG_FAC,
                        DEFAULT_CHARSET
                      };

static int   serverSock = 0;
static char  messagebuffer[BUFFER_SIZE];
static char  responsemessage[BUFFER_SIZE];
static char  timebuffer[15];
static char  charbuff[11];
static char  sysnbuff[6];

struct  sockaddr_in  serv_addr;
struct  sockaddr_in  clientAddr;


// main( ) Start ****************************************************
//
//
int main(int argc, char *argv[])
{
    int  ch;
    int  clientSock;

    // Get the USER response variable.  If USER is set then use it or set
    // it to the DEFAULT_RESPONSE_STRING which is "os2user" defined in
    // identd.hpp.  Can over ride with the command line option [-u user]
    prg_options.respvar   = getenv("USER");  // check ENV for USER else os2user
    if(!prg_options.respvar) prg_options.respvar = DEFAULT_RESPONSE_STRING;

    // Parse the command line arguments and assign, if required.
    while ((ch = getopt(argc, argv, "ho:p:t:c:u:sv:")) != -1)
    {
        switch (ch)
        {
           case 'c': prg_options.connects = atoi(optarg);
                     break;

           case 'h': usage( );
                     exit(1);
                     break;

           case 'o': prg_options.systemname = (atoi(optarg) + 21);
                     break;

           case 'p': prg_options.useport = atoi(optarg);
                     break;

           case 's': prg_options.syslog = 1;
                     break;

           case 't': prg_options.timeout = atoi(optarg);
                     break;

           case 'u': prg_options.respvar = optarg;
                     break;

           case 'v': prg_options.charset = (atoi(optarg) + 29);
                     if(prg_options.charset < 30 || prg_options.charset > 39)
                     {
                         handleinternalmsg(0, STERROR_1);
                         cout << endl;
                         usage( );
                         exit(1);
                     }
                     break;

           default:  usage( );
                     exit(1);
                     break;
        }
    }
    if ((argc -= optind) != 0) usage();
    // end of parse args

    // prg_options.
    setupresponse( );

    serverSock = setup_server(prg_options.useport);

    // EXLST_ADD (1) Add an address to the termination list.
    // Org: DosExitList(EXLST_ADD | 0x00000000, (PFNEXITLIST) cleanup);
    // DosExitList(EXLST_ADD | 0x00000000, (PFNEXITLIST)cleanup);

    (void)signal(SIGTERM, cleanup);
    (void)signal(SIGINT,  cleanup);

    // working loop *************************************************
    while (1)
    {
        clientSock = accept_connection(serverSock);

        handle_connection(clientSock);

        if (shutdown(clientSock,2) == -1) handleinternalmsg(LOG_CRIT, ERROR_8);

        if (soclose(clientSock) == -1)
        {
            handleinternalmsg(LOG_CRIT, ERROR_8);
            cleanup(ERROR_8);
        }
    }  // End of working loop
}  //$* End of Main


// cleanup()
//
//  Added to ExitList (DosExitList) to cleanup on exit.
//
void cleanup(int signo)
{
    if (soclose(serverSock) == -1) handleinternalmsg(LOG_ERR, ERROR_9);

    cout << gettimestamp( ) << " : " << "Identd/2 shutting down" << endl;

    syslog((LOG_AUTH | LOG_INFO), "%s", "Identd/2 shutting down");
    closelog( );

    exit(signo);

}  //$* End of cleanup


// setup_server
//
int setup_server(int port)
{
    BOOL flag = 1;

    if (sock_init())
    {
        handleinternalmsg(LOG_CRIT, ERROR_2);
        cleanup(ERROR_2);
    }

    int sockdescript = socket(AF_INET, SOCK_STREAM, 0);

    if (sockdescript == -1)
    {
        handleinternalmsg(LOG_CRIT, ERROR_3);
        cleanup(ERROR_3);
    }

    memset(&serv_addr, (char) 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    // setsockopt so I can stop and restart the server - set socket to reuse.
    if(setsockopt(sockdescript, SOL_SOCKET, SO_REUSEPORT,
                  (const char*)&flag,sizeof(flag)) == -1)
    {
        handleinternalmsg(LOG_CRIT, ERROR_1);
        cleanup(ERROR_1);
    }

    if (bind(sockdescript, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
    {
        handleinternalmsg(LOG_CRIT, ERROR_4);
        cleanup(ERROR_4);
    }

    if (listen(sockdescript, prg_options.connects))
    {
        handleinternalmsg(LOG_CRIT, ERROR_5);
        cleanup(ERROR_5);
    }

    return(sockdescript);
}  //$* End of setup_server


// accept_connection
//
int accept_connection(int sock)
{

    int addrSize = sizeof(clientAddr);

    int newSock = accept(sock, (struct sockaddr *)&clientAddr, &addrSize);
    if (newSock == -1)
    {
        handleinternalmsg(LOG_CRIT, ERROR_6);
        cleanup(ERROR_6);
    }

    struct hostent *h = gethostbyaddr((char *) &clientAddr.sin_addr,
                                      sizeof(clientAddr.sin_addr),
                                      AF_INET);

    char *hname = (h == 0) ? (char *)inet_ntoa(clientAddr.sin_addr) : h->h_name;

    (void)strset(messagebuffer, '\0');

    sprintf(messagebuffer, "connect from %s [%s]",
                           (char *)inet_ntoa(clientAddr.sin_addr),
                           hname);
    logerror(LOG_NOTICE, 0, messagebuffer);

    return(newSock);

}  //$* End of accept_connection


// handle_connection
//
void handle_connection(int sock)
{
    int socks[1];
    long timeout = (long)prg_options.timeout * 1000;

    int  port_server;
    int  port_client;
    int  bytes_received;
    char working_buffer[BUFFER_SIZE];

    socks[0] = sock;

    strset(messagebuffer, '\0');

    int timer = os2_select(socks, 1, 0, 0, timeout);

    // os2_select error timer = -1
    if(timer == -1)
    {
        handleinternalmsg(LOG_ERR, ERROR_7);
        return;
    }

    if(timer == 0)
    {
        sprintf(messagebuffer, "0 , 0 : ERROR : connection timed out");
        logerror(LOG_INFO, 0, messagebuffer);
    }
    else if(timer == 1)
         {
             bytes_received = recv(sock, working_buffer, sizeof(working_buffer), 0);

             if (bytes_received > 0)
             {
                 if (sscanf(working_buffer, "%d , %d", &port_server, &port_client) != 2)
                                sprintf(messagebuffer, "0 , 0 : ERROR : INVALID-PORT");
                 else
                 {
                     sprintf(messagebuffer,
                             "%d , %d%s",
                             port_server, port_client, responsemessage);
                     if (send(sock, messagebuffer, strlen(messagebuffer), 0) == -1)
                                                  handleinternalmsg(LOG_ERR, ERROR_10);
                 }
                 logerror(LOG_INFO, 0, messagebuffer);
             }
             else
             {
                 sprintf(messagebuffer, "0 , 0 : ERROR : UNKNOWN-ERROR");
                 logerror(LOG_INFO, 0, messagebuffer);
             }
         }
}  //$* End of handle_connection


// handleinternalmsg( )
//
//  Message numbers passed are pulled from stringtable and send to stderr.
//
void handleinternalmsg(int severity, int msgnum)
{
    char    StringBuf[256];
    HAB     hab;

    (void)WinLoadString(hab, NULLHANDLE, msgnum, sizeof(StringBuf), StringBuf);

    logerror(severity, msgnum, StringBuf);

}  //$* end of handleinternalmsg


// logerror( )
//
void logerror(int severity, int msgnum, const char *fmt, ...)
{
    va_list ap;
    char tmpbuf[BUFFER_SIZE];
    char buf[BUFFER_SIZE];

    va_start(ap, fmt);

    (void)vsnprintf(tmpbuf, sizeof(tmpbuf), fmt, ap);

    va_end(ap);

    (void)snprintf(buf, sizeof(buf), "%s", tmpbuf);

    cout << gettimestamp( ) << " : " << buf << endl;

    if(prg_options.syslog && msgnum < 40)
    {
        int pri = (LOG_AUTH | severity);

        syslog(pri, "%s", buf);
        closelog( );
    }
    return;

}  //$* end of logerror


// gettimestamp( )
//
//  TIMESTAMP format "Mmm dd hh:mm:ss"
//
char *gettimestamp( void )
{
    time_t TimeOfDay;
    char *p;

    TimeOfDay = time( NULL );
    strftime(timebuffer, 15, "%b %d %T", localtime(&TimeOfDay));  // timestamp
    p = timebuffer;

    return(p);

}  //$* end of gettimestamp


// usage( )
//
//  display the commandline usage for indentd/2
//
void usage( void )
{

cout << "Identd/2  v1.31 for OS/2  " << __DATE__ << " Compiled with Watcom 1.3" << endl
     << "              Michael Greene <greenemk@cox.net>" << endl << endl;

cout << "usage: identd [-h] [ [-s n] [-p port] [-t timeout] [-c #connect]\n"
        "                     [-u user] [-v charset]]\n\n"
        " -h       Display usage.\n\n"
        " -c n     Number of connects allowed on socket. (Default: 10)\n\n"
        " -s       Enable syslogd logging (LOG_AUTH)\n\n"
        " -t n     where n is how many seconds to wait between a connection\n"
        "          and the completion of the transaction.  (Default: 30)\n\n"
        " -o n     Use alternate system name (Default: OS/2)\n"
        "            1 eCS            2 UNIX           3 OTHER\n\n"
        " -p n     where n is the port to listen to.  (Default: 113)\n\n"
        " -u user  Response string, overrides environment variable USER.\n"
        "          (Default: os2user)\n\n"
        " -v n     Send an alternate character set (Default: None), where\n"
        "          n is 1-10:\n"
        "            1 US-ASCII       5 ISO-8859-4     9  ISO-8859-8\n"
        "            2 ISO-8859-1     6 ISO-8859-5     10 ISO-8859-9\n"
        "            3 ISO-8859-2     7 ISO-8859-6     \n"
        "            4 ISO-8859-3     8 ISO-8859-7     \n\n";
}  //$* end of usage


// setupresponse( )
//
//  Output the program settings and response string for user verification
//
void setupresponse( void )
{
    HAB     hab;

    if(prg_options.charset != 0)
    {
        (void)WinLoadString(hab, NULLHANDLE, prg_options.charset, 11, charbuff);
    }

    handleinternalmsg(LOG_INFO, INFO_1);

    sprintf(messagebuffer, "Options: Port %d  Timeout %d  Connects %d",
               prg_options.useport, prg_options.timeout, prg_options.connects);
    logerror(LOG_INFO, 0, messagebuffer);

    if(prg_options.syslog)
    {
        strset(messagebuffer, '\0');

        sprintf(messagebuffer, "Options: Syslogd enabled");
        logerror(LOG_INFO, 0, messagebuffer);
    }

    strset(messagebuffer, '\0');

    if(prg_options.systemname < 21 || prg_options.systemname > 24)
                                                 prg_options.systemname = 21;

    (void)WinLoadString(hab, NULLHANDLE, prg_options.systemname, 6, sysnbuff);


    if(prg_options.charset != 0)
    {
        sprintf(responsemessage, " : USERID : %s , %s : %s",
                                sysnbuff, charbuff, prg_options.respvar);
    }
    else
    {
        sprintf(responsemessage, " : USERID : %s : %s",
                                sysnbuff, prg_options.respvar);
    }

    sprintf(messagebuffer, "Resp: <port-pair> %s", responsemessage);
    logerror(LOG_INFO, 0, messagebuffer);

}  //$* end of setupresponse

