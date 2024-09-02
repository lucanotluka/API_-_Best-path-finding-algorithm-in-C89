 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAXCARS 512
#define AGGIUNGISTAZIONE "aggiungi-stazione"
#define DEMOLISCISTAZIONE "demolisci-stazione"
#define AGGIUNGIAUTO "aggiungi-auto"
#define ROTTAMAAUTO "rottama-auto"
#define PIANIFICAPERCORSO "pianifica-percorso"

//===================   STATIONS & CARS   =======================

typedef struct car_{
	int fuel;
	struct car_ *parent, *left, *right;
}car_t;


typedef struct station_{
	int distance;	//intero non negativo
	struct station_ *parent, *left, *right;

	struct car_ *carsLot;
	int nCars;	//max 512
	int bestCar;	//auto ad autonomia massima
}station_t;

typedef struct list_{
	struct list_ *next;
	int stazione;
	station_t *pStazione;

	int color;
	int dist;
	struct list_ *prec;
}list_t;

typedef struct que_{
	struct que_ *next;
	list_t *nodo; 
}que_t;

//=======================   FUNCTIONS   ========================

station_t* ricercaStazione(int, station_t*);
station_t* aggiungiStazione(int, station_t**);
void demolisciStazione(station_t*, station_t**);

station_t* successore(station_t*);
station_t* predecessore(station_t*);
station_t* minimo(station_t*);
station_t* massimo(station_t*);


car_t* ricercaAuto(int, car_t*);
void aggiungiAuto(int, station_t*);
car_t* rottamaAuto(car_t*, station_t*);

void eliminaTutto(car_t*);
car_t* successoreCar(car_t*);
car_t* minimoCar(car_t*);


list_t* inserisci(list_t*, int, station_t*);
void stampa(list_t*);
que_t* enqueue(que_t*, list_t*);
que_t* dequeue(que_t**);

void pianificaPercorso(station_t*, station_t*);
int pianificaRic(list_t*, station_t*, station_t*);
int pianificaIter(list_t*, station_t*, station_t*);


//=========================   MAIN    =========================

int main(){
    station_t *root, *stazione, *stazioneFinale;
	car_t *currentCar;
	char comando[20];
	
	root = stazione = stazioneFinale = NULL;
	currentCar = NULL;

	int currCar, howManyCars, kmFinalStation, kmStation;

	while(scanf(" %s %d", comando, &kmStation) != EOF){

		stazione = ricercaStazione(kmStation, root);	//ricerco sempre a priori

		if(strcmp(comando, AGGIUNGISTAZIONE) == 0){
			
			if(scanf(" %d", &howManyCars)){

				if(stazione == NULL && howManyCars <= MAXCARS)
				{
					stazione = aggiungiStazione(kmStation, &root);

					if(stazione != NULL)
					{
						while(howManyCars > 0)
						{
							if(scanf(" %d", &currCar)){
								aggiungiAuto(currCar, stazione);
								(stazione->nCars)++;
								howManyCars--;
							}
						}
						
						printf("aggiunta\n");
					}
					else
						printf("non aggiunta\n");
				}
				else
					printf("non aggiunta\n");
			}
		}
		
		else if(strcmp(comando, DEMOLISCISTAZIONE) == 0){
			
			if(stazione != NULL){

				demolisciStazione(stazione, &root);
				printf("demolita\n");
			}
			else
				printf("non demolita\n");
		}

		else if(strcmp(comando, AGGIUNGIAUTO) == 0){
			
			assert(scanf(" %d", &currCar));
			
			if(stazione != NULL && stazione->nCars < MAXCARS)
			{
				aggiungiAuto(currCar, stazione);
				(stazione->nCars)++;
				
				printf("aggiunta\n");
			}
			else
				printf("non aggiunta\n");
		}

		else if(strcmp(comando, ROTTAMAAUTO) == 0)
		{
			if(scanf(" %d", &currCar)){}

			if(stazione != NULL)
			{	
				currentCar = ricercaAuto(currCar, stazione->carsLot);

				if(currentCar != NULL){
					stazione->carsLot = rottamaAuto(currentCar, stazione);
					(stazione->nCars)--;
					printf("rottamata\n");
				}
				else
					printf("non rottamata\n");
			}
			else
				printf("non rottamata\n");
		}

		else if(strcmp(comando, PIANIFICAPERCORSO) == 0){
			
			if(scanf(" %d", &kmFinalStation)){
				stazioneFinale = ricercaStazione(kmFinalStation, root);

				pianificaPercorso(stazione, stazioneFinale);
			}
		}

	}

    return 0;
}


