// BigDictSeg.h: interface for the BigDictSeg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BIGDICTSEG_H__2519F522_F214_439F_B8C4_D9AB0E2A0BF4__INCLUDED_)
#define AFX_BIGDICTSEG_H__2519F522_F214_439F_B8C4_D9AB0E2A0BF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TERM_MAX_LEN 128
#define TERM_TABLE_MAX_SIZE 1500000
#define TERM_FW_MAX_SIZE 10000
#define STEP_LEN 30

typedef struct term_first_word_prir
{
	int  nStart;
	int  nEnd;
	char word[3];
	
}TERMFW;


//词表
typedef struct term_table
{
	int nTermID;
	char strterm[TERM_MAX_LEN];		//词长度128最大,假设
	char firstword[3];				//首字
	
}TERMTABLE;


class BigDictSeg  
{
public:
	BigDictSeg();
	virtual ~BigDictSeg();

//methods
public:

	int BigSegProc(char* s,char* u, int step=STEP_LEN);	//s是原始输入串，u是标注好的输出串，step是每次长度
	int InitProc(char* table, char* idx);
	int ExitProc();

//private:
public:

	//Init proc
	int LoadTermTable(char* filename);
	int LoadFirstWordIdx(char* filename);
	
	//exit mem proc
	int UnloadTermTable();
	int UnloadFirstWordIdx();

	//core proc
	bool GetTopBottom(char* s, long* top, long* bottom);
	int  GetMaxMatch(char* s, long top, long bottom);

	//functions
	char* DelHeadHalfWidthChar(char* s);

//members

public:
	
	long    m_nTermCount;   
	long    m_nFWCount;
	TERMFW* m_strTermFW;
	TERMTABLE* m_strTermTable;
	

};

#endif // !defined(AFX_BIGDICTSEG_H__2519F522_F214_439F_B8C4_D9AB0E2A0BF4__INCLUDED_)
