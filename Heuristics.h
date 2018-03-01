#ifndef _Heuristics_h
#define _Heuristics_h
#include "Functions.h"
 
 /*
 * 		#### HEURISTICS ####
 */

void heuristic1(int size, int pos[][size], int flow[][size], int gsize){
    int aux,temp,i,j;
    typedef struct facilities{
        int f;
        struct facilities *n;
        struct facilities *p;
    } fac;
    fac *pp = malloc(sizeof(fac)),*ps,*pe,*t;
    // ## GROUP PERMUTATIONS ##
    while(gsize<size-1){
        for(i = 1; i < size-1; i+=(2*gsize)){
            if(i+gsize<size){
                int c = 1;
                temp = 0;
                /*
                 * temp join configuration
                 * 0 = beginning of second to ending of first (does not need to change anything)
                 * 1 = ending of second to beginning of first (only exchange positioning)
                 * 2 = ending of second to ending of first (reverse the second)
                 * 3 = beginning of first to beginning of second (reverse the first)
                 */
                aux = -1;
                if(flow[(i+gsize-1)][i+gsize+(gsize)-1] > flow[i+gsize-1][i+gsize]){
                    //case 2
                    temp = 2;
                    aux = flow[(i+gsize-1)][i+gsize+(gsize)-1];
                }else{
                    //case 0
                    temp = 0;
                    aux = flow[i+gsize-1][i+gsize];
                }
                if(flow[i][(i+gsize)+(gsize)-1] > aux ||  flow[i][i+gsize] > aux)
                    if(flow[i][(i+gsize)+gsize-1] > flow[i][i+gsize])
                        //case 1
                        temp = 1;
                    else
                        //case 3
                        temp = 3;
                switch(temp){
                    case 1:
                        for(j = i; j < (i+gsize-1) && j+gsize < size; j++){
                            temp = pos[0][j];
                            pos[0][j] = pos[0][j+gsize];
                            pos[0][j+gsize] = temp;
                        }
                        break;
                    case 2:
                        if(i+2*gsize > size){
                        	pp = malloc(sizeof(fac));
                        	ps = pp; pe = pp;
                        	pp->n = (fac *) NULL;
                        	pp->p = (fac *) NULL;
                        	j = i+gsize;
                        	while(j < size){
                        		(*pe).f = pos[0][j++];
                        		pe->n = malloc(sizeof(fac));
                                pe = pe->n;
                            }
                            for(j = 1; j <= gsize ; j++)
                                pos[0][size-j] = pos[0][i+gsize-j];
                            j = 0;
                            while(ps != pe){
                            	pos[0][i+(j++)] = (*ps).f;
                                ps = ps->n;
                            }
                            gsize+=gsize;
                        }else{
                            for(j = (i+gsize); j < (i+gsize+gsize-c); j++){
                                temp = pos[0][j];
                                pos[0][j] = pos[0][i+gsize+gsize-c];
                                pos[0][i+gsize+gsize-c] = temp;
                                c++;
                            }
                        }
                        break;
                    case 3:
                        for(j = i; j < (i+gsize-c); j++){
                            temp = pos[0][j];
                            pos[0][j] = pos[0][i+gsize-c];
                            pos[0][i+gsize-c] = temp;
                            c++;
                        }
                        break;
                    default:
                        //do nothing
                        break;
                    }
                }
            /*
             * temp will receive the facility to be tested
             * aux will receive the position of the best permutation
             */
            int used[2*gsize],c=0;
            for(aux=0; aux<2*gsize; aux++)
                used[aux] = 0;
            int wasUsed(int used[], int n){
                int a;
                for(a = 0; a < 2*gsize ; a++)
                    if(used[a] == n)
                        return 1;
                return 0;
            }
            while(i+c < size && c < 2*gsize){
                //Makes permutations inside the new formed groups
                aux = i; //default
                for(j = i; j < size && j < i+2*gsize; j++){
                    if(flow[pos[0][i]][pos[0][j]] > flow[pos[0][i]][pos[0][aux]] 
                    	&& wasUsed(used,pos[0][j]) == 0){
                        //so the facilities already moved won't be used again
                        if(j >= i+2*gsize-1 || j == i)
                            aux = j;
                        else{
                            //if the facility is going to be inserted between two other facilities
                            //it's value will only be saved if it's flow is better than the current pair
                            if(flow[pos[0][i]][pos[0][j+1]] > flow[pos[0][i]][pos[0][aux]])
                                aux = j;
                        }
                    }
                }
                if(aux != i){
                    used[c] = pos[0][i];
                    temp = pos[0][j];
                    pos[0][j] = pos[0][j+1];
                    pos[0][j+1] = temp;
                }
                c++;
            }
        }
        gsize+=gsize;
    }
    free(pp);
}

