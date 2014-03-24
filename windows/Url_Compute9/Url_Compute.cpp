//////////////////////////////////////////////////////////////////////////
/*	Ŀǰ��Ҫ��hash��ռ�����ڴ棻
	�������ӽڵ�Ķ�Ӧ��ϵһ�߶���һ������ֲ��ڲ�ͬ���ļ����У���ռ���ڴ棻

	ÿ���ڵ�ĳ��ȴ�hash���п��Եõ�;
	��������ĸ���㣬��hash���в��ң����ҵ�������ȼ�1����û���ҵ����������У�
	�������Ӧ���ӽڵ㣬��hash���в��ң����ҵ��򷵻أ���û���ҵ�����в��룻
	-Kevinew@������Լ����Ƽ���˾
*/
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ChainingHash.h"
#include "Url_Compute.h"
#include <algorithm>
using namespace std;


#define  MaxLen 10000
#define  MPr 10000000
#define  NumBlock	128

UINT64 vect[1024*768];
double  Pr[2][MPr];


struct dist{
	UINT64 docid1,docid2;
	UINT64 outdegree;
} didsave[1024*768];
int Ndid = 0;



//4 distribution blocks file
char BlockFile[4][16] = {"Block_0.txt" , "Block_1.txt" , "Block_2.txt" , "Block_3.txt"}; 
char DBlockFile[4][16] = {"DBlock_0.txt" , "DBlock_1.txt" , "DBlock_2.txt" , "DBlock_3.txt"};  //���ڽӱ�


bool cmp(dist a,dist b){
	if (a.docid2 < b.docid2) return true;	
	return false;
}


/* ���ջ��ֿ� �����ӹ�ϵ */
void Distribution_Head(int n){					//��URL�����ӹ�ϵ���з�����n����ͬ���ļ���--Block_i.txt
	int i,len;
	char strget[2048], fname[20], iname[20] = {"Block_"};
	UINT64 docid1,docid2,siteid1,siteid2;		//docid1 -> docid2 (link point to)	
	FILE *fpblock[1024];

	for ( i = 0; i < n; i ++ ){		//һ�δ�n�����ļ�,׼�������д����
		strcpy( fname , iname );
		sprintf(fname + 6 , "%d" , i ) ;
		strcat( fname, ".txt" );
		fpblock[i] = fopen( fname , "wb" );
	}
  
	FILE *fp;
	if( (fp = fopen("UrlRelationship.txt","rb+") ) == NULL) { //��ԭURL���ӹ�ϵ�ļ�
		printf("cann't open file UrlRelationship.txt");
		return ;  
	}

	while(fread(strget,sizeof(char),4,fp)){	//һ�ߴ�ԭʼ�ļ����ж��룬һ�߽����ļ��ķ�������docid�����ӹ�ϵ���б��档
		len=*(int*)strget;					//len
		fread(strget,sizeof(char),len,fp);	//linkword
		fread(strget,sizeof(char),8,fp);	//docid of mainpage
		docid1=*(UINT64*)strget;
		fread(strget,sizeof(char),8,fp);	//siteid of mainpage
		siteid1=*(UINT64*)strget;		
		fread(strget,sizeof(char),8,fp);	//docid
		docid2=*(UINT64*)strget;
		fread(strget,sizeof(char),8,fp);	//sitecid
		siteid2=*(UINT64*)strget;
		
		//Havn't save the relationship between father and son docid to Haid.
		
		if ( len < 0 || len > 100 ) {		//��ԭʼ�ļ��޳��쳣����
			continue;
		}

		fprintf( fpblock[docid2 % n], "%I64u %I64u %I64u %I64u ", docid1, docid2, siteid1, siteid2 );//����ģ����벻ͬ�Ŀ��ļ��У�	
	}

	fclose(fp);	

	for ( i = 0; i < n; i ++ ){		//�ر�n����������ļ�ָ��
		fclose( fpblock[i] ); fpblock[i] = NULL;
	}
}


