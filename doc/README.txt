QWebService library helps connecting to and communicating with web services in Qt applications.
qtWsdlConverter automatically creates Qt/C++ classes for a given WSDL file or URL.

Author: Tomasz Siekierda
Nickname: sierdzio
Email: sierdzio@gmail.com

Distributed under GNU GPLv3.

1. QWebService.



2. qtWsdlConverter

2.1 Syntax
  qtwsdlconvert [options] <WSDL file or URL> [output directory, defaults to web service name] [base output class name, defaults to web service name]

  2.1.1 Possible options
    --soap10, --soap12, --http, --synchronous, --asynchronous, --help,
    --full-mode, --debug-mode, --compact-mode,
    --standard-structure, --no-messages-structure, --all-in-one-dir-structure,
    --qmake, --cmake, --scons, --no-build-system, --force.

  2.1.2 Default switches
    --synchronous, --soap12, --standard-structure, --full-mode, --qmake

2.2 Meaning
  2.2.1 Protocols
    --soap10 - classes will contact with web service using SOAP1.0 protocol,
    --soap12 - classes will contact with web service using SOAP1.2 protocol,
    --http   - classes will contact with web service using HTTP protocol.

  2.2.2 Synchronousness
    --synchronous  - communication with server will be performed using synchronous calls,
    --asynchronous - communication with server will be performed using asynchronous calls.

  2.2.3 Modes
    --full-mode    - created code has all relevant code found in QWebService's classes,
    --debug-mode   - as full-mode, but the code ships with additional debugging information,
    --compact-mode - code is stripped down to bare minimum. For example, only the code connected with chosen protocol is copied into messages' classes (if you chose soap12, only this option will be copied. Other protocols will not be available).

  2.2.4 Structures
    --standard-structure - headers are copied to <wsName>/headers, sources to <wsName>/sources, build file to <wsName>/<buildFile>,
    --no-messages-structure - no custom message code will be created. All messages will be sent by using general QSoapMessage objects,
    --all-in-one-dir-structure - as sandard-structure, but all files are copied into <wsName>.

  2.2.5 Build systems
    --qmake - a .pro file will be created for the project,
    --cmake - files needed by cmake will be created for the project,
    --scons - files needed by scons will be created for the project,
    --no-build-system - no build-system-related files will be created.

  2.2.6 Miscellaneous
    --force - if the <wsName> dir already exists, converter will delete and recreate it,
    --help  - displays a simple help message and information. Does not proceed with any other action.