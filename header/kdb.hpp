#ifndef __KDB_HPP__
#define __KDB_HPP__

#include <DatabaseConnection.h>

namespace Database {

class KDB {
 public:
  KDB(DatabaseConfiguration configuration, DatabaseCredentials credentials) : m_credentials(credentials) {
    m_connection = DatabaseConnection{};
    m_connection.setConfig(configuration);
  }

  QueryValues select(std::string table, Fields fields,
                     QueryFilter filter = {}) {
    try {
      DatabaseQuery select_query{.table = table,
                                 .fields = fields,
                                 .type = QueryType::SELECT,
                                 .filter = filter};
      QueryResult result = m_connection.query(select_query);
      if (!result.values.empty()) {
        return result.values;
      }
    } catch (const pqxx::sql_error &e) {
      KLOG->info("Database error: {}. Query was {}.", e.what(), e.query());
    } catch (const std::exception &e) {
      KLOG->error("Error", e.what());
    }
    return {{}};
  }

  QueryValues select(std::string table, Fields fields,
                     QueryComparisonFilter filter = {}) {
    try {
      ComparisonSelectQuery select_query{
          .table = table, .fields = fields, .filter = filter};
      QueryResult result = m_connection.query(select_query);
      if (!result.values.empty()) {
        return result.values;
      }
    } catch (const pqxx::sql_error &e) {
      // log
    } catch (const std::exception &e) {
      // log
    }
    return {{}};
  }

  QueryValues selectCompare(std::string table, Fields fields,
                            std::vector<CompFilter> filter = {}) {
    try {
      ComparisonBetweenSelectQuery select_query{
          .table = table, .fields = fields, .filter = filter};
      QueryResult result = m_connection.query(select_query);
      if (!result.values.empty()) {
        return result.values;
      }
    } catch (const pqxx::sql_error &e) {
      // log
    } catch (const std::exception &e) {
      // log
    }
    return {{}};
  }

  QueryValues selectMultiFilter(std::string table, Fields fields,
                                std::vector<GenericFilter> filters) {
    try {
      MultiFilterSelect select_query{
          .table = table, .fields = fields, .filter = filters};
      QueryResult result = m_connection.query(select_query);
      if (!result.values.empty()) {
        return result.values;
      }
    } catch (const pqxx::sql_error &e) {
      // log
    } catch (const std::exception &e) {
      // log
    }
    return {{}};
  }

  std::string update(std::string table, Fields fields, Values values,
                     QueryFilter filter, std::string returning) {
    try {
      // TODO: At the moment, we are married to passing the "id" field name as the "returning" argument
      UpdateReturnQuery update_query{
          .table = table, .fields = fields, .values = values, .filter = filter, .returning = returning};
      std::string result = m_connection.query(update_query);
      // log
      return result;
    } catch (const pqxx::sql_error &e) {
      // log
    } catch (const std::exception &e) {
      // log
    }
    return "";
  }

  bool insert(std::string table, Fields fields, Values values) {
    DatabaseQuery insert_query{.table = table,
                               .fields = fields,
                               .type = QueryType::INSERT,
                               .values = values};

    QueryResult result = m_connection.query(insert_query);
    // TODO: add try/catch and handle accordingly
    return true;
  }

  std::string insert(std::string table, Fields fields, Values values,
                     std::string returning) {
    InsertReturnQuery insert_query{.table = table,
                                   .fields = fields,
                                   .type = QueryType::INSERT,
                                   .values = values,
                                   .returning = returning};

    return m_connection.query(insert_query);
  }

 private:
  DatabaseConnection m_connection;
  DatabaseCredentials m_credentials;
};

}  // namespace Database

#endif  // __KDB_HPP__
