#ifndef ORM_CONCEPTS_HPP
#define ORM_CONCEPTS_HPP

namespace std
{
    template<typename ..._Types>
    class variant;
}

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

namespace Query
{
    class Builder;
    class Expression;
}
    using QueryBuilder = Query::Builder;

    /*! Concept for a queryable parameter. */
    template<typename T>
    concept Queryable = std::convertible_to<T, Orm::QueryBuilder &> ||
                        std::invocable<T, Orm::QueryBuilder &>;

    /*! Concept for whereSub()'s value parameter. */
    template<typename T>
    concept WhereValueSubQuery = Queryable<T>;

    /*! Concept for the subquery, used in the from clause (tablename), join clause, ... */
    template<typename T>
    concept SubQuery = std::convertible_to<T, QString> ||
                       Queryable<T>;

    /*! Concept for where()'s value parameter. */
    template<typename T>
    concept WhereValue = std::convertible_to<T, const QVariant &> ||
                         WhereValueSubQuery<T>;

    /*! Concept for the join's table parameter. */
    template<typename T>
    concept JoinTable = std::same_as<T, Query::Expression> ||
                        std::convertible_to<T, QString>;

    /*! Concept for the table column. */
    template<typename T>
    concept ColumnConcept =
            std::convertible_to<T, std::variant<QString, Query::Expression>>;

    /*! Concept for the QString. */
    template<typename T>
    concept QStringConcept = std::convertible_to<T, const QString &> ||
                             std::convertible_to<T, QString>;

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // ORM_CONCEPTS_HPP
