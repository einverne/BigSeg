// BigDictSeg.cpp: implementation of the BigDictSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "BigDictSeg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BigDictSeg::BigDictSeg()
{
	m_nTermCount = 0;   
	m_nFWCount = 0;
	m_strTermFW = NULL;
	m_strTermTable = NULL;

}

BigDictSeg::~BigDictSeg()
{

}


int BigDictSeg::InitProc(char* table, char* idx)
{
	if (!LoadTermTable(table))
	{
		printf("Load Term Table error!\n");
		return 0;
	}
	if (!LoadFirstWordIdx(idx))
	{
		printf("Load Index file error!\n");
		return 0;
	}
	return 1;
}	


int BigDictSeg::ExitProc()
{
	UnloadTermTable();
	UnloadFirstWordIdx();
	return 1;
}


int BigDictSeg::LoadTermTable(char* filename)
{
	FILE* fp;

	printf("Begin load term table...\n");

	fp = fopen(filename,"rt");
	if(fp == NULL)
	{
		printf("文件打开或者创建错误，请检查！\n");
		return 0;
	}

	//建立termtable内存区
	m_strTermTable = new TERMTABLE[TERM_TABLE_MAX_SIZE];   //1,500,000
	//ASSERT(m_strTermTable);
	////////

	char *newline = new char[1024];
	char *temp;

	long linenum = 0;

	while(!feof(fp))
	{
		memset(newline,'\0',1024);
		fgets(newline,1024,fp);
		
		temp = strtok(newline,"\r\n");
		if (temp == NULL)
		{
			continue;
		}
		
		m_strTermTable[linenum].nTermID = linenum;
		strcpy(m_strTermTable[linenum].strterm,temp);
		memcpy(m_strTermTable[linenum].firstword,temp,2);
		m_strTermTable[linenum].firstword[2] = '\0';

		linenum++;
		if(linenum%1000 == 0)
			printf("\r%d",linenum);

	}
	
	m_nTermCount = linenum;   //all term count

	printf("\nFinished!\n");
	
	delete [] newline;
	return 1;
}

int BigDictSeg::UnloadTermTable()
{
	
	if (m_strTermTable != NULL)
	{
		delete [] m_strTermTable;
		m_strTermTable = NULL;
	}

	return 1;
}


//加载首字索引
int BigDictSeg::LoadFirstWordIdx(char* filename)
{
	FILE* fp;	
	printf("Load term 'first word index'\n");
	
	fp = fopen(filename,"rt");
	if(fp == NULL)
	{
		printf("文件打开或者创建错误，请检查！\n");
		return 0;
	}
	
	//建立内存区
	m_strTermFW = new TERMFW[TERM_FW_MAX_SIZE];  //10,000
	//ASSERT(m_strTermFW);
	////////
		
	char *newline = new char[1024];
	char *temp;
	
	long nextstart = 0;	
	long linenum = 0;

	//first line
	memset(newline,'\0',1024);
	fgets(newline,1024,fp);
	temp = strtok(newline,"\r\n");

	if (temp == NULL)
		return 0;
		
	temp = strtok(temp,"|||");
	strcpy(m_strTermFW[0].word,temp);

	temp = strtok(NULL,"|||");
	m_strTermFW[0].nStart = 0;			//index form '0'
	m_strTermFW[0].nEnd = atol(temp);
	
	nextstart = m_strTermFW[0].nEnd + 1;   //next start idx
	linenum++;


	//left lines
	while(!feof(fp))
	{
		memset(newline,'\0',1024);
		fgets(newline,1024,fp);
		
		temp = strtok(newline,"\r\n");
		if (temp == NULL)
			continue;

		temp = strtok(temp,"|||");
		strcpy(m_strTermFW[linenum].word,temp);

		temp = strtok(NULL,"|||");
		m_strTermFW[linenum].nStart = nextstart;
		m_strTermFW[linenum].nEnd = atol(temp);
		
		nextstart = m_strTermFW[linenum].nEnd +1;
		linenum++;

	}
	
	m_nFWCount = linenum;

	delete [] newline;

	
	printf("Finished!\n");

	return 1;
	
}


int BigDictSeg::UnloadFirstWordIdx()
{
	
	if (m_strTermFW != NULL)
	{
		delete [] m_strTermFW;
		m_strTermFW = NULL;
	}
	
	return 1;
}

char* BigDictSeg::DelHeadHalfWidthChar(char* s)
{
	//去掉开头的半角字符
	char tmp = s[0];

	while(tmp > 0)    
	{
		s++;
		tmp = s[0];
	}

	return s;
}



