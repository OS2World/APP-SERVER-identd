
                              IDENTD v1.31


Changes since v1.3:

- Fixed shutdown error.

By request:

- made the alternate character set optional
- provide the ability to send ECS instead of OS/2


I have also received an email that the individual believes that
identd is causing his system to lockup after 2-3 days. I have been
using identd on my gateway server with no lockups (system running MCP2).

If anyone runs into this problem email me and try to increase allowable
connections ( -c #).



Identd/2  v1.31 for OS/2  Oct 26 2004 Compiled with Watcom 1.3
              Michael Greene <greenemk@cox.net>

usage: identd [-h] [ [-s n] [-p port] [-t timeout] [-c #connect]
                     [-u user] [-v charset]]

 -h       Display usage.

 -c n     Number of connects allowed on socket. (Default: 10)

 -s       Enable syslogd logging (LOG_AUTH)

 -t n     where n is how many seconds to wait between a connection
          and the completion of the transaction.  (Default: 30)

 -o n     Use alternate system name (Default: OS/2)
            1 eCS            2 UNIX           3 OTHER

 -p n     where n is the port to listen to.  (Default: 113)

 -u user  Response string, overrides environment variable USER.
          (Default: os2user)

 -v n     Send an alternate character set (Default: None), where
          n is 1-10:
            1 US-ASCII       5 ISO-8859-4     9  ISO-8859-8
            2 ISO-8859-1     6 ISO-8859-5     10 ISO-8859-9
            3 ISO-8859-2     7 ISO-8859-6
            4 ISO-8859-3     8 ISO-8859-7


Changes since v1.2:

Well, thanks to Alexander Ostapchuk pointing out a format problem in
the sent message.

While I was fixing, the option to have identd log to syslogd if identd is
started with the -s option. In addition, I added -v option so other than
the default US-ASCII character set can be used - I don't know if it even makes
a difference.




When identd is started the following is displayed:

   Oct 15 07:34:02 : Identd/2 starting
   Oct 15 07:34:02 : Options: Port 113  Timeout 30  Connects 10
 * Oct 15 07:34:02 : Options: Syslogd enabled
   Oct 15 07:34:02 : RESPOND: <port-pair> : USERID : OS/2 , US-ASCII : os2-user

* This line will only be displayed if -s option is used.

Line 4 is the message that will be sent based on your options.


In Chatzilla you should see this if identd is working:

	===	*** Looking up your hostname
	===	*** Checking Ident
	===	*** Found your hostname
	===	*** Got ident response        <=== identd working



Also, in the src directory I made an attempted a small makefile. I am not very 
good at makefiles, I use the IDE.


Mike Greene
<greenemk@cox.net or greenemk@gmail.com>

http://groups-beta.google.com/group/openwatcomos2/about
http://members.cox.net/mikeos2/ 

IRC: http://www.os2irc.org/      #os/2warp     MikeG
     or 
     http://www.webbnet.info/    #os/2         MikeG


  Help support the Open Watcom (http://www.openwatcom.org/) and Voice 
  (http://www.os2voice.org/)


*******************************************************************************


1.  IDENTD 1.2 README

2.  ORIGINAL IDENTD README.TXT


1.  IDENTD 1.2 README 

  Here is the quick and dirty.  I am learning C / C++ and decided to fix
  someone troubles Mr. William Hartzell was having with the identd server by 
  Roddy Collins (v1.1 1997).  The source that I used is included and ready to
  be re-built with Watcom.  Just execute identd -h to get the options.

    This is a small identd (RFC 1413) server; it returns the USER
    environment variable in response to any well-formed identd request.
    It has the following options:
 
    usage: identd [-h] [[-p port] [-t timeout] [-c #connect] [-u user]]
 
      -h       Display usage.

      -p n,   where n is the port to listen to.  (Default: 113)
 
      -t n,   where n is how many seconds to wait between a connection and
              the completion of the transaction.  (Default: 30)

      -c n,   number of connects allowed on socket. (Default: 10)
 
      -u user response string, overrides environment variable USER (Default: os2user)
 
     accepts rfc1413 (identd) requests, and says either "ERROR: INVALID-PORT"
     if it couldn't understand the request, or "OS/2: FOOBAR" where
     'foobar' is whatever your USER environment variable is set to (or 'os2-user'
     if it's not set or the option passed by -u)

  * Verbose option can be passed on the command line but is not used.


  Example start and output:

  [D:\source\identd12\source]identd -u OS2USER -c 9 -t 10
  Starting with options:
  Port: 113  Timeout: 10  User: OS2USER  Connects: 9

  Sun Aug  8 14:39:24 2004 : connect from 192.168.1.7 [192.168.1.7 ]
  Sun Aug  8 14:39:25 2004 : Request was 113,52535
  Sun Aug  8 14:39:25 2004 : Response was 113, 52535 : OS/2 : OS2USER
  Sun Aug  8 14:39:32 2004 : connect from 192.168.1.7 [192.168.1.7 ]
  Sun Aug  8 14:39:42 2004 : ERROR : CONNECTION TIMEOUT
  Sun Aug  8 14:40:01 2004 : connect from 192.168.1.7 [192.168.1.7 ]
  Sun Aug  8 14:40:02 2004 : Request was
  Sun Aug  8 14:40:02 2004 : Response was 0 , 0 : ERROR : INVALID-PORT

  In this example I started with options *just as an example* timeout should be left
  in default (30 seconds) or higher.  Connections works good at 10 (default), the old
  identd had 5 connections hardcoded at 5 on the socket.

  At start the options identd are displayed.

  The first 3 lines (prefixed with Sun Aug  8... ) are a normal ident connection:
      1. connection from
      2. requested
      3. this was a good port pair request so respond: port pair : system : userid

  The next 2 lines show a connection made and then timeout (no request sent).

  The last three lines show a connection, bad request, and the error reply 
  (0 , 0 : ERROR : INVALID-PORT).  I changed the function so if a telnet connection
  is attempted the first key is displayed and the ERROR : INVALID-PORT is sent.  In 
  the following I made a telnet connection to the ident server and pressed "1":

  Sun Aug  8 14:49:32 2004 : connect from 192.168.1.7 [192.168.1.7 ]
  Sun Aug  8 14:49:34 2004 : Request was 1
  Sun Aug  8 14:49:34 2004 : Response was 0 , 0 : ERROR : INVALID-PORT



  Help support the Open Watcom (http://www.openwatcom.org/) and Voice 
  (http://www.os2voice.org/).

  Michael Greene (greenemk@cox.net)
  Chesapeake, Virginia  8 August 2004

  IRC: Find a close server at http://www.webbnet.info/  
       - I am usually on #OS/2 or #voice - MikeG
 

2. ORIGINAL IDENTD README.TXT:

  Yet another identd server... provided both as a hopefully useful
  widget in itself and in the hopes that its source code will be useful
  to others writing tcp/ip applications for OS/2.

  This is a small identd (RFC 1413) server; it returns the USER
  environment variable in response to any well-formed identd request.
  It has three options:

    -v n,   n=0,1,2:  how verbose it should be.  0 will produce no output;
            1 will print requests and responses on stderr, and 2 will
            also add a little debugging information.  (Default: 1)

    -t n,   where n is how many seconds to wait between a connection and
            the completion of the transaction.  (Default: 30)

    -p n,   where n is the port to listen to.  (Default: 113)


  The program and code come with no warranty; it stands so absolutely
  bereft of quality assessment that if assurances were sunbeams the
  executable would be shivering in a cold, dark cave.  However, you have
  the source code, and you can do whatever you want with it... 

  It's also free; the only cost is an extremely mild request that I'd
  like to know if either the program or the code is useful (or buggy)
  but I've certainly ignored my share of similar requests in the past and
  I'll understand if you ignore this one, too... :)

  Roddy
  collinsr@cs.rpi.edu



 
