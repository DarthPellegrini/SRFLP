#ifndef _ISgenerators_h
#define _ISgenerators_h
#include <math.h>

 /*
 * 		#### INITIAL SOLUTION GENERATORS ####
 */

void findPath(int size, int pos[], int flow[][size], int gsize){
    /*
         *  VARIABLES
         *  gsize: Size of Groups
         *  aux: Max value found in the inferior triangle from the 'flow' matrix/auxiliary variable
         *  temp: temporary auxiliary variable
         *  i & j: Loop and position controllers
         *  *ps & *ps: Start and End of Pair pointers(respectively)
         *  *pp: Memory Space Pointer
         *  odd: If the number of facilities is odd, some alterations on the sub-routines will be executed
         */
    int aux,temp,i=1,j=1;
    int *ps,*pe,*pp=malloc(sizeof(int));
    // ## GROUP CREATION ##
    while(flow[0][0] < size){  //while there still are facilities to be utilized
        //## GROUPING PAIRS ##
        ps=pp;
        pe=ps+1;
        aux = -1; //max flow
        //if a group can be formed
        for(i=1; i<size; i++)
            if(flow[i][0] == 0)
                for(j=1; j<i; j++){
                    //if the highest valid flow is found
                    if(flow[j][0] == 0 && flow[i][j] >= aux){
                        *ps = i;
                        *pe = j; //the pair is saved
                        aux = flow[i][j];
                    }
                }
        //saving pair
        flow[*ps][0] = 1;
        flow[*pe][0] = 1;

        //## GROUPS ASSEMBLER ##
        //while there are enough facilities to create another group AND the size of the group don't exceed the defined size
        aux = 2;
        while((flow[0][0]+aux) < size && aux < gsize){
            int positioning = 0; //defines the positioning of the new facility (0=beginning/1=ending)
            temp = -1;
            for(i = 1; i < size; i++)
                if(*ps != i && flow[i][0] == 0 && flow[*ps][i] >= temp){
                    j = i;
                    temp = flow[*ps][i];
                    positioning = 0;
                }
            for(i = 1; i < size; i++)
                if(*pe != i && flow[i][0] == 0 && flow[*pe][i] >= temp){
                    j = i;
                    temp = flow[*pe][i];
                    positioning = 1;
                }
            if(positioning == 0){
                ps--;
                *ps=j;
            }else{
                pe++;
                *pe=j;
            }
            flow[j][0] = 1;
            aux++;
        }
        //## SAVING GROUP INTO 'POS' & NULLIFYING USED FACILITIES ##
        while(ps<=pe){
        	pos[flow[0][0]++] = *ps;
            ps++;
        }
        //## ODD FACILITY NUMBER CASE ##
        if(flow[0][0] == size-1){
            for(i = 1; i < size; i++)
                if(flow[i][0] == 0){
                    pos[size-1]= i;
                    flow[i][0] = 1;
                    flow[0][0]++;
                    break;
                }
        }
    }
    free(pp);
}

void findPathFlow(int size, int pos[], int flow[][size]){
    /*
     *  VARIABLES
     *  aux,t & temp: auxiliary variables
     *  i & j: Matrix position controllers
	 *	m & n: pos insertion controllers
	 *	flow[0][1 to size]: stores the values of the facilities
     */
	int i,j,aux=0,temp,t,m=1,n=size-1;
	//saving values
	for(i = 1; i < size; i++){
		flow[0][i]=0;
		for(j = 1; j < size; j++){
            flow[0][i]+=flow[j][i];
		}
	}

	//## POSITION SAVING ##
	for(i = 1; i < size; i++){
		//selection logic
		temp = 99999999;
		for(j = 1; j < size; j++){
			if(flow[0][j] != -1 && flow[0][j] < temp){
				temp = flow[0][j];
				t = j;
			}
		}
		//assembly logic
		if(temp < 99999999){
			flow[0][t]=-1;
			if(aux==0){
				aux = 1;
				pos[m++]=t;
			}else{
				aux = 0;
				pos[n--]=t;
			}
		}
	}

}

