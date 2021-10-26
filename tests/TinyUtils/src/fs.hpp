#pragma once
#ifndef TINYUTILS_FS_HPP
#define TINYUTILS_FS_HPP

#include <QtGlobal>

#include "export.hpp"

class QString;

namespace TestUtils
{

    /*! Utility functions related to the filesystem. */
    class TINYUTILS_EXPORT Fs final
    {
        Q_DISABLE_COPY(Fs)

    public:
        /*! Deleted Fs's default constructor, this is a pure library class. */
        Fs() = delete;
        /*! Deleted Fs's destructor. */
        ~Fs() = delete;

        /*! Trim and clean a path. */
        static QString cleanPath(const QString &path);

        /*! Return cleaned absolute path. */
        static QString absolutePath(const QString &path);
    };

} // namespace TestUtils

#endif // TINYUTILS_FS_HPP
