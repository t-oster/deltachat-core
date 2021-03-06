/*******************************************************************************
 *
 *                              Delta Chat Core
 *                      Copyright (C) 2017 Björn Petersen
 *                   Contact: r10s@b44t.com, http://b44t.com
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/ .
 *
 ******************************************************************************/


#include "mrmailbox_internal.h"
#include "mrtoken.h"


void mrtoken_save__(mrmailbox_t* mailbox, mrtokennamespc_t namespc, uint32_t foreign_id, const char* token)
{
	sqlite3_stmt* stmt = NULL;

	if( mailbox == NULL || mailbox->m_magic != MR_MAILBOX_MAGIC || token == NULL ) { // foreign_id may be 0
		goto cleanup;
	}

	stmt = mrsqlite3_prepare_v2_(mailbox->m_sql,
		"INSERT INTO tokens (namespc, foreign_id, token, timestamp) VALUES (?, ?, ?, ?);");
	sqlite3_bind_int  (stmt, 1, (int)namespc);
	sqlite3_bind_int  (stmt, 2, (int)foreign_id);
	sqlite3_bind_text (stmt, 3, token, -1, SQLITE_STATIC);
	sqlite3_bind_int64(stmt, 4, time(NULL));
	sqlite3_step(stmt);

cleanup:
	if( stmt ) { sqlite3_finalize(stmt); }
}


char* mrtoken_lookup__(mrmailbox_t* mailbox, mrtokennamespc_t namespc, uint32_t foreign_id)
{
	char*         token = NULL;
	sqlite3_stmt* stmt  = NULL;

	if( mailbox == NULL || mailbox->m_magic != MR_MAILBOX_MAGIC ) {
		goto cleanup;
	}

	stmt = mrsqlite3_prepare_v2_(mailbox->m_sql,
		"SELECT token FROM tokens WHERE namespc=? AND foreign_id=?;");
	sqlite3_bind_int (stmt, 1, (int)namespc);
	sqlite3_bind_int (stmt, 2, (int)foreign_id);
	sqlite3_step(stmt);

	token = strdup_keep_null((char*)sqlite3_column_text(stmt, 0));

cleanup:
	if( stmt ) { sqlite3_finalize(stmt); }
	return token;
}


int mrtoken_exists__(mrmailbox_t* mailbox, mrtokennamespc_t namespc, const char* token)
{
	int           exists = 0;
	sqlite3_stmt* stmt   = NULL;

	if( mailbox == NULL || mailbox->m_magic != MR_MAILBOX_MAGIC || token == NULL ) {
		goto cleanup;
	}

	stmt = mrsqlite3_prepare_v2_(mailbox->m_sql,
		"SELECT id FROM tokens WHERE namespc=? AND token=?;");
	sqlite3_bind_int (stmt, 1, (int)namespc);
	sqlite3_bind_text(stmt, 2, token, -1, SQLITE_STATIC);

	exists = (sqlite3_step(stmt)!=0);

cleanup:
	if( stmt ) { sqlite3_finalize(stmt); }
	return exists;
}
