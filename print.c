#include <unistd.h>
#include <stdio.h>
#include "tp.h"
#include "tp_y.h"

extern TreeP programme;
extern PCLASS classActuel;
extern void setError();
extern TreeP getChild(TreeP tree, int rank);
extern bool verbose;
extern bool noEval;

/** permet d'afficher un arbre à n fils **/
void pprintTreeMain(TreeP tree) {
	if(tree == NIL(Tree)){
		printf("ARBRE = NIL\n");
	}

	int nbChild = tree->nbChildren;
	int i;
	printf("(TreeP(nbChildren=%d et ", nbChild);
	switch (tree->op) {
		case IDENTIFICATEUR:    printf("op=IDENTIFICATEUR"); break;
		case PLUSUNAIRE:   	printf("op=PLUSUNAIRE"); break;
		case MINUSUNAIRE:    	printf("op=MINUSUNAIRE"); break;
		case CONCATENATION:    	printf("op=CONCATENATION"); break;
		case PLUSBINAIRE:  	printf("op=PLUSBINAIRE"); break;
		case MINUSBINAIRE:   	printf("op=MINUSBINAIRE"); break;
		case DIVISION:    	printf("op=DIVISION"); break;
		case MULTIPLICATION:    printf("op=MULTIPLICATION"); break;
		case OPCOMPARATEUR:     printf("op=OPCOMPARATEUR"); break;
		case OPERATEUR:     printf("op=OPERATEUR"); break;
		case SELECTION:    	printf("op=SELECTION"); break;
		case CONSTANTE:    	printf("op=CONSTANTE"); break;
		case EXPRESSION:  	printf("op=EXPRESSION");  break;
		case INSTANCIATION:   	printf("op=INSTANCIATION"); break;
		case ENVOIMESSAGE:    	printf("op=ENVOIMESSAGE"); break;
		case EXPRESSIONRETURN:    printf("op=EXPRESSIONRETURN"); break;
		case IDENTIFICATEURCLASS: printf("op=IDENTIFICATEURCLASS"); break;
		case CSTSTRING:    	printf("op=CSTSTRING"); break;
		case CSTENTIER:    	printf("op=CSTENTIER"); break;
		case RETURN_VOID:  	printf("op=RETURN_VOID"); break;
		case EXTENTION:   	printf("op=EXTENTION"); break;
		case PARAM:    		printf("op=PARAM"); break;
		case STATIQUE:    	printf("op=STATIQUE"); break;
		case SURCHARGE:   	printf("op=SURCHARGE"); break;
		case DEFTOKEN:    	printf("op=DEFTOKEN"); break;
		case LISTEMETHODE:    	printf("op=LISTEMETHODE"); break;
		case PROGRAM:  		printf("op=PROGRAM"); break;
		case LISTCLASS:   	printf("op=LISTCLASS"); break;
		case CONTENUBLOC:    	printf("op=CONTENUBLOC"); break;
		case ETIQUETTE_IS:  	printf("op=ETIQUETTE_IS"); break;
		case ETIQUETTE_YIELD: 	printf("op=ETIQUETTE_YIELD"); break;
		case ETIQUETTE_AFFECT:  printf("op=ETIQUETTE_AFFECT"); break;
		case IFTHENELSE:    	printf("op=IFTHENELSE"); break;
		case CONTENUCLASS:  	printf("op=CONTENUCLASS"); break;
		case LISTEARG:   	printf("op=LISTEARG"); break;
		case LISTEPARAM:    	printf("op=LISTEPARAM"); break;
		case LIST_INSTRUCTION:  printf("op=LIST_INSTRUCTION"); break;
		case LISTEVAR:   	printf("op=LISTEVAR"); break;
		default: break;
	}
	printf(")\n");
	if(tree->op==OPERATEUR){
		printf("=============================================\n");
		/* printf("OPERATEUR = %d \n", tree->u.val);*/
		switch(tree->u.val){
				case EQ : printf("OPERATEUR : <>\n");break;
				case NE : printf("OPERATEUR : <>\n");break;
				case LT : printf("OPERATEUR : <\n");break;
				case LE : printf("OPERATEUR : <=\n");break;
				case GT : printf("OPERATEUR : >\n");break;
				case GE : printf("OPERATEUR : >=\n");break;
				}
		printf("=============================================\n");
	}
	else if(tree->op==RETURN_VOID || tree->op==IDENTIFICATEUR || tree->op==IDENTIFICATEURCLASS || tree->op==CSTSTRING){
		printf("=============================================\n");
		printf("---tree->u.str = %s\n", tree->u.str);
		printf("=============================================\n");
	}
	else if(tree->op==CSTENTIER){
		printf("=============================================\n");
		printf("---tree->u.val = %d\n", tree->u.val);
		printf("=============================================\n");
	}
	else if(tree->op==LISTEVAR){ 
		printf("===================================================\n");
		printVar(tree->u.var);
		printf("===================================================\n");
	}
	else if(tree->op==LISTCLASS){ 
		printf("===================================================\n");
		printClasse(tree->u.classe);
		printf("===================================================\n");
	}
	else if(tree->op==LISTEMETHODE){
		printf("===================================================\n");
		printMethode(tree->u.methode);
		printf("===================================================\n");
	}
	else if(nbChild == 0){
		printf("------------------- Probleme?!\n");
	}
	else if(nbChild>0){
		for(i=0; i<nbChild; i++){
			printf("---child[%d]\n", i);
			if(tree->u.children[i] == NIL(Tree)){
				printf("--------> NIL\n");
			}
			else{
				pprintTreeMain(tree->u.children[i]);
			}
		}
	}
	else{
		printf("-----------------Probleme 2\n");
	}
}