//=======================   FUNCTIONS   ========================

station_t* ricercaStazione(int km, station_t* tmp){//		DONE
	if(tmp == NULL || tmp->distance == km)	
		return tmp;
	if (km < tmp->distance)
		return ricercaStazione(km, tmp->left);
	else
		return ricercaStazione(km, tmp->right);
}

// 	=======================================		DONE

car_t* ricercaAuto(int km, car_t* tmp){	//ok
	if(tmp == NULL || tmp->fuel == km)
		return tmp;
	if (km < tmp->fuel)
		return ricercaAuto(km, tmp->left);
	else
		return ricercaAuto(km, tmp->right);
}

// 	=======================================		DONE

station_t* aggiungiStazione(int km, station_t** root){	//2 do AVL
	station_t *pre, *new, *curr;
	pre = new = NULL;
	curr = *root;	//mantengo il rif. a root per le eventuali rotazioni

	// cerco il precedente della foglia per inserire new station, come foglia
	while(curr != NULL){

		pre = curr;
		if(km < curr->distance)
			curr = curr->left;
		else
			curr = curr->right;
	}

	// se malloc good, allora inizializzo stazione vuota come foglia
	if((new = (station_t*)malloc(sizeof(station_t))) != NULL){
		new->parent = pre;
		new->left = new->right = NULL;
		new->distance = km;
		new->carsLot = NULL;
		new->nCars = new->bestCar = 0;
	}

	if(pre != NULL && new != NULL){	//se avevo root, sistemo il figlio a pre
		
		if(km < pre->distance/* && (pre->left) == NULL*/) //verifico sia effettivamente figlio left/right
			pre->left = new;
		else /*if(pre->right == NULL)*/
			pre->right = new;
	} 
	else	//allora aggiungo root
		*root = new;
	
	return new;
}

// 	=======================================		DONE

void aggiungiAuto(int autonomia, station_t* stazio){	//2 do AVL
	car_t *pre, *new, *curr;
	pre = new = NULL;
	curr = stazio->carsLot;

	while(curr != NULL){

		pre = curr;
		if(autonomia < curr->fuel)
			curr = curr->left;
		else
			curr = curr->right;
	}

	//alloco memoria
	if((new = (car_t*)malloc(sizeof(car_t))) != NULL){
		new->parent = pre;
		new->left = new->right = NULL;
		new->fuel = autonomia;
	}


	if(pre != NULL && new != NULL){	//ovvero ho gia' la root
		if(autonomia < pre->fuel)
			pre->left = new;
		else
			pre->right = new;
	}
	else if(pre == NULL){	//cioe' e' la root
			stazio->carsLot = new;
		}

	if(stazio->bestCar <= autonomia)	//nuova auto migliore
		stazio->bestCar = autonomia;

	return;
}

// 	=======================================		TO DO AVL

void demolisciStazione(station_t* stazione, station_t** root){	//2 do AVL
	station_t *sottoalbero, *daCanc, *sta;
	car_t *tmp = NULL;
	daCanc = sottoalbero = sta = NULL;

	if(stazione->left == NULL || stazione->right == NULL)
		daCanc = stazione;
	else if ((sta = successore(stazione)) != NULL)
		daCanc = successore(stazione);

	if(daCanc->left != NULL)
		sottoalbero = daCanc->left;
	else if (daCanc->right != NULL)
		sottoalbero = daCanc->right;

	if(sottoalbero != NULL)
		sottoalbero->parent = daCanc->parent;

	if(daCanc->parent == NULL)
		*root = sottoalbero;
	else if(daCanc == (daCanc->parent)->left)
		(daCanc->parent)->left = sottoalbero;
	else
		(daCanc->parent)->right = sottoalbero;


	if(daCanc != stazione){	//tutto il nodo
		stazione->distance = daCanc->distance;
		stazione->nCars = daCanc->nCars;
		stazione->bestCar = daCanc->bestCar;

		tmp = stazione->carsLot;
		stazione->carsLot = daCanc->carsLot;
		daCanc->carsLot = tmp;
	}

	//dealloco le cose in heap di daCanc, effettivamente da distruggere
	
	eliminaTutto(daCanc->carsLot);		
	free(daCanc);

	return;
}

