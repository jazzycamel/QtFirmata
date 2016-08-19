#ifndef QTFIRMATA_GLOBAL_H
#define QTFIRMATA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTFIRMATA_LIBRARY)
#  define QTFIRMATASHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTFIRMATASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QTFIRMATA_GLOBAL_H