//core函数，输入一个串，直接返回所需要的结果串
//可以考虑写到文件中
//output是输出参数，在使用前要进行memset
int BigDictSeg::BigSegProc(char* s,char* output, int step)
{
	if (output == NULL || s == NULL)
		return false;

	int rawStrLen = strlen(s);
	int nlen = rawStrLen;	 //待处理长度

	char *pInputStr = new char[rawStrLen+16];  
	memset(pInputStr,'\0',rawStrLen+16);
	char* ptmp = pInputStr;

	strcpy(pInputStr,s);
	char *pOutputStr = new char[rawStrLen*8];  //输出使用的串
	memset(pOutputStr,'\0',rawStrLen*2);


	char* pMatchStr = new char[step+2];		//存放当前max match串
	memset(pMatchStr,'\0',step+2);
	char* ptmpstr = new char[step+2];      //存放临时结果
	memset(ptmpstr,'\0',step+2);

	while (nlen > 0)
	{
		//int n;
		ptmp = DelHeadHalfWidthChar(ptmp);		//去掉开头的半角字符
		nlen = strlen(ptmp);
		if (nlen <= 0)
			break;

		if (nlen <= step)
			strcpy(pMatchStr,ptmp);
		else
			strncpy(pMatchStr,ptmp,step);
		

		long t,b;
		int mmlen;
		if ( GetTopBottom(pMatchStr,&t,&b) )       //curstr首字有匹配
		{
			mmlen = GetMaxMatch(pMatchStr,t,b);    //curstr的mm匹配长度
		
			if (mmlen>0)
			{
				//如果有match，输出，修改指针，修改nlen
				strncpy(ptmpstr,pMatchStr,mmlen);
				strcat(pOutputStr,ptmpstr);
				strcat(pOutputStr,"|||");
				memset(ptmpstr,'\0',step);
				nlen -= mmlen;
				ptmp += mmlen;
				
			}
			else  //无匹配
			{
				ptmp++;
				ptmp++;   //越过一个汉字
			}
		}
		else   //curstr首字无匹配	
		{
// 			strncpy(ptmpstr,pMatchStr,2);
// 			strcat(pOutputStr,ptmpstr);
// 			memset(ptmpstr,'\0',step);
			ptmp++;
			ptmp++;   //越过一个汉字
		}
	}

	//output/////////
	strcpy(output,pOutputStr);
	///////////////

	//del mem
	
	delete [] pInputStr;
	delete [] pOutputStr; 
	delete [] pMatchStr;
	delete [] ptmpstr;

	return 1;
}


bool BigDictSeg::GetTopBottom(char* s, long* top, long* bottom)
{
//取出s的第一个汉字，在FW中进行二分查找，找到后返回开始和结束索引
//如果找不到，说明术语表中没有以该字开头的术语，返回false
	
	char* ss = s;
	char tmp[3];
	memcpy(tmp,ss,2);
	tmp[2] = '\0';

	long ulTop,ulMiddle,ulBottom;//二分法查找

	ulTop = 0;
	ulBottom = m_nFWCount;			//FW大小 索引大小

	while (ulTop <= ulBottom)	
	{
		ulMiddle = (ulTop+ulBottom)/2;

		int abc = strcmp(tmp,m_strTermFW[ulMiddle].word);

		if (abc == 0)			
		{
			*top = m_strTermFW[ulMiddle].nStart;
			*bottom = m_strTermFW[ulMiddle].nEnd;
			return true;			
		}	
		
		if (abc > 0) 
		{
			ulTop = ulMiddle+1;
		}
		if (abc < 0) 
		{
			if (ulMiddle > 0) 
				ulBottom = ulMiddle-1;
			else break;
		}
	}	
		
	return false;
}


int BigDictSeg::GetMaxMatch(char* s, long top, long bottom)
{
//输入一个字符串，以及该字符串首字符对应的上下端点
//在termtable中寻找，返回该字符串最长匹配长度
//如果无法匹配，返回-1
//这里要注意全角半角的问题，在回溯时考虑

	char target[TERM_MAX_LEN];			//输入字符串s的长度限定在60字，即128byte   每个词的长度
	memset(target,'\0',TERM_MAX_LEN);
	strcpy(target,s);
	int len = strlen(target);
	
	if (len < 0 )
	{
		return -1;	//s 中未找到
	}
	char* subs = new char[len];
	while(top<=bottom){
		char* term = m_strTermTable[bottom].strterm;
		int term_len = strlen(term);
		memset(subs,'\0',len);
		memcpy(subs,target,term_len);
		int re = strcmp(subs,term);
		if (re == 0)
		{
			return term_len;
		}
		bottom--;
	}
	

// 	long tmpT = top;
// 	long tmpB = bottom;
// 	long tmpM = (tmpT+tmpB)/2;
// 	//循环搜索
// 	while (len > 0)
// 	{
// 		//二分搜索target,每次循环都重新赋值top,bottom
// 		tmpT = top;
// 		tmpB = bottom;
// 
// 		while (tmpT <= tmpB)	
// 		{
// 			tmpM = (tmpT+tmpB)/2;
// 			char* pterm = m_strTermTable[tmpM].strterm;
// 			
// 			int abc = strcmp(target,pterm);
// 			if (abc == 0)			
// 			{
// 				//匹配成功，返回最大匹配长度
// 				return len;
// 			}
// 			if (abc > 0) //target在pterm下方
// 			{
// 				tmpT = tmpM+1;
// 			}
// 			if (abc < 0)   //target在pterm上方
// 			{
// 				if (tmpM > 0)
// 					tmpB = tmpM-1;
// 				else break;
// 			}
// 		}
// 		//二分搜索结束，没有找到精确匹配
// 
// 		if (target[len-1] >= 0)
// 		{
// 			target[len-1] = '\0';  //最后字符是半角，退后一个char
// 			len -= 1;
// 		}
// 		else
// 		{
// 			target[len-1] = '\0';
// 			target[len-2] = '\0';   //最后字符是全角，退后1个汉字
// 			len -=2;				
// 		}
// 	}

	return -1;   //一个字都无法匹配，返回-1
				//这种情况出现在：存在以某个字开头的 术语，而该字本身不是一个单字术语
}