// 	=======================================	DONE

void eliminaTutto(car_t* curr){	//ok
	if(curr != NULL){
		eliminaTutto(curr->left);
		eliminaTutto(curr->right);
		free(curr);
	}
	return;
}

// 	=======================================		DONE 

car_t* rottamaAuto(car_t* car, station_t* stazio){	//2 do AVL
	car_t *sottoalbero, *daCanc, *a;
	sottoalbero = daCanc = a = NULL;


	if(car->left == NULL || car->right == NULL)
		daCanc = car;
	else if((a = successoreCar(car)) != NULL)
		daCanc = a;

	if(daCanc && daCanc->left != NULL)
		sottoalbero = daCanc->left;
	else if (daCanc && daCanc->right != NULL)
		sottoalbero = daCanc->right;
	
	if(stazio->bestCar == car->fuel){	//sistemo bestCar
		if(car->right != NULL)
			stazio->bestCar = (car->right)->fuel;
		else if(car->left != NULL)
			stazio->bestCar = (car->left)->fuel;
		else if (car->parent != NULL)
			stazio->bestCar = (car->parent)->fuel;
		else
			stazio->bestCar = 0;
	}

	if(sottoalbero != NULL)
		sottoalbero->parent = daCanc->parent;

	if(daCanc->parent == NULL)
		stazio->carsLot = sottoalbero;
	else if(daCanc == (daCanc->parent)->left)
		(daCanc->parent)->left = sottoalbero;
	else
		(daCanc->parent)->right = sottoalbero;

	if(daCanc && daCanc != car)
		car->fuel = daCanc->fuel;

	if(daCanc)
		free(daCanc);

	return stazio->carsLot;
}

// 	=======================================	DONE

list_t* inserisci(list_t* coda, int km, station_t* stazioneToAdd){

	list_t* tmp;
	
	if (coda != NULL){
			tmp = (list_t*)malloc(sizeof(list_t));
			
			if(stazioneToAdd == NULL){	// aggiungo solo per stampa

				tmp->stazione = km;
				tmp->pStazione = NULL;
			}
			else 	//aggiungo stazione per lista
			{	
				tmp->stazione = stazioneToAdd->distance;
				tmp->pStazione = stazioneToAdd;
				tmp->dist = -1;	// inizializzo irraggiungibile
				tmp->prec = NULL;
				tmp->color = 0;
			}

			tmp->next = NULL;
			coda->next = tmp;
			coda = tmp;
		}

	return coda;
}

// 	=======================================		TO DOOO 4R

void pianificaPercorso(station_t* stazPa, station_t* stazAr){	//2 do
	list_t *head, *realHead;
	int esito = -1;	// 1 se percorso esiste, -1 se nessun percorso

	head = realHead = NULL;

	if(stazPa == NULL || stazAr == NULL)
		esito = -1;
	else
	{	//else ho entrambe le stazioni
		if((realHead = (list_t*)malloc(sizeof(list_t)))){
			head = realHead;
			realHead->next = NULL;
			
			realHead->stazione = stazAr->distance;	// procedo a ritroso
			realHead->pStazione = stazAr;

			if(stazPa->distance <= stazAr->distance)
				esito = pianificaRic(head, stazPa, stazAr);		//	Pa --> Ar
			else 
				esito = pianificaIter(head, stazPa, stazAr);	// Ar <-- Pa
		}
	}

	if(esito == -1){
		printf("nessun percorso\n");
	}
	else if(realHead != NULL)
	{	
		stampa(realHead);
		printf("\n");
	}

	// while(realHead != NULL){
	// 	head = realHead;			
	// 	realHead = realHead->next;			
	// 	free(head);
	// }

	return;
}

// 	=======================================	DONE

void stampa(list_t *head){
	if(head == NULL)
		return;

	stampa(head->next);
	printf("%d ", head->stazione);
	
	return;
}


// 	=======================================	

que_t* enqueue(que_t* queue, list_t* nodoList) {

	que_t *tmp, *copy;

	copy = (que_t*)malloc(sizeof(que_t));

	copy->nodo = nodoList; 
	copy->next = NULL;
			
	if(queue == NULL){			
		queue = copy;
	} else {
		for(tmp = queue; tmp != NULL && tmp->next != NULL; tmp = tmp->next);
		tmp->next = copy;
	}

	return queue;
}

// 	=======================================	

