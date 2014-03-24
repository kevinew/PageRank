//ChainingHash.h

#pragma  once


typedef	unsigned __int64  UINT64 ;




//docid��siteid��һһ��Ӧ�Ĺ�ϵ
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
		N++;  //�������Ԫ�ظ���

		int i = (int)(v%M);
		
		if (heads[i] == 0){
			heads[i] = new node(v,u);		//�������һ��Դ�㣬���ʼoutdegreeΪ1
			return;
		}
		
		node *p = heads[i], *q = p;
		while (p)
		{
			if (p->doc_id == v)
			{
				p->pr += u; 
				N--;		//ֱ���ۼ���ȥ��ʱ��Nֵ��ԭ���ֲ���	
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
				ResCom += pow(p->pr*C+(1-C)-u,2);	//���ƽ������Ϊresidual				
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
			while (p){				//��ͬ��docid���ᱣ���ڲ�ͬ�Ļ�β�ֿ��ļ���
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
			if (p->doc_id == v)	{		//�ҵ���docid	
				p->outdegree++;
				return true;
			}			
			p = p->next;
		}

		return false;
	}



	void search_to( const UINT64& v , const UINT64& u)	{	//�������ָ��ĵ㣬����������룬���ı�outdegree
		N++;  //�������Ԫ�ظ���

		int i = (int)(v%M);
		
		if (heads[i] == 0){
			heads[i] = new node(v,u);		//��ʱ�ĳ�ʼoutdegreeΪ0
			return;
		}
		
		node *p = heads[i], *q = p;
		while (p){
			if (p->doc_id == v)	{	//�ҵ���docid			
				N--;
				return ;
			}
			q = p;
			p = p->next;
		}
		q->next = new node(v,u);	//���������һ����ӣ����ڱ�β 
	}
	
	bool tail_search(const UINT64& v , const UINT64& u , const UINT64& odg)	{
		int i = (int)(v%M);


		if (heads[i] == 0){
			heads[i] = new node(v,u,odg);
			return false;
		}

		node *p = heads[i] ,*q;
		while (p){
			if (p->doc_id == v)	{		//�ҵ���docid
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
	{//���hash����ÿ���ڵ�������Ϣ���ļ��У�
		int i;
		node *p; 

		for ( i = 0; i < M; i++ ){ 
			p = heads[i];
			while (p){				//��ͬ��docid���ᱣ���ڲ�ͬ�Ļ�β�ֿ��ļ���
				fprintf(fp[p->doc_id%nb],"%I64u %I64u %I64u " , p->doc_id, p->site_id, p->outdegree ); 
				p = p->next;
			}
		}
	}

	void SaveAFile( FILE *fp)		//docid-siteid-outdegree
	{//���hash����ÿ���ڵ�������Ϣ���ļ��У�
		int i;
		node *p; 

		for ( i = 0; i < M; i++ ){ 
			p = heads[i];
			while (p){				//��ͬ��docid���ᱣ���ڲ�ͬ�Ļ�β�ֿ��ļ���
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

		node() : doc_id(), next(0){}			//�������캯��
		node(const UINT64& v,const double & p) : doc_id(v), pr(p),next(0){}
	};

	node ** heads;

	int N, M;
	double C;

};
