/* ���ѵõĻ��ֿ��ļ� �ֱ���а�Դ����� */
void Distribution_Tail(int n){				//����β���з���  ----Tail_i.txt
	int i,j;
	char fname[20], iname[20] = {"Block_"}, tail[20] = {"Tail_"};
	UINT64 docid1,docid2,siteid1,siteid2;	//docid1 -> docid2 (link point to)	
	FILE *fpblock[1024], *fptail[1024]; 

	for ( j = 0 ; j < n; j++ ){				//��n����β���ļ�
		strcpy( fname , tail );
		sprintf( fname + 5 , "%d" , j ) ;
		strcat( fname, ".txt" );
		fptail[j] = fopen(fname , "wb") ;	//��д��洢��tail�ļ���
	}
	for ( i = 0; i < n; i ++ ){				//һ�δ�n�����ļ�,׼������Ķ�����
		ChainingHash<UINT64>	Ha(1024*1024);

		strcpy( fname , iname );
		sprintf( fname + 6 , "%d" , i ) ;
		strcat( fname, ".txt" );
		fpblock[i] = fopen( fname , "rb" ); //�򿪻�ͷ�ļ�fpblock[i]

		while (fscanf(fpblock[i], "%I64u %I64u %I64u %I64u ", &docid1, &docid2, &siteid1, &siteid2 ) != -1){
			if (!Ha.search(docid1)) {		//����β�����ҵ���ֻ�轫outdegree��1��
				Ha.insert(docid1,siteid1);  //��û���ҵ������轫����룬������docid��siteidһͬ�������Ľڵ��У�
			}
			Ha.search_to(docid2,siteid2);	//����ͷ����û�ҵ���������ڱ�β��
		}

		Ha.SavetoFile( fptail , n );		//���ݻ�β�ļ�ָ�룬n����ͷ�ֿ��ļ�,���������������ͬ���ļ���

		fclose(fpblock[i]);					//�رջ�ͷ�ļ�fpblock[i]

		//���뿪����ѭ��ʱ��Ha�뿪�����������Զ��������������� �����ڲ��þ͸�����~ ��
	}

	for ( j = 0 ; j < n; j++ ){				//�Ի�β�ļ�j��ȡ����Ȼ��ͳһ���������д����ļ�		
		fclose( fptail[j] );
	}

}


//* �Ի�β�ļ�����ͳ�� �õ�ÿ��docid��outdegree */
void Tail_Statistic(int n){					//�ۺϼ���outdegree, ������Tail_i.txt�ļ��� 
	int j;
	UINT64 did,sid,od;
	char fname[20],tail[20] = {"Tail_"};
	FILE *fptail[1024];

	for ( j = 0 ; j < n; j++ ){				//��n����β���ļ�
		strcpy( fname , tail );
		sprintf( fname + 5 , "%d" , j ) ;				
		strcat( fname, ".txt" );
		fptail[j] = fopen(fname , "rb") ;	//��tail�ļ��е����ݽ��д���
		ChainingHash<UINT64>	Ha(1024*1024);

		while ( fscanf(fptail[j], "%I64u %I64u %I64u",&did,&sid,&od) != -1 ){		
			Ha.tail_search(did,sid,od);
		}
		fclose(fptail[j]);

		fptail[j] = fopen(fname , "wb") ; 
		Ha.SaveAFile(fptail[j]); 
		fclose(fptail[j]);
	}
}


/* Ѱ�ҳ��� */
UINT64 Find_Outdegree( UINT64 docid1,int n ){  
	int j;
	UINT64 did,sid,od;
	char fname[20],tail[20] = {"Tail_"};
	FILE *fp;

	j = docid1 % n;
	strcpy( fname , tail );
	sprintf( fname + 5 , "%d" , j ) ;				
	strcat( fname, ".txt" );
	fp = fopen(fname , "rb");
	if (fp == NULL){
		fprintf(stderr, "%s\n", strerror(errno));
		return -1;
	}
	while ( fscanf(fp, "%I64u %I64u %I64u",&did,&sid,&od) != -1 ){
		if ( did == docid1 ){
				fclose(fp);
			return od;
		}
	}
	fclose(fp);

	return -1;
}