que_t* dequeue(que_t** queue) {
	
	que_t *nodo;

	if(*queue == NULL){			
		nodo = NULL;
	} else {
		nodo = *queue;
		*queue = (*queue)->next;
	}

	return nodo;
}


// 	=======================================	

int pianificaIter(list_t *head, station_t *stazPa, station_t *stazAr){
	
	int esito = -1;
	
	station_t *current;
	list_t *coda, *testa, *v;
	que_t *queue, *u;


	// 		BFS	dritto


	// creo la lista topologicamente ordinata
	testa = coda = (list_t*)malloc(sizeof(list_t));
	testa->color = 1;
	testa->dist = 0;
	testa->next = testa->prec = NULL;
	testa->pStazione = stazPa;
	testa->stazione = stazPa->distance;

	for(current = predecessore(stazPa); current != NULL && current != stazAr; current = predecessore(current)){
		if(current->bestCar > 0 && current->nCars > 0)
			coda = inserisci(coda, 0, current);
	} 
	coda = inserisci(coda, 0, stazAr);

	// inizializzo la queue
	queue = u = NULL;
	queue = enqueue(queue, testa);


	while(queue != NULL){	// finche' non termino i nodi visitabili

		u = dequeue(&queue);	
		
		if( u->nodo->pStazione == stazAr ){
			esito = 1;
			u->nodo->color = 2;
			break;
		}

		v = u->nodo->next;	// qui l'ultima iter. u->nodo->pStaz: predecessore(stazAr), v->nodo->pStaz: stazAr

		if(u == NULL || v == NULL)
			break;

		// pool di v adjacent a u: aka stazioni raggiungibili con autonomia
		while( u != NULL && v != NULL && u->nodo->pStazione->bestCar - (u->nodo->stazione - v->stazione) >= 0){

			if(v->color == 0){	// aggiunta stazione alla queue
				v->color = 1;
				v->prec = u->nodo;
				v->dist = u->nodo->dist + 1;
				queue = enqueue(queue, v);
				
			}
			else if(v->pStazione == stazAr){
				if(v->color == 1 && v->dist >= u->nodo->dist + 1)	// cioe' ha gia' un prec
				{
					v->prec = u->nodo;	// aggiorno il prec
					v->dist = u->nodo->dist + 1;
				}
				break;
			}

			v = v->next;

		}

		u->nodo->color = 2; // black, visited

	}

	if(esito == 1){
		while(coda != testa){
			coda = coda->prec;
			head = inserisci(head, coda->stazione, NULL);
		}
	}


/*




	coda = head;
	base = stazAr;	// serve come segnapunto per stazione intermedia
	current = successore(stazAr);	// scorre la stazione corrente

	while(current != NULL && esito == 1){			

		next = NULL; // ogni iter. ri-azzero la next. se non c'e', allora rimane null e si esce

		while(current != NULL){

			// spostamento
			if(current->bestCar >= current->distance - base->distance){
				next = current;
			}

			// interruzione
			if(current->distance == stazPa->distance)
				break;

			// successore
			current = successore(current);
		}

		if(next != NULL){

			coda = inserisci(coda, 0, next);
					
			if(next == stazPa)
				break;
			
			current = successore(next);
			base = next;
		}
		else
		{	// break: non ho auto 
			esito = -1;
			break;
		}	
	}

	*/
	/*

	// DAG dritto:		 Ar <-- Pa

	list_t *testa, *scorri, *vertex;
	station_t* current;

	vertex = NULL;
	current = stazPa;

	scorri = testa = (list_t*)malloc(sizeof(list_t));
	testa->pStazione = current;
	testa->stazione = current->distance;
	testa->next = testa->prec = NULL;
	testa->dist = 0;

	while(current){
		scorri = inserisci(scorri, 0, current);
		if(current == stazAr)
			break;
		current = predecessore(current);
	}

	// ora, testa e' la mia lista ordinata

	scorri = testa;
	while(scorri != NULL && scorri->pStazione != stazAr){
		
		for(vertex = scorri->next; vertex != NULL && scorri->pStazione->bestCar >= scorri->stazione - vertex->stazione; vertex = vertex->next){
			if(vertex->stazione > scorri->stazione + 1){
				vertex->dist = scorri->dist + 1;
				vertex->prec = scorri;
			}
		}

		if(vertex->pStazione == stazAr)
		{
			esito = -1;
			break;
		}

		scorri = scorri->next;
	}

	for(; scorri != testa; scorri = scorri->prec){ printf("%d ", scorri->stazione);}

	*/
	/*

	// ---------- CORRETTEZZA dell'algoritmo: scelgo il percorso giusto ---------

	if(esito == 1 && head->next != coda){
		
		station_t *giusta = NULL;

		// precedente nella lista
		list_t *prec;
		for(prec = head; prec->next != NULL && prec->next != coda; prec = prec->next);

		while(coda != head){
			
			// quello topologicamente precedente potrebbe essere quello giusto
			current = predecessore(prec->pStazione);

			while(current != NULL && current->distance >= stazAr->distance &&
				coda->pStazione->distance - current->distance <= coda->pStazione->bestCar)
			{																					// cerco fino a che non finisco l'autonomia
				if(current->distance - current->bestCar <= prec->pStazione->distance - prec->pStazione->bestCar)
					giusta = current;			// assegno la giusta stazione

				current = predecessore(current);
			}


			// aggiorno il nodo prec
			if(giusta != NULL){
				prec->pStazione = giusta;
				prec->stazione = giusta->distance;
			}
			giusta = NULL;

			// nuova coda
			coda = prec;
			
			// nuovo precedente
			for(prec = head; prec->next && prec->next != coda; prec = prec->next);

		}
	}	

	*/
	/*




		----------------------------------- LA ROBA RICORSIVA -----------------------


		int autonomiaResidua, autonomiaNext;
		list_t *testa, *coda, *tmp;
		testa = coda = tmp = NULL;


		// lista per gestire i nodi nella ricorsione (nel balzo) corrente
		coda = testa = (list_t*)malloc(sizeof(list_t)); 
		testa->next = NULL;
		testa->stazione = 0;	// testa della lista
		testa->pStazione = NULL;



							// RICERCA NEXT TAPPA & altre ipotetiche


		// inizializzazione variabili di confronto: sempre stazPa come ipotetica next, poi se lo rimane, siamo nella merda
		current = predecessore(stazPa);
		autonomiaResidua = stazPa->bestCar - (stazPa->distance - current->distance);
		next = stazPa;

		// cerco la next tappa fino a dove stazPa puo' arrivare
		while(current != NULL && autonomiaResidua >= 0){
			

			// interruzione: arrivati alla fine: non cerco oltre.
			if(current->distance == stazAr->distance){
				if(stazPa == next){						// significa che e' l'ultimo balzo: corner case
					testa->pStazione = stazPa;
					testa->stazione = stazPa->distance;
				}
				break;
			}
			

			// verifica e spostamento della next
			autonomiaNext = next->bestCar - (next->distance - current->distance);
			
			if(current->nCars > 0	&&	current->bestCar >= autonomiaNext){	// condizione per selezionare la candidata next
				next = current;

				// resetto la lista
				coda = testa->next;
				while(coda != NULL){
					tmp = coda;			
					coda = coda->next;			
					free(tmp);
				}

				// e la rifaccio da qui
				testa->next = NULL;
				testa->pStazione = current;
				testa->stazione = current->distance;
				coda = testa;
			}



			//	 accodamento: condizione di tutte le stazioni da controllare
			else if(current->bestCar >= autonomiaResidua)
				coda = inserisci(coda, 0, current);



			// scorrimento: aggiornamento current e condizioni autonomia
			current = predecessore(current);
			autonomiaResidua = stazPa->bestCar - (stazPa->distance - current->distance);
		}




					// VERIFICA RISULTATI RICERCA & RICORSIONI 


		if(testa->stazione != 0 && testa->stazione != stazPa->distance){	// DFS del pool di stazioni

			
			int nBalziNext, nBalziCurrent, ctr;	// contatori di confronto
			nBalziCurrent = nBalziNext = 0;


			// inizializzo nBalziNext
			coda = testa;
			
			ctr = pianificaRic(head, next, stazAr, 0);

			if(ctr == -1)	// se anche il migliore non trova un percorso, tutti gli altri non lo troveranno
				nBalziNext = -1;
			else
				nBalziNext = 1 + ctr;	// se lo trova, conto un balzo in piu'
			
			coda = coda->next;


							// DFS del POOL

			// verifico per tutte le stazioni nella lista
			while(coda != NULL && nBalziNext > 0){
				

				ctr = pianificaRic(head, coda->pStazione, stazAr, 0);
				
				if(ctr == -1){	// la curr stazio non ha un percorso
					coda = coda->next;
					continue;
				}
				else	// c'e' un percorso: allora confronto
					nBalziCurrent = 1 + ctr;
				
				
				// confronto: seleziono il percorso con meno balzi

				if(nBalziCurrent > 0 && nBalziCurrent <= nBalziNext){
					next = coda->pStazione;
					nBalziNext = nBalziCurrent;
				}
				
				coda = coda->next;
			}



			// verifica risultati

			if(nBalziNext != -1){	// ho il successo, accodo a head e returno i balzi finora fatti
				head = inserisci(head, next->distance, NULL);
				return nBalziNext;
			}
			else	// di tutto il pool, non ho trovato percorsi
				return -1;

		} 
		else	//fallimento: next == stazPa significa che non ho un'auto che balzi piu' lontano di quanto gia' possa fare la prima bestCar
			return -1;

*/

	return esito;
}


