#pragma once
#ifndef ORM_TINY_MODEL_HPP
#define ORM_TINY_MODEL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#ifdef __GNUG__
#include <map>
#endif
#include <unordered_map>

#include "orm/concepts.hpp"
#include "orm/concerns/hasconnectionresolver.hpp"
#include "orm/connectionresolverinterface.hpp"
#include "orm/exceptions/invalidformaterror.hpp"
#include "orm/tiny/concerns/guardsattributes.hpp"
#include "orm/tiny/concerns/hasrelationstore.hpp"
#include "orm/tiny/exceptions/massassignmenterror.hpp"
#include "orm/tiny/exceptions/relationnotfounderror.hpp"
#include "orm/tiny/exceptions/relationnotloadederror.hpp"
#include "orm/tiny/modelproxies.hpp"
#include "orm/tiny/relations/belongsto.hpp"
#include "orm/tiny/relations/belongstomany.hpp"
#include "orm/tiny/relations/hasmany.hpp"
#include "orm/tiny/relations/hasone.hpp"
#include "orm/tiny/tinybuilder.hpp"
#include "orm/utils/string.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

    class DatabaseConnection;

namespace Tiny
{
namespace Relations {
    class IsPivotModel;
}

#ifdef TINYORM_TESTS_CODE
    /*! Used by tests to override connection in the Model. */
    struct ConnectionOverride
    {
        /*! The connection to use in the Model, this data member is picked up
            in the Model::getConnectionName(). */
        thread_local
        inline static QString connection;
    };
#endif

