//////////////////////////////////////////////////////////////////////////
/*	目前主要是hash表占用了内存；
	父结点和子节点的对应关系一边读入一边输出分布在不同的文件块中，不占用内存；

	每个节点的出度从hash表中可以得到;
	对于输入的父结点，在hash表中查找，若找到则其出度加1，若没有找到，则插入表中；
	对于其对应的子节点，在hash表中查找，若找到则返回，若没有找到则进行插入；
	-Kevinew@空中有约网络科技公司
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
char DBlockFile[4][16] = {"DBlock_0.txt" , "DBlock_1.txt" , "DBlock_2.txt" , "DBlock_3.txt"};  //存邻接表


bool cmp(dist a,dist b){
	if (a.docid2 < b.docid2) return true;	
	return false;
}


/* 按照汇点分块 存链接关系 */
void Distribution_Head(int n){					//对URL的链接关系进行分区到n个不同的文件中--Block_i.txt
	int i,len;
	char strget[2048], fname[20], iname[20] = {"Block_"};
	UINT64 docid1,docid2,siteid1,siteid2;		//docid1 -> docid2 (link point to)	
	FILE *fpblock[1024];

	for ( i = 0; i < n; i ++ ){		//一次打开n个块文件,准备后面的写操作
		strcpy( fname , iname );
		sprintf(fname + 6 , "%d" , i ) ;
		strcat( fname, ".txt" );
		fpblock[i] = fopen( fname , "wb" );
	}
  
	FILE *fp;
	if( (fp = fopen("UrlRelationship.txt","rb+") ) == NULL) { //打开原URL链接关系文件
		printf("cann't open file UrlRelationship.txt");
		return ;  
	}

	while(fread(strget,sizeof(char),4,fp)){	//一边从原始文件进行读入，一边进行文件的分区，按docid的链接关系进行保存。
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
		
		if ( len < 0 || len > 100 ) {		//对原始文件剔除异常数据
			continue;
		}

		fprintf( fpblock[docid2 % n], "%I64u %I64u %I64u %I64u ", docid1, docid2, siteid1, siteid2 );//按照模余存入不同的块文件中；	
	}

	fclose(fp);	

	for ( i = 0; i < n; i ++ ){		//关闭n个块区域的文件指针
		fclose( fpblock[i] ); fpblock[i] = NULL;
	}
}


/* 对已得的汇点分块文件 分别进行按源点分类 */
void Distribution_Tail(int n){				//按弧尾进行分类  ----Tail_i.txt
	int i,j;
	char fname[20], iname[20] = {"Block_"}, tail[20] = {"Tail_"};
	UINT64 docid1,docid2,siteid1,siteid2;	//docid1 -> docid2 (link point to)	
	FILE *fpblock[1024], *fptail[1024]; 

	for ( j = 0 ; j < n; j++ ){				//打开n个弧尾块文件
		strcpy( fname , tail );
		sprintf( fname + 5 , "%d" , j ) ;
		strcat( fname, ".txt" );
		fptail[j] = fopen(fname , "wb") ;	//待写入存储的tail文件；
	}
	for ( i = 0; i < n; i ++ ){				//一次打开n个块文件,准备后面的读操作
		ChainingHash<UINT64>	Ha(1024*1024);

		strcpy( fname , iname );
		sprintf( fname + 6 , "%d" , i ) ;
		strcat( fname, ".txt" );
		fpblock[i] = fopen( fname , "rb" ); //打开弧头文件fpblock[i]

		while (fscanf(fpblock[i], "%I64u %I64u %I64u %I64u ", &docid1, &docid2, &siteid1, &siteid2 ) != -1){
			if (!Ha.search(docid1)) {		//处理弧尾，若找到则只需将outdegree加1；
				Ha.insert(docid1,siteid1);  //若没有找到，则需将其插入，并附带docid和siteid一同存入插入的节点中；
			}
			Ha.search_to(docid2,siteid2);	//处理弧头，若没找到则将其插入在表尾；
		}

		Ha.SavetoFile( fptail , n );		//传递弧尾文件指针，n个弧头分块文件,把整个表输出到不同的文件中

		fclose(fpblock[i]);					//关闭弧头文件fpblock[i]

		//当离开当此循环时，Ha离开它的作用域，自动调用了析构函数 （长期不用就搞忘了~ ）
	}

	for ( j = 0 ; j < n; j++ ){				//对弧尾文件j读取后处理，然后统一化整理后再写入该文件		
		fclose( fptail[j] );
	}

}


