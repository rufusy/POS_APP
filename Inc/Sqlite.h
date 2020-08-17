#ifndef __SQLITE_H_
#define __SQLITE_H_

int SqliteDB_Init(void);
int Sqlite_Get_Menu(const char * parentID,char * data);
int Sqlite_Run_Statement_MultiRecord(const char * SqlStatement,char * data);
int Sqlite_Run_Statement_MultiRecord_NoColumnName(const char * SqlStatement,char * data);
int Sqlite_Run_Statement(const char * statement,char * data);
int sqlite_Get_LOG_Record(word RRN,word APPRVCODE,word STAN);
int sqlite_CloseVoid(char * STAN);

#endif