    // TODO repeat, from time to time try to compile without microsoft extensions, QMAKE_CXXFLAGS *= -Za silverqx
    // TODO repeat, recompile without PCH silverqx
    // TODO repeat, open cmake generated Visual Studio 2019 solution and fix all errors/warnings silverqx
    // TODO repeat, clang's UndefinedBehaviorSanitizer at https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html silverqx
    // TODO repeat, merge changes from latest Eloquent silverqx
    // TODO decide/unify when to use class/typename keywords for templates silverqx
    // TODO model missing methods Soft Deleting, Model::trashed()/restore()/withTrashed()/forceDelete()/onlyTrashed(), check this methods also on EloquentBuilder and SoftDeletes trait silverqx
    // TODO model missing methods Model::replicate() silverqx
    // TODO model missing methods Comparing Models silverqx
    // TODO model missing methods Model::firstOr() silverqx
    // TODO model missing methods Model::updateOrInsert() silverqx
    // TODO model missing methods Model::loadMissing() silverqx
    // TODO model missing methods Model::whereExists() silverqx
    // TODO model missing methods Model::whereBetween() silverqx
    // TODO model missing methods Model::exists()/notExists() silverqx
    // TODO model missing methods Model::saveOrFail() silverqx
    // TODO model missing methods EloquentCollection::toQuery() silverqx
    // TODO model missing methods Model::whereRow() silverqx
    // FEATURE EloquentCollection, solve how to implement, also look at Eloquent's Custom Collections silverqx
    // FUTURE try to compile every header file by itself and catch up missing dependencies and forward declaration, every header file should be compilable by itself silverqx
    // FUTURE include every stl dependency in header files silverqx
    // FEATURE logging, add support for custom logging, logging to the defined stream?, I don't exactly know how I will solve this issue, design it 🤔 silverqx
    // TODO QueryBuilder::updateOrInsert() silverqx
    // FEATURE code coverage silverqx
    // FEATURE CI/CD silverqx
    // TODO perf, run TinyOrmPlayground 30 times with disabled terminal output and calculate sum value of execution times to compare perf silverqx
    // TODO dilemma, function params. like direction asc/desc for orderBy, operators for where are QStrings, but they should be flags for performance reasons, how to solve this and preserve nice clean api? that is the question 🤔 silverqx
    // CUR1 connection SSL support silverqx
    // BUG Qt sql drivers do not work with mysql json columns silverqx
    // CUR1 add Relations::Concerns::ComparesRelatedModels silverqx
    // FEATURE build systems, add autotools build silverqx
    // FEATURE build systems, add docs on how to make a production build of the TinyORM library silverqx
    // FEATURE build systems, add docs on how to set up dev. environment and how to run auto tests silverqx
    // FEATURE build systems, libuv example how it could look like https://github.com/libuv/libuv silverqx
    // CUR1 reorder all methods in model class silverqx
    // FUTURE cmake can generate export header file by GenerateExportHeader module, find way to use it, because I have own export header file, how to unify this? I don't know now silverqx
    // CUR try clang-include-fixer at https://clang.llvm.org/extra/clang-include-fixer.html silverqx
    // CUR try iwyu at https://include-what-you-use.org/ silverqx
    // CUR generate pkg-config file on unix silverqx
    // CUR cmake use gold linker option, https://github.com/frobware/c-hacks/blob/master/cmake/use-gold-linker.cmake silverqx
    // BUG mingw64, TinyOrmPlayground seg. fault at the end, but everything runs/passes correctly, but only when invoked from mintty terminal, when I run it from QtCreator that uses cmd I don't see any seg. fault silverqx
    // BUG mingw64, seg fault in some tests eg. tst_model, and couldn't execute tests again, mingw64 shell works silverqx
    // FUTURE mingw64, find out better solution for .text section exhausted in debug build, -Wa,-mbig-obj didn't help, -flto helps, but again it can not find reference to WinMain, so I had to compile with -O1, then it is ok silverqx
    // FUTURE tests, QtCreator Qt AutoTests how to pass -maxwarnings silverqx
    // FUTURE constexpr, version header file, good example https://googleapis.dev/cpp/google-cloud-common/0.6.1/version_8h_source.html silverqx
    // BUG qmake impossible to add d postfix for dlls silverqx
    // BUG SemVer version cmake/qmake silverqx
    // CUR cmake/qmake SOVERSION silverqx
    // CUR 0.1.0 vs 0.1.0.0 Product/FileVersion, investigate, also check versions in pc, prl, ... silverqx
    // CUR enable QT_ASCII_CAST_WARNINGS silverqx
    // CUR enable QT_NO_CAST_FROM_ASCII silverqx
    // CUR autoconfigure qmake with qmake/TinyOrm.pri and TINY_ROOT_DIR and TINY_TINYORM_BUILDS_DIR silverqx
    // BUG qmake MinGW UCRT64 clang static build duplicit symbols, this is MinGW bug silverqx
    // BUG qmake MinGW UCRT64 clang shared build with inline_constants cause crashes of 50% of tests, this will be MinGW clang or clang bug, on unix it works without problems silverqx
    // BUG cmake MinGW UCRT64 clang static build builds, but cause problem with inline_constants ; shared build with inline_constants cause crashes of 50% of tests, like bug above, this will be MinGW clang or clang bug, on unix it works without problems silverqx
    // FUTURE linux, add linker version script https://github.com/sailfishos/qtbase/commit/72ba0079c3967bdfa26acdce78ce6cb98b30c27b?view=parallel https://www.gnu.org/software/gnulib/manual/html_node/Exported-Symbols-of-Shared-Libraries.html https://stackoverflow.com/questions/41061220/where-do-object-file-version-references-come-from silverqx
    // TODO Visual Studio memory analyzer https://docs.microsoft.com/en-us/visualstudio/profiling/memory-usage-without-debugging2?view=vs-2019 silverqx
    // CUR fix all modernize-pass-by-value silverqx
    // CUR use using inside classes where appropriate silverqx
    // CUR I'm still not comfortable with using Orm::Constants, inside/outside namespace in cpp/hpp files silverqx
    // CUR using namespace Orm::Constants; in ormtypes.hpp silverqx
    // CUR make global static const silverqx
    // CUR make all * to *const silverqx
    // CUR merge changes from latest Eloquent silverqx
    // CUR unfinished instantiation, DatabaseManager::create() allow to create more instances, so m_instance is useless silverqx
    // CUR DB/DatabaseManager/DatabaseConnection all have defaultConnectionName, merge them? silverqx
    // CUR remove SHAREDLIB_EXPORT on classes in header files only silverqx
    // CUR mingw and pragma system_header ignored outside include file silverqx
    // CUR init all inline static {}, verify whether default ctor is called during dyn. init. silverqx
    // CUR TLS, clang linux crashes, gcc mingw lld.ld doesn't compile (duplicit tls), clang mingw stdlib works silverqx
    // CUR order/prettify/add section names for DM and DatabaseConnection silverqx
    // CUR unify inherit ctors silverqx
    // CUR caught exceptions in tinyplay when threading enabled, they should match with non-threading version silverqx
    // CUR divide tinyplay and all test to smaller TU so decreas memory usage during compilation, and also compilation times overall because it will recompile only small TU silverqx
    // CUR cmake/qmake run tests in parallel for tests that allow it silverqx
    // CUR add NL \n to constants silverqx
    // CUR check Caught expected exception with message in tinyplay silverqx
    // CUR study how to use acquire/release memory order for m_queryLogId atomic silverqx
    // CUR u_guarded in Pivot/BasePivot and TLS silverqx
    // CUR rename m_db to m_dm in tinyplay silverqx
    /*! Base model class. */
    template<typename Derived, AllRelationsConcept ...AllRelations>
    class Model :
            public ModelProxies<Derived, AllRelations...>,
            public Concerns::HasRelationStore<Derived, AllRelations...>,
            public Orm::Concerns::HasConnectionResolver,
            public Concerns::GuardsAttributes<Derived, AllRelations...>
    {
        // Helps to avoid 'friend Derived' declarations in models
        friend Concerns::GuardsAttributes<Derived, AllRelations...>;
        // Used by BaseRelationStore::visit() and also by visted methods
        friend Concerns::HasRelationStore<Derived, AllRelations...>;
        // Used by QueriesRelationships::has()
        friend Concerns::QueriesRelationships<Derived>;
        // FUTURE try to solve problem with forward declarations for friend methods, to allow only relevant methods from TinyBuilder silverqx
        // Used by TinyBuilder::eagerLoadRelations()
        friend TinyBuilder<Derived>;

        /*! Apply all the Model's template parameters to the passed T template
            argument. */
        template<template<typename ...> typename T>
        using ModelTypeApply = T<Derived, AllRelations...>;

    public:
        /*! The "type" of the primary key ID. */
        using KeyType = quint64;

        /* Constructors */
        /*! Create a new TinORM model instance. */
        Model();

        /*! Model's copy constructor. */
        inline Model(const Model &) = default;
        /*! Model's move constructor. */
        inline Model(Model &&) noexcept = default;

        /*! Model's copy assignment operator. */
        inline Model &operator=(const Model &) = default;
        /*! Model's move assignment operator. */
        inline Model &operator=(Model &&) noexcept = default;

        /*! Create a new TinORM model instance from attributes
            (converting constructor). */
        explicit Model(const QVector<AttributeItem> &attributes);
        /*! Create a new TinORM model instance from attributes
            (converting constructor). */
        explicit Model(QVector<AttributeItem> &&attributes);

        /*! Create a new TinORM model instance from attributes
            (list initialization). */
        Model(std::initializer_list<AttributeItem> attributes);

        /* Static operations on a model instance */
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(const QVector<AttributeItem> &attributes);
        /*! Create a new TinyORM model instance with given attributes. */
        static Derived instance(QVector<AttributeItem> &&attributes);

        /*! Begin querying the model. */
        static std::unique_ptr<TinyBuilder<Derived>> query();
        /*! Begin querying the model on a given connection. */
        static std::unique_ptr<TinyBuilder<Derived>> on(const QString &connection = "");

        /*! Get all of the models from the database. */
        static QVector<Derived> all(const QVector<Column> &columns = {ASTERISK});

        /* Operations on a model instance */
        /*! Save the model to the database. */
        bool save(SaveOptions options = {});
        /*! Save the model and all of its relationships. */
        bool push();

        /*! Update records in the database. */
        bool update(const QVector<AttributeItem> &attributes, SaveOptions options = {});

        /*! Delete the model from the database. */
        bool remove();
        /*! Delete the model from the database (alias). */
        bool deleteModel();

        /*! Reload a fresh model instance from the database. */
        std::optional<Derived> fresh(const QVector<WithItem> &relations = {});
        /*! Reload a fresh model instance from the database. */
        std::optional<Derived> fresh(const QString &relation);
        /*! Reload the current model instance with fresh attributes from the database. */
        Derived &refresh();

        /*! Eager load relations on the model. */
        Derived &load(const QVector<WithItem> &relations);
        /*! Eager load relations on the model. */
        Derived &load(const QString &relation);

        /*! Determine if two models have the same ID and belong to the same table. */
        template<typename ModelToCompare>
        bool is(const std::optional<ModelToCompare> &model) const;
        /*! Determine if two models are not the same. */
        template<typename ModelToCompare>
        bool isNot(const std::optional<ModelToCompare> &model) const;

        /*! Fill the model with a vector of attributes. */
        Derived &fill(const QVector<AttributeItem> &attributes);
        /*! Fill the model with a vector of attributes. */
        Derived &fill(QVector<AttributeItem> &&attributes);
        /*! Fill the model with a vector of attributes. Force mass assignment. */
        Derived &forceFill(const QVector<AttributeItem> &attributes);

        /* Model Instance methods */
        /*! Get a new query builder for the model's table. */
        std::unique_ptr<TinyBuilder<Derived>> newQuery();
        /*! Get a new query builder that doesn't have any global scopes. */
        std::unique_ptr<TinyBuilder<Derived>> newQueryWithoutScopes();
        /*! Get a new query builder that doesn't have any global scopes or
            eager loading. */
        std::unique_ptr<TinyBuilder<Derived>> newModelQuery();
        /*! Get a new query builder with no relationships loaded. */
        std::unique_ptr<TinyBuilder<Derived>> newQueryWithoutRelationships();
        /*! Create a new Tiny query builder for the model. */
        std::unique_ptr<TinyBuilder<Derived>>
        newTinyBuilder(const QSharedPointer<QueryBuilder> &query);

        /*! Create a new model instance that is existing. */
        Derived newFromBuilder(const QVector<AttributeItem> &attributes = {},
                               const std::optional<QString> &connection = std::nullopt);
        /*! Create a new instance of the given model. */
        Derived newInstance();
        /*! Create a new instance of the given model. */
        Derived newInstance(const QVector<AttributeItem> &attributes,
                            bool exists = false);
        /*! Create a new instance of the given model. */
        Derived newInstance(QVector<AttributeItem> &&attributes,
                            bool exists = false);

        /*! Create a new pivot model instance. */
        template<typename PivotType = Relations::Pivot, typename Parent>
        PivotType
        newPivot(const Parent &parent, const QVector<AttributeItem> &attributes,
                 const QString &table, bool exists) const;

        /*! Static cast this to a child's instance type (CRTP). */
        Derived &model();
        /*! Static cast this to a child's instance type (CRTP), const version. */
        const Derived &model() const;

        /* Getters / Setters */
        /*! Get the current connection name for the model. */
        const QString &getConnectionName() const;
        /*! Get the database connection for the model. */
        ConnectionInterface &getConnection() const;
        /*! Set the connection associated with the model. */
        Derived &setConnection(const QString &name);
        /*! Set the table associated with the model. */
        Derived &setTable(const QString &value);
        /*! Get the table associated with the model. */
        const QString &getTable() const;
        /*! Get the primary key for the model. */
        const QString &getKeyName() const;
        /*! Get the table qualified key name. */
        QString getQualifiedKeyName() const;
        /*! Get the value of the model's primary key. */
        QVariant getKey() const;
        /*! Get the value indicating whether the IDs are incrementing. */
        bool getIncrementing() const;
        /*! Set whether IDs are incrementing. */
        Derived &setIncrementing(bool value);

        /* Others */
        /*! Qualify the given column name by the model's table. */
        QString qualifyColumn(const QString &column) const;

        /*! Indicates if the model exists. */
        bool exists = false;

        /* HasAttributes */
        /*! Set a given attribute on the model. */
        Derived &setAttribute(const QString &key, QVariant value);
        /*! Set a vector of model attributes. No checking is done. */
        Derived &setRawAttributes(const QVector<AttributeItem> &attributes,
                                  bool sync = false);
        /*! Sync the original attributes with the current. */
        Derived &syncOriginal();
        /*! Get all of the current attributes on the model (insert order). */
        const QVector<AttributeItem> &getAttributes() const;
        /*! Get all of the current attributes on the model (for fast lookup). */
        const std::unordered_map<QString, int> &getAttributesHash() const;
        /*! Get an attribute from the model. */
        QVariant getAttribute(const QString &key) const;
        /*! Get a plain attribute (not a relationship). */
        QVariant getAttributeValue(const QString &key) const;
        /*! Get an attribute from the m_attributes vector. */
        QVariant getAttributeFromArray(const QString &key) const;
        /*! Get the model's original attribute value (transformed). */
        QVariant getOriginal(const QString &key,
                             const QVariant &defaultValue = {}) const;
        /*! Get the model's original attribute values (transformed and insert order). */
        QVector<AttributeItem> getOriginals() const;
        /*! Get the model's original attributes hash (for fast lookup). */
        const std::unordered_map<QString, int> &getOriginalsHash() const;
        /*! Get the model's raw original attribute value. */
        QVariant getRawOriginal(const QString &key,
                                const QVariant &defaultValue = {}) const;
        /*! Get the model's raw original attribute values (insert order). */
        const QVector<AttributeItem> &getRawOriginals() const;
        /*! Unset an attribute on the model, returns the number of attributes removed. */
        Derived &unsetAttribute(const AttributeItem &value);
        /*! Unset an attribute on the model. */
        Derived &unsetAttribute(const QString &key);

        /*! Get a relationship for Many types relation. */
        template<typename Related,
                 template<typename> typename Container = QVector>
        const Container<Related *>
        getRelationValue(const QString &relation);
        /*! Get a relationship for a One type relation. */
        template<typename Related, typename Tag> requires std::same_as<Tag, One>
        Related *
        getRelationValue(const QString &relation);

        /*! Get the attributes that have been changed since last sync
            (insert order). */
        QVector<AttributeItem> getDirty() const;
        /*! Get the attributes that have been changed since last sync
            (for fast lookup). */
        std::unordered_map<QString, int> getDirtyHash() const;
        /*! Determine if the model or any of the given attribute(s) have
            been modified. */
        bool isDirty(const QStringList &attributes = {}) const;
        /*! Determine if the model or any of the given attribute(s) have
            been modified. */
        bool isDirty(const QString &attribute) const;
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        bool isClean(const QStringList &attributes = {}) const;
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        bool isClean(const QString &attribute) const;

        /*! Get the attributes that were changed (insert order). */
        const QVector<AttributeItem> &getChanges() const;
        /*! Get the attributes that were changed (for fast lookup). */
        const std::unordered_map<QString, int> &getChangesHash() const;
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        bool wasChanged(const QStringList &attributes = {}) const;
        /*! Determine if the model and all the given attribute(s) have
            remained the same. */
        bool wasChanged(const QString &attribute) const;

        /*! Get the format for database stored dates. */
        const QString &getDateFormat() const;
        /*! Set the date format used by the model. */
        Derived &setDateFormat(const QString &format);
        /*! Convert a DateTime to a storable string. */
        QVariant fromDateTime(const QVariant &value) const;
        /*! Convert a DateTime to a storable string. */
        QString fromDateTime(const QDateTime &value) const;
        /*! Get the attributes that should be converted to dates. */
        const QStringList &getDates() const;

        /*! Proxy for an attribute element used in the operator[] &. */
        class AttributeReference
        {
            friend Model<Derived, AllRelations...>;

        public:
            /*! Assign a value of the QVariant to the referenced attribute. */
            const AttributeReference & // NOLINT(misc-unconventional-assign-operator)
            operator=(const QVariant &value) const;
            /*! Assign a value of another attribute reference to the referenced
                attribute. */
            const AttributeReference & // NOLINT(misc-unconventional-assign-operator)
            operator=(const AttributeReference &attributeReference) const;

            /*! Accesses the contained value, only const member functions. */
            const QVariant *operator->() const;
            /*! Accesses the contained value. */
            QVariant value() const;
            /*! Accesses the contained value. */
            QVariant operator*() const;
            /*! Converting operator to the QVariant type. */
            operator QVariant() const; // NOLINT(google-explicit-constructor)

        private:
            /*! AttributeReference's private constructor. */
            AttributeReference(Model<Derived, AllRelations...> &model,
                               // NOLINTNEXTLINE(modernize-pass-by-value)
                               const QString &attribute);

            /*! The model on which is an attribute set. */
            Model<Derived, AllRelations...> &m_model;
            /*! Attribute key name. */
            const QString m_attribute;
            /*! The temporary cache used during operator->() call, to be able
                to return the QVariant *. */
            mutable QVariant m_attributeCache;
        };

        /*! Return modifiable attribute reference, can be used on the left-hand side
            of an assignment operator. */
        AttributeReference operator[](const QString &attribute) &;
        /*! Return an attribute by the given key. */
        QVariant operator[](const QString &attribute) const &;
        /*! Return an attribute by the given key. */
        QVariant operator[](const QString &attribute) &&;
        /*! Return an attribute by the given key. */
        QVariant operator[](const QString &attribute) const &&;

        /* HasRelationships */
        /*! Get a specified relationship. */
        template<typename Related,
                 template<typename> typename Container = QVector>
        const Container<Related *>
        getRelation(const QString &relation);
        /*! Get a specified relationship as Related type, for use with HasOne and
            BelongsTo relation types. */
        template<typename Related, typename Tag> requires std::same_as<Tag, One>
        Related *getRelation(const QString &relation);

        /*! Determine if the given relation is loaded. */
        bool relationLoaded(const QString &relation) const;

        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, const QVector<Related> &models);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, QVector<Related> &&models);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, const std::optional<Related> &model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        Derived &setRelation(const QString &relation, std::optional<Related> &&model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        requires std::is_base_of_v<Relations::IsPivotModel, Related>
        Derived &setRelation(const QString &relation, const std::optional<Related> &model);
        /*! Set the given relationship on the model. */
        template<typename Related>
        requires std::is_base_of_v<Relations::IsPivotModel, Related>
        Derived &setRelation(const QString &relation, std::optional<Related> &&model);

        /*! Get the default foreign key name for the model. */
        QString getForeignKey() const;

        /*! Define a one-to-one relationship. */
        template<typename Related>
        std::unique_ptr<Relations::HasOne<Derived, Related>>
        hasOne(QString foreignKey = "", QString localKey = "");
        /*! Define an inverse one-to-one or many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::BelongsTo<Derived, Related>>
        belongsTo(QString foreignKey = "", QString ownerKey = "", QString relation = "");
        /*! Define a one-to-many relationship. */
        template<typename Related>
        std::unique_ptr<Relations::HasMany<Derived, Related>>
        hasMany(QString foreignKey = "", QString localKey = "");
        /*! Define a many-to-many relationship. */
        template<typename Related, typename PivotType = Relations::Pivot>
        std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
        belongsToMany(QString table = "", QString foreignPivotKey = "",
                      QString relatedPivotKey = "", QString parentKey = "",
                      QString relatedKey = "", QString relation = "");

        /*! Touch the owning relations of the model. */
        void touchOwners();

        /*! Get the relationships that are touched on save. */
        const QStringList &getTouchedRelations() const;
        /*! Determine if the model touches a given relation. */
        bool touches(const QString &relation) const;

        /*! Get all the loaded relations for the instance. */
#ifdef __GNUG__
        const std::map<QString, RelationsType<AllRelations...>> &
#else
        const std::unordered_map<QString, RelationsType<AllRelations...>> &
#endif
        getRelations() const;
        /*! Get all the loaded relations for the instance. */
#ifdef __GNUG__
        std::map<QString, RelationsType<AllRelations...>> &
#else
        std::unordered_map<QString, RelationsType<AllRelations...>> &
#endif
        getRelations();

        /*! Unset all the loaded relations for the instance. */
        Derived &unsetRelations();
        /*! Unset a loaded relationship. */
        Derived &unsetRelation(const QString &relation);

        /* HasTimestamps */
        /*! Update the model's update timestamp. */
        bool touch();
        /*! Update the creation and update timestamps. */
        void updateTimestamps();

        /*! Set the value of the "created at" attribute. */
        Derived &setCreatedAt(const QDateTime &value);
        /*! Set the value of the "updated at" attribute. */
        Derived &setUpdatedAt(const QDateTime &value);

        /*! Get a fresh timestamp for the model. */
        QDateTime freshTimestamp() const;
        /*! Get a fresh timestamp for the model. */
        QString freshTimestampString() const;

        /*! Determine if the model uses timestamps. */
        bool usesTimestamps() const;
        /*! Set the value of the u_timestamps attribute. */
        Derived &setUseTimestamps(bool value);

        /*! Get the name of the "created at" column. */
        static const QString &getCreatedAtColumn();
        /*! Get the name of the "updated at" column. */
        static const QString &getUpdatedAtColumn();

        /*! Get the fully qualified "created at" column. */
        QString getQualifiedCreatedAtColumn() const;
        /*! Get the fully qualified "updated at" column. */
        QString getQualifiedUpdatedAtColumn() const;

        /*! Determine if the given model is ignoring touches. */
        template<typename ClassToCheck = Derived>
        static bool isIgnoringTouch();

    protected:
        /*! Relation visitor lambda type. */
        using RelationVisitor = std::function<void(
                typename Concerns::HasRelationStore<Derived, AllRelations...>
                                 ::BaseRelationStore &)>;

        /*! Get a new query builder instance for the connection. */
        QSharedPointer<QueryBuilder> newBaseQueryBuilder() const;

        /* HasAttributes */
        /*! Transform a raw model value using mutators, casts, etc. */
        QVariant transformModelValue(const QString &key, const QVariant &value) const;
        /*! Get the model's original attribute values. */
        QVariant getOriginalWithoutRewindingModel(
                const QString &key, const QVariant &defaultValue = {}) const;

        /*! Get a relationship value from a method. */
        template<class Related,
                 template<typename> typename Container = QVector>
        Container<Related *>
        getRelationshipFromMethod(const QString &relation);
        /*! Get a relationship value from a method. */
        template<class Related, typename Tag> requires std::same_as<Tag, One>
        Related *
        getRelationshipFromMethod(const QString &relation);

        /*! Determine if any of the given attributes were changed. */
        bool hasChanges(const std::unordered_map<QString, int> &changes,
                        const QStringList &attributes = {}) const;
        /*! Sync the changed attributes. */
        Derived &syncChanges();

        /*! Determine if the new and old values for a given key are equivalent. */
        bool originalIsEquivalent(const QString &key) const;

        /*! Determine if the given attribute is a date. */
        bool isDateAttribute(const QString &key) const;
        /*! Return a timestamp as DateTime object. */
        QDateTime asDateTime(const QVariant &value) const;
        /*! Obtain timestamp column names. */
        const QStringList &timestampColumnNames() const;

        /*! Rehash attribute positions from the given index. */
        void rehashAttributePositions(
                const QVector<AttributeItem> &attributes,
                std::unordered_map<QString, int> &attributesHash,
                int from = 0);

        /* HasRelationships */
        /*! Create a new model instance for a related model. */
        template<typename Related>
        std::unique_ptr<Related> newRelatedInstance() const;

        /*! Instantiate a new HasOne relationship. */
        template<typename Related>
        std::unique_ptr<Relations::HasOne<Derived, Related>>
        newHasOne(std::unique_ptr<Related> &&related, Derived &parent,
                  const QString &foreignKey, const QString &localKey) const;
        /*! Instantiate a new BelongsTo relationship. */
        template<typename Related>
        std::unique_ptr<Relations::BelongsTo<Derived, Related>>
        newBelongsTo(std::unique_ptr<Related> &&related,
                     Derived &child, const QString &foreignKey,
                     const QString &ownerKey, const QString &relation) const;
        /*! Instantiate a new HasMany relationship. */
        template<typename Related>
        std::unique_ptr<Relations::HasMany<Derived, Related>>
        newHasMany(std::unique_ptr<Related> &&related, Derived &parent,
                   const QString &foreignKey, const QString &localKey) const;
        /*! Instantiate a new BelongsToMany relationship. */
        template<typename Related, typename PivotType>
        std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
        newBelongsToMany(std::unique_ptr<Related> &&related, Derived &parent,
                         const QString &table, const QString &foreignPivotKey,
                         const QString &relatedPivotKey, const QString &parentKey,
                         const QString &relatedKey, const QString &relation) const;

        /*! Guess the "belongs to" relationship name. */
        template<typename Related>
        QString guessBelongsToRelation() const;
        /*! Guess the "belongs to many" relationship name. */
        template<typename Related>
        QString guessBelongsToManyRelation() const;
        /*! Get the joining table name for a many-to-many relation. */
        template<typename Related>
        QString pivotTableName() const;

        /*! Set the entire relations hash on the model. */
        Derived &setRelations(
#ifdef __GNUG__
                const std::map<QString, RelationsType<AllRelations...>> &relations);
#else
                const std::unordered_map<QString,
                                         RelationsType<AllRelations...>> &relations);
