/**
 * @file    Transaction.cpp
 * @ingroup SQLiteCpp
 * @brief   A Transaction is way to group multiple SQL statements into an atomic secured operation.
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#include <SQLiteCpp/Transaction.h>

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Assertion.h>


namespace SQLite
{


// Begins the SQLite transaction
Transaction::Transaction(Database& aDatabase, TransactionType type) 
: mDatabase(aDatabase), mbCommited(false)
{
    switch (type)
    {
        case TransactionType::Deferred:
            mDatabase.exec("BEGIN DEFERRED TRANSACTION");
            break;

        case TransactionType::Immediate:
            mDatabase.exec("BEGIN IMMEDIATE TRANSACTION");
            break;

        case TransactionType::Exclusive:
            mDatabase.exec("BEGIN EXCLUSIVE TRANSACTION");
            break;

        default:
            throw SQLite::Exception("Unknown transaction type");
    }
}

// Safely rollback the transaction if it has not been committed.
Transaction::~Transaction() noexcept // nothrow
{
    if (false == mbCommited)
    {
        try
        {
            mDatabase.exec("ROLLBACK");
        }
        catch (SQLite::Exception& e)
        {
            // Never throw an exception in a destructor
            SQLITECPP_ASSERT(false, e.what());  // See SQLITECPP_ENABLE_ASSERT_HANDLER
        }
    }
}

// Commit the transaction.
void Transaction::commit()
{
    if (false == mbCommited)
    {
        mDatabase.exec("COMMIT");
        mbCommited = true;
    }
    else
    {
        throw SQLite::Exception("Transaction already commited");
    }
}

void Transaction::SetSavepoint(const std::string& savepoint)
{
    mDatabase.exec("SAVEPOINT " + savepoint);
}

void Transaction::ReleaseSavepoint(const std::string& savepoint)
{
    mDatabase.exec("RELEASE SAVEPOINT " + savepoint);
}

void Transaction::RollbackSavepoint(const std::string& savepoint)
{
    mDatabase.exec("ROLLBACK TO SAVEPOINT " + savepoint);
}



}  // namespace SQLite
