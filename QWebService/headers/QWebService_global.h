#ifndef QWEBSERVICE_GLOBAL_H
#define QWEBSERVICE_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>

#if defined(QWEBSERVICE_LIBRARY)
#  define QWEBSERVICESHARED_EXPORT Q_DECL_EXPORT
#else
#  define QWEBSERVICESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QWEBSERVICE_GLOBAL_H