#endif
        /*! Set the entire relations hash on the model. */
        Derived &setRelations(
#ifdef __GNUG__
                std::map<QString, RelationsType<AllRelations...>> &&relations);
#else
                std::unordered_map<QString, RelationsType<AllRelations...>> &&relations);
#endif

        /* Others */
        /*! Perform the actual delete query on this model instance. */
        void performDeleteOnModel();

        /*! Set the keys for a save update query. */
        TinyBuilder<Derived> &
        setKeysForSaveQuery(TinyBuilder<Derived> &query);
        /*! Get the primary key value for a save query. */
        QVariant getKeyForSaveQuery() const;
        /*! Set the keys for a select query. */
        TinyBuilder<Derived> &
        setKeysForSelectQuery(TinyBuilder<Derived> &query);
        /*! Get the primary key value for a select query. */
        QVariant getKeyForSelectQuery() const;

        /*! Perform a model insert operation. */
        bool performInsert(const TinyBuilder<Derived> &query);
        /*! Perform a model insert operation. */
        bool performUpdate(TinyBuilder<Derived> &query);
        /*! Perform any actions that are necessary after the model is saved. */
        void finishSave(SaveOptions options = {});

        /*! Insert the given attributes and set the ID on the model. */
        quint64 insertAndSetId(const TinyBuilder<Derived> &query,
                               const QVector<AttributeItem> &attributes);

        /*! The table associated with the model. */
        QString u_table;
        /*! The connection name for the model. */
        QString u_connection;
        /*! Indicates if the model's ID is auto-incrementing. */
        bool u_incrementing = true;
        /*! The primary key associated with the table. */
        QString u_primaryKey = ID;

        /*! Map of relation names to methods. */
        QHash<QString, RelationVisitor> u_relations;
        // TODO detect (best at compile time) circular eager relation problem, the exception which happens during this problem is stackoverflow in QRegularExpression silverqx
        /*! The relations to eager load on every query. */
        QVector<QString> u_with;
        /*! The relationship counts that should be eager loaded on every query. */
//        QVector<WithItem> u_withCount;

        /* HasAttributes */
        /*! The model's default values for attributes. */
        thread_local
        inline static QVector<AttributeItem> u_attributes;
        /*! The model's attributes (insert order). */
        QVector<AttributeItem> m_attributes;
        /*! The model attribute's original state (insert order).
            On the model from many-to-many relation also contains all pivot values,
            that is normal (insert order). */
        QVector<AttributeItem> m_original;
        /*! The changed model attributes (insert order). */
        QVector<AttributeItem> m_changes;

        /* Don't want to use std::reference_wrapper to attributes, because if a copy
           of the model is made, all references would be invalidated. */
        /*! The model's attributes hash (for fast lookup). */
        std::unordered_map<QString, int> m_attributesHash;
        /*! The model attribute's original state (for fast lookup). */
        std::unordered_map<QString, int> m_originalHash;
        /*! The changed model attributes (for fast lookup). */
        std::unordered_map<QString, int> m_changesHash;

        // TODO add support for 'U' like in PHP to support unix timestamp, I will have to manually check if u_dateFormat contains 'U' and use QDateTime::fromSecsSinceEpoch() silverqx
        /*! The storage format of the model's date columns. */
        thread_local
        inline static QString u_dateFormat;
        /*! The attributes that should be mutated to dates. @deprecated */
        thread_local
        inline static QStringList u_dates;

        /* HasRelationships */
        // BUG std::unordered_map prevents to compile on GCC, if I comment out std::optional<AllRelations>... in the RelationsType<AllRelations...>, or I change it to the QHash, then it compile, I'm absolutelly lost why this is happening 😞😭, I can't change to the QHash because of 25734deb, I have created simple test project gcc_trivial_bug_test in merydeye-gentoo, but std::map works so it is a big win, because now I can compile whole project on gcc ✨🚀 silverqx
        /*! The loaded relationships for the model. */
#ifdef __GNUG__
        std::map<QString, RelationsType<AllRelations...>> m_relations;
#else
        std::unordered_map<QString, RelationsType<AllRelations...>> m_relations;