/*��Block_i.txt�����ڽӱ�洢������ͷ�ڵ��Ÿõ�ĳ��Ⱥ͸õ��ڴ��ļ��д��ڵ��ڽӵ����*/
void GetLinkFile (int n){
	//����ԭ�ļ�������洢�ṹ����ͬ��docid1��һ�����Դ˴����ö��м�������󣬲�����Ⱥ��ӽڵ������������һ�ļ�
	//��Block_i.txt�ļ�����ȡLTable_i.txtΪ�ڽӱ�洢�ļ�
	int i,j;
	char fname[20], iname[20] = {"Block_"}, lname[20] = {"LTable_"};
	UINT64 odg,docid1,docid2,siteid1,siteid2,tablehead,nv;	//docid1 -> docid2 (link point to)	; odg-outdegree
	FILE *fpblock[1024] , *fptable[1024];

	for(i = 0 ; i < n ; i++ ){
		strcpy( fname , iname );
		sprintf( fname + 6 , "%d" , i ) ;
		strcat( fname, ".txt" );
		fpblock[i] = fopen( fname , "rb" );		//�򿪻�ͷ�ļ�fpblock[i]
		
		strcpy( fname , lname );
		sprintf( fname + 7 , "%d" , i ) ;
		strcat( fname, ".txt" );
		fptable[i] = fopen( fname , "wb" );		//���ڽ��ļ�fptable[i]
		
		nv = 0 ; tablehead = 0 ;	//��ֵ

		while (fscanf(fpblock[i], "%I64u %I64u %I64u %I64u ", &docid1, &docid2, &siteid1, &siteid2 ) != -1)   
		{//һ��docid1��Ӧ�Ŷ��docid2
			if (docid1 == tablehead){
				vect[nv++] = docid2; //docid2����У����������ԭ�ļ��Ĵ洢�ṹ��      
				continue; 
			} 

			if ( tablehead== 0){
				tablehead = docid1	;		//�ı�tablehead��ֵ
				vect[nv++] = docid2	;		//���Կ�ʼ�������
				continue; 				
			}
			//���vect����һ�ļ�

			//odg Ϊ tablehead �� outdegree    ????????
			odg = Find_Outdegree(tablehead , n);	//��û�ҵ����ȣ��򷵻�-1

			if (odg == -1){
				nv = 0 ; continue;			 
			}

			//fprintf( fptable[i] , "%I64u %I64u %d " , tablehead , odg , nv );
			//for ( j = 0 ; j < nv ; j ++ ){
			//	fprintf( fptable[i] , "%I64u " , vect[j] );
			//}
			didsave[Ndid].docid1 = tablehead;
			didsave[Ndid].docid2 = tablehead;
			didsave[Ndid].outdegree = odg;
			for ( j = 0 ; j < nv ; j ++ ){
				didsave[Ndid].docid1 = tablehead;
				didsave[Ndid].docid2 = vect[j];
				didsave[Ndid].outdegree =  odg;
				Ndid ++ ;
			}


			nv = 0;
			tablehead = docid1;
			vect[nv++] = docid2;

		}
		
		odg = Find_Outdegree(tablehead , n);	//��û�ҵ����ȣ��򷵻� -1
		
		if (odg == -1)
		{
			nv = 0 ;
		//	fclose(fptable[i]);
		//	fclose(fpblock[i]);
		//	continue;
		}
		
	//	fprintf ( fptable[i] , "%I64u %I64u %d " , tablehead , odg , nv );
	//	for ( j = 0 ; j < nv ; j ++ ){
	//		fprintf( fptable[i] , "%I64u " , vect[j] );
	//	}
		for ( j = 0 ; j < nv ; j ++ ){
				didsave[Ndid].docid1 = tablehead;
				didsave[Ndid].docid2 = vect[j];
				didsave[Ndid].outdegree =  odg;
				Ndid ++ ;
			}
		fclose(fptable[i]);  fclose(fpblock[i]); 
	} //for(i)
	sort(didsave,didsave + Ndid, cmp);

	for ( j = 0 ; j < Ndid ; j ++ ){
		fprintf( fptable[i] , "%I64u %I64u %I64u " , didsave[j].docid1, didsave[j].docid2, didsave[j].outdegree );
	}
	
}



/* ������ */
int main( void ){		 

	_setmaxstdio(2048);					//�ı�stdio����ͬʱ���ļ���������
	
	Distribution_Head (NumBlock);		//�������ݷ����ĸ������ȳ�����4�������е����޸�

	Distribution_Tail (NumBlock);		//�Ի�β���зֿ�

	Tail_Statistic (NumBlock);			//��ÿ����β�ֿ��ļ�����ͳ�ƴ����õ�ÿ������ܳ����ڸ����ļ���

	GetLinkFile (NumBlock);				//�õ��ڽӱ��ļ�


	system("pause");
	return 0;	
}

