// 	=======================================		TO DOOOOO 4R

int pianificaRic(list_t *head, station_t *stazPa, station_t *stazAr){

	if(stazPa->distance == stazAr->distance)
		return 1;

	station_t *current, *next;

		current = predecessore(stazAr);
		next = NULL;
		
		while(current != NULL){

			// spostamento
			if((current->bestCar >= stazAr->distance - current->distance) && (current->nCars > 0))	//tengo l'ultimo current, che e' quello buono
				next = current;

			// interruzione: stazione iniziale
			if(current->distance == stazPa->distance)
				break;

			// scorrimento
			current = predecessore(current);
		}

		if(next != NULL){ //successo: accodo e proseguo
			head = inserisci(head, next->distance, NULL);
			return pianificaRic(head, stazPa, next);
		} 
		else	//fallimento
			return -1;

}

// 	=======================================	DONE

station_t* successore(station_t* stazione){	//ok
	station_t* current = NULL;

	if(stazione){
		if(stazione->right != NULL)
			return minimo(stazione->right);

		if((current = stazione->parent) != NULL){
			while(current != NULL && current->right == stazione){
				stazione = current;
				current = current->parent;
			}
		}
	}

	return current;
}

// 	=======================================	DONE

car_t* successoreCar(car_t* car){	//ok
	car_t* current = NULL;

	if(car){
		if(car->right != NULL)
			return minimoCar(car->right);

		if((current = car->parent) != NULL){
			while(current != NULL && current->right == car){
				car = current;
				current = current->parent;
			}
		}
	}

	return current;
}