float heuristic2(int size, int pos[][size], int flow[][size],float value[][2],float last){
    /*
     *  VARIABLES
     *  aux,i & j: auxiliary variables
     *  solution[1 to size]: possible permutation
     *	used[1 to size]: facilities already moved
     *  s: possible solution
     *  ls: last solution
     *  lsp: last solution using permutation
     *	lsi: last solution using insertion
     *  best: best solution found
     *  facilities: auxiliary struct used to easily do the permutation of facilities
     *  ps: pointer to the start of struct array of facilities
     *  pp,t & p: struct array pointers
     *  tpp: stores the old p position
     *	mode: Type of position exchanger
     *  pospp: possible position of 't' to create a better bound
     */
    int temp,aux,i,j,used[size],solution[size],mode;
    float ls,lsp,lsi;
    //creates a struct to hold the facilities
    typedef struct facilities{
        int f;
        struct facilities *n;
        struct facilities *p;
    } fac;
    //## ARRAY TRANSPOSITION ##
    //main array
    fac *ps=malloc(sizeof(fac)),*pp,*t,*tp,*oldt;
    (*ps).f = pos[0][1];
    t = ps;
    ps->p = (fac *) NULL;
    used[1]=0;
    solution[1]=pos[0][1];
    for(i = 2; i < size; i++){
    	used[i]=0;
        solution[i]=pos[0][i];
        pp = (fac *) malloc(sizeof(fac));
        (*pp).f = pos[0][i];
        t->n = pp;
        pp->p = t;
        pp->n = (fac *) NULL;
        pp = pp->n;
        t = t->n;
    }
    t = ps; oldt = t;
    ls = objectiveFunction(size,solution,flow,value);
    //printf("sol = %.1f\n",ls);
    //## PERMUTATION LOGIC ##
    while(t->n != (fac *) NULL){
        //moves the pointer to the next facility to be tested
        //change p to compare to beginning
        fac *p = ps,*tpp=p,*pospp=t;
        lsi = 0; lsp = 0;
        //test every change
        while(p != (fac *) NULL){
        	if(p != t && p != tpp){
	            //insertion tester
	            //step 0.0
	            tpp = p;
	            t = t->n;
	            pp = t->p;
	            //*if the position to be exchanged is ps, move ps as well
	            //step 1.1
	            if(pp == ps){
	                    ps = ps->n;
	                    ps->p = (fac *) NULL;
	            }else
	                pp->p->n = pp->n;
	            //step 1.2
	            pp->n->p = pp->p;
	            //step 2.1
	            if(p->n != (fac *) NULL){
	                p->n->p = pp;
	                pp->n = p->n;
	            }else
	                pp->n = (fac *) NULL;
	            //step 2.2
	            p->n = pp;
	            pp->p = p;
	            //saves the new permutation and verifies if it's better than the last solution
	            pp = ps;
	            i = 1;
	            while(pp != (fac *) NULL){
	                solution[i++] = (*pp).f;
	                pp = pp->n;
	            }
	            lsi = objectiveFunction(size,solution,flow,value);
	            if(lsi < ls && (lsp == 0 || lsi < lsp)){
	            	//printf("sol 2 = %.1f\n",lsi);
	            	//printf("best values = (%d,%d)\n",(*t).f,(*p).f);
	            	ls = lsi;
	            	mode = 1;
	                pospp = tpp;
	            }
	            //undoing changes
	            //step 2.2
	            p = tpp;
	            pp = p->n;
	            //step 2.1
	            if(pp->n != (fac *) NULL){
	                pp->n->p = p;
	                p->n = pp->n;
	            }else
	                 p->n = (fac *) NULL;
	            //step 1.2
	            pp->n = t;
	            //step 1.1
	            if(t == ps){
	            	t->p = pp;
	                ps = pp;
	                pp->p = (fac *) NULL;
	            }else{
	                t->p->n = pp;
	                pp->p = t->p;
	            }
	            //step 0.0
				t->p = pp;
	            t = t->p;
	            p = tpp;
	            
	            //position exchanger
	            aux = (*t).f;
	            (*t).f = (*p).f;
	            (*p).f = aux;
	            
	            pp = ps;
	            i = 1;
	            while(pp != (fac *) NULL){
	                solution[i++] = (*pp).f;
	                pp = pp->n;
	            }
	            lsp = objectiveFunction(size,solution,flow,value);
	            if(lsp < ls && (lsi == 0 || lsp < lsi)){
	            	//printf("sol 1 mid = %.1f  \n",lsp);	            
	            	//printf("best values = (%d,%d)\n",(*t).f,(*p).f);
	            	ls = lsp;
	            	mode = 0;
	                pospp = tpp;
	            }
		        (*p).f = (*t).f;
		        (*t).f = aux;
	            
	        }
            p = p->n;
        }
        //if a better solution is found
        if(pospp != t){
        	/*
			 *	VALUE CHANGER
			 *	If a lower higher bound is needed, the following command can be excluded
			 *	In that way, the algorithm won't be stuck with a local minimun
			 *	But, in the other hand, some performance issues are to be expected
			 *	Notation:
			 *	LIMITED: Not allowing the algorithm to move facilities that were already moved
			 *	UNLIMITED:	Allowing the algorithm to move all facilities
			 */
			p = pospp;
			pp = t;
			if(mode == 0){
        		//position exchanger module
        		//printf("\n[%d,%d] best = %.1f\n",(*pp).f,(*pospp).f,ls);
        		aux = (*p).f;
        		(*p).f = (*pp).f;
        		(*pp).f = aux;
        		pp = ps;
	            i = 1;
	            while(pp != (fac *) NULL){
	                solution[i++] = (*pp).f;
	                pp = pp->n;
	            }
	            //printf("sol 1 = %.1f\n\n",objectiveFunction(size,solution,flow,value));
        	}else{
		    	//position insertion module
		    	//printf("\n{%d,%d} best = %.1f\n",(*pospp).f,(*pp).f,ls);
			    //if the position to be exchanged is ps, move ps as well
			    //step 1.1
			    if(pp == ps){
			    	ps = ps->n;
			    	ps->p = (fac *) NULL;
			    }else
			        pp->p->n = pp->n;
			    //step 1.2
			    pp->n->p = pp->p;
			    //step 2.1
			    if(p->n != (fac *) NULL){
			        p->n->p = pp;
			        pp->n = p->n;
			    }else
			        pp->n = (fac *) NULL;
			    //step 2.2
			    p->n = pp;
			    pp->p = p;
			    pp = ps;
	            i = 1;
	            while(pp != (fac *) NULL){
	                solution[i++] = (*pp).f;
	                pp = pp->n;
	            }
	            //printf("sol 2 = %.1f\n\n",objectiveFunction(size,solution,flow,value));
			}
			t = ps;
        }else
        	t = t->n;
    }
    //## SAVING THE BEST SOLUTION FOUND ##
    if(ls < last){
    	pp = ps;
        i=1;
        while(pp->n != (fac *) NULL){
            pos[0][i++]=(*pp).f;
            pp = pp->n;
        }
        pos[0][i]=(*pp).f;
    }
    //printf("sol end = %.1f\n",objectiveFunction(size,pos,flow,value));
    free(ps); free(pp);
    return ls;
}

float heuristic3(int size, int pos[][size], int flow[][size],float value[][2],float last){
    /*
     *  VARIABLES
     *  aux,i & j: auxiliary variables
     *  ps & las: possible and last solution respectively
     */
    int temp,aux,i,j;
    float ps = last,las;
    //## PERMUTATION STATION ##
    i = 1;
    while(i < size){
    	aux = i;
    	for(j = 1; j < size; j++)
    		if(j != i){
    			temp = pos[0][i];
    			pos[0][i] = pos[0][j];
    			pos[0][j] = temp;
    			las = objectiveFunction(size,pos[0],flow,value);
    			if(las < ps){
					aux = j;
					ps = las;
				}
				pos[0][j] = pos[0][i];
				pos[0][i] = temp;
    		}
    	if(aux != i){
    		temp = pos[0][i];
    		pos[0][i] = pos[0][aux];
    		pos[0][aux] = temp;
    		ps = objectiveFunction(size,pos[0],flow,value);
    		printf("[%d:%d] best = %.1f\n",i,aux,ps);
    		i = 1;
    	}else
			i++;    	
    }
    puts("done");
    return ps;
}
 
#endif
