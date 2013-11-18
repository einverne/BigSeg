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
		printf("�ļ��򿪻��ߴ����������飡\n");
		return 0;
	}

	//����termtable�ڴ���
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


//������������
int BigDictSeg::LoadFirstWordIdx(char* filename)
{
	FILE* fp;	
	printf("Load term 'first word index'\n");
	
	fp = fopen(filename,"rt");
	if(fp == NULL)
	{
		printf("�ļ��򿪻��ߴ����������飡\n");
		return 0;
	}
	
	//�����ڴ���
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
	//ȥ����ͷ�İ���ַ�
	char tmp = s[0];

	while(tmp > 0)    
	{
		s++;
		tmp = s[0];
	}

	return s;
}



//core����������һ������ֱ�ӷ�������Ҫ�Ľ����
//���Կ���д���ļ���
//output�������������ʹ��ǰҪ����memset
int BigDictSeg::BigSegProc(char* s,char* output, int step)
{
	if (output == NULL || s == NULL)
		return false;

	int rawStrLen = strlen(s);
	int nlen = rawStrLen;	 //��������

	char *pInputStr = new char[rawStrLen+16];  
	memset(pInputStr,'\0',rawStrLen+16);
	char* ptmp = pInputStr;

	strcpy(pInputStr,s);
	char *pOutputStr = new char[rawStrLen*8];  //���ʹ�õĴ�
	memset(pOutputStr,'\0',rawStrLen*2);


	char* pMatchStr = new char[step+2];		//��ŵ�ǰmax match��
	memset(pMatchStr,'\0',step+2);
	char* ptmpstr = new char[step+2];      //�����ʱ���
	memset(ptmpstr,'\0',step+2);

	while (nlen > 0)
	{
		//int n;
		ptmp = DelHeadHalfWidthChar(ptmp);		//ȥ����ͷ�İ���ַ�
		nlen = strlen(ptmp);
		if (nlen <= 0)
			break;

		if (nlen <= step)
			strcpy(pMatchStr,ptmp);
		else
			strncpy(pMatchStr,ptmp,step);
		

		long t,b;
		int mmlen;
		if ( GetTopBottom(pMatchStr,&t,&b) )       //curstr������ƥ��
		{
			mmlen = GetMaxMatch(pMatchStr,t,b);    //curstr��mmƥ�䳤��
		
			if (mmlen>0)
			{
				//�����match��������޸�ָ�룬�޸�nlen
				strncpy(ptmpstr,pMatchStr,mmlen);
				strcat(pOutputStr,ptmpstr);
				strcat(pOutputStr,"|||");
				memset(ptmpstr,'\0',step);
				nlen -= mmlen;
				ptmp += mmlen;
				
			}
			else  //��ƥ��
			{
				ptmp++;
				ptmp++;   //Խ��һ������
			}
		}
		else   //curstr������ƥ��	
		{
// 			strncpy(ptmpstr,pMatchStr,2);
// 			strcat(pOutputStr,ptmpstr);
// 			memset(ptmpstr,'\0',step);
			ptmp++;
			ptmp++;   //Խ��һ������
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
//ȡ��s�ĵ�һ�����֣���FW�н��ж��ֲ��ң��ҵ��󷵻ؿ�ʼ�ͽ�������
//����Ҳ�����˵���������û���Ը��ֿ�ͷ���������false
	
	char* ss = s;
	char tmp[3];
	memcpy(tmp,ss,2);
	tmp[2] = '\0';

	long ulTop,ulMiddle,ulBottom;//���ַ�����

	ulTop = 0;
	ulBottom = m_nFWCount;			//FW��С ������С

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
//����һ���ַ������Լ����ַ������ַ���Ӧ�����¶˵�
//��termtable��Ѱ�ң����ظ��ַ����ƥ�䳤��
//����޷�ƥ�䣬����-1
//����Ҫע��ȫ�ǰ�ǵ����⣬�ڻ���ʱ����

	char target[TERM_MAX_LEN];			//�����ַ���s�ĳ����޶���60�֣���128byte   ÿ���ʵĳ���
	memset(target,'\0',TERM_MAX_LEN);
	strcpy(target,s);
	int len = strlen(target);
	
	if (len < 0 )
	{
		return -1;	//s ��δ�ҵ�
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
// 	//ѭ������
// 	while (len > 0)
// 	{
// 		//��������target,ÿ��ѭ�������¸�ֵtop,bottom
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
// 				//ƥ��ɹ����������ƥ�䳤��
// 				return len;
// 			}
// 			if (abc > 0) //target��pterm�·�
// 			{
// 				tmpT = tmpM+1;
// 			}
// 			if (abc < 0)   //target��pterm�Ϸ�
// 			{
// 				if (tmpM > 0)
// 					tmpB = tmpM-1;
// 				else break;
// 			}
// 		}
// 		//��������������û���ҵ���ȷƥ��
// 
// 		if (target[len-1] >= 0)
// 		{
// 			target[len-1] = '\0';  //����ַ��ǰ�ǣ��˺�һ��char
// 			len -= 1;
// 		}
// 		else
// 		{
// 			target[len-1] = '\0';
// 			target[len-2] = '\0';   //����ַ���ȫ�ǣ��˺�1������
// 			len -=2;				
// 		}
// 	}

	return -1;   //һ���ֶ��޷�ƥ�䣬����-1
				//������������ڣ�������ĳ���ֿ�ͷ�� ��������ֱ�����һ����������
}


