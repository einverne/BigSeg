// BigSeg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "bigdictseg.h"

#define BUFFER_SIZE 40960


int main(int argc, char* argv[])
{

	if (argc < 6)
	{
		//bigseg.exe term_dict.txt index2.txt test_cop.txt index2_out.txt 
		printf("USEAGE: bigseg.exe termlistfile FW_Idxfile inputfile outputfile stepsize\r\n");
		printf("step size is an int, indicates max-match step length\r\n");
		return -1;
	}
	
	//read args
	char *ptermlistfile = new char[1024];
	char *pFWfile = new char[1024];
	char *pinputfile = new char[1024];
	char *poutputfile = new char[1024];
	
	strcpy(ptermlistfile,argv[1]);
	strcpy(pFWfile,argv[2]);	
	strcpy(pinputfile,argv[3]);
	strcpy(poutputfile,argv[4]);
	int nStep = atoi(argv[5]);

// 	char *ptermlistfile = "term_dict.txt";
// 	char* pFWfile = "index1.txt";
// 	char* pinputfile = "test_cop.txt";
// 	char* poutputfile = "index1_out.txt";
// 	int nStep = 300;

	FILE *fpInput = fopen(pinputfile,"rt");
	if(fpInput == NULL)
	{
		printf("输入文件打开错误，请检查！\n");
		return -1;
	}

	FILE *fpOutput = fopen(poutputfile,"wt");
	if (fpOutput == NULL)
	{
		printf("输出文件打开错误，请检查！\n");
		return -1;
	}

//////////////////////////////////////////
//
//		start processing...
//
//////////////////////////////////////////

	BigDictSeg segObjct;
	
	//加载termTable,字典程序
	int nflag =	segObjct.LoadTermTable(ptermlistfile);
	if (nflag == 0)
		return -1;
	
	//加载index
	nflag = segObjct.LoadFirstWordIdx(pFWfile);
	if (nflag == 0)
		return -1;

	char *pInputBuf = new char[BUFFER_SIZE];
	char *pOutputBuf = new char[BUFFER_SIZE*2];

	char *temp;
	long linenum = 0;

	while(!feof(fpInput))
	{
		memset(pInputBuf,'\0',BUFFER_SIZE);
		memset(pOutputBuf,'\0',BUFFER_SIZE*2);

		fgets(pInputBuf,BUFFER_SIZE,fpInput);

		
		//lang line detection
		if (strlen(pInputBuf)>= BUFFER_SIZE-10)
				printf("too long line at:%d\r\n",linenum);
		///////////////
		
		temp = strtok(pInputBuf,"\r\n");
		if (temp == NULL)
			continue;
				
		linenum++;
		if(linenum%1000 == 0)
		{//	printf("\r%d",linenum);
			printf("%d\r\n",linenum);
		}
		segObjct.BigSegProc(temp,pOutputBuf,nStep);
		fprintf(fpOutput,"%d###%s\n",linenum,pOutputBuf);
	
	}
	
	if (fpInput != NULL)
		fclose(fpInput);
	
	if (fpOutput != NULL)
		fclose(fpOutput);
	
	delete [] pInputBuf;
	delete [] pOutputBuf;

	delete [] ptermlistfile;
	delete [] pFWfile;
	delete [] pinputfile;
	delete [] poutputfile;
	return 0;
}

