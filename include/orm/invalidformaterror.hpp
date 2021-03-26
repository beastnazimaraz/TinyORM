#ifndef INVALIDFORMATERROR_H
#define INVALIDFORMATERROR_H

#include "orm/logicerror.hpp"

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    class SHAREDLIB_EXPORT InvalidFormatError : public LogicError
    {
    public:
        using LogicError::LogicError;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // INVALIDFORMATERROR_H
