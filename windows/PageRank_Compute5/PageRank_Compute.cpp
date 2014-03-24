//////////////////////////////////////////////////////////////////////////
/*
����������PageRankֵ�ļ���,��Ҫ���ļ�����������
Ŀǰ���е��ļ�����Ϊ��
Block_i.txt	:��ԭʼ�������ļ�UrlRelationship.txt�����˷��ࣨ����ͷ�ֵģ�	
Tail_i.txt	:����β���з��ಢͳ�Ƴ���
LTable_i.txt:����docid,outdegree,outnum...���ڽӱ��ļ�
����PageRank��Ҫ������LTable_i.txt�ļ�
Prֵ����double�ʹ洢��
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


//�洢ID��PRֵ��Ӧ��ϵ
struct DocidPageRank {
	UINT64	id;
	double pr;
	DocidPageRank():pr(0){}
	bool friend	operator <(DocidPageRank x, DocidPageRank y){	//��������
		if (x.id < y.id) return true;
		return false;
	}
}DPR[1024*1024];
int Ndpr;




void PageRank_Initial(int n){					//Pr�ļ��ĳ�ʼ����
	//��Tail_i.txt�ļ������㣬���Ѵ洢��docid������outdegree
	//Ŀǰ��Ҫ����ȡ������docid��������Pr��ֵΪ1������Pr_0_i.txt
	int i;
	char fname[256];
	UINT64 did,sid,od;
	FILE *ftail,*fpr;
	
	FORI(i,0,n){							//Ҳ��4�����ļ�
		sprintf(fname,"../Data/Tail/Tail_%d.txt",i);
		ftail= fopen(fname,"rb");
		sprintf(fname,"../Data/Pr/Pr_0_%d.txt",i);		//Ҳ��4�����ļ�
		fpr= fopen(fname,"wb");
		
		while (fscanf(ftail,"%I64u %I64u %I64u",&did,&sid,&od) != -1) {//�߶���д
			fprintf(fpr,"%I64u 1 ",did);
		}
		
		fclose(ftail);						//�ļ��򿪺�һ��Ҫע��ر�
		fclose(fpr);
	}	
}




void PageRank_Compute(int n) {
	int i,j,k,nf,outnum,numPoint;
	UINT64	did1,did2,did;			//docid of URL
	double	Outdgeree,data_pr;		//���ڼ���ĳ���
	double residual;
	double avg_pr;
	char  fname[30],LTable[30]={"LTable_"};
	bool find;
	FILE *pr[2][1024],*lt[1024];		//�����ļ�ָ�����飬����Pr�ļ���
	
	FILE *fpIterationTime;				//���ļ����������������
	int NIteration = 0;
	
	fpIterationTime = fopen("../Data/IterationTimes.txt","w");
	
	//�ļ���ʼ��
	
	k=0;
	do {	//from k read, and from 1-k write
		numPoint=0;
		//��PRֵ�洢�ļ�
		FORI(i,0,n){
			//	sprintf( fname , "Pr_%d_%d.txt" ,k,i);	//��k���ļ�
			//	pr[k][i]= fopen(fname , "rb");			//�������ļ�			
			sprintf( fname , "../Data/Pr/Pr_%d_%d.txt" ,1-k,i);//д��1-k���ļ�
			pr[1-k][i]= fopen(fname , "wb");		//�Զ��������ļ�
		}
		
		//�˴�Ӧ����ÿ��residual�����ĳ�ʼ��
		residual = 0.0;
		
		//��n���ڽӱ����ɨ�裬����ڽӵ��Prֵ
		FORI(i,0,n){
			sprintf( fname , "../Data/LTable/%s%d.txt" ,LTable,i);		//���ڽӱ��ļ�
			lt[i] = fopen(fname , "rb");
			ChainingHash Ht(1024*1024);
			
			//====�˴������˴���  ����������
			while( fscanf(lt[i],"%I64u %lf %d",&did1,&Outdgeree,&outnum) != -1 ){//�����ڽӱ�
				
				//���ҳ�did1��Prֵ  data_pr
				find = false;
				nf = did1%n;
				sprintf( fname , "../Data/Pr/Pr_%d_%d.txt" ,k,nf);	//��k���ļ�
				pr[k][nf] = fopen(fname , "rb");			//�������ļ�
				while ( fscanf(pr[k][nf],"%I64u %lf",&did,&data_pr) != -1 ) {
					if ( did1 == did ) {				//��k���ļ��в���did1��ԭʼprֵ
						find = true;
						break;
					}
				}
				fclose(pr[k][nf]);
				
				
				avg_pr= data_pr/Outdgeree;//did1��prֵ������ƽ���ַ�
				while (outnum--) {	//��did1��prֵ��outdegree��֮һ���ۼӵ�did2��
					fscanf(lt[i],"%I64u",&did2);					
					if ( avg_pr>0 && Outdgeree>0 )   Ht.insert(did2,avg_pr);
					//fprintf(pr[1-k][did2%4],"%I64u %lf ",did2,data_pr/Outdgeree);//���з�������ͬ���ļ�
				}
				//printf( "%d\n", Ht.GetN() );	//hash���е������۲�				
			}
			fclose(lt[i]);		//�رյ�i���ڽӱ��ļ�
			
			//did2��k��i�е�prֵ������1-k��i�е�prֵ�ı仯����������			
			
			sprintf( fname , "../Data/Pr/Pr_%d_%d.txt" ,k,i );
			pr[k][i] = fopen(fname , "rb");
			while (fscanf(pr[k][i],"%I64u %lf",&did,&data_pr) != -1) {	//��k�ļ���1-k�ļ��Ƚ�
				Ht.ResidualCompute(did,data_pr);				
			}
			
			residual += Ht.ResCom;
			
			////////////////////////////////////////////////////////////////////////// 
			printf("Residual = %.6lf\n",residual);
			fclose(pr[k][i]);	
			
			Ht.output(pr[1-k][i]);		//��id��prֵ��������ļ�pr[1-k][i],����������pr[k][i]������
			printf( "��%d��\n", i );
			numPoint+=Ht.GetN();
		}//��n���ڽӱ�ɨ�����
		
		
		
		//�ر�����pr�ļ�
		FORI(j,0,n)    fclose(pr[1 - k][ j ]); 
				
		k = 1 - k;
				
		printf("�������� -�� %.6lf\n",residual);
		fprintf(fpIterationTime,"%d\n",++NIteration);		//��������������ļ���
		printf("��������Ϊ ��%d\n", NIteration);
		printf("�ڵ���numPoint ��%d\n", numPoint);
		
	} while ( residual > 0.0001 );	//������ĳ��ʱ����ѭ��,������
	fclose(fpIterationTime);
	
}



//0.032906









int main( void ){						//������   
	
	_setmaxstdio(2048);//��   
	
	clock_t start, finish;
	double  duration;
	start = clock();
	
	PageRank_Initial(NumBlock);			//Prֵ�洢�ļ��ĳ�ʼ��
	PageRank_Compute(NumBlock);			//Prֵ�ļ���
	
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf( "%f hours\n", duration/3600.0 );
	
	system("pause");
	
	return 0;
}






