void findPathRelation(int size, int pos[], int flow[][size]){
	/*
     *  VARIABLES
     *  aux & temp | p1 & p2: auxiliary variables
	 *	arr: current array size
     *  i & j: Matrix position controllers
     *  ptm & pt: position and array pointers
	 *	pp & pn: auxiliary pointers
	 *	ps & pe: beginning and ending of the facilities array
     */
	int aux, temp, i, j, p1, p2, arr = 0;
	typedef struct facilities{
        int f;
        struct facilities *n;
        struct facilities *p;
    } fac;
    fac *ps,*pe = ps,*pp,*pn,*ptm,*pt;
    ps = malloc(sizeof(fac));
    pe = malloc(sizeof(fac));
    (*ps).f = -1;
    ps->p = (fac *) NULL;
    pe->p = ps;
    ps->n = pe;
    pe->n = (fac *) NULL;
	while(arr < size-1){
		// ## SEARCHING FOR SUITABLE PAIRS ##
		aux = -1;
		for(i = 1; i < size; i++){
			//only facilities that weren't used are able to form pairs
			if(flow[0][i] != 0 && i != (*ps).f && i != (*pe).f)
				for(j = 1; j < size; j++){
					//both facilities need to not have formed pairs
					if(flow[0][j] != 0 && flow[i][j] > aux && i != j){
						aux = flow[i][j];
						p1 = i;	p2 = j;
					}
				}
		} 
		/*
		 *	## INSERTION IN THE ARRAY ##
		 *
		 *	Rules:
		 *	1. If the second element of the pair was already selected,
		 *	the first will be inserted before or after it.
		 *	2. If the pair was not yet selected, it will be inserted
		 *	in the best position related to one of the ends.
		 */
		if((*ps).f == -1){
			(*ps).f = p1;
			(*pe).f = p2;
			arr+=2;
		}else
			if(p2 == (*ps).f){
				//puts("n1");
				flow[0][(*ps).f] = 0;
				ps->p = malloc(sizeof(fac));
				ps->p->n = ps;
				ps->p->p = (fac *) NULL;
				ps = ps->p;
				(*ps).f = p1;
				arr++;
			}else
				if(p2 == (*pe).f){
					flow[0][(*pe).f] = 0;
					pe->n = malloc(sizeof(fac));
					pe->n->p = pe;
					pe->n->n = (fac *) NULL;
					pe = pe->n;
					(*pe).f = p1;
					arr++;
				}else{
					// # pair initialization #
					pp = malloc(sizeof(fac));
					pn = malloc(sizeof(fac));
					(*pp).f = p1;
					(*pn).f = p2;
					pp->n = pn;
					pn->p = pp;
					pp->p = (fac *) NULL;
					pn->n = (fac *) NULL;
					ptm = malloc(sizeof(fac));
					ptm->p = (fac *) NULL;
					ptm->n = (fac *) NULL;
					// # pointer assembler #
					if(flow[(*pp).f][(*pe).f] > flow[(*pn).f][(*ps).f])
						ptm->p = pe;
					else
						ptm->n = ps;
					pt = ps;
					do{
						if(flow[(*pp).f][(*pt).f] > flow[(*(pt->n)).f][(*pt).f]
							|| flow[(*pn).f][(*(pt->n)).f] > flow[(*pt).f][(*(pt->n)).f]){
							ptm->n = pt->n;
							ptm->p = pt;
						}
					pt = pt->n;
					}while(pt != pe);
					// # insertion logic #
					pp->p = ptm->p;
					pn->n = ptm->n;
					if(pp->p != (fac *) NULL){
						pp->p->n = pp;
						flow[0][(*pp).f] = 0;
					}else{
						ps = pp;
						flow[0][(*(pn->n)).f] = 0;
					}

					if(pn->n != (fac *) NULL){
						pn->n->p = pn;
						flow[0][(*pn).f] = 0;
					}else{
						pe = pn;
						flow[0][(*(pp->p)).f] = 0;
					}
					arr+=2;
				}
	}
	// ## SAVING INTO POS ##
	for(i = 1; i < size; i++){
		flow[0][i]=-1;
		pos[i] = (*ps).f;
		ps = ps->n;
	}
}

void swap(int *x1,int *x2){
    int x=*x1;
    *x1=*x2;
    *x2=x;
}
void switcher(int *resp, int temp, int size, int flow[][size],int *arr,int st,int ls){
    int i=0;
    if(st==ls){
        int k,help=0;
        for(k=0;k<ls-1;k++)
            help += flow[arr[k]][arr[k+1]];
		if(help < temp){
			for(k=0;k<ls;k++)
				resp[k] = arr[k];
			temp = help;
		}
	}else{
        for(i=st;i<ls;i++){
            swap(arr+st,arr+i);
            switcher(resp,temp,size,flow,arr,st+1,ls);
            swap(arr+st,arr+i);
        }
    }
}

