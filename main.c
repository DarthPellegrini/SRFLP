#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Functions.h"
#include "Heuristics.h"
#include "ISgenerators.h"


/*
    This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
    To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/
    or send a letter to Creative Commons,PO Box 1866, Mountain View, CA 94042, USA.

    --- IMPORTANT ---
    DATA FILE (data.dat) to be read MUST be in the format
    *file begins*
    i
    l
    v
    ...
    *file ends*
    Where i is the instance path, l is the output log path and v is the result to be compared (best from literature).
    This format can be repeated in the file for as many instances as desired.
    ex.:
    *file begins*
    data/QAP_sko42_01_n
	outputNew/QAP_sko42_01_n
	25525
	data/QAP_sko42_02_n
	outputNew/QAP_sko42_02_n
	216120.5
	data/QAP_sko42_03_n
	outputNew/QAP_sko42_03_n
	173267.5
	data/QAP_sko42_04_n
	outputNew/QAP_sko42_04_n
	137615
	data/QAP_sko42_05_n
	outputNew/QAP_sko42_05_n
	248238.5
    *file ends*
    
    INSTANCE FILE to be read MUST be in the format
    *file begins*
    n
    A
    B
    *file ends*
    Where n is the instance's size, A is the distance between each facility and B is the flow Matrix.
    ex.:
    *file begins*
    5
    3 4 5 1 2
    0 2 1 0 1
    2 0 0 2 2
    1 0 0 6 3
    0 2 6 0 4
    1 2 3 4 0
    *file ends*
*/

/*
*	MAIN FUNCTION
*/

void main(int argc, char *argv[]){
    /*
     *  VARIABLES
     * 	UPDATE THIS LATER
     *  arq: file pointer
     *  fdata: data read from the file
     *	idata: instance data 
     *	ldata: log data
     *  last: last solution found
     *  size: size of the facility (+1 for easy calculations)
     *	solsize: quantity of solutions (must be quantity+1, position 0 is used to store the best solution)
     *  i & j: loop controlling
     *  value: value of each facility and half of his value
     *	pos[0][1 to size]: current best solution
     *	pos[1 to solsize][1 to size]: solution generated
     *  Flow[][] configurations is as follows:
     *  Flow[1 to size][1 to size]: flow between facilities
     *  Flow[i to size][0]: if the facility was already used in a permutation(0=no,1=yes)
     *  Flow[0][0]: counter of facilities for group assembly (index)
     */
    FILE *arq,*log,*data,*lit;
    char fdata[256],idata[256],ldata[256];
    data = fopen("data.dat","r");
    if(data == NULL){
	    puts("Erro - arquivo de dados nao existe");
	    exit(1);
	}
    while (fgets(idata, sizeof(idata),data) != NULL){
    	// ### READING DATA ###
		int size,solsize=21,i=1,j=1,temp;
		char *p;
    	idata[strlen(idata)-1] = '\0';
    	arq = fopen(idata,"rb");
		if(arq == NULL){
		    puts("Erro - arquivo de instancia nao existe");
		    exit(1);
		}
		printf("\nInstance: %s\n",idata);
		fgets(ldata, sizeof(ldata),data);
		ldata[strlen(ldata)-1] = '\0';
		log = fopen(ldata,"wb+");
		//saves the size of the Facility
		fgets(fdata,1024,arq);
		size = (1+atoi(fdata));
		//flow matrix and positioning declarations
		int flow[size][size],pos[solsize][size],aux[size];
		float value[size][2],last;
		double res;
		//saving positioning
		fgets(fdata,1024,arq);
		p = fdata;
		//saving literature result
		fgets(idata, sizeof(idata),data);
		idata[strlen(idata)-1] = '\0';
		res = strtod(idata,NULL);
		//printf("Literature result = %.1f\n",res);
		//converting from char array to int
		while(i < size){
		    pos[i][0] = 0;
		    flow[i][0] = 0;
			flow[0][i] = -1;
		    value[i][0] = strtol(p,&p,10);
		    value[i][1] = value[i][0]/2;
		    if((*p) == ' ')
		        p++;
		    i++;
		}
		//saving the flow
		flow[0][0]=1;
		for(i=1; i<size; i++){
		    fgets(fdata,sizeof(fdata),arq);
		    p = fdata;
		    for(j=1; j<size; j++){
		        flow[i][j] = strtol(p,&p,10);
		        if((*p) == ' ')
		        	p++;
		    }
		}
		fclose(arq);
		
		// ### HEURISTICS AND POSITIONING LOGIC ###
		/* 
		 *  NOTE 
		 *	to create multiple solutions to the other PathFinders, 
		 *	they need to be changed, or a overall logic must be created
		 * 	Not working in the current state:
		 * 	execution(size,solsize,pos,flow,value,findPath,heuristic2,0,res,ldata,log);
		 *	execution(size,solsize,pos,flow,value,findPathFlow,heuristic2,1,res,ldata,log);
		 *	execution(size,solsize,pos,flow,value,findPathRelation,heuristic2,2,res,ldata,log);
		 */
		execution(size,solsize,pos,flow,value,findPathRelationFlow,heuristic2,3,res,ldata,log);
		fclose(log);
    }
    fclose(data);
}
