//ChainingHash.h

#pragma  once


typedef	unsigned __int64  UINT64 ;




//docid和siteid是一一对应的关系
class ChainingHash{

public:

	ChainingHash(int maxN) : M(maxN), N(0) ,C(0.85), ResCom(0){
		heads = new node*[M];
		
		for ( int i = 0; i < M; i++ )
			heads[i] = 0;
	}

	~ChainingHash(){ 
		node *p, *q;

		for (int i = 0; i < M; i++)	{
			p = heads[i];

			while( p ){
				q = p->next;
				delete p;
				p = q;
			}
		}
		delete[] heads;
	}

	//int count() const { return N; }



	
	void insert( const UINT64& v , const double& u)	{
		N++;  //共插入的元素个数

		int i = (int)(v%M);
		
		if (heads[i] == 0){
			heads[i] = new node(v,u);		//插入的是一个源点，其初始outdegree为1
			return;
		}
		
		node *p = heads[i], *q = p;
		while (p)
		{
			if (p->doc_id == v)
			{
				p->pr += u; 
				N--;		//直接累加上去的时候，N值还原保持不变	
				return;
			}
			q = p;
			p = p->next;
		}
		q->next = new node(v,u);

	}




	void ResidualCompute(const UINT64& v , const double& u){
		int i = (int)(v%M);
	
		node *p = heads[i], *q = p;
		while (p)
		{
			if (p->doc_id == v)
			{
				ResCom += pow(p->pr*C+(1-C)-u,2);	//差的平方和作为residual				
				return;
			}
			q = p;
			p = p->next;
		}

	}
	
	


	
	void output(FILE *fp){
		int i;
		node *p; 

		for ( i = 0; i < M; i++ ){ 
			p = heads[i];
			while (p){				//不同的docid将会保存在不同的弧尾分块文件中
				fprintf(fp,"%I64u %.6lf " , p->doc_id, p->pr*C+1-C ); 
				p = p->next;
			}
		}
	}



	
	/*
	bool search(const UINT64& v)	{
		int i = (int)(v%M);
		node *p = heads[i];

		while (p){
			if (p->doc_id == v)	{		//找到该docid	
				p->outdegree++;
				return true;
			}			
			p = p->next;
		}

		return false;
	}



	void search_to( const UINT64& v , const UINT64& u)	{	//这里查找指向的点，不存在则插入，不改变outdegree
		N++;  //共插入的元素个数

		int i = (int)(v%M);
		
		if (heads[i] == 0){
			heads[i] = new node(v,u);		//此时的初始outdegree为0
			return;
		}
		
		node *p = heads[i], *q = p;
		while (p){
			if (p->doc_id == v)	{	//找到该docid			
				N--;
				return ;
			}
			q = p;
			p = p->next;
		}
		q->next = new node(v,u);	//搜索到最后一个添加，加在表尾 
	}
	
	bool tail_search(const UINT64& v , const UINT64& u , const UINT64& odg)	{
		int i = (int)(v%M);


		if (heads[i] == 0){
			heads[i] = new node(v,u,odg);
			return false;
		}

		node *p = heads[i] ,*q;
		while (p){
			if (p->doc_id == v)	{		//找到该docid
				p->outdegree+=odg;
				return true;
			}	
			q = p;
			p = p->next;
		}
		q->next = new node(v,u,odg);

		return false;
	}

	void SavetoFile( FILE **fp , int nb)		//docid-siteid-outdegree
	{//输出hash表中每个节点的相关信息到文件中；
		int i;
		node *p; 

		for ( i = 0; i < M; i++ ){ 
			p = heads[i];
			while (p){				//不同的docid将会保存在不同的弧尾分块文件中
				fprintf(fp[p->doc_id%nb],"%I64u %I64u %I64u " , p->doc_id, p->site_id, p->outdegree ); 
				p = p->next;
			}
		}
	}

	void SaveAFile( FILE *fp)		//docid-siteid-outdegree
	{//输出hash表中每个节点的相关信息到文件中；
		int i;
		node *p; 

		for ( i = 0; i < M; i++ ){ 
			p = heads[i];
			while (p){				//不同的docid将会保存在不同的弧尾分块文件中
				fprintf(fp,"%I64u %I64u %I64u " , p->doc_id, p->site_id, p->outdegree ); 
				p = p->next;	
			}
		}
	}
	
	*/


	int  GetN(){
		return N;
	}

	int  GetM(){
		return M;
	}


//	double c=0.85;




	double ResCom;


private:
	
	struct node {
		UINT64  doc_id;
		double  pr;

		node *next;

		node() : doc_id(), next(0){}			//两个构造函数
		node(const UINT64& v,const double & p) : doc_id(v), pr(p),next(0){}
	};

	node ** heads;

	int N, M;
	double C;

};
















