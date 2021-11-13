#pragma once
#ifndef ORM_EXCEPTIONS_INVALIDTEMPLATEARGUMENTERROR_HPP
#define ORM_EXCEPTIONS_INVALIDTEMPLATEARGUMENTERROR_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/exceptions/invalidargumenterror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Exceptions
{

    /*! Invalid template argument exception. */
    class InvalidTemplateArgumentError : public InvalidArgumentError
    {
        /*! Inherit constructors. */
        using InvalidArgumentError::InvalidArgumentError;
    };

} // namespace Orm::Exceptions

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_EXCEPTIONS_INVALIDTEMPLATEARGUMENTERROR_HPP
