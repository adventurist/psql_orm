#pragma once

#include "database_connection.hpp"
#include "db_structs.hpp"
#include <variant>

namespace Database {

class PSQLORM {
 public:
  PSQLORM(DatabaseConfiguration config)
  : m_connection(std::move(
    std::unique_ptr<DatabaseConnection>{
      new DatabaseConnection
    }
  )) {
    m_connection->setConfig(config);
  }

  QueryValues select(std::string table, Fields fields,
                     QueryFilter filter = {}) {
    try {
      QueryResult result = m_connection->query(
        DatabaseQuery{
          .table  = table,
          .fields = fields,
          .type   = QueryType::SELECT,
          .values = {},
          .filter = filter
        });
      if (!result.values.empty())
        return result.values;

    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
    return QueryValues{};
  }

  QueryValues select(std::string table, Fields fields,
                     QueryComparisonFilter filter = {}) {
    try {
      QueryResult result = m_connection->query(
        ComparisonSelectQuery{
          .table  = table,
          .fields = fields,
          .values = {},
          .filter = filter
        });
      if (!result.values.empty()) {
        return result.values;
      }
    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
    return QueryValues{};
  }

  QueryValues selectCompare(std::string table, Fields fields,
                            std::vector<CompFilter> filter = {}) {
    try {
      ComparisonBetweenSelectQuery select_query{
        .table  = table,
        .fields = fields,
        .values = {},
        .filter = filter
      };
      QueryResult result = m_connection->query(select_query);
      if (!result.values.empty()) {
        return result.values;
      }
    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
    return QueryValues{};
  }

  QueryValues selectMultiFilter(std::string table, Fields fields,
                                std::vector<GenericFilter> filters) {
    try {
      MultiFilterSelect select_query{
        .table  = table,
        .fields = fields,
        .filter = filters
      };
      QueryResult result = m_connection->query(select_query);
      if (!result.values.empty()) {
        return result.values;
      }
    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
    return QueryValues{};
  }

  template <typename FilterA, typename FilterB>
  QueryValues selectMultiFilter(
      std::string table, Fields fields,
      std::vector<std::variant<FilterA, FilterB>> filters) {
    try {
      MultiVariantFilterSelect<std::vector<std::variant<FilterA, FilterB>>> select_query{
        .table  = table,
        .fields = fields,
        .filter = filters
      };
      QueryResult result = m_connection->query(select_query);
      for (const auto &value : result.values) {
        std::cout << "Query value: " << value.second << std::endl;
      }
      if (!result.values.empty()) {
        return result.values;
      }
    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
    return {{}};
  }

  template <typename FilterA, typename FilterB, typename FilterC>
  QueryValues selectMultiFilter(
      std::string table, Fields fields,
      std::vector<std::variant<FilterA, FilterB, FilterC>> filters) {
    try {
      MultiVariantFilterSelect<std::vector<std::variant<FilterA, FilterB, FilterC>>> select_query{
        .table  = table,
        .fields = fields,
        .filter = filters
      };
      QueryResult result = m_connection->query(select_query);
      if (!result.values.empty()) {
        return result.values;
      }
    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
    return QueryValues{};
  }

  template <typename T>
  QueryValues selectJoin(std::string table, Fields fields, T filters, Joins joins) {
    try {
      JoinQuery<T> select_query{
        .table  = table,
        .fields = fields,
        .filter = filters,
        .joins   = joins
      };
      QueryResult result = m_connection->query(select_query);
      if (!result.values.empty()) {
        return result.values;
      }
    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
    return QueryValues{};
  }

  template <typename T = QueryFilter>
  QueryValues selectSimpleJoin(std::string table, Fields fields, T filter, Join join) {
    try {
      SimpleJoinQuery<T> select_query{
        .table  = table,
        .fields = fields,
        .filter = filter,
        .join   = join
      };
      QueryResult result = m_connection->query(select_query);
      return result.values;

    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
  }

  std::string update(std::string table, Fields fields, Values values,
                     QueryFilter filter, std::string returning) {
    try {
      UpdateReturnQuery update_query{
        .table     = table,
        .fields    = fields,
        .type      = QueryType::UPDATE,
        .values    = values,
        .filter    = filter,
        .returning = returning
      };
      return m_connection->query(update_query);
    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
    return "";
  }

  std::string remove(std::string table, QueryFilter filter) {
    try {
      auto result = m_connection->query(
        DatabaseQuery{
          .table  = table,
          .fields = {},
          .type   = QueryType::DELETE,
          .values = {},
          .filter = filter
        }
      );

      if (!result.values.empty()) {
        return result.values.at(0).second;
      }
    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
    return "";
  }

  bool insert(std::string table, Fields fields, Values values) {
    try {
      QueryResult result = m_connection->query(
        DatabaseQuery{
          .table  = table,
          .fields = fields,
          .type   = QueryType::INSERT,
          .values = values
        }
      );
    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
    return true;
  }

  std::string insert(std::string table, Fields fields, Values values,
                     std::string returning) {
    try {
      if (!m_connection) {
        std::cout << "No connection" << std::endl;
      }
      return m_connection->query(
        InsertReturnQuery{
          .table     = table,
          .fields    = fields,
          .type      = QueryType::INSERT,
          .values    = values,
          .returning = returning
        }
      );
    } catch (const pqxx::sql_error &e) {
      throw e;
    } catch (const std::exception &e) {
      throw e;
    }
  }

 private:
  std::unique_ptr<DatabaseConnection> m_connection;
  DatabaseCredentials                 m_credentials;
};

}  // namespace Database
