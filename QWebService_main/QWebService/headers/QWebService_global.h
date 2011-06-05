#ifndef QWEBSERVICE_GLOBAL_H
#define QWEBSERVICE_GLOBAL_H

#include <QtCore/qglobal.h>
#include "qsoapmessage.h"
#include "qwsdl.h"
#include "qwebserviceabstract.h"
//#include "qwebservicereaderabstract.h"
#include "qwebservicemethodabstract.h"

#if defined(QWEBSERVICE_LIBRARY)
#  define QWEBSERVICESHARED_EXPORT Q_DECL_EXPORT
#else
#  define QWEBSERVICESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QWEBSERVICE_GLOBAL_H