#endif
        /*! The relationships that should be touched on save. */
        QStringList u_touches;
        // CUR1 use sets instead of QStringList where appropriate silverqx
        /*! Currently loaded Pivot relation names. */
        std::unordered_set<QString> m_pivots;

        /* HasTimestamps */
        /*! The name of the "created at" column. */
        inline static const QString CREATED_AT = Constants::CREATED_AT; // NOLINT(cppcoreguidelines-interfaces-global-init)
        /*! The name of the "updated at" column. */
        inline static const QString UPDATED_AT = Constants::UPDATED_AT; // NOLINT(cppcoreguidelines-interfaces-global-init)
        /*! Indicates if the model should be timestamped. */
        bool u_timestamps = true;

    private:
        /*! Alias for the enum struct RelationNotFoundError::From. */
        using RelationFrom = Exceptions::RelationNotFoundError::From;

        /* Eager load from TinyBuilder */
        /*! Invoke Model::eagerVisitor() to define template argument Related
            for eagerLoaded relation. */
        void eagerLoadRelationWithVisitor(
                const WithItem &relation, const TinyBuilder<Derived> &builder,
                QVector<Derived> &models);

        /*! Get Related model table name if the relation is BelongsToMany, otherwise
            empty std::optional. */
        std::optional<QString>
        getRelatedTableForBelongsToManyWithVisitor(const QString &relation);

        /* HasAttributes */
        /*! Get all of the current attributes on the model. */
        const QVector<AttributeItem> &getRawAttributes() const;

        /*! Throw InvalidArgumentError if attributes passed to the constructor contain
            some value, which will cause access of some data member in a derived
            instance. */
        void throwIfCRTPctorProblem(const QVector<AttributeItem> &attributes) const;
        /*! The QDateTime attribute detected, causes CRTP ctor problem. */
        void throwIfQDateTimeAttribute(const QVector<AttributeItem> &attributes) const;

        /*! Get the attributes that should be converted to dates. */
        QStringList getDatesInternal() const;

        /* HasRelationships */
        /*! Throw exception if a relation is not defined. */
        void validateUserRelation(const QString &name,
                                  RelationFrom from = RelationFrom::UNDEFINED) const;

        /*! Obtain related models from "relationships" data member hash
            without any checks. */
        template<class Related,
                 template<typename> typename Container = QVector>
        Container<Related *>
        getRelationFromHash(const QString &relation);
        /*! Obtain related models from "relationships" data member hash
            without any checks. */
        template<class Related, typename Tag> requires std::same_as<Tag, One>
        Related *
        getRelationFromHash(const QString &relation);

        /*! Create lazy store and obtain a relationship from defined method. */
        template<typename Related, typename Result>
        Result getRelationshipFromMethodWithVisitor(const QString &relation);

        /*! Throw exception if correct getRelation/Value() method was not used, to avoid
            std::bad_variant_access. */
        template<typename Result, typename Related, typename T>
        void checkRelationType(const T &relationVariant, const QString &relation,
                               const QString &source) const;

        /*! Guess the relationship name for belongsTo/belongsToMany. */
        template<typename Related>
        QString guessBelongsToRelationInternal() const;

        /* Push relation store related */
        /*! Create push store and call push for every model. */
        bool pushWithVisitor(const QString &relation,
                             RelationsType<AllRelations...> &models);

        /*! On the base of alternative held by m_relations decide, which
            pushVisitied() to execute. */
        template<typename Related>
        void pushVisited();
        /*! Push for Many relation types. */
        template<typename Related, typename Tag> requires std::same_as<Tag, Many>
        void pushVisited();
        /*! Push for One relation type. */
        template<typename Related, typename Tag> requires std::same_as<Tag, One>
        void pushVisited();

        /* Touch owners store related */
        /*! Create 'touch owners relation store' and touch all related models. */
        void touchOwnersWithVisitor(const QString &relation);
        /*! On the base of alternative held by m_relations decide, which
            touchOwnersVisited() to execute. */
        template<typename Related, typename Relation>
        void touchOwnersVisited(Relation &&relation);

        /* QueriesRelationships store related */
        /*! Create 'QueriesRelationships relation store' to obtain relation instance. */
        template<typename Related = void>
        void queriesRelationshipsWithVisitor(
                const QString &relation, Concerns::QueriesRelationships<Derived> &origin,
                const QString &comparison, qint64 count, const QString &condition,
                const std::function<void(
                        Concerns::QueriesRelationshipsCallback<Related> &)> &callback,
                std::optional<std::reference_wrapper<
                        QStringList>> relations = std::nullopt);

        /* Others */
        /*! Obtain all loaded relation names except pivot relations. */
        QVector<WithItem> getLoadedRelationsWithoutPivot();

        /*! Replace relations in the m_relation. */
        void replaceRelations(
#ifdef __GNUG__
                std::map<QString, RelationsType<AllRelations...>> &relations,
#else
                std::unordered_map<QString, RelationsType<AllRelations...>> &relations,
#endif
                const QVector<WithItem> &onlyRelations);

        /*! The reference to the u_relations hash. */
        const QHash<QString, RelationVisitor> &
        getRelationsRawMapInternal() const;

        /* GuardsAttributes */
        /* Getters for u_ data members defined in the Derived models, helps to avoid
           'friend Derived' declarations in models. */
        /*! Get the fillable attributes for the model. */
        QStringList &getFillableInternal();
        /*! Get the fillable attributes for the model. */
        const QStringList &getFillableInternal() const;
        /*! Get the guarded attributes for the model. */
        QStringList &getGuardedInternal();
        /*! Get the guarded attributes for the model. */
        const QStringList &getGuardedInternal() const;
    };

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model()
    {
        // Compile time check if a primary key type is supported by a QVariant
        qMetaTypeId<typename Derived::KeyType>();

        // Default Attribute Values
        fill(Derived::u_attributes);

        syncOriginal();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(const QVector<AttributeItem> &attributes)
        : Model()
    {
        throwIfCRTPctorProblem(attributes);

        fill(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(QVector<AttributeItem> &&attributes)
        : Model()
    {
        throwIfCRTPctorProblem(attributes);

        fill(std::move(attributes));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::Model(
            std::initializer_list<AttributeItem> attributes
    )
        : Model(QVector<AttributeItem>(attributes.begin(), attributes.end()))
    {}

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(const QVector<AttributeItem> &attributes)
    {
        Derived model;

        model.fill(attributes);

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::instance(QVector<AttributeItem> &&attributes)
    {
        Derived model;

        model.fill(std::move(attributes));

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::query()
    {
        return Derived().newQuery();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::on(const QString &connection)
    {
        /* First we will just create a fresh instance of this model, and then we can
           set the connection on the model so that it is used for the queries we
           execute, as well as being set on every relation we retrieve without
           a custom connection name. */
        Derived instance;

        instance.setConnection(connection);

        return instance.newQuery();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVector<Derived>
    Model<Derived, AllRelations...>::all(const QVector<Column> &columns)
    {
        return query()->get(columns);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::save(const SaveOptions options)
    {
//        mergeAttributesFromClassCasts();

        // Ownership of a unique_ptr()
        auto query = newModelQuery();

        auto saved = false;

        /* If the "saving" event returns false we'll bail out of the save and return
           false, indicating that the save failed. This provides a chance for any
           listeners to cancel save operations if validations fail or whatever. */
//        if (fireModelEvent('saving') === false) {
//            return false;
//        }

        /* If the model already exists in the database we can just update our record
           that is already in this database using the current IDs in this "where"
           clause to only update this model. Otherwise, we'll just insert them. */
        if (exists)
            saved = isDirty() ? performUpdate(*query) : true;

        // If the model is brand new, we'll insert it into our database and set the
        // ID attribute on the model to the value of the newly inserted row's ID
        // which is typically an auto-increment value managed by the database.
        else {
            saved = performInsert(*query);

            if (const auto &connection = query->getConnection();
                getConnectionName().isEmpty()
            )
                setConnection(connection.getName());
        }

        /* If the model is successfully saved, we need to do a few more things once
           that is done. We will call the "saved" method here to run any actions
           we need to happen after a model gets successfully saved right here. */
        if (saved)
            finishSave(options);

        return saved;
    }

    // FUTURE support for SaveOptions parameter, Eloquent doesn't have this parameter, maybe there's a reason for that, but I didn't find anything on github issues silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::push()
    {
        if (!save())
            return false;

        /* To sync all of the relationships to the database, we will simply spin through
           the relationships and save each model via this "push" method, which allows
           us to recurse into all of these nested relations for the model instance. */
        for (auto &[relation, models] : m_relations)
            // FUTURE Eloquent uses array_filter on models, investigate when this happens, null value (model) in many relations? silverqx
            /* Following Eloquent API, if any push failed, then quit, remaining push-es
               will not be processed. */
            if (!pushWithVisitor(relation, models))
                return false;

        return true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::pushWithVisitor(
            const QString &relation, RelationsType<AllRelations...> &models)
    {
        // TODO prod remove, I don't exactly know if this can really happen silverqx
        /* Check for empty variant, the std::monostate is at zero index and
           consider it as success to continue 'pushing'. */
        const auto variantIndex = models.index();
        Q_ASSERT(variantIndex > 0);
        if (variantIndex == 0)
            return true;

        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->createPushStore(models).visit(relation);

        const auto pushResult = this->pushStore().m_result;

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();

        return pushResult;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    void Model<Derived, AllRelations...>::pushVisited()
    {
        const RelationsType<AllRelations...> &models = this->pushStore().m_models;

        // Invoke pushVisited() on the base of hold alternative in the models
        if (std::holds_alternative<QVector<Related>>(models))
            pushVisited<Related, Many>();
        else if (std::holds_alternative<std::optional<Related>>(models))
            pushVisited<Related, One>();
        else
            throw Orm::Exceptions::RuntimeError(
                    "this->pushStore().models holds unexpected alternative.");
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, Many>
    void Model<Derived, AllRelations...>::pushVisited()
    {
        auto &pushStore = this->pushStore();

        for (auto &model : std::get<QVector<Related>>(pushStore.m_models))
            if (!model.push()) {
                pushStore.m_result = false;
                return;
            }

        pushStore.m_result = true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, One>
    void Model<Derived, AllRelations...>::pushVisited()
    {
        auto &pushStore = this->pushStore();

        auto &model = std::get<std::optional<Related>>(pushStore.m_models);
        Q_ASSERT(model);

        // Skip a null model, consider it as success
        if (!model) {
            pushStore.m_result = true;
            return;
        }

        pushStore.m_result = model->push();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void
    Model<Derived, AllRelations...>::touchOwnersWithVisitor(const QString &relation)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->createTouchOwnersStore(relation).visit(relation);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Relation>
    void Model<Derived, AllRelations...>::touchOwnersVisited(Relation &&relation)
    {
        const auto &relationName = this->touchOwnersStore().m_relation;

        relation->touch();

        // Many type relation
        if constexpr (std::is_base_of_v<Relations::ManyRelation,
                                        typename Relation::element_type>)
        {
            for (auto *const relatedModel : getRelationValue<Related>(relationName))
                // WARNING check and add note after, if many type relation QVector can contain nullptr silverqx
                if (relatedModel)
                    relatedModel->touchOwners();
        }

        // One type relation
        else if constexpr (std::is_base_of_v<Relations::OneRelation,
                                             typename Relation::element_type>)
        {
            if (auto *const relatedModel = getRelationValue<Related, One>(relationName);
                relatedModel
            )
                relatedModel->touchOwners();
        } else
            throw Orm::Exceptions::RuntimeError(
                    "Bad relation type passed to the Model::touchOwnersVisited().");
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    void Model<Derived, AllRelations...>::queriesRelationshipsWithVisitor(
            const QString &relation, Concerns::QueriesRelationships<Derived> &origin,
            const QString &comparison, const qint64 count, const QString &condition,
            const std::function<void(
                Concerns::QueriesRelationshipsCallback<Related> &)> &callback,
            const std::optional<std::reference_wrapper<QStringList>> relations)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->template createQueriesRelationshipsStore<Related>(
                    origin, comparison, count, condition, callback, relations)
                .visit(relation);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::update(
            const QVector<AttributeItem> &attributes,
            const SaveOptions options)
    {
        if (!exists)
            return false;

        return fill(attributes).save(options);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::remove()
    {
        // FEATURE castable silverqx
//        mergeAttributesFromClassCasts();

        if (getKeyName().isEmpty())
            throw Orm::Exceptions::RuntimeError("No primary key defined on model.");

        /* If the model doesn't exist, there is nothing to delete so we'll just return
           immediately and not do anything else. Otherwise, we will continue with a
           deletion process on the model, firing the proper events, and so forth. */
        if (!exists)
            // NOTE api different silverqx
            return false;

        // FUTURE add support for model events silverqx
//        if ($this->fireModelEvent('deleting') === false) {
//            return false;
//        }

        /* Here, we'll touch the owning models, verifying these timestamps get updated
           for the models. This will allow any caching to get broken on the parents
           by the timestamp. Then we will go ahead and delete the model instance. */
        touchOwners();

        // FUTURE performDeleteOnModel() and return value, check logic here, eg what happens when no model is delete and combinations silverqx
        // FUTURE inconsistent return values save(), update(), remove(), ... silverqx
        performDeleteOnModel();

        /* Once the model has been deleted, we will fire off the deleted event so that
           the developers may hook into post-delete operations. We will then return
           a boolean true as the delete is presumably successful on the database. */
//        $this->fireModelEvent('deleted', false);

        return true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::deleteModel()
    {
        return model().remove();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::fresh(
            const QVector<WithItem> &relations)
    {
        if (!exists)
            return std::nullopt;

        return model().setKeysForSelectQuery(*newQueryWithoutScopes())
                .with(relations)
                .first();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<Derived>
    Model<Derived, AllRelations...>::fresh(const QString &relation)
    {
        return fresh(QVector<WithItem> {{relation}});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::refresh()
    {
        if (!exists)
            return model();

        setRawAttributes(model().setKeysForSelectQuery(*newQueryWithoutScopes())
                         .firstOrFail().getRawAttributes());

        // And reload them again, refresh relations
        load(getLoadedRelationsWithoutPivot());

        syncOriginal();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVector<WithItem>
    Model<Derived, AllRelations...>::getLoadedRelationsWithoutPivot()
    {
        QVector<WithItem> relations;

        /* Current model (this) contains a pivot relation alternative
           in the m_relations std::variant. */
        auto hasPivotRelation = std::disjunction_v<std::is_base_of<
                                Relations::IsPivotModel, AllRelations>...>;

        /* Get all currently loaded relation names except pivot relations. We need
           to check for the pivot models, but only if the std::variant which holds
           relations also holds a pivot model alternative, otherwise it is useless. */
        for (const auto &relation : m_relations) {
            const auto &relationName = relation.first;

            // Skip pivot relations
            if (hasPivotRelation && m_pivots.contains(relationName))
                    continue;

            relations.append({relationName});
        }

        return relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::replaceRelations(
#ifdef __GNUG__
            std::map<QString, RelationsType<AllRelations...>> &relations,
#else
            std::unordered_map<QString, RelationsType<AllRelations...>> &relations,
#endif
            const QVector<WithItem> &onlyRelations)
    {
        /* Replace only relations which was passed to this method, leave other
           relations untouched. */
        for (auto itRelation = relations.begin(); itRelation != relations.end();
             ++itRelation)
        {
            const auto &key = itRelation->first;

            const auto relationsContainKey =
                    ranges::contains(onlyRelations, true, [&key](const auto &relation)
            {
                if (!relation.name.contains(COLON))
                    return relation.name == key;

                // Support for select constraints
                return relation.name.split(COLON).at(0).trimmed() == key;
            });

            if (!relationsContainKey)
                continue;

            m_relations[key] = std::move(itRelation->second);
        }
    }

    // FUTURE LoadItem for Model::load() even it will have the same implmentation, or common parent and inherit silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::load(
            const QVector<WithItem> &relations)
    {
        // Ownership of a unique_ptr()
        auto builder = newQueryWithoutRelationships();

        builder->with(relations);

        // FUTURE make possible to pass single model to eagerLoadRelations() and whole relation flow, I indicative counted how many methods would have to rewrite and it is around 12 methods silverqx
        /* I have to make a copy here of this, because of eagerLoadRelations(),
           the solution would be to add a whole new chain for eager load relations,
           which will be able to work only on one Model &, but it is around
           refactoring of 10-15 methods, or add a variant which can process
           QVector<std::reference_wrapper<Derived>>.
           For now, I have made a copy here and save it into the QVector and after
           that move relations from this copy to the real instance. */
        QVector<Derived> models {model()};

        builder->eagerLoadRelations(models);

        /* Replace only relations which was passed to this method, leave other
           relations untouched.
           They do not need to be removed before 'eagerLoadRelations(models)'
           call, because only the relations passed to the 'with' at the beginning
           will be loaded anyway. */
        replaceRelations(models.first().getRelations(), relations);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::load(const QString &relation)
    {
        return load(QVector<WithItem> {{relation}});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename ModelToCompare>
    bool Model<Derived, AllRelations...>::is(
            const std::optional<ModelToCompare> &model) const
    {
        return model
                && getKey() == model->getKey()
                && this->model().getTable() == model->getTable()
                && getConnectionName() == model->getConnectionName();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename ModelToCompare>
    bool Model<Derived, AllRelations...>::isNot(
            const std::optional<ModelToCompare> &model) const
    {
        return !is(model);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::fill(const QVector<AttributeItem> &attributes)
    {
        const auto totallyGuarded = this->totallyGuarded();

        for (auto &attribute : this->fillableFromArray(attributes))
            /* The developers may choose to place some attributes in the "fillable" vector
               which means only those attributes may be set through mass assignment to
               the model, and all others will just get ignored for security reasons. */
            if (auto &key = attribute.key;
                this->isFillable(key)
            )
                setAttribute(key, std::move(attribute.value));

            else if (totallyGuarded)
                throw Exceptions::MassAssignmentError(
                        QStringLiteral("Add '%1' to u_fillable data member to allow "
                                       "mass assignment on '%2'.")
                        .arg(key, Utils::Type::classPureBasename<Derived>()));

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::fill(QVector<AttributeItem> &&attributes)
    {
        const auto totallyGuarded = this->totallyGuarded();

        for (auto &attribute : this->fillableFromArray(std::move(attributes))) {
            /* The developers may choose to place some attributes in the "fillable" vector
               which means only those attributes may be set through mass assignment to
               the model, and all others will just get ignored for security reasons. */
            if (auto &key = attribute.key;
                this->isFillable(key)
            )
                setAttribute(key, std::move(attribute.value));

            else if (totallyGuarded)
                throw Exceptions::MassAssignmentError(
                        QStringLiteral("Add '%1' to u_fillable data member to allow "
                                       "mass assignment on '%2'.")
                        .arg(key, Utils::Type::classPureBasename<Derived>()));
        }

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::forceFill(
            const QVector<AttributeItem> &attributes)
    {
        // Prevent unnecessary unguard
        if (attributes.isEmpty())
            return model();

        this->unguarded([this, &attributes]
        {
            fill(attributes);
        });

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQuery()
    {
        return newQueryWithoutScopes();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQueryWithoutScopes()
    {
        // Transform the QString vector to the WithItem vector
        const auto &relations = model().u_with;

        QVector<WithItem> relationsConverted;
        relationsConverted.reserve(relations.size());

        for (const auto &relation : relations)
            relationsConverted.append({relation});

        // Ownership of a unique_ptr()
        auto tinyBuilder = newModelQuery();

        tinyBuilder->with(relationsConverted);

        return tinyBuilder;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newModelQuery()
    {
        // Ownership of the QSharedPointer<QueryBuilder>
        const auto query = newBaseQueryBuilder();

        /* Model is passed to the TinyBuilder ctor, because of that setModel()
           isn't used here. Can't be const because of passed non-const model
           to the TinyBuilder. */
        return newTinyBuilder(query);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newQueryWithoutRelationships()
    {
        return newModelQuery();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unique_ptr<TinyBuilder<Derived>>
    Model<Derived, AllRelations...>::newTinyBuilder(
            const QSharedPointer<QueryBuilder> &query)
    {
        return std::make_unique<TinyBuilder<Derived>>(query, model());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newFromBuilder(
            const QVector<AttributeItem> &attributes,
            const std::optional<QString> &connection)
    {
        auto model = newInstance({}, true);

        model.setRawAttributes(attributes, true);

        model.setConnection(connection ? *connection : getConnectionName());

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline Derived
    Model<Derived, AllRelations...>::newInstance()
    {
        return newInstance({});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newInstance(
            const QVector<AttributeItem> &attributes, const bool exists_)
    {
        /* This method just provides a convenient way for us to generate fresh model
           instances of this current model. It is particularly useful during the
           hydration of new objects via the QueryBuilder instances. */
        Derived model;

        /* setAttribute() can call getDateFormat() inside and it tries to obtain
           the date format from grammar which is obtained from the connection, so
           the connection have to be set before fill(). */
        model.setConnection(getConnectionName());

        model.fill(attributes);

        model.exists = exists_;
        model.setTable(this->model().getTable());

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived
    Model<Derived, AllRelations...>::newInstance(
            QVector<AttributeItem> &&attributes, const bool exists_)
    {
        /* This method just provides a convenient way for us to generate fresh model
           instances of this current model. It is particularly useful during the
           hydration of new objects via the QueryBuilder instances. */
        Derived model;

        /* setAttribute() can call getDateFormat() inside and it tries to obtain
           the date format from grammar which is obtained from the connection, so
           the connection have to be set before fill(). */
        model.setConnection(getConnectionName());

        model.fill(std::move(attributes));

        model.exists = exists_;
        model.setTable(this->model().getTable());

        return model;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename PivotType, typename Parent>
    PivotType
    Model<Derived, AllRelations...>::newPivot(
            const Parent &parent, const QVector<AttributeItem> &attributes,
            const QString &table, const bool exists_) const
    {
        if constexpr (std::is_same_v<PivotType, Relations::Pivot>)
            return PivotType::template fromAttributes<Parent>(
                        parent, attributes, table, exists_);
        else
            return PivotType::template fromRawAttributes<Parent>(
                        parent, attributes, table, exists_);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline Derived &Model<Derived, AllRelations...>::model()
    {
        // Can not be cached with static because a copy can be made
        return static_cast<Derived &>(*this);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const Derived &Model<Derived, AllRelations...>::model() const
    {
        return static_cast<const Derived &>(*this);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getConnectionName() const
    {
#ifdef TINYORM_TESTS_CODE
        // Used from tests to override connection
        if (const auto &connection = ConnectionOverride::connection;
            !connection.isEmpty()
        )
            return connection;
#endif

        return model().u_connection;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline ConnectionInterface &
    Model<Derived, AllRelations...>::getConnection() const
    {
        return m_resolver->connection(getConnectionName());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline Derived &
    Model<Derived, AllRelations...>::setConnection(const QString &name)
    {
        model().u_connection = name; return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline Derived &
    Model<Derived, AllRelations...>::setTable(const QString &value)
    {
        model().u_table = value;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getTable() const
    {
        const auto &table = model().u_table;

        // Get pluralized snake-case table name
        if (table.isEmpty())
            const_cast<QString &>(model().u_table) =
                QStringLiteral("%1s").arg(
                    Utils::String::toSnake(Utils::Type::classPureBasename<Derived>()));

        return table;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QString &
    Model<Derived, AllRelations...>::getKeyName() const
    {
        return model().u_primaryKey;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QString
    Model<Derived, AllRelations...>::getQualifiedKeyName() const
    {
        return qualifyColumn(getKeyName());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QVariant
    Model<Derived, AllRelations...>::getKey() const
    {
        return getAttribute(getKeyName());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline bool
    Model<Derived, AllRelations...>::getIncrementing() const
    {
        return model().u_incrementing;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setIncrementing(const bool value)
    {
        model().u_incrementing = value;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QSharedPointer<QueryBuilder>
    Model<Derived, AllRelations...>::newBaseQueryBuilder() const
    {
        return getConnection().query();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, template<typename> typename Container>
    const Container<Related *> // NOLINT(readability-const-return-type)
    Model<Derived, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships hash, it just means the
            relationship has already been loaded, so we'll just return it out of
            here because there is no need to query within the relations twice. */
        if (relationLoaded(relation))
            return getRelationFromHash<Related, Container>(relation);

        /*! If the relation is defined on the model, then lazy load and return results
            from the query and hydrate the relationship's value on the "relationships"
            data member m_relations. */
        if (model().u_relations.contains(relation))
            return getRelationshipFromMethod<Related, Container>(relation);

        return {};
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    Model<Derived, AllRelations...>::getRelationValue(const QString &relation)
    {
        /*! If the key already exists in the relationships hash, it just means the
            relationship has already been loaded, so we'll just return it out of
            here because there is no need to query within the relations twice. */
        if (relationLoaded(relation))
            return getRelationFromHash<Related, Tag>(relation);

        /*! If the relation is defined on the model, then lazy load and return results
            from the query and hydrate the relationship's value on the "relationships"
            data member m_relations. */
        if (model().u_relations.contains(relation))
            return getRelationshipFromMethod<Related, Tag>(relation);

        return nullptr;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related *>
    Model<Derived, AllRelations...>::getRelationshipFromMethod(const QString &relation)
    {
        // Obtain related models
        auto relatedModels =
                getRelationshipFromMethodWithVisitor<Related, QVector<Related>>(relation);

        setRelation(relation, std::move(relatedModels));

        return getRelationFromHash<Related, Container>(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    Model<Derived, AllRelations...>::getRelationshipFromMethod(const QString &relation)
    {
        // Obtain related model
        auto relatedModel =
                getRelationshipFromMethodWithVisitor<Related,
                                                     std::optional<Related>>(relation);

        setRelation(relation, std::move(relatedModel));

        return getRelationFromHash<Related, Tag>(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Result>
    Result
    Model<Derived, AllRelations...>::getRelationshipFromMethodWithVisitor(
            const QString &relation)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Save model/s to the store to avoid passing variables to the visitor
        this->template createLazyStore<Related>().visit(relation);

        // Obtain result, related model/s
        const auto lazyResult = this->template lazyStore<Related>().m_result;

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();

        // Check relation type to avoid std::bad_variant_access
        checkRelationType<Result, Related>(lazyResult, relation, "getRelationValue");

        return std::get<Result>(lazyResult);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVector<AttributeItem>
    Model<Derived, AllRelations...>::getDirty() const
    {
        QVector<AttributeItem> dirty;

        for (const auto &attribute : getAttributes())
            if (const auto &key = attribute.key;
                !originalIsEquivalent(key)
            )
                dirty.append({key, attribute.value});

        return dirty;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::unordered_map<QString, int>
    Model<Derived, AllRelations...>::getDirtyHash() const
    {
        const auto size = m_attributes.size();
        std::unordered_map<QString, int> dirtyHash(static_cast<std::size_t>(size));

        for (auto i = 0; i < size; ++i)
            if (const auto &key = m_attributes.at(i).key;
                !originalIsEquivalent(key)
            )
                dirtyHash.emplace(m_attributes.at(i).key, i);

        return dirtyHash;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline bool
    Model<Derived, AllRelations...>::isDirty(const QStringList &attributes) const
    {
        return hasChanges(getDirtyHash(), attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline bool
    Model<Derived, AllRelations...>::isDirty(const QString &attribute) const
    {
        return hasChanges(getDirtyHash(), QStringList {attribute});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline bool
    Model<Derived, AllRelations...>::isClean(const QStringList &attributes) const
    {
        return !isDirty(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline bool
    Model<Derived, AllRelations...>::isClean(const QString &attribute) const
    {
        return !isDirty(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QVector<AttributeItem> &
    Model<Derived, AllRelations...>::getChanges() const
    {
        return m_changes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const std::unordered_map<QString, int> &
    Model<Derived, AllRelations...>::getChangesHash() const
    {
        return m_changesHash;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline bool
    Model<Derived, AllRelations...>::wasChanged(const QStringList &attributes) const
    {
        return hasChanges(getChangesHash(), attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline bool
    Model<Derived, AllRelations...>::wasChanged(const QString &attribute) const
    {
        return hasChanges(getChangesHash(), QStringList {attribute});
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::hasChanges(
            const std::unordered_map<QString, int> &changes,
            const QStringList &attributes) const
    {
        /* If no specific attributes were provided, we will just see if the dirty hash
           already contains any attributes. If it does we will just return that this
           count is greater than zero. Else, we need to check specific attributes. */
        if (attributes.isEmpty())
            return !changes.empty();

        /* Here we will spin through every attribute and see if this is in the hash of
           dirty attributes. If it is, we will return true and if we make it through
           all of the attributes for the entire vector we will return false at end. */
        return std::ranges::any_of(attributes, [&changes](const auto &attribute)
        {
            return changes.contains(attribute);
        });
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::syncChanges()
    {
        m_changes = getDirty();

        rehashAttributePositions(m_changes, m_changesHash);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    Model<Derived, AllRelations...>::originalIsEquivalent(const QString &key) const
    {
        if (!m_originalHash.contains(key))
            return false;

        const auto attribute = getAttributeFromArray(key);
        const auto original = getRawOriginal(key);

        // Takes into account also milliseconds for the QDateTime attribute
        if (attribute == original)
            return true;
        // TODO next solve how to work with null values and what to do with invalid/unknown values silverqx
        if (!attribute.isValid() || attribute.isNull())
            return false;

        // This check ignores milliseconds in the QDateTime attribute
        if (isDateAttribute(key))
            return fromDateTime(attribute) == fromDateTime(original);

//        if (hasCast(key, ['object', 'collection']))
//            return castAttribute(key, attribute) == castAttribute(key, original);
//        if (hasCast(key, ['real', 'float', 'double'])) {
//            if (($attribute === null && $original !== null) || ($attribute !== null && $original === null))
//                return false;

//            return abs($this->castAttribute($key, $attribute) - $this->castAttribute($key, $original)) < PHP_FLOAT_EPSILON * 4;
//        }
//        if ($this->hasCast($key, static::$primitiveCastTypes)) {
//            return $this->castAttribute($key, $attribute) ===
//                   $this->castAttribute($key, $original);
//        }

//        return is_numeric($attribute) && is_numeric($original)
//               && strcmp((string) $attribute, (string) $original) === 0;

        return false;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::isDateAttribute(const QString &key) const
    {
        // FEATURE castable silverqx
        /* I don't have support for castable attributes, this solution is temporary. */
        return getDates().contains(key);
    }

    // TODO would be good to make it the c++ way, make overload for every type, asDateTime() is protected, so I have full control over it, but I leave it for now, because there will be more methods which will use this method in the future, and it will be more clear later on silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    QDateTime
    Model<Derived, AllRelations...>::asDateTime(const QVariant &value) const
    {
        /* If this value is already a QDateTime instance, we shall just return it as is.
           This prevents us having to re-parse a QDateTime instance when we know
           it already is one. */
        if (
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            value.typeId() == QMetaType::QDateTime
#else
            value.userType() == QMetaType::QDateTime
#endif
        )
            return value.value<QDateTime>();

        /* If this value is an integer, we will assume it is a UNIX timestamp's value
           and format a Carbon object from this timestamp. This allows flexibility
           when defining your date fields as they might be UNIX timestamps here. */
        if (value.canConvert<QString>() &&
            Utils::String::isNumber(value.value<QString>())
        )
            // TODO switch ms accuracy? For the u_dateFormat too? silverqx
            return QDateTime::fromSecsSinceEpoch(value.value<qint64>());

        const auto &format = getDateFormat();

        /* Finally, we will just assume this date is in the format used by default on
           the database connection and use that format to create the QDateTime object
           that is returned back out to the developers after we convert it here. */
        if (auto date = QDateTime::fromString(value.value<QString>(), format);
            date.isValid()
        )
            return date;

        throw Orm::Exceptions::InvalidFormatError(
                    QStringLiteral("Could not parse the datetime '%1' using "
                                   "the given format '%2'.")
                    .arg(value.value<QString>(), format));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::timestampColumnNames() const
    {
        /* Fuckin static, it works like is described here:
           https://stackoverflow.com/questions/2737013/static-variables-in-static-method-in-base-class-and-inheritance. */
        static const QStringList cached {
            getCreatedAtColumn(),
            getUpdatedAtColumn(),
        };

        return cached;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::rehashAttributePositions(
            const QVector<AttributeItem> &attributes,
            std::unordered_map<QString, int> &attributesHash,
            const int from)
    {
        /* This member function is universal and can be used for m_attributes,
           m_changes and m_original and it associated unordered_maps m_attributesHash,
           m_changesHash and m_originalHash. */
        for (auto i = from; i < attributes.size(); ++i)
            // 'i' is the position
            attributesHash[attributes.at(i).key] = i;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, template<typename> typename Container>
    Container<Related *>
    Model<Derived, AllRelations...>::getRelationFromHash(const QString &relation)
    {
        auto &relationVariant = m_relations.find(relation)->second;

        // Check relation type to avoid std::bad_variant_access
        checkRelationType<QVector<Related>, Related>(
                    relationVariant, relation, "getRelation");

        /* Obtain related models from data member hash as QVector, it is internal
           format and transform it into a Container of pointers to related models,
           so a user can directly modify these models and push or save them
           afterward. */
        namespace views = ranges::views;
        return std::get<QVector<Related>>(relationVariant)
                | views::transform([](Related &model) -> Related * { return &model; })
                | ranges::to<Container<Related *>>();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<class Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    Model<Derived, AllRelations...>::getRelationFromHash(const QString &relation)
    {
        auto &relationVariant = m_relations.find(relation)->second;

        // Check relation type to avoid std::bad_variant_access
        checkRelationType<std::optional<Related>, Related>(
                    relationVariant, relation, "getRelation");

        /* Obtain related model from data member hash and return it as a pointer or
           nullptr if no model is associated, so a user can directly modify this
           model and push or save it afterward. */

        auto &relatedModel = std::get<std::optional<Related>>(relationVariant);

        return relatedModel ? &*relatedModel : nullptr;
    }

    // TODO make getRelation() Container argument compatible with STL containers API silverqx
    // TODO solve different behavior like Eloquent getRelation() silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, template<typename> typename Container>
    const Container<Related *> // NOLINT(readability-const-return-type)
    Model<Derived, AllRelations...>::getRelation(const QString &relation)
    {
        if (!relationLoaded(relation))
            throw Exceptions::RelationNotLoadedError(
                    Orm::Utils::Type::classPureBasename<Derived>(), relation);

        return getRelationFromHash<Related, Container>(relation);
    }

    // TODO smart pointer for this relation stuffs? silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename Tag> requires std::same_as<Tag, One>
    Related *
    Model<Derived, AllRelations...>::getRelation(const QString &relation)
    {
        if (!relationLoaded(relation))
            throw Exceptions::RelationNotLoadedError(
                    Orm::Utils::Type::classPureBasename<Derived>(), relation);

        return getRelationFromHash<Related, Tag>(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 const QVector<Related> &models)
    {
        m_relations[relation] = models;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 QVector<Related> &&models)
    {
        m_relations[relation] = std::move(models);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 const std::optional<Related> &model)
    {
        m_relations[relation] = model;

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 std::optional<Related> &&model)
    {
        m_relations[relation] = std::move(model);

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    requires std::is_base_of_v<Relations::IsPivotModel, Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 const std::optional<Related> &model)
    {
        m_pivots.insert(relation);

        m_relations[relation] = model;

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    requires std::is_base_of_v<Relations::IsPivotModel, Related>
    Derived &
    Model<Derived, AllRelations...>::setRelation(const QString &relation,
                                                 std::optional<Related> &&model)
    {
        m_pivots.insert(relation);

        m_relations[relation] = std::move(model);

        return this->model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    Model<Derived, AllRelations...>::qualifyColumn(const QString &column) const
    {
        if (column.contains(DOT))
            return column;

        return QStringLiteral("%1.%2").arg(model().getTable(), column);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::setAttribute(
            const QString &key, QVariant value)
    {
        /* If an attribute is listed as a "date", we'll convert it from a DateTime
           instance into a form proper for storage on the database tables using
           the connection grammar's date format. We will auto set the values. */
        if (value.isValid() && !value.isNull() && (isDateAttribute(key) ||
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            value.typeId() == QMetaType::QDateTime
#else
            value.userType() == QMetaType::QDateTime
#endif
        ))
            value = fromDateTime(value);

        // Found
        if (const auto attribute = m_attributesHash.find(key);
            attribute != m_attributesHash.end()
        )
            m_attributes[attribute->second].value.swap(value);

        // Not Found
        else {
            auto position = m_attributes.size();

            m_attributes.append({key, value});
            m_attributesHash.emplace(key, position);
        }

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setRawAttributes(
            const QVector<AttributeItem> &attributes,
            const bool sync)
    {
        m_attributes.reserve(attributes.size());
        m_attributes = Utils::Attribute::removeDuplicitKeys(attributes);

        // Build attributes hash
        m_attributesHash.clear();
        m_attributesHash.reserve(static_cast<std::size_t>(m_attributes.size()));

        rehashAttributePositions(m_attributes, m_attributesHash);

        if (sync)
            syncOriginal();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::syncOriginal()
    {
        m_original = getAttributes();

        rehashAttributePositions(m_original, m_originalHash);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QVector<AttributeItem> &
    Model<Derived, AllRelations...>::getAttributes() const
    {
        // FEATURE castable silverqx
//        mergeAttributesFromClassCasts();

        // TODO attributes, getAttributes() doesn't apply transformModelValue() on attributes, worth considering to make getRawAttributes() to return raw and getAttributes() to return transformed values, after this changes would be this api different than Eloquent silverqx
        return m_attributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const std::unordered_map<QString, int> &
    Model<Derived, AllRelations...>::getAttributesHash() const
    {
        // FEATURE castable silverqx
//        mergeAttributesFromClassCasts();

        return m_attributesHash;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant Model<Derived, AllRelations...>::getAttribute(const QString &key) const
    {
        if (key.isEmpty() || key.isNull())
            return {};

        /* If the attribute exists in the attribute hash or has a "get" mutator we will
           get the attribute's value. Otherwise, we will proceed as if the developers
           are asking for a relationship's value. This covers both types of values. */
        if (m_attributesHash.contains(key)
//            || array_key_exists($key, $this->casts)
//            || hasGetMutator(key)
//            || isClassCastable(key)
        )
            return getAttributeValue(key);

        // FUTURE add getRelationValue() overload without Related template argument, after that I will be able to use it here, Related template parameter will be obtained by the visitor, I think this task is impossible to do silverqx
        // NOTE api different silverqx
        return {};
//        return $this->getRelationValue($key);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getAttributeValue(const QString &key) const
    {
        return transformModelValue(key, getAttributeFromArray(key));
    }

    // TODO candidate for optional const reference, to be able return null value and use reference at the same time silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getAttributeFromArray(const QString &key) const
    {
        // Not found
        if (!m_attributesHash.contains(key))
            return {};

        return m_attributes.at(m_attributesHash.at(key)).value;
    }

    // NOTE api different silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getOriginal(
            const QString &key, const QVariant &defaultValue) const
    {
        return Derived().setRawAttributes(m_original, true)
                .getOriginalWithoutRewindingModel(key, defaultValue);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVector<AttributeItem>
    Model<Derived, AllRelations...>::getOriginals() const
    {
        QVector<AttributeItem> originals;
        originals.reserve(m_original.size());

        for (const auto &original : m_original) {
            const auto &key = original.key;

            originals.append({key, transformModelValue(key, original.value)});
        }

        return originals;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const std::unordered_map<QString, int> &
    Model<Derived, AllRelations...>::getOriginalsHash() const
    {
        return m_originalHash;
    }

    // NOTE api different silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getRawOriginal(
            const QString &key, const QVariant &defaultValue) const
    {
        // Found
        if (m_originalHash.contains(key))
            return m_original.at(m_originalHash.at(key)).value;

        // Not found, return the default value
        return defaultValue;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QVector<AttributeItem> &
    Model<Derived, AllRelations...>::getRawOriginals() const
    {
        return m_original;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::unsetAttribute(const AttributeItem &value)
    {
        const auto &key = value.key;

        // Not found
        if (!m_attributesHash.contains(key))
            return model();

        const auto position = m_attributesHash.at(key);

        // FUTURE all the operations on this containers should be synchronized silverqx
        m_attributes.removeAt(position);
        m_attributesHash.erase(key);

        // Rehash attributes, but only attributes which were shifted
        rehashAttributePositions(m_attributes, m_attributesHash, position);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::unsetAttribute(const QString &key)
    {
        // Not found
        if (!m_attributesHash.contains(key))
            return model();

        const auto position = m_attributesHash.at(key);

        m_attributes.removeAt(position);
        m_attributesHash.erase(key);

        // Rehash attributes, but only attributes which were shifted
        rehashAttributePositions(m_attributes, m_attributesHash, position);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QString &
    Model<Derived, AllRelations...>::getDateFormat() const
    {
        return Derived::u_dateFormat.isEmpty()
                ? getConnection().getQueryGrammar().getDateFormat()
                : Derived::u_dateFormat;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setDateFormat(const QString &format)
    {
        Derived::u_dateFormat = format;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::fromDateTime(const QVariant &value) const
    {
        if (value.isNull())
            return value;

        return asDateTime(value).toString(getDateFormat());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString
    Model<Derived, AllRelations...>::fromDateTime(const QDateTime &value) const
    {
        if (value.isValid())
            return value.toString(getDateFormat());

        return {};
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    const QStringList &
    Model<Derived, AllRelations...>::getDates() const
    {
        thread_local
        static const QStringList &dates = getDatesInternal();

        return dates;
    }

    /* Model::AttributeReference - begin */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Model<Derived, AllRelations...>::AttributeReference::AttributeReference(
            Model<Derived, AllRelations...> &model,
            // NOLINTNEXTLINE(modernize-pass-by-value)
            const QString &attribute
    )
        : m_model(model)
        , m_attribute(attribute)
    {}

    // NOLINTNEXTLINE(misc-unconventional-assign-operator)
    template<typename Derived, AllRelationsConcept ...AllRelations>
    const typename Model<Derived, AllRelations...>::AttributeReference &
    Model<Derived, AllRelations...>::AttributeReference::operator=(
            const QVariant &value) const
    {
        m_model.setAttribute(m_attribute, value);

        return *this;
    }

    // NOLINTNEXTLINE(misc-unconventional-assign-operator)
    template<typename Derived, AllRelationsConcept ...AllRelations>
    const typename Model<Derived, AllRelations...>::AttributeReference &
    Model<Derived, AllRelations...>::AttributeReference::operator=(
            const AttributeReference &attributeReference) const
    {
        m_model.setAttribute(m_attribute, attributeReference.value());

        return *this;
    }


    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QVariant *
    Model<Derived, AllRelations...>::AttributeReference::operator->() const
    {
        /* The reason why m_attributeCache exists and why QVariant * is returned:
           The overload of operator -> must either return a raw pointer, or return
           an object (by reference or by value) for which operator -> is in turn
           overloaded. */
        m_attributeCache = value();

        return &m_attributeCache;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QVariant
    Model<Derived, AllRelations...>::AttributeReference::value() const
    {
        return m_model.getAttribute(m_attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QVariant
    Model<Derived, AllRelations...>::AttributeReference::operator*() const
    {
        return value();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline Model<Derived, AllRelations...>::AttributeReference::operator QVariant() const
    {
        return value();
    }

    /* Model::AttributeReference - end */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline typename Model<Derived, AllRelations...>::AttributeReference
    Model<Derived, AllRelations...>::operator[](const QString &attribute) &
    {
        return AttributeReference(*this, attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QVariant
    Model<Derived, AllRelations...>::operator[](const QString &attribute) const &
    {
        return getAttribute(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QVariant
    Model<Derived, AllRelations...>::operator[](const QString &attribute) &&
    {
        return getAttribute(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QVariant
    Model<Derived, AllRelations...>::operator[](const QString &attribute) const &&
    {
        return getAttribute(attribute);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant Model<Derived, AllRelations...>::transformModelValue(
            const QString &key,
            const QVariant &value) const
    {
        /* Qt's SQLite driver doesn't apply any logic on the returned types, it returns
           them without type detection, and it is logical, because SQLite only supports
           numeric and string types, it doesn't distinguish datetime type or any other
           types.
           Qt's MySql driver behaves differently, QVariant already contains the QDateTime
           values, because Qt's MySQL driver returns QDateTime when the value from
           the database is datetime, the same is true for all other types, Qt's driver
           detects it and creates QVariant with proper types. */

        if (!value.isValid() || value.isNull())
            return value;

        /* If the attribute is listed as a date, we will convert it to a QDateTime
           instance on retrieval, which makes it quite convenient to work with
           date fields without having to create a mutator for each property. */
        if (getDates().contains(key))
            return asDateTime(value);

        return value;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant
    Model<Derived, AllRelations...>::getOriginalWithoutRewindingModel(
            const QString &key, const QVariant &defaultValue) const
    {
        // Found
        if (m_originalHash.contains(key))
            return transformModelValue(key, m_original.at(m_originalHash.at(key)).value);

        // Not found, return the default value
        return defaultValue;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool
    Model<Derived, AllRelations...>::relationLoaded(const QString &relation) const
    {
        return m_relations.contains(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString Model<Derived, AllRelations...>::getForeignKey() const
    {
        return QStringLiteral("%1_%2").arg(
                    Utils::String::toSnake(
                        Utils::Type::classPureBasename<Derived>()),
                    getKeyName());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::touchOwners()
    {
        for (const auto &relation : getTouchedRelations())
            touchOwnersWithVisitor(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QStringList &
    Model<Derived, AllRelations...>::getTouchedRelations() const
    {
        return model().u_touches;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline bool
    Model<Derived, AllRelations...>::touches(const QString &relation) const
    {
        return getTouchedRelations().contains(relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
#ifdef __GNUG__
    inline const std::map<QString, RelationsType<AllRelations...>> &
#else
    inline const std::unordered_map<QString, RelationsType<AllRelations...>> &
#endif
    Model<Derived, AllRelations...>::getRelations() const
    {
        return m_relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
#ifdef __GNUG__
    inline std::map<QString, RelationsType<AllRelations...>> &
#else
    inline std::unordered_map<QString, RelationsType<AllRelations...>> &
#endif
    Model<Derived, AllRelations...>::getRelations()
    {
        return m_relations;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::unsetRelations()
    {
        m_relations.clear();

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::unsetRelation(const QString &relation)
    {
        m_relations.erase(relation);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Related>
    Model<Derived, AllRelations...>::newRelatedInstance() const
    {
        auto instance = std::make_unique<Related>();

        if (instance->getConnectionName().isEmpty())
            instance->setConnection(getConnectionName());

        return instance;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    inline std::unique_ptr<Relations::HasOne<Derived, Related>>
    Model<Derived, AllRelations...>::newHasOne(
            std::unique_ptr<Related> &&related, Derived &parent,
            const QString &foreignKey, const QString &localKey) const
    {
        return Relations::HasOne<Derived, Related>::instance(
                    std::move(related), parent, foreignKey, localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    inline std::unique_ptr<Relations::BelongsTo<Derived, Related>>
    Model<Derived, AllRelations...>::newBelongsTo(
            std::unique_ptr<Related> &&related, Derived &child,
            const QString &foreignKey, const QString &ownerKey,
            const QString &relation) const
    {
        return Relations::BelongsTo<Derived, Related>::instance(
                    std::move(related), child, foreignKey, ownerKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    inline std::unique_ptr<Relations::HasMany<Derived, Related>>
    Model<Derived, AllRelations...>::newHasMany(
            std::unique_ptr<Related> &&related, Derived &parent,
            const QString &foreignKey, const QString &localKey) const
    {
        return Relations::HasMany<Derived, Related>::instance(
                    std::move(related), parent, foreignKey, localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename PivotType>
    inline std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
    Model<Derived, AllRelations...>::newBelongsToMany(
            std::unique_ptr<Related> &&related, Derived &parent, const QString &table,
            const QString &foreignPivotKey, const QString &relatedPivotKey,
            const QString &parentKey, const QString &relatedKey,
            const QString &relation) const
    {
        return Relations::BelongsToMany<Derived, Related, PivotType>::instance(
                    std::move(related), parent, table, foreignPivotKey,
                    relatedPivotKey, parentKey, relatedKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasOne<Derived, Related>>
    Model<Derived, AllRelations...>::hasOne(QString foreignKey, QString localKey)
    {
        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = model().getForeignKey();

        if (localKey.isEmpty())
            localKey = getKeyName();

        return newHasOne<Related>(std::move(instance), model(),
                                  QStringLiteral("%1.%2").arg(instance->getTable(),
                                                              foreignKey),
                                  localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::BelongsTo<Derived, Related>>
    Model<Derived, AllRelations...>::belongsTo(QString foreignKey, QString ownerKey,
                                               QString relation)
    {
        /* If no relation name was given, we will use the Related class type to extract
           the name and use that as the relationship name as most of the time this
           will be what we desire to use for the belongsTo relationships. */
        if (relation.isEmpty())
            relation = guessBelongsToRelation<Related>();

        auto instance = newRelatedInstance<Related>();

        const auto &relatedKeyName = instance->getKeyName();

        /* If no foreign key was supplied, we can guess the proper foreign key name
           by using the snake case name of the relationship, which when combined
           with an "_id" should conventionally match the columns. */
        if (foreignKey.isEmpty())
            foreignKey = QStringLiteral("%1_%2").arg(relation, relatedKeyName);

        /* Once we have the foreign key names, we return the relationship instance,
           which will actually be responsible for retrieving and hydrating every
           relations. */
        if (ownerKey.isEmpty())
            ownerKey = relatedKeyName;

        return newBelongsTo<Related>(std::move(instance), model(),
                                     foreignKey, ownerKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    std::unique_ptr<Relations::HasMany<Derived, Related>>
    Model<Derived, AllRelations...>::hasMany(QString foreignKey, QString localKey)
    {
        auto instance = newRelatedInstance<Related>();

        if (foreignKey.isEmpty())
            foreignKey = model().getForeignKey();

        if (localKey.isEmpty())
            localKey = getKeyName();

        return newHasMany<Related>(std::move(instance), model(),
                                   QStringLiteral("%1.%2").arg(instance->getTable(),
                                                               foreignKey),
                                   localKey);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related, typename PivotType>
    std::unique_ptr<Relations::BelongsToMany<Derived, Related, PivotType>>
    Model<Derived, AllRelations...>::belongsToMany(
            QString table, QString foreignPivotKey, QString relatedPivotKey,
            QString parentKey, QString relatedKey, QString relation)
    {
        /* If no relation name was given, we will use the Related class type to extract
           the name, suffix it with 's' and use that as the relationship name, as most
           of the time this will be what we desire to use for the belongsToMany
           relationships. */
        if (relation.isEmpty())
            relation = guessBelongsToManyRelation<Related>();

        /* First, we'll need to determine the foreign key and "other key"
           for the relationship. Once we have determined the keys, we'll make
           the relationship instance we need for this. */
        auto instance = newRelatedInstance<Related>();

        if (foreignPivotKey.isEmpty())
            foreignPivotKey = model().getForeignKey();

        if (relatedPivotKey.isEmpty())
            relatedPivotKey = instance->getForeignKey();

        /* If no table name was provided, we can guess it by concatenating the two
           models using underscores in alphabetical order. The two model names
           are transformed to snake case from their default CamelCase also. */
        if (table.isEmpty())
            table = pivotTableName<Related>();

        if (parentKey.isEmpty())
            parentKey = getKeyName();

        if (relatedKey.isEmpty())
            relatedKey = instance->getKeyName();

        return newBelongsToMany<Related, PivotType>(
                    std::move(instance), model(), table, foreignPivotKey,
                    relatedPivotKey, parentKey, relatedKey, relation);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::guessBelongsToRelation() const
    {
        static const QString relation = guessBelongsToRelationInternal<Related>();

        /* validateUserRelation() method call can not be cached, has to be called
           every time, to correctly inform the user about invalid relation name. */

        // Validate if the guessed relation name exists in the u_relations
        validateUserRelation(relation, RelationFrom::BELONGS_TO);

        return relation;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::guessBelongsToManyRelation() const
    {
        static const QString relation =
                QStringLiteral("%1s").arg(guessBelongsToRelationInternal<Related>());

        /* validateUserRelation() method call can not be cached, has to be called
           every time, to correctly inform the user about invalid relation name. */

        // Validate if the guessed relation name exists in the u_relations
        validateUserRelation(relation, RelationFrom::BELONGS_TO_MANY);

        return relation;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::guessBelongsToRelationInternal() const
    {
        // TODO reliability, also add Utils::String::studly silverqx
        auto relation = Utils::Type::classPureBasename<Related>();

        relation[0] = relation[0].toLower();

        return relation;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Related>
    QString Model<Derived, AllRelations...>::pivotTableName() const
    {
        /* The joining table name, by convention, is simply the snake cased, models
           sorted alphabetically and concatenated with an underscore, so we can
           just sort the models and join them together to get the table name. */
        QStringList segments {
            // The table name of the current model instance
            Utils::Type::classPureBasename<Derived>(),
            // The table name of the related model instance
            Utils::Type::classPureBasename<Related>(),
        };

        /* Now that we have the model names in the vector, we can just sort them and
           use the join function to join them together with an underscore,
           which is typically used by convention within the database system. */
        segments.sort(Qt::CaseInsensitive);

        return segments.join(UNDERSCORE).toLower();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setRelations(
#ifdef __GNUG__
            const std::map<QString, RelationsType<AllRelations...>> &relations)
#else
            const std::unordered_map<QString, RelationsType<AllRelations...>> &relations)
#endif
    {
        m_relations = relations;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &
    Model<Derived, AllRelations...>::setRelations(
#ifdef __GNUG__
            std::map<QString, RelationsType<AllRelations...>> &&relations)
#else
            std::unordered_map<QString, RelationsType<AllRelations...>> &&relations)
#endif
    {
        m_relations = std::move(relations);

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::eagerLoadRelationWithVisitor(
            const WithItem &relation, const TinyBuilder<Derived> &builder,
            QVector<Derived> &models)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation.name);

        /* Save the needed variables to the store to avoid passing variables
           to the visitor. */
        this->createEagerStore(builder, models, relation).visit(relation.name);

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    std::optional<QString>
    Model<Derived, AllRelations...>::getRelatedTableForBelongsToManyWithVisitor(
            const QString &relation)
    {
        // Throw excpetion if a relation is not defined
        validateUserRelation(relation);

        // Create the store and visit relation
        this->createBelongsToManyRelatedTableStore().visit(relation);

        // NRVO kicks in
        auto relatedTable = this->belongsToManyRelatedTableStore().m_result;

        // Releases the ownership and destroy the top relation store on the stack
        this->resetRelationStore();

        return relatedTable;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QVector<AttributeItem> &
    Model<Derived, AllRelations...>::getRawAttributes() const
    {
        return m_attributes;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline void Model<Derived, AllRelations...>::throwIfCRTPctorProblem(
            const QVector<AttributeItem> &attributes) const
    {
        throwIfQDateTimeAttribute(attributes);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::throwIfQDateTimeAttribute(
            const QVector<AttributeItem> &attributes) const
    {
        static const QString message = QStringLiteral(
            "Attributes passed to the '%1' model's constructor can't contain the "
            "QDateTime attribute, to create a '%1' model instance with attributes "
            "that contain the QDateTime attribute use %1::instance() "
            "method instead.");

        for (const auto &attribute : attributes)
            if (const auto &value = attribute.value;
                value.isValid() && !value.isNull() &&
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                value.typeId() == QMetaType::QDateTime
#else
                value.userType() == QMetaType::QDateTime
#endif
            )
                throw Orm::Exceptions::InvalidArgumentError(
                        message.arg(Utils::Type::classPureBasename<Derived>()));
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QStringList
    Model<Derived, AllRelations...>::getDatesInternal() const
    {
        if (!usesTimestamps())
            return Derived::u_dates;

        // It can be static, doesn't matter anyway
        static const QStringList &defaults = timestampColumnNames();

        auto dates = Derived::u_dates + defaults;

        dates.removeDuplicates();

        return dates;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void
    Model<Derived, AllRelations...>::validateUserRelation(
            const QString &name, const RelationFrom from) const
    {
        if (!model().u_relations.contains(name))
            throw Exceptions::RelationNotFoundError(
                    Orm::Utils::Type::classPureBasename<Derived>(), name, from);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename Result, typename Related, typename T>
    void Model<Derived, AllRelations...>::checkRelationType(
            const T &relationVariant, const QString &relation,
            const QString &source) const
    {
        if constexpr (std::is_same_v<Result, std::optional<Related>>) {
            if (!std::holds_alternative<Result>(relationVariant))
                throw Orm::Exceptions::RuntimeError(
                        QStringLiteral(
                            "The relation '%1' is many type relation, use "
                            "%2<%3>() method overload without an 'Orm::One' tag.")
                        .arg(relation, source,
                             Utils::Type::classPureBasename<Related>()));
        } else if constexpr (std::is_same_v<Result, QVector<Related>>) {
            if (!std::holds_alternative<Result>(relationVariant))
                throw Orm::Exceptions::RuntimeError(
                        QStringLiteral(
                            "The relation '%1' is one type relation, use "
                            "%2<%3, Orm::One>() method overload "
                            "with an 'Orm::One' tag.")
                        .arg(relation, source,
                             Utils::Type::classPureBasename<Related>()));
        } else
            throw Orm::Exceptions::InvalidArgumentError(
                    "Unexpected 'Result' template argument.");
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::performDeleteOnModel()
    {
        /* Ownership of a unique_ptr(), if right passed down, then the
           will be destroyed right after this command. */
        model().setKeysForSaveQuery(*newModelQuery()).remove();

        this->exists = false;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    TinyBuilder<Derived> &
    Model<Derived, AllRelations...>::setKeysForSaveQuery(TinyBuilder<Derived> &query)
    {
        return query.where(getKeyName(), EQ, getKeyForSaveQuery());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QVariant Model<Derived, AllRelations...>::getKeyForSaveQuery() const
    {
        // Found
        if (const auto keyName = getKeyName();
            m_originalHash.contains(keyName)
        )
            return m_original.at(m_originalHash.at(keyName)).value;

        // Not found, return the primary key value
        return getKey();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    TinyBuilder<Derived> &
    Model<Derived, AllRelations...>::setKeysForSelectQuery(TinyBuilder<Derived> &query)
    {
        return query.where(getKeyName(), EQ, getKeyForSelectQuery());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QVariant Model<Derived, AllRelations...>::getKeyForSelectQuery() const
    {
        // Currently is the implementation exactly the same, so I can call it
        return getKeyForSaveQuery();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::performInsert(
            const TinyBuilder<Derived> &query)
    {
//        if (!fireModelEvent("creating"))
//            return false;

        /* First we'll need to create a fresh query instance and touch the creation and
           update timestamps on this model, which are maintained by us for developer
           convenience. After, we will just continue saving these model instances. */
        if (usesTimestamps())
            updateTimestamps();

        /* If the model has an incrementing key, we can use the "insertGetId" method on
           the query builder, which will give us back the final inserted ID for this
           table from the database. Not all tables have to be incrementing though. */
        const auto &attributes = getAttributes();

        if (getIncrementing())
            insertAndSetId(query, attributes);

        /* If the table isn't incrementing we'll simply insert these attributes as they
           are. These attribute vectors must contain an "id" column previously placed
           there by the developer as the manually determined key for these models. */
        else
            if (attributes.isEmpty())
                return true;
            else
                query.insert(attributes);

        /* We will go ahead and set the exists property to true, so that it is set when
           the created event is fired, just in case the developer tries to update it
           during the event. This will allow them to do so and run an update here. */
        this->exists = true;

//        fireModelEvent("created", false);

        return true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::performUpdate(TinyBuilder<Derived> &query)
    {
        /* If the updating event returns false, we will cancel the update operation so
           developers can hook Validation systems into their models and cancel this
           operation if the model does not pass validation. Otherwise, we update. */
//        if (!fireModelEvent("updating"))
//            return false;

        /* First we need to create a fresh query instance and touch the creation and
           update timestamp on the model which are maintained by us for developer
           convenience. Then we will just continue saving the model instances. */
        if (usesTimestamps())
            updateTimestamps();

        /* Once we have run the update operation, we will fire the "updated" event for
           this model instance. This will allow developers to hook into these after
           models are updated, giving them a chance to do any special processing. */
        const auto dirty = getDirty();

        if (!dirty.isEmpty()) {
            model().setKeysForSaveQuery(query).update(
                        Utils::Attribute::convertVectorToUpdateItem(dirty));

            syncChanges();

//            fireModelEvent("updated", false);
        }

        return true;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::finishSave(const SaveOptions options)
    {
//        fireModelEvent('saved', false);

        if (isDirty() && options.touch)
            touchOwners();

        syncOriginal();
    }

    // FEATURE dilemma primarykey, add support for Derived::KeyType silverqx
    template<typename Derived, AllRelationsConcept ...AllRelations>
    quint64 Model<Derived, AllRelations...>::insertAndSetId(
            const TinyBuilder<Derived> &query,
            const QVector<AttributeItem> &attributes)
    {
        const auto &keyName = getKeyName();

        const auto id = query.insertGetId(attributes, keyName);

        // NOTE api different, Eloquent doesn't check like below and returns void instead silverqx
        // When insert was successful
        if (id != 0)
            setAttribute(keyName, id);

        /* QSqlQuery returns an invalid QVariant if can't obtain last inserted id,
           which is converted to 0. */
        return id;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    bool Model<Derived, AllRelations...>::touch()
    {
        if (!usesTimestamps())
            return false;

        updateTimestamps();

        return save();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    void Model<Derived, AllRelations...>::updateTimestamps()
    {
        const auto time = freshTimestamp();

        const QString &updatedAtColumn = getUpdatedAtColumn();

        if (!updatedAtColumn.isEmpty() && !isDirty(updatedAtColumn))
            setUpdatedAt(time);

        const QString &createdAtColumn = getCreatedAtColumn();

        if (!exists && !createdAtColumn.isEmpty() && !isDirty(createdAtColumn))
            setCreatedAt(time);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::setCreatedAt(const QDateTime &value)
    {
        return setAttribute(getCreatedAtColumn(), value);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    Derived &Model<Derived, AllRelations...>::setUpdatedAt(const QDateTime &value)
    {
        return setAttribute(getUpdatedAtColumn(), value);
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QDateTime
    Model<Derived, AllRelations...>::freshTimestamp() const
    {
        return QDateTime::currentDateTime();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QString Model<Derived, AllRelations...>::freshTimestampString() const
    {
        return fromDateTime(freshTimestamp());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline bool Model<Derived, AllRelations...>::usesTimestamps() const
    {
        return model().u_timestamps;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline Derived &
    Model<Derived, AllRelations...>::setUseTimestamps(const bool value)
    {
        model().u_timestamps = value;

        return model();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QString &
    Model<Derived, AllRelations...>::getCreatedAtColumn()
    {
        return Derived::CREATED_AT;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QString &
    Model<Derived, AllRelations...>::getUpdatedAtColumn()
    {
        return Derived::UPDATED_AT;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString Model<Derived, AllRelations...>::getQualifiedCreatedAtColumn() const
    {
        return qualifyColumn(getCreatedAtColumn());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    QString Model<Derived, AllRelations...>::getQualifiedUpdatedAtColumn() const
    {
        return qualifyColumn(getUpdatedAtColumn());
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    template<typename ClassToCheck>
    bool Model<Derived, AllRelations...>::isIgnoringTouch()
    {
        // FUTURE implement withoutTouching() and related data member $ignoreOnTouch silverqx

        return !ClassToCheck().usesTimestamps()
                || ClassToCheck::getUpdatedAtColumn().isEmpty();
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QHash<QString,
                       typename Model<Derived, AllRelations...>::RelationVisitor> &
    Model<Derived, AllRelations...>::getRelationsRawMapInternal() const
    {
        return model().u_relations;
    }

    /* GuardsAttributes */

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QStringList &
    Model<Derived, AllRelations...>::getFillableInternal()
    {
        return Derived::u_fillable;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QStringList &
    Model<Derived, AllRelations...>::getFillableInternal() const
    {
        return Derived::u_fillable;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline QStringList &
    Model<Derived, AllRelations...>::getGuardedInternal()
    {
        return Derived::u_guarded;
    }

    template<typename Derived, AllRelationsConcept ...AllRelations>
    inline const QStringList &
    Model<Derived, AllRelations...>::getGuardedInternal() const
    {
        return Derived::u_guarded;
    }

} // namespace Tiny
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

// TODO study, find out how to avoid pivot include at the end of Model's header file silverqx
#include "orm/tiny/relations/pivot.hpp"

#endif // ORM_TINY_MODEL_HPP
