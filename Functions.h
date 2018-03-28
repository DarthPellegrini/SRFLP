#ifndef _Functions_h
#define _Functions_h

 /*
 * 		#### OBJECTIVE FUNCTION ####
 */

float objectiveFunction(int size,int sol[],int flow[][size],float value[][2]){
    //calculates the Objective Value of the informed Instance/Set and the found Permutation.
    int i,j;
    float result = 0;
    for(i=1; i<(size-1); i++){
        //member
        for(j=i+1; j<size; j++){
            //permutations
            int st = i,c = 0;
            while(st+1 < j){
                c+= value[sol[st+1]][0];
                st++;
            }
            result += (c+value[sol[i]][1]+value[sol[j]][1])*flow[sol[i]][sol[j]];
        }
    }
    return result;
} 

/*
*	#### RECURSIVE HEURISTIC FUNCTION CALLER ####
*/

float hRecursion(int size, int pos[], int flow[][size],float value[][2],float last,float (*heuristic)(int,int[],int[][size],float[][2],float)){
	float aux = heuristic(size,pos,flow,value,last);
	//printf("Current Solution = %.1f\n",aux);
	if(aux < last)
		return hRecursion(size,pos,flow,value,aux,heuristic);		
	else
		return aux;
}


/*
*	#### PERMUTATION FUNCTIONS ####
*/

void swap(int *x1,int *x2){
    int x=*x1;
    *x1=*x2;
    *x2=x;
}
void switcher(int *resp, int temp, int size, int flow[][size],int *arr,int st,int ls){
    int i=0,x;
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

/*
*	#### EXECUTION FUNCTION ####
*/
void execution(int size, int solsize, int pos[][size], int flow[][size], float value[][2], void (*pathfinder)(int,int,int[][size],int[][size]),float (*heuristic)(int,int[],int[][size],float[][2],float), int IS, double res, char ldata[], FILE *log){
	/*
	 *	VARIABLES:
	 *	begin & end: beginning and ending clock times for calculation, respectively
	 *	s[0 to 3]: the pathfinder's names
	 *	fdata: auxiliary variable for log savings
	 *	perc: used for calculations
	 *	IS: pathfinder name controller
	 *	res: literature solution
	 *	temp: auxiliary variable
	 *	last: last solution generated
	 *  best: best solution generated
	 */
	clock_t begin,end; char s[4][25], fdata[256];
	double perc,ttime; float last,best; int temp,i,msec,sec,min,hour;
	strcpy(s[0],"Default");
	strcpy(s[1],"MaxFlow");
	strcpy(s[2],"Relation");
	strcpy(s[3],"MaxFlow Relation");
	
	//## LOG OPENING ##
	log = fopen(ldata,"a");
	
	//## SHOWING THE INITIAL SOLUTION ##
	pathfinder(size,solsize,pos,flow);
	last = objectiveFunction(size,pos[0],flow,value);
	printf("\nSolution using %s PathFinder = %.1f\n",s[IS],last);
	fputs("Solution using ",log);
	fputs(s[IS] ,log); fputs(" PathFinder = ",log);
	snprintf(fdata, 50, "%.1f", last);
	fputs(fdata,log); fputs("\n",log);
	
	//## EXECUTING THE ALGORITHM ##
	begin = clock();
	pathfinder(size,solsize,pos,flow);
	best = last = objectiveFunction(size,pos[0],flow,value);
	for(temp = 1; temp < solsize; temp++){
		last = hRecursion(size,pos[temp],flow,value,last,heuristic);
		printf("Multiple Soltions in phase %d with the result %.1f\n", temp, last);
		if (last < best){
			for(i = 1; i < size; i++)
				pos[0][i] = pos[temp][i];
			best = last;
		}
	}
	end = clock();
	
	//## EXECUTION DATA ##
	ttime = (double)(end - begin)/1000000;
	hour = ttime/3600;
	min = abs((hour*60)-ttime/60);
	sec = abs(min*60-ttime);
	msec = (ttime-sec)*100000;
	printf("Total Execution Time: %.4fs\n", ttime);
	printf("Execution time %dh %dm %ds %dms\n",hour,min,sec,msec);
	fputs("Execution time: ",log);
	snprintf(fdata, 30, "%dh ",hour); fputs(fdata,log); 
	snprintf(fdata, 30, "%dm ",min); fputs(fdata,log);
	snprintf(fdata, 30, "%ds ",sec); fputs(fdata,log);
	snprintf(fdata, 30, "%dms ",msec); fputs(fdata,log);
	fputs("\n",log);
	printf("Solution using a Permutation Heuristic = %.1f\n",best);
	fputs("Solution using a permutation Heuristic = ",log);
	snprintf(fdata, 50, "%.1f", best);	fputs(fdata,log); 
	printf("Solution from the Literature = %.1f\n",res);
	fputs("\nSolution from the Literature = ",log);
	snprintf(fdata, 50, "%.1f", res);
	fputs(fdata,log); perc = ((best-res)*100/best);
	if(perc != 0){
		printf("Percentual left = %.4f%%\n",perc);
		fputs("\nPercentual left = ",log);
		snprintf(fdata, 50, "%.4f", perc);
		fputs(fdata,log); fputs("%",log);
	}else{
		printf("Literature solution reached!\n");
		fputs("\nLiterature solution reached!",log);
	}
	
	//## CURRENT PERMUTATION ##
	printf("Permutation: ");
	fputs("\nPermutation: ",log);
	for(temp = 1; temp < size; temp++){
	    printf("%d ",pos[0][temp]);
	    snprintf(fdata, 50, "%d", pos[0][temp]);
	    fputs(fdata,log); fputs(" ",log);
	}
	puts(""); fputs("\n\n",log);
	fclose(log);
}

#endif
