/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2016 The EVEmu Team
    For the latest information visit http://evemu.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:     Zhur
*/

#ifndef __DATABASE__DBCORE_H__INCL__
#define __DATABASE__DBCORE_H__INCL__

//this whole file could be interface-ized to support a different database
//if you can get over the SQL incompatibilities and mysql auto increment problems.

#include "database/dbtype.h"
#include "threading/Mutex.h"
// MySQL
#include <mysql.h>
#include <mysqld_error.h>
#include <errmsg.h>

class DBcore;

class DBerror
{
public:
    DBerror();

    uint32 GetErrNo() const { return mErrNo; }
    const char* GetError() const { return mErrStr.c_str(); }

    const char* c_str() const { return GetError(); }

protected:
    //for DBcore:
    friend class DBcore;
    void SetError( uint32 err, const char* str );
    void ClearError();

    std::string mErrStr;
    uint32 mErrNo;
};


class DBResultRow;
class DBQueryResult
{
public:
    DBQueryResult();
    ~DBQueryResult();

    /* error during the query, if RunQuery returned false. */
    DBerror error;

    bool GetRow( DBResultRow& into );
    size_t GetRowCount() { return (size_t)mResult->row_count; }
    void Reset();

    uint32 ColumnCount() const { return mColumnCount; }
    const char* ColumnName( uint32 index ) const;
    DBTYPE ColumnType( uint32 index ) const;

    bool IsUnsigned( uint32 index ) const;
    bool IsBinary( uint32 index ) const;

protected:
    //for DBcore:
    friend class DBcore;
    void SetResult( MYSQL_RES** res, uint32 colCount );

    uint32 mColumnCount;
    MYSQL_RES* mResult;
    MYSQL_FIELD** mFields;

    static const DBTYPE MYSQL_DBTYPE_TABLE_SIGNED[];
    static const DBTYPE MYSQL_DBTYPE_TABLE_UNSIGNED[];
};

class DBResultRow
{
public:
    DBResultRow();

    bool IsNull( uint32 index ) const { return ( NULL == GetText( index ) ); }

    const char* GetText( uint32 index ) const { return mRow[ index ]; }
    int32 GetInt( uint32 index ) const;
    bool GetBool( uint32 index ) const;
    uint32 GetUInt( uint32 index ) const;
    int64 GetInt64( uint32 index ) const;
    uint64 GetUInt64( uint32 index ) const;
    float GetFloat( uint32 index ) const;
    double GetDouble( uint32 index ) const;

    //proxy methods up to our query result:
    uint32 ColumnCount() const { return mResult->ColumnCount(); }
    const char* ColumnName( uint32 index ) const { return mResult->ColumnName( index ); }
    DBTYPE ColumnType( uint32 index ) const { return mResult->ColumnType( index ); }
    uint32 ColumnLength( uint32 index ) const;

    bool IsUnsigned( uint32 index ) const { return mResult->IsUnsigned( index ); }
    bool IsBinary( uint32 index ) const { return mResult->IsBinary( index ); }


    /**
     * Get a database row string value with null check.
     * @param index The column.
     * @return The string or an empty string on null.
     */
    std::string getTextNC(int index);
    /**
     * Get a database row int value with null check.
     * @param index The column.
     * @return The int or an zero on null.
     */
    int32 getIntNC(int index);
    /**
     * Get a database row double value with null check.
     * @param index The column.
     * @return The double or an zero on null.
     */
    double getDoubleNC(int index);

protected:
    //for DBQueryResult
    friend class DBQueryResult;
    void SetData( DBQueryResult* res, MYSQL_ROW& row, const unsigned long* lengths );

    MYSQL_ROW mRow;
    const unsigned long* mLengths;

    DBQueryResult* mResult;
};

class DBcore
{
public:
    enum eStatus { Closed, Connected, Error };

    static eStatus GetStatus()
    {
        return pStatus;
    }

    //new shorter syntax:
    //query which returns a result (error is stored in the result if it occurs)
    static bool RunQuery(DBQueryResult &into, const char *query_fmt, ...);
    //query which returns no information except error status
    static bool RunQuery(DBerror &err, const char *query_fmt, ...);
    //query which returns affected rows:
    static bool RunQuery(DBerror &err, uint32 &affected_rows, const char *query_fmt, ...);
    //query which returns last insert ID:
    static bool RunQueryLID(DBerror &err, uint32 &last_insert_id, const char *query_fmt, ...);

    //multiple command query which returns no information except error status
    static bool RunQueryMulti(DBerror &err, const char *query_fmt, ...);

    //old style to be used with MakeAnyLengthString
    static bool RunQuery(const char* query, int32 querylen, char* errbuf = 0, MYSQL_RES** result = 0, int32* affected_rows = 0, int32* last_insert_id = 0, int32* errnum = 0, bool retry = true);

    static int32 DoEscapeString(char* tobuf, const char* frombuf, int32 fromlen);
    static void DoEscapeString(std::string &to, const std::string &from);
    static bool IsSafeString(const char *str);
    static void ping();

//  static bool ReadDBINI(char *host, char *user, char *pass, char *db, int32 &port, bool &compress, bool *items);
    static bool Open(const char* iHost, const char* iUser, const char* iPassword, const char* iDatabase, int16 iPort, int32* errnum = 0, char* errbuf = 0, bool iCompress = false, bool iSSL = false);
    static bool Open(DBerror &err, const char* iHost, const char* iUser, const char* iPassword, const char* iDatabase, int16 iPort, bool iCompress = false, bool iSSL = false);

private:
    //MDatabase must be locked before these calls:
    static bool Open_locked(int32* errnum = 0, char* errbuf = 0);
    static bool DoQuery_locked(DBerror &err, const char *query, int32 querylen, bool retry = true);

    static Mutex MDatabase;
    static eStatus pStatus;

    static std::string pHost;
    static std::string pUser;
    static std::string pPassword;
    static std::string pDatabase;
    static bool pCompress;
    static int16 pPort;
    static bool pSSL;
};

#endif /* !__DATABASE__DBCORE_H__INCL__ */