void printClasse(PCLASS classe){
	PCLASS tmp=classe;
	if(tmp != NULL){
		printf("=======================\n");
		printf("--- classe=%s\n", tmp->nom);
		if(tmp->param_constructeur != NULL){
			printf("param_constructeur :\n");
			printVar(tmp->param_constructeur);
		}
		/*
		if(tmp->corps_constructeur != NULL){
			printf("corps_constructeur : \n");
			pprintTreeMain(tmp->corps_constructeur);
		}
		*/
		if(tmp->liste_methodes != NULL){
			printf("liste_methodes :\n");	
			printMethode(tmp->liste_methodes);
		}
		if(tmp->liste_champs != NULL){
			printf("liste_champs :\n");
			printVar(tmp->liste_champs);
		}
		if(tmp->classe_mere != NULL){
			printf("Classe mere = %s\n", tmp->classe_mere->nom);
		}
		printf("isExtend=%d\n", tmp->isExtend);
		printf("=======================\n");
		printClasse(tmp->suivant);
	}
	else{
		printf("\n");
	}
}

void printMethode(PMETH methode){
	PMETH tmp=methode;
	if(tmp != NULL){
		printf("---------------\n");
		printf("- methode=%s\n", tmp->nom);
		printf("isStatic = %d\nisRedef = %d\n", tmp->isStatic, tmp->isRedef);
		
		printf(" ========  corpMethode ========= \n");
		printf("nom = %s\n",tmp->nom);
		if(tmp->corps != NULL){
			pprintTreeMain(tmp->corps);
		}
		printf(" ======== FIN corpMethode ========= \n");
		if(tmp->typeRetour != NULL){
			printf("typeRetour = %s\n", tmp->typeRetour->nom);
			/* FONCTIONNE AUSSI : printClasse(tmp->typeRetour); */
		}
		if(tmp->params != NULL){
			printf("Liste de param :\n");
			printVar(tmp->params);
		}
		if(tmp->home != NULL){
			printf("Home = %s\n", tmp->home->nom);
		}
		printf("---------------\n");
		printMethode(tmp->suivant);
	}
	else{
		printf("\n");
	}
}

void printVar(PVAR var){
	PVAR tmp=var;
	if(tmp != NULL){
		printf("_______________________\n");
		printf("-- var=%s\n", tmp->nom);
		if(tmp->type != NULL){
			printf("type = %s\n", tmp->type->nom);
				printf("========= DEB ARBRE DE TYPE ==========");
				printClasse(tmp->type);
			printf("========= FIN ARBRE DE TYPE ==========");
		}
		printf("categorie = %d\n", tmp->categorie);

		printf("init = \n");
		if(!(tmp->init==NIL(Tree))){
		pprintTreeMain(tmp->init);
		}else{
			printf("NULL");
			}

		/*if(tmp->init != NULL){
			pprintTreeMain(tmp->init);
		}*/

		printf("_______________________\n");
		printVar(tmp->suivant);
	}
	else{
		printf("\n");
	}
}