void findPathRelationFlow(int size, int pos[], int flow[][size]){
	/*
     *  VARIABLES
     *  aux & temp | p1 & p2: auxiliary variables
     *	gsize: Group size
     *	resp: Group's current facilities
     *  i & j: Matrix position controllers
     *	gs & ge: First and Last Group in group array, respectively
     *	gp & gaux & gtemp: Auxiliary Group Pointers
     */
    int i,j,p1,p2,aux,temp,gsize=8,resp[gsize+1];
    flow[0][0] = 0;
    for(i = 1; i < size; i++){
	   	flow[i][0] = 0;
	   	flow[0][i] = 0;
		for(j=1; j<size; j++)
			flow[0][i]+=flow[i][j];
	}
	typedef struct group{
		int value,size;
		int fac[gsize];
		struct group *p;
		struct group *n;
	} g; g *gs,*ge,*gp,*gaux,*gtemp,*gfe;
	gp = malloc(sizeof(g));
	(*gp).value = 0;
	(*gp).size = 0;
	gp->n = (g *) NULL;
	gp->p = (g *) NULL;
	gs = gp;	ge = gp;
	gtemp = gp;
	temp = 1;
	while(temp < size-1/2){
		//## PAIR UNIT LOGIC ## 
		p1 = 0; p2 = 0; aux = -1;
		for(i=1; i<size; i++)
			if(flow[i][0] == 0)
				for(j=1; j<size; j++)
					if(flow[j][0] == 0 && i != j && flow[i][j] > aux){
						p1 = i;	p2 = j;
						aux = flow[i][j];
					}
		//## GROUP INSERTION ##
		if(p1 != 0 && p1 < size){
			flow[p1][0]++;
			(*gp).fac[(*gp).size++] = p1;
			(*gp).value+= flow[0][p1];
			flow[0][0]++;
		}
		if(p2 != 0 && p2 < size){
			flow[p2][0]++;
			(*gp).fac[(*gp).size++] = p2;
			(*gp).value+= flow[0][p2];
			flow[0][0]++;
		}
		//## GROUP ASSEMBLER ##
		if((*gp).size >= gsize || ((*gp).size >= 2 && flow[0][0]>size-3)){
			if(gp != gs){
				if((*gp).value < (*gs).value){
					gp->n = gs;
					gs->p = gp;
					gs = gp;
				}else{
					g *p = gs;
					gaux = gs;
					while(gaux != ge){
						if((*gp).value > (*gaux).value)
							p = gaux;
						gaux = gaux->n;
					}
					if(p == ge)
						ge = gp;
					else{
						p->n->p = gp;
						gp->n = p->n;
					}
					gp->p = p;
					p->n = gp;
				}
				gtemp = gp;
			}
			gp = malloc(sizeof(g));
			(*gp).value = 0;
			(*gp).size = 0;
			gp->n = (g *) NULL;
			gp->p = (g *) NULL;
		}
		temp++;
	}
	if((size-1)%gsize!=0 && flow[0][0] < size-1){
		i = 1;
		while(flow[i][0] != 0 && i < size)
			i++;
		(*gtemp).fac[(*gtemp).size++]=i;
		(*gtemp).value+=flow[0][i];
		flow[i][0]++;
	}
	//## INDIVIDUAL GROUP PERMUTATION ##
	gaux = gs; aux = 0;
	while(gaux != (g *) NULL){
		resp[(*gaux).size]= 0;
		for(i = 0; i<(*gaux).size; i++)
			resp[i] = (*gaux).fac[i];
		for(i = 0; i<(*gaux).size-1; i++)
			temp += flow[(*gaux).fac[i]][(*gaux).fac[i+1]];
		switcher(resp,(*gaux).value,size,flow,(*gaux).fac,0,(*gaux).size);
		for(i = 0; i<(*gaux).size; i++)
			(*gaux).fac[i] = resp[i];
		gaux = gaux->n;
	}
	//## SAVING TO POS ##
	temp = 1; p1 = 1; p2 = size-1;
	while(gs != (g *) NULL){
		for(i=0;i<(*gs).size;i++)
			if(temp%2!=0)
				pos[p1++] = (*gs).fac[i];
			else
				pos[p2--] = (*gs).fac[i];
		gs = gs->n;
		temp++;
	}
}
 
#endif
