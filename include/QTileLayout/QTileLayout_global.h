#ifndef QTILELAYOUT_GLOBAL_H
#define QTILELAYOUT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTILELAYOUT_LIBRARY)
#define QTILELAYOUT_EXPORT Q_DECL_EXPORT
#else
#define QTILELAYOUT_EXPORT Q_DECL_IMPORT
#endif

#endif // QTILELAYOUT_GLOBAL_H
