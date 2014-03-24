



int main( void ){											//============================主函数
	int i,j,t;
	int listnum;
//	ChainingHash<UINT64>	Hashid(1024*1024);	 

	_setmaxstdio(2048);					//改变stdio层上同时打开文件的最大个数
	
	Distribution_Head (NumBlock);		//参数传递分区的个数，先初步分4个区进行调试修改

	Distribution_Tail (NumBlock);		//对弧尾进行分块

	Tail_Statistic (NumBlock);			//对每个弧尾分块文件进行统计处理，得到每个点的总出度于各个文件中

	GetLinkFile (NumBlock);				//得到邻接表文件



/*	FILE *fp_link;
	if( (fp_link = fopen("Link.txt","ab+") ) == NULL) {  //ab+  : 不存在该文件时则自动创建一个
		printf("cann't open file Link.txt");
		return 0;
	}

	//文件分区

	
	listnum=0; //initial	
	*/
 

 
	//printf("%d\n",Hashid.GetN());			//72997个结点 

	//for ( i = 0; i < 4; i ++ ){
	//	fclose( fpblock[i] );
	//}


//	sort(Stable,Stable+Ns);					//sort Stable







//	Deal_Block_File(fpblock);				//得到每个区域图的邻接表关系

/*	Pr_Compute() ;

	FILE *pr = fopen("PageRank.txt","r");

	for(i = 0 ; i < Ns; i++ ){
		fprintf(pr, "%I64u   %.6lf\n ", Stable[i].Did , Pr[0][i]);
	}
*/


	system("pause");
	return 0;
}