//* 对弧尾文件进行统计 得到每个docid的outdegree */
void Tail_Statistic(int n){					//综合计算outdegree, 还存于Tail_i.txt文件中 
	int j;
	UINT64 did,sid,od;
	char fname[20],tail[20] = {"Tail_"};
	FILE *fptail[1024];

	for ( j = 0 ; j < n; j++ ){				//打开n个弧尾块文件
		strcpy( fname , tail );
		sprintf( fname + 5 , "%d" , j ) ;				
		strcat( fname, ".txt" );
		fptail[j] = fopen(fname , "rb") ;	//对tail文件中的数据进行处理；
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


/* 寻找出度 */
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


/*对Block_i.txt进行邻接表存储，其中头节点存放该点的出度和该点在此文件中存在的邻接点个数*/
void GetLinkFile (int n){
	//由于原文件的特殊存储结构，相同的docid1在一起，所以此处利用队列计其个数后，插入出度和子节点个数保存于另一文件
	//从Block_i.txt文件中提取LTable_i.txt为邻接表存储文件
	int i,j;
	char fname[20], iname[20] = {"Block_"}, lname[20] = {"LTable_"};
	UINT64 odg,docid1,docid2,siteid1,siteid2,tablehead,nv;	//docid1 -> docid2 (link point to)	; odg-outdegree
	FILE *fpblock[1024] , *fptable[1024];

	for(i = 0 ; i < n ; i++ ){
		strcpy( fname , iname );
		sprintf( fname + 6 , "%d" , i ) ;
		strcat( fname, ".txt" );
		fpblock[i] = fopen( fname , "rb" );		//打开弧头文件fpblock[i]
		
		strcpy( fname , lname );
		sprintf( fname + 7 , "%d" , i ) ;
		strcat( fname, ".txt" );
		fptable[i] = fopen( fname , "wb" );		//打开邻接文件fptable[i]
		
		nv = 0 ; tablehead = 0 ;	//初值

		while (fscanf(fpblock[i], "%I64u %I64u %I64u %I64u ", &docid1, &docid2, &siteid1, &siteid2 ) != -1)   
		{//一个docid1对应着多个docid2
			if (docid1 == tablehead){
				vect[nv++] = docid2; //docid2入队列（充分利用了原文件的存储结构）      
				continue; 
			} 

			if ( tablehead== 0){
				tablehead = docid1	;		//改变tablehead的值
				vect[nv++] = docid2	;		//可以开始入队列了
				continue; 				
			}
			//输出vect于另一文件

			//odg 为 tablehead 的 outdegree    ????????
			odg = Find_Outdegree(tablehead , n);	//若没找到出度，则返回-1

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
		
		odg = Find_Outdegree(tablehead , n);	//若没找到出度，则返回 -1
		
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



/* 主函数 */
int main( void ){		 

	_setmaxstdio(2048);					//改变stdio层上同时打开文件的最大个数
	
	Distribution_Head (NumBlock);		//参数传递分区的个数，先初步分4个区进行调试修改

	Distribution_Tail (NumBlock);		//对弧尾进行分块

	Tail_Statistic (NumBlock);			//对每个弧尾分块文件进行统计处理，得到每个点的总出度于各个文件中

	GetLinkFile (NumBlock);				//得到邻接表文件


	system("pause");
	return 0;	
}

