// 	=======================================	DONE

car_t* minimoCar(car_t* car){		//ok
	while(car && car->left != NULL)
		car = car->left;
	return car;
}

// 	=======================================	DONE

station_t* minimo(station_t* stazione){		//ok
	while(stazione && stazione->left != NULL)
		stazione = stazione->left;
	return stazione;
}

// 	=======================================	DONE

station_t* predecessore(station_t* stazione){	//ok
	station_t* current = NULL;

	if(stazione){
		if(stazione->left != NULL)
			return massimo(stazione->left);

		if((current = stazione->parent) != NULL){
			while(current != NULL && current->left == stazione){
				stazione = current;
				current = current->parent;
			}
		}
	}

	return current;
}

// 	=======================================	DONE

station_t* massimo(station_t* stazione){	//ok
	while(stazione && stazione->right != NULL)
		stazione = stazione->right;
	return stazione;
}


/*		STUFFFFFFFFFFFFFFFFFFFFF
// 	=======================================





	//da qui in poi, DA IGNORARE il resto della foo

	// if(stazPa->distance == stazAr->distance)	//condizione d'arrivo
	// {
	// 	head = inserisci(head, stazAr->distance);
	// 	return 1;
	// }
	// else if(stazPa->bestCar >= abs(stazAr->distance - stazPa->distance))	//ho autonomia per fare l'ultimo balzo
	// {
	// 	head = inserisci(head, stazPa->distance);
	// 	return pianificaRic(head, stazAr, stazAr);
	// }


	// //else: non sono ancora arrivato all'ultima stazione, o devo ancora spostarmici vicino

	// station_t *curr, *candidato;

	// candidato = stazPa;
	// candidato->distance = stazPa->distance;
	// candidato->bestCar = stazPa->bestCar;

	// //candidato e' la staz che e' piu' probabile che sia la tappa effettiva (rimane variabile fino all'esaurirsi delle condizioni)
	// //inizialmente il candidato e' la partenza stessa. se non si verifica la cond. d'arrivo sopra e candidato e' la partenza fino alla fine del while, allora non ho un percorso, poiche' nessuna auto potrebbe percorrere piu' km dell'attuale del candidato.
	// //curr e' la stazione corrente, quella che viene messa al vaglio delle condizioni di candidata o meno. curr scorrera' linearmente tutte le stazioni, poiche' devo quantomento leggerle per poter applicare un algoritmo corretto e completo.
	
	// int distPerc;

	// if(stazPa->bestCar > 0 && stazPa->nCars > 0){	//condizioni necessarie per spostarsi da una stazione, altrimenti fallisco a priori.

	// 	//	 A --> B
	// 	if(stazPa->distance < stazAr->distance){

	// 		curr = successore(stazPa);	//fisso la prima prossima tappa
	// 		distPerc = abs(curr->distance - stazPa->distance);	//e la prima distanza da percorrere
			
	// 		//stazPa->bestCar >= abs(stazAr->distance - stazPa->distance)
	// 		while(curr != NULL && (stazPa->bestCar - distPerc >= 0) && //CONDIZIONE di SPOSTAMENTO EFFETTIVO (cioe' valuto se nuova ricorsione o fallimento)
	// 			candidato->bestCar < (stazAr->distance - candidato->distance))	//condizione di quando trovo l'ultima tappa
	// 		{
	// 			// condizioni per il candidato
	// 			if((candidato->bestCar - (curr->distance - candidato->distance)) < curr->bestCar){	//in B-->A, bisogna mettere la condizione che curr->distance < candidato->distance
	// 				candidato = curr;
	// 				candidato->distance = curr->distance;
	// 				candidato->bestCar = curr->bestCar;
	// 			}
				
	// 			//iter. successiva
	// 			curr = successore(curr);
	// 			if(curr != NULL)
	// 				distPerc = curr->distance - stazPa->distance;
	// 		}

	// 		// il candidato deve cambiare dalla partenza (caso base gia' trattato sopra), altrimenti non ci sono autos
	// 		if(candidato != stazPa && (pianificaRic(head, candidato, stazAr) != -1)){
	// 			head = inserisci(head, candidato->distance);
	// 			return 1;
	// 		}
	// 		else	//altrimenti significa che ho zero auto swag!
	// 			return -1;
	// 	}


	// 	//	A <-- B		TO DO

	// 	else{
	// 		// curr = predecessore(stazPa);
	// 		// distPerc = stazPa->distance - curr->distance;
	// 		return -1;
	// 	}

	// }
	// else	// non ho auto swag nella stazione di partenza
	// 	return -1;

search(int )

int searchArray(int array[], int key, int n, int nCars){	//la prima ricorsione n=0
	if (array[n] = key || array[n] = NIL)	//definisci NIL
		return array[n];	//puo' essere NIL se non c'e' oppure il valore trovato
	if (array[n] < key)
		return searchArray(array[], (2 * n + 2));
	else return searchArray(array[], (2 * n + 1));
}

int minArray(int array[], int n){
	for(; array[n]; n = 2*n + 1);
	return n;
}

int maxArray(int array[], int n){
	for(; array[n]; n = 2*n + 2);
	return n;
}

int successorArray(int array[], int n){
	if(array[(2*n + 2)])
		return minArray(array[], (2 * n + 2))
}


void maxHeapify(int array[], int n, int nCars){
	int l, r, maxpos, tmp;
	l = 2*n + 1;	//assegna figlio sx e dx rispetto al nodo n
	r = 2*n + 2;
	if((l <= nCars) && (array[l] > array[n]))	// la condizione su nCars e' < o <=?
		maxpos = l;
	else
		maxpos = n;
	if((r <= nCars) && (array[r] > array[n]))
		maxpos = r;
	if (maxpos != n){
		tmp = array[n];
		array[n] = array[posmax];
		array[posmax] = tmp;
		maxHeapify(array[], posmax);
	}
	return;
}

void buildMaxHeap(int array[], int nCars){
	for(int i=(nCars/2); i>=0; i--)	//???? controlla i da dove a dove
		maxHeapify(array[], i, nCars);
	return;
}

int maxCar(int array[]){	//per vergogna la implemento nel main
	return array[0];
}

 */
