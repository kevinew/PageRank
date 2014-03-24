//ChainingHash.h

#pragma  once


typedef	unsigned __int64  UINT64 ;


struct Node {
	Node *next;
	int suffix;	
};


struct SNode{
	UINT64 Did ,outdegree;
	int numout;
	Node *next;
	SNode() : next(0){}
	bool friend operator < ( SNode a, SNode b){
		if ( a.Did < b.Did ) return true;
		return false;
	}
}Stable[80000];

int Ns=0;



//docid��siteid��һһ��Ӧ�Ĺ�ϵ
template<typename T>
class ChainingHash{
public:

	ChainingHash(int maxN) : M(maxN), N(0){	
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

	void insert( const T& v , const T& u)	{
		N++;  //�������Ԫ�ظ���

		int i = (int)(v%M);
		
		if (heads[i] == 0){
			heads[i] = new node(v,u,1);		//�������һ��Դ�㣬���ʼoutdegreeΪ1
			return;
		}
		
		/*node *p = heads[i], *q = p;
		while (p)
		{
			q = p;
			p = p->next;
		}
		q->next = new node(v);*/

		node *q=new node(v,u,1);
		q->next=heads[i]->next;
		heads[i]->next=q->next;
	}

	bool search(const T& v)	{
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



	void search_to( const T& v , const T& u)	{	//�������ָ��ĵ㣬����������룬���ı�outdegree
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
	
	bool tail_search(const T& v , const T& u , const T& odg)	{
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


	int  GetN(){
		return N;
	}

	int  GetM(){
		return M;
	}

private:
	
	struct node {
		T doc_id,site_id,outdegree;	
		node *next;

		node() : doc_id(), next(0){}			//�������캯��
		node(const T& v,const T& u) : doc_id(v), site_id(u), outdegree(0), next(0){}
		node(const T& v,const T& u, const T& od) : doc_id(v), site_id(u), outdegree(od), next(0){}
	};

	node ** heads;

	int M, N; //M - Length of hash; N - Number of node.

};








