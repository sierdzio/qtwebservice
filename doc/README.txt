QWebService library helps connecting to and communicating with web services in Qt applications.
qtWsdlConverter automatically creates Qt/C++ classes for a given WSDL file or URL.

Author: Tomasz Siekierda
Nickname: sierdzio
Email: sierdzio@gmail.com

Distributed under GNU LGPL 2.1.
Remote Features and TODO's list can be found at: http://goo.gl/L1C6X
Short introductory video can be found at: http://www.youtube.com/watch?v=Y4cqrBBi-JA

------------------
1. QWebService
This project provides a set of classes (compilable into a library, if you prefer), that help quickly access a web service, send and receive messages, gather web service information etc.

1.1 Classes (* indicates possible major API changes in the future)

  1.1.1 QWebMethod (formerly QSoapMessage)
  Supports HTTP, SOAP 1.0, SOAP 1.2 and JSON, XML and RESTful web services.
  Used to send messages to a web service, and read replys from it. Can be used asynchronously (indicates, when reply is ready by emitting a replyReady() signal). If you need synchronous operation,either use QWebServiceMethod, or subclass QWbMethod.

  1.1.2 QWsdl
  Useful for reading web service description contained in WSDL file. In the constructor, or using setWsdlFile(), or resetWsdl(), you can specify an URL to a web service's description, or - if you have one - a path to a local WSDL file. Some example files can be found in 'examples' forlder in project's source.

  1.1.3 QWebService (formerly QWebServiceAbstract)
  In it's current shape, this is mostly a QWsdl wrapper class which hides the WSDL parsing machinery and exposes useful stuff only.

  1.1.4 QWebServiceReader (formerly QWebServiceReaderAbstract) (*)
  A derivative of QWebService, more focused on "reading" a web service (that is, sending messages and receiving replies). Currently not very innovative, it does nothing more that QWebService. Possible updates in the future, bringing new functionality.

  1.1.5 QwebServiceServer (or QWebServiceWriter) (*)
  Currently does not exist. A proposed class, derived from QWebService, aimed at providing web service server functionality.

  1.1.6 QWebServiceMethod
  Subclass of QWebMethod, contains many generic methods for sending messages. Can be used both synchronously (through static sendMessage() method), or asynchronously (indicates, when reply is ready by emitting a replyReady() signal).

------------------
2. qtWsdlConverter

WARNING: Remember to first compile QWebService library, and set it's path in run environment for Converter (it requires this lib to run. By default, it puts the library into ../lib).

2.1 Syntax
  qtwsdlconvert [options] <WSDL file or URL> [output directory, defaults to web service name] [base output class name, defaults to web service name]

  2.1.1 Possible options (unavailable marked with *)
    --help,
    --soap10, --soap12, --http, --json, *--xml, *--rest={POST, GET, PUT, DELETE, post, get, put, delete}
    --synchronous, --asynchronous,
    --subclass, --full-mode, *--debug-mode, *(partial)--compact-mode,
    --standard-structure, --no-messages-structure, --all-in-one-dir-structure,
    --qmake, --cmake, --scons, --no-build-system,
    --objSuffix=, --msgSuffix=,
    --force.

  2.1.2 Default switches
    --synchronous, --soap12, --standard-structure, --full-mode, --qmake

2.2 Meaning
All entries in single sections (with an exception of build system related switches, and '--rest') are exclusive - you can use only one at a time (the lates one specified is used). For example, if you use --http and --json, only JSON flag will be set.

  2.2.1 Protocols
    --soap10 - classes will contact with web service using SOAP1.0 protocol,
    --soap12 - classes will contact with web service using SOAP1.2 protocol,
    --http   - classes will contact with web service using HTTP protocol,
    --json   - classes will contact with web service using JSON protocol,
    --xml    - classes will contact with web service using XML protocol (non-SOAP),
    --rest=  - classes will contact with RESTful web service (this can be combined with previous flags. HTTP method
	       can be specified after equals mark, for example: --rest=POST or --rest=put. If none is given,
	       POST is assumed. Available options: POST, GET, PUT, DELETE.

  2.2.2 Synchronousness
    --synchronous  - communication with server will be performed using synchronous calls,
    --asynchronous - communication with server will be performed using asynchronous calls.

  2.2.3 Modes
    --subclass	   - converter creates messages by subclassing QWebMethod (this option requires QWebService library
		     to be present)
    --full-mode    - created code has all relevant code found in QWebService's classes,
    --debug-mode   - as full-mode, but the code ships with additional debugging information,
    --compact-mode - code is stripped down to bare minimum. For example, only the code connected with chosen protocol
		     is copied into messages' classes (if you chose soap12, only this option will be copied.
		     Other protocols will not be available).

  2.2.4 Structures
    --standard-structure       - headers are copied to <wsName>/headers, sources to <wsName>/sources, build file
				 to <wsName>/<buildFile>,
    --no-messages-structure    - no custom message code will be created. All messages will be sent by using general
				 QWebMethod objects (this option requires QWebService library to be present),
    --all-in-one-dir-structure - as sandard-structure, but all files are copied into <wsName>.

  2.2.5 Build systems
  All build systems can be built simultaneously (that is, you can use --qmake, --scons, --cmake all at once). When --no-build-system is chosen, it overrides all previous settings - turns them off.
    --qmake - a .pro file will be created for the project,
    --cmake - files needed by cmake will be created for the project,
    --scons - files needed by scons will be created for the project,
    --no-build-system - no build-system-related files will be created.

  2.2.6 Suffixes
  --objSuffix=<var>	- appends <var> to object names created in asynchronous mode (default "Msg"),
  --msgSuffix=<var>	- appends <var> to sending methods' names created in asynchronous mode (default "Send").

  2.2.7 Miscellaneous
    --force - if the <wsName> dir already exists, converter will delete and recreate it,
    --help  - displays a simple help message and information. Does not proceed with any other action.