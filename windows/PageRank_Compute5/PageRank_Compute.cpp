//////////////////////////////////////////////////////////////////////////
/*
本程序用于PageRank值的计算,主要是文件的批量操作
目前已有的文件数据为：
Block_i.txt	:对原始的连接文件UrlRelationship.txt进行了分类（按狐头分的）	
Tail_i.txt	:按弧尾进行分类并统计出度
LTable_i.txt:存有docid,outdegree,outnum...的邻接表文件
计算PageRank主要利用了LTable_i.txt文件
Pr值均以double型存储。
*/
//////////////////////////////////////////////////////////////////////////


#include "stdio.h"
#include "string.h"
#include "math.h"
#include "ChainingHash.h"
#include "algorithm"
#include "time.h"
using namespace std;


#define  NumBlock	256
#define FORI(a,b,c)		for ( (a) = (b); (a) < (c) ; (a) ++ )

typedef unsigned __int64 UINT64 ;


//存储ID和PR值对应关系
struct DocidPageRank {
	UINT64	id;
	double pr;
	DocidPageRank():pr(0){}
	bool friend	operator <(DocidPageRank x, DocidPageRank y){	//用作排序
		if (x.id < y.id) return true;
		return false;
	}
}DPR[1024*1024];
int Ndpr;




void PageRank_Initial(int n){					//Pr文件的初始化；
	//由Tail_i.txt文件来计算，它已存储了docid分类后的outdegree
	//目前主要是提取分类后的docid，设置其Pr初值为1，存入Pr_0_i.txt
	int i;
	char fname[256];
	UINT64 did,sid,od;
	FILE *ftail,*fpr;
	
	FORI(i,0,n){							//也分4个块文件
		sprintf(fname,"../Data/Tail/Tail_%d.txt",i);
		ftail= fopen(fname,"rb");
		sprintf(fname,"../Data/Pr/Pr_0_%d.txt",i);		//也分4个块文件
		fpr= fopen(fname,"wb");
		
		while (fscanf(ftail,"%I64u %I64u %I64u",&did,&sid,&od) != -1) {//边读边写
			fprintf(fpr,"%I64u 1 ",did);
		}
		
		fclose(ftail);						//文件打开后一定要注意关闭
		fclose(fpr);
	}	
}




void PageRank_Compute(int n) {
	int i,j,k,nf,outnum,numPoint;
	UINT64	did1,did2,did;			//docid of URL
	double	Outdgeree,data_pr;		//用于计算的出度
	double residual;
	double avg_pr;
	char  fname[30],LTable[30]={"LTable_"};
	bool find;
	FILE *pr[2][1024],*lt[1024];		//滚动文件指针数组，用于Pr的计算
	
	FILE *fpIterationTime;				//该文件用于输出迭代次数
	int NIteration = 0;
	
	fpIterationTime = fopen("../Data/IterationTimes.txt","w");
	
	//文件初始化
	
	k=0;
	do {	//from k read, and from 1-k write
		numPoint=0;
		//打开PR值存储文件
		FORI(i,0,n){
			//	sprintf( fname , "Pr_%d_%d.txt" ,k,i);	//读k类文件
			//	pr[k][i]= fopen(fname , "rb");			//打开已有文件			
			sprintf( fname , "../Data/Pr/Pr_%d_%d.txt" ,1-k,i);//写入1-k类文件
			pr[1-k][i]= fopen(fname , "wb");		//自动创建新文件
		}
		
		//此处应该有每次residual迭代的初始化
		residual = 0.0;
		
		//对n块邻接表进行扫描，输出邻接点的Pr值
		FORI(i,0,n){
			sprintf( fname , "../Data/LTable/%s%d.txt" ,LTable,i);		//打开邻接表文件
			lt[i] = fopen(fname , "rb");
			ChainingHash Ht(1024*1024);
			
			//====此处出现了错误  ？？？？？
			while( fscanf(lt[i],"%I64u %lf %d",&did1,&Outdgeree,&outnum) != -1 ){//处理邻接表
				
				//先找出did1的Pr值  data_pr
				find = false;
				nf = did1%n;
				sprintf( fname , "../Data/Pr/Pr_%d_%d.txt" ,k,nf);	//读k类文件
				pr[k][nf] = fopen(fname , "rb");			//打开已有文件
				while ( fscanf(pr[k][nf],"%I64u %lf",&did,&data_pr) != -1 ) {
					if ( did1 == did ) {				//在k类文件中查找did1的原始pr值
						find = true;
						break;
					}
				}
				fclose(pr[k][nf]);
				
				
				avg_pr= data_pr/Outdgeree;//did1的pr值往下游平均分发
				while (outnum--) {	//将did1的pr值的outdegree分之一，累加到did2上
					fscanf(lt[i],"%I64u",&did2);					
					if ( avg_pr>0 && Outdgeree>0 )   Ht.insert(did2,avg_pr);
					//fprintf(pr[1-k][did2%4],"%I64u %lf ",did2,data_pr/Outdgeree);//进行分流到不同的文件
				}
				//printf( "%d\n", Ht.GetN() );	//hash表中的项数观察				
			}
			fclose(lt[i]);		//关闭第i个邻接表文件
			
			//did2在k，i中的pr值与其在1-k，i中的pr值的变化加在收敛上			
			
			sprintf( fname , "../Data/Pr/Pr_%d_%d.txt" ,k,i );
			pr[k][i] = fopen(fname , "rb");
			while (fscanf(pr[k][i],"%I64u %lf",&did,&data_pr) != -1) {	//将k文件与1-k文件比较
				Ht.ResidualCompute(did,data_pr);				
			}
			
			residual += Ht.ResCom;
			
			////////////////////////////////////////////////////////////////////////// 
			printf("Residual = %.6lf\n",residual);
			fclose(pr[k][i]);	
			
			Ht.output(pr[1-k][i]);		//将id和pr值导入分流文件pr[1-k][i],并计算其与pr[k][i]的收敛
			printf( "第%d块\n", i );
			numPoint+=Ht.GetN();
		}//对n块邻接表扫描完毕
		
		
		
		//关闭所有pr文件
		FORI(j,0,n)    fclose(pr[1 - k][ j ]); 
				
		k = 1 - k;
				
		printf("收敛因子 -〉 %.6lf\n",residual);
		fprintf(fpIterationTime,"%d\n",++NIteration);		//输出迭代次数于文件中
		printf("迭代次数为 ：%d\n", NIteration);
		printf("节点数numPoint ：%d\n", numPoint);
		
	} while ( residual > 0.0001 );	//收敛于某数时结束循环,判收敛
	fclose(fpIterationTime);
	
}



//0.032906









int main( void ){						//主函数   
	
	_setmaxstdio(2048);//？   
	
	clock_t start, finish;
	double  duration;
	start = clock();
	
	PageRank_Initial(NumBlock);			//Pr值存储文件的初始化
	PageRank_Compute(NumBlock);			//Pr值的计算
	
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf( "%f hours\n", duration/3600.0 );
	
	system("pause");
	
	return 0;
}






























