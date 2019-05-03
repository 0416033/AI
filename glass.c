#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
#define ATTR 9
#define CLASS 7
#define DATA "glass.data"
#define FOREST 10
#define INSTANCES 214
typedef struct
	{
		float feature[ATTR];
		int Class;
	}plant;
typedef struct Dec
    {
        int feature;
        float threashold;
        float ig;
        struct Dec* L;
        struct Dec* H;
        plant* d;
        int size;
        int Class;
    }decision;
void random_forest(decision** node,plant* set,int size);
void generate_tree(decision*node, plant* set,int size,int attr[], int attr_size);
void shuffle(plant* set, int size);
int main(int argc, char** argv)
{
	plant iris[INSTANCES];
	plant train[INSTANCES*2];
	plant test;
    FILE *data;
    data = fopen(DATA, "r");
    decision *root;
    decision *roots[FOREST];
    root = (decision *)malloc(sizeof(decision));
    for(int i=0;i<FOREST;i++)
    {
        roots[i] = (decision *)malloc(sizeof(decision));
    }
    decision *p;
	float accuracy=0.0;
	float T_accuracy=0.0;
	float F_accuracy=0.0;
	float FT_accuracy=0.0;
	int test_size = INSTANCES/5;
	int k,l;
	int id;
	int attr[ATTR];
	for(int i=0;i<ATTR;i++)
    {
        attr[i] = i;
    }
	for(int i=0;i<INSTANCES;i++)
    {
		char temp[50];
		fscanf(data,"%d,",&id);
        for(int j=0;j<ATTR;j++)
        {
            //fscanf(data,"%f,%f,%f,%f,",&iris[i].feature[0],&iris[i].feature[1],&iris[i].feature[2],&iris[i].feature[3]);
            fscanf(data,"%f,",&iris[i].feature[j]);
        }
		fscanf(data,"%d",&iris[i].Class);
	}
	shuffle(iris, INSTANCES);
	for(int i=0;i<INSTANCES;i++)
    {
        train[i] = iris[i];
        train[i+INSTANCES] = iris[i];
    }
    for(int i=0;i<5;i++)//k-fold
    {
        for(int j=0;j<INSTANCES;j++)
        {
            iris[j] = train[test_size*i+j];
        }
        //one tree
        generate_tree(root,iris,INSTANCES -test_size, attr, ATTR);
        for(int j=0 ;j<INSTANCES -test_size;j++)
        {
            p = root;
            test = iris[j];
            while(p->L != NULL)
            {
                if(test.feature[p->feature]<(p->threashold))
                {
                    p = p->L;
                }
                else
                {
                    p = p->H;
                }
            }
			if(test.Class == p->Class)
				T_accuracy+= 1;
        }
        for(int j=INSTANCES -test_size ;j<INSTANCES;j++)
        {
            p = root;
            test = iris[j];
            while(p->L != NULL)
            {
                if(test.feature[p->feature]<(p->threashold))
                {
                    p = p->L;
                }
                else
                {
                    p = p->H;
                }
            }
			if(test.Class == p->Class)
				accuracy+= 1;
        }

        //forest
        random_forest(roots, iris, (int)INSTANCES-test_size);
        for(int j=0 ;j<INSTANCES -test_size;j++)
        {
            int poll[CLASS] ={0};
            int result =0;
            int max = 0;
            for(int k=0;k<FOREST;k++)
            {
                p = roots[k];
                test = iris[j];
                while(p->L != NULL)
                {
                    if(test.feature[p->feature]<(p->threashold))
                    {
                        p = p->L;
                    }
                    else
                    {
                        p = p->H;
                    }
                }
                poll[p->Class-1]++;
            }
            for(int k=0;k<CLASS;k++)
            {
                if(poll[k] > max)
                {
                    max = poll[k];
                    result = k;
                }
            }
            if(test.Class == result+1)
                FT_accuracy+= 1;
        }
        for(int j=INSTANCES -test_size ;j<INSTANCES;j++)
        {
            int poll[CLASS] = {0};
            int result =0;
            int max = 0;
            for(int k=0;k<FOREST;k++)
            {
                p = roots[k];
                test = iris[j];
                while(p->L != NULL)
                {
                    if(test.feature[p->feature]<(p->threashold))
                    {
                        p = p->L;
                    }
                    else
                    {
                        p = p->H;
                    }
                }
                poll[p->Class-1]++;
            }
            for(int k=0;k<CLASS;k++)
            {
                if(poll[k] > max)
                {
                    max = poll[k];
                    result = k;
                }
            }
            if(test.Class == result+1)
                F_accuracy+= 1;
        }
    }
    printf("Data set: %s\n\n",DATA);
    printf("One full tree\n");
    printf("Training error rate: %f\n", 1.0 - T_accuracy/((INSTANCES-test_size)*5));
    printf("Testing error rate: %f\n\n", 1.0 - accuracy/(test_size*5));
    printf("Random forest\n");
    printf("Training error rate: %f\n", 1.0 - FT_accuracy/((INSTANCES-test_size)*5));
    printf("Testing error rate: %f\n\n", 1.0 - F_accuracy/(test_size*5));
    system("pause");
	return 0;
}
/*float entropy(int a,int b, int c)
{
    float p1,p2,p3;
    if(a == 0)
        p1 = 0;
    else
        p1 = log2f(a/(float)(a+b+c))*(-a/(float)(a+b+c));
    if(b == 0)
        p2 = 0;
    else
        p2 = log2f(b/(float)(a+b+c))*(-b/(float)(a+b+c));
    if(c == 0)
        p3 = 0;
    else
        p3 = log2f(c/(float)(a+b+c))*(-c/(float)(a+b+c));
    return (p1+p2+p3);
}*/
float Gindex(int a[], int n)
{
	float G = 1.0;
	for(int i=0;i<CLASS;i++)
	{
		G -= (a[i]/(float)n)*(a[i]/(float)n);
	}
    return G;
}
float InfoGain(plant* set,int size, int f, float threashold)
{
    float infogain;
	int O[CLASS] = {0};
    int partL[CLASS]={0};
    int partH[CLASS]={0};
	int L = 0;
	int H = 0;
    for(int i=0;i<size;i++)
    {
        if(set[i].feature[f]<threashold)
        {
            partL[set[i].Class - 1]++;
			O[set[i].Class - 1]++;
			L++;
        }
        else
        {
            partH[set[i].Class - 1]++;
			O[set[i].Class - 1]++;
			H++;
        }
    }
    float rem = 0;
    rem = ((L*Gindex(partL,L))+(H*Gindex(partH,H)))/size;
    infogain = Gindex(O,size) - rem;
    return infogain;
}
void shuffle(plant* set, int size)
{
    plant temp;
    int j;
    srand(time(NULL));
    for(int i=0;i<size;i++)
    {
        j = rand()%size;
        temp = set[i];
        set[i] = set[j];
        set[j] = temp;
    }
}
void sort(plant* set, int size, int f)
{
    plant temp;
    for(int i=size-1;i>0;i--)
    {
        for(int j=0;j<i;j++)
        {
            if(set[j].feature[f]>set[j+1].feature[f])
            {
                temp = set[j];
                set[j] = set[j+1];
                set[j+1] = temp;
            }
        }
    }
}
void random_forest(decision** node,plant* set,int size)
{
    int f[3] = {0};
    for(int i=0;i<FOREST;i++)
    {
        while(f[0] == f[1] || f[0] == f[2] || f[1] == f[2])
        {
            f[0] = rand()%ATTR;
            f[1] = rand()%ATTR;
            f[2] = rand()%ATTR;
        }
        shuffle(set, size);
        generate_tree(node[i], set, size/5, f, 3);
    }
}
void generate_tree(decision* node,plant* set,int size , int attr[], int attr_size)
{
    node->d = set;
    node->size = size;
    float max_ig =0.0;
    int max_feature=0;
    float max_treashold = 0.0;
    int max_size =0;
    float threashold = 0.0;
    float ig = 0.0;
    if(size == 0)
    {
        return;
    }
    else
    {
        for(int k=0;k<size;k++)
        {
            node->Class = set[0].Class;
        }
    }
    for(int i=0;i<attr_size;i++)
    {
        sort(set,size,attr[i]);
        for(int j=0;j<size-1;j++)
        {
            if(set[j].Class != set[j+1].Class)
            {
                threashold = (set[j].feature[attr[i]] + set[j+1].feature[attr[i]])/2;
                ig = InfoGain(set,size,attr[i],threashold);
                if(ig >max_ig)
                {
                    max_ig = ig;
                    max_feature = attr[i];
                    max_treashold = threashold;
                    max_size = j+1;
                }
            }
        }
    }
    if(max_ig == 0.0)
    {
        node->L = NULL;
        node->H = NULL;
        return;
    }
    else
    {
        node->ig = max_ig;
        node->feature = max_feature;
        node->threashold = max_treashold;
        sort(set,size,max_feature);
        node->L = (decision *)malloc(sizeof(decision));
        node->H = (decision *)malloc(sizeof(decision));
        generate_tree(node->L,set, max_size, attr, attr_size);
        generate_tree(node->H,(set+max_size),(size-max_size), attr, attr_size);
        return;
    }
}
