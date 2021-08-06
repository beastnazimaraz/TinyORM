#pragma once
#ifndef CONNECTORINTERFACE_HPP
#define CONNECTORINTERFACE_HPP

#include <QVariantHash>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm::Connectors
{
    using ConnectionName = QString;

    /*! Connectors interface class. */
    class ConnectorInterface
    {
    public:
        /*! Default virtual destructor. */
        inline virtual ~ConnectorInterface() = default;

        /*! Establish a database connection. */
        virtual ConnectionName connect(const QVariantHash &config) const = 0;
    };

} // namespace Orm::Connectors
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONNECTORINTERFACE_HPP
