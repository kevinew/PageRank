



int main( void ){											//============================������
	int i,j,t;
	int listnum;
//	ChainingHash<UINT64>	Hashid(1024*1024);	 

	_setmaxstdio(2048);					//�ı�stdio����ͬʱ���ļ���������
	
	Distribution_Head (NumBlock);		//�������ݷ����ĸ������ȳ�����4�������е����޸�

	Distribution_Tail (NumBlock);		//�Ի�β���зֿ�

	Tail_Statistic (NumBlock);			//��ÿ����β�ֿ��ļ�����ͳ�ƴ����õ�ÿ������ܳ����ڸ����ļ���

	GetLinkFile (NumBlock);				//�õ��ڽӱ��ļ�



/*	FILE *fp_link;
	if( (fp_link = fopen("Link.txt","ab+") ) == NULL) {  //ab+  : �����ڸ��ļ�ʱ���Զ�����һ��
		printf("cann't open file Link.txt");
		return 0;
	}

	//�ļ�����

	
	listnum=0; //initial	
	*/
 

 
	//printf("%d\n",Hashid.GetN());			//72997����� 

	//for ( i = 0; i < 4; i ++ ){
	//	fclose( fpblock[i] );
	//}


//	sort(Stable,Stable+Ns);					//sort Stable







//	Deal_Block_File(fpblock);				//�õ�ÿ������ͼ���ڽӱ��ϵ

/*	Pr_Compute() ;

	FILE *pr = fopen("PageRank.txt","r");

	for(i = 0 ; i < Ns; i++ ){
		fprintf(pr, "%I64u   %.6lf\n ", Stable[i].Did , Pr[0][i]);
	}
*/


	system("pause");
	return 0;
}




