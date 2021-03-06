
/** ANALYSE SYNTAXIQUE + GRAMMAIRE **/

/* FIXME Ajouter une methode qui copie les methodes de la classe mere dans l'attribut classe->override
 * SI EXTENDS
 * Aussi copier les attributs
 */

/* les tokens ici sont ceux supposes etre renvoyes par l'analyseur lexical
 * A adapter par chacun en fonction de ce qu'il a ecrit dans tp.l
 * Bison ecrase le contenu de tp_y.h a partir de la description de la ligne
 * suivante. C'est donc cette ligne qu'il faut adapter si besoin, pas tp_y.h !
 */
%token CLASS VAR EXTENDS IS DEF OVERRIDE RETURNS YIELD IF THEN ELSE NEWO PLUS MINUS AFFECT MUL DIV CONCAT
%token <S> ID CSTS IDCLASS RETURN STATIC
%token <I> CSTE RELOP

/* indications de precedence d'associativite. Les operateurs sur une meme
 * ligne (separes par un espace) ont la meme priorite. Les ligns sont donnees
 * par precedence croissante d'operateurs.
 */
%left CONCAT
%nonassoc RELOP
%left PLUS MINUS
%left MUL DIV 
%left '.'
%left unaire

/* voir la definition de YYSTYPE dans main.h 
 * Les indications ci-dessous servent a indiquer a Bison que les "valeurs" $i
 * ou $$ associees a ces non-terminaux doivent utiliser la variante indiquee
 * de l'union YYSTYPE (par exemple la variante D ou S, etc.)
 * La "valeur" associee a un terminal utilise toujours la meme variante
 */

%type <T> expr Programme Bloc BlocOpt ContenuBloc YieldOpt Cible Instruction ContenuClassOpt AffectExprOpt BlocOuExpr selection constante instanciation envoiMessage LInstruction LInstructionOpt OuRien ListOptArg LArg
%type <V> ListDeclVar LDeclChampsOpt LParam ListParamOpt Param
%type <M> Methode LDeclMethodeOpt
%type <CL> LClassOpt DeclClass ListExtendsOpt DefClass
%type <I> OverrideOuStaticOpt StaticOpt 

/* Initialisation des variables globales*/

%{
#include "tp.h"     /* les definition des types et les etiquettes des noeuds */
PCLASS classActuel; /* Classe en cours d'analyse*/
TreeP programme;

extern int yylex(); /* fournie par Flex */
extern void yyerror();  /* definie dans tp.c */

%}

%% 
 /*
 * Attention: on est dans un analyseur ascendant donc on s'occupe des composants
 * d'une construction avant de traiter la construction elle-meme. Dans le cas
 * d'un IF on traitera donc la condition et les parties THEN et ELSE avant
 * de traiter le 'IF' lui-meme. Comme on ne doit evaluer que l'une des deux
 * branches 'then' ou 'else' selon la valeur de la condition, on ne peut pas
 * evaluer les expressions au fur et a mesure qu'on les rencontre puisqu'on ne
 * sait pas si on fait partie d'un IF ou pas. On doit attendre de connaitre
 * l'expression complete pour savoir si telle partie doit etre evaluee ou non.
 *
 * Les macros d'allocation NEW et de nullite NIL sont definies dans tp.h.
 * Leur usage n'est bien sur pas obligatoire, juste conseille !
 *
 * Les definition des types YYSTYPE, VarDecl, VarDeclP, Tree, TreeP et autres
 * sont dans tp.h
 */

/*
 * Declaration d'une classe => vérifier ce qui est optionnel ou non
 * Les : c'est à mettre entre cote au niveau de ID: ?
 * epsilon est declare au dessus
 */

/*
 * Axiome : Liste de classe optionnel suivi d'un bloc obligatoire
 */ 

Programme : LClassOpt Bloc		{$$=makeTree(PROGRAM,2,makeLeafClass(LISTCLASS,$1),$2);programme=$$;}

/*
 * Liste de classes optionnelle : Vide ou composee d'au moins une declaration de classe
 */
LClassOpt : DeclClass LClassOpt   	{$1->suivant=$2; $$=$1;}
            | /* epsilon */       	{$$=NIL(SCLASS);}
            ;

/*
 * Un bloc est defini par une entite avec 2 accolades entourant un contenu
 */
Bloc : '{' ContenuBloc '}'      	{$$=$2;}
      ;

/*
 * Le contenu d'un bloc : Une 
 * une liste d'instruction optionnelle suivi d'un Yield option => S'il y a un yield => bloc procedural
 * Ou si on a une List de declaration de valeur => oblige apres le IS d'avoir une Liste d'instruction
 * suivi par un Yield optionnel
 */

ContenuBloc : LInstructionOpt YieldOpt        {$$=makeTree(CONTENUBLOC,3,NIL(Tree),$1,$2);}
      | ListDeclVar IS LInstruction YieldOpt  {$$=makeTree(CONTENUBLOC,3,makeLeafVar(LISTEVAR, $1),$3,$4);}  
      ;

/*
 * Sert a differencier les deux types de bloc : fonctionnel et procedural
 */

YieldOpt : YIELD expr     	{$$=makeTree(ETIQUETTE_YIELD, 1, $2);}
        | /* epsilon */       	{$$=NIL(Tree);}
	;

ListDeclVar : VAR StaticOpt ID ':' IDCLASS AffectExprOpt ';' LDeclChampsOpt 
            {
		$$=makeListVar($3,getClasseBis(listeDeClass,$5),$2,$6);
            	$$->suivant=$8;
            }
            ;

/*
 * Liste d'instructions optionnel 
 */

LInstructionOpt : Instruction LInstructionOpt 	{$$=makeTree(LIST_INSTRUCTION, 2, $1, $2);}
                | /* epsilon */ 		{$$=NIL(Tree);}
                ;
/*
 * Liste d'instructions obligatoires 
 */
LInstruction : Instruction LInstructionOpt  	{$$=makeTree(LIST_INSTRUCTION, 2, $1, $2);}
              ;

/*
 * Une instruction c'est : 
    *  expression;
    * bloc-procedural
    * cible:= expression;
    * if expression then instruction else instruction
 */

Instruction : expr ';'            				{$$=$1;}
            | RETURN expr ';'         				{$$=makeTree(EXPRESSIONRETURN, 1, $2);}
            | Bloc           					{$$=$1;}
            | Cible AFFECT expr ';'       			{$$=makeTree(ETIQUETTE_AFFECT, 2, $1, $3);} 
            | IF expr THEN Instruction ELSE Instruction   	{$$=makeTree(IFTHENELSE, 3, $2, $4, $6);}
            | RETURN ';'          				{$$=makeLeafStr(RETURN_VOID, MSG_VOID);}  /* return void */

            ;
/*
 * La cible de l'affectation ne peut etre qu'un identifiant : 
 * un nom d'un objet x := new Point(1,5)
 * un nom de classe var res : Point := new Point(x, y);
 */

Cible : ID    			{$$=makeLeafStr(IDENTIFICATEUR,$1);}
      | selection 		{$$=$1;}
      ;

/*
 * Bloc optionnel
 */
BlocOpt : Bloc 		   	{$$=$1;}
        | /* epsilon */ 	{$$=NIL(Tree);}
        ;


/* 	Definition de la classe (exemple : class Point) poour que cette classe puisse avoir des methode/champs 
 *	qui sont du meme type que la classe
 */
DefClass : CLASS IDCLASS 
{		
	/* probleme : la classe qu'on souhaite declaree existe deja */		
	if(estDansListClasse(listeDeClass, $2) == TRUE){
		printf("La classe %s existe deja\n", $2);
		char* message = NEW(SIZE_ERROR,char);
		sprintf(message,"Erreur la classe %s est deja declaree",$2);
		$$ = NULL; 
	} 
	
	/* Pas de probleme : ajout de la classe */
	else{
		$$=makeDefClasse($2); 
		classActuel=$$; 		
		/* mise a jour de la variable globale (la liste des classes) */
		if(listeDeClass==NULL){
			listeDeClass=$$;
		}
		else{
			PCLASS tmp=listeDeClass;
			while(tmp->suivant!=NULL){
				tmp=tmp->suivant;	
			}
			tmp->suivant=$$;
		}
	}
}
	 ;

/*
 * class nom(param, ...) [extends nom(arg, ...)] [bloc] is {decl, ...}
 */
DeclClass : DefClass'('ListParamOpt')' ListExtendsOpt BlocOpt IS '{'ContenuClassOpt'}' 
{   
    	if($1 == NULL){
		printf("Erreur dans DefClass : veuillez resoudre le probleme avant de continuer les checks\n");
		exit(0);
	}
	else {
		int isExtend; 
		/** cas ou une classe n'herite pas d'une classe mere **/
       		if($5==NIL(SCLASS) || $5==NULL){
          		isExtend=0;
        	}
		/** cas ou une classe herite d'une classe mere **/
		else{
          		isExtend=1;
       		 }

        	$1=makeClasseApresDef($1,$3,$6,$9->u.children[1]->u.methode,$9->u.children[0]->u.var, $5,isExtend);
        	
		/*      Pour nous simplifier la tache, on copie les attributs et methodes 
		 *	de la classe mere dans les attributs et methodes de la classe fille 
		 */
		if(isExtend == 1){ 	
			/* Si une classe n'a pas de constructeur
			 Ajout des attributs ($5=classe mere)*/ 
			 

                  
			if($5->liste_champs != NULL){	/* cas ou la classe mere a d'attributs */
				/* cas ou la classe fille a des attributs -> faut ceux de la mere ajouter a la fin */
				if(classActuel->liste_champs!=NULL){
					PVAR tmp_liste_champs = classActuel->liste_champs;
					while(tmp_liste_champs->suivant != NULL){
						tmp_liste_champs=tmp_liste_champs->suivant;
					}
					PVAR tmp_champs_mere = $5->liste_champs;
					PVAR champs_a_ajoutee = NULL;
					/* On parcours les champs de la mere */
					while(tmp_champs_mere != NULL){

						/* Si le champ de la mere n'est pas dans la fille -> on ajoute */
						if(varEstDansListe(classActuel->liste_champs, tmp_champs_mere->nom)==FALSE){
							if(champs_a_ajoutee == NULL){
								champs_a_ajoutee = makeListVar(tmp_champs_mere->nom, tmp_champs_mere->type, tmp_champs_mere->categorie, tmp_champs_mere->init);
							}
							else{
								SVAR copieVar = *champs_a_ajoutee;
								champs_a_ajoutee = makeListVar(tmp_champs_mere->nom, tmp_champs_mere->type, tmp_champs_mere->categorie, tmp_champs_mere->init);
								champs_a_ajoutee->suivant = NEW(1, SVAR);
								*champs_a_ajoutee->suivant = copieVar;
							}
						}
						
						tmp_champs_mere = tmp_champs_mere->suivant;
					}

					/* ajout des attributs (en fin de liste) */
					if(champs_a_ajoutee!=NULL){
						
						SVAR copieVarFinale = *champs_a_ajoutee;
						PVAR tmp_champs_a_ajoutee = NEW(1,SVAR);	
						*tmp_champs_a_ajoutee = copieVarFinale;
						tmp_liste_champs->suivant = tmp_champs_a_ajoutee;
					}
				}
				/* cas ou la classe fille n'a pas d'attributs -> on ajoute directement ceux de la mere */
				else{
					classActuel->liste_champs = $5->liste_champs;	
				}
			}

			 
			/* Ajout des methodes ($5 = classe mere)
			 *	-> Remarque : si classe fille redefinie une methode -> on n'ajoute pas celle de la mere
			 */
			 
			if($5->liste_methodes != NULL){
				/* cas ou la classe fille n'a pas de methode -> on ajoute directement celles de la mere */
				if(classActuel->liste_methodes==NULL){
					classActuel->liste_methodes = $5->liste_methodes;
				}
				/* cas ou la classe fille a des methodes -> on doit aller a la fin de la liste */
				else{
					PMETH tmp_liste_methodes = classActuel->liste_methodes;
					while(tmp_liste_methodes->suivant != NULL){
						tmp_liste_methodes=tmp_liste_methodes->suivant;
					}
	 				/* Ajout de maniere iterative les methodes car il faut les verifier 1 par 1 */
					PMETH tmp_liste_methodes_classMere = $5->liste_methodes;
					PMETH liste_a_ajoutee = NULL;

					/* Parcours des methodes de la classe mere */
					while(tmp_liste_methodes_classMere != NULL){
						/* On regarde si la classe fille a la meme methode que la classe mere
							-> si oui : on regarde isRedef de la methode de la classe fille
								-> si il est = a 0 : le mettre à 1 */
						PMETH methClassActuel=getMethode(classActuel, tmp_liste_methodes_classMere);
						/*if(methodeDansClasse(classActuel, tmp_liste_methodes_classMere)==TRUE){*/
						if(methClassActuel != NULL){
							if(methClassActuel->isRedef==0){
								methClassActuel->isRedef = 1;
							}
						}
						/* la methode de la mere n'est pas dans la classe fille */
						else{
							/* Condition pour que la classe mere puisse etre ajoutee dans la classe fille */
							if(tmp_liste_methodes_classMere->isStatic == 0){
							/* Ici, les methodes ajoutees ne peuvent pas etre static ni etre override */
							/* Il faut gerer le cas ou elles sont pas dites "redefinies" alors qu'elles le sont */
								if(liste_a_ajoutee == NULL){
									liste_a_ajoutee = makeMethode(tmp_liste_methodes_classMere->nom, 0, tmp_liste_methodes_classMere->corps, tmp_liste_methodes_classMere->typeRetour, tmp_liste_methodes_classMere->params, tmp_liste_methodes_classMere->home);
								}
								else{
									SMETH copie = *liste_a_ajoutee;
									liste_a_ajoutee = makeMethode(tmp_liste_methodes_classMere->nom, 0, tmp_liste_methodes_classMere->corps, tmp_liste_methodes_classMere->typeRetour, tmp_liste_methodes_classMere->params, tmp_liste_methodes_classMere->home);
									liste_a_ajoutee->suivant = NEW(1,SMETH);
									*liste_a_ajoutee->suivant = copie;
								}
							}
						}
						tmp_liste_methodes_classMere = tmp_liste_methodes_classMere->suivant;
					}
					if(liste_a_ajoutee!=NULL){
						SMETH copieFinale = *liste_a_ajoutee;
						PMETH tmp_liste_a_ajoutee = NEW(1,SMETH);	
						*tmp_liste_a_ajoutee = copieFinale;
						tmp_liste_methodes->suivant = tmp_liste_a_ajoutee;
					}
				}
			}	
		}
   		$$=classActuel;
	}
}
    ;

/* Contenu d'une classe : elle peut contenir une liste des champs et/ou des methodes */
ContenuClassOpt : LDeclChampsOpt LDeclMethodeOpt  {$$=makeTree(CONTENUCLASS,2,makeLeafVar(LISTEVAR,$1),makeLeafMeth(LISTEMETHODE,$2));}
    ;

/* 
 * var [static] nom : type [:= expression]; 
 */
LDeclChampsOpt : VAR StaticOpt ID ':' IDCLASS AffectExprOpt ';' LDeclChampsOpt  
       			{$$=makeListVar($3,getClasseBis(listeDeClass,$5),$2,$6); $$->suivant=$8;}
              |     	{$$=NIL(SVAR);}
              ;

/* Renvoie un entier : 1 si la methode est static, 0 sinon */
StaticOpt : STATIC 	{$$=1;}
          |     	{$$=0;}
          ;

/* Affectation optionnelle d'une expression */
AffectExprOpt : AFFECT expr    	{$$=makeTree(ETIQUETTE_AFFECT, 1, $2);}
              | 		{$$=NIL(Tree);}
              ;

/* Declaration d'une Methode :
 * def [override | static] nom (param, ...) returns Classe bloc
 * def [override | static] nom (param, ...) returns Classe := expression
 */

Methode: DEF OverrideOuStaticOpt ID '(' ListParamOpt ')' RETURNS IDCLASS BlocOuExpr 
      {$$=makeMethode($3,$2,$9,getClasseBis(listeDeClass,$8),$5,classActuel);}
  ;

/* Liste de methode */
LDeclMethodeOpt : Methode LDeclMethodeOpt 	{$1->suivant=$2; $$=$1;}
              |         			{$$=NIL(SMETH);}
              ;

/* Renvoie un entier : 0 si la methode n'est ni statique, ni override; 1 si elle est override et 2 si elle est static */ 
OverrideOuStaticOpt : OVERRIDE     	{$$=1;}
                      | STATIC     	{$$=2;}
                      | /* epsilon */   {$$=0;}
                      ;

/* Soit un bloc, soit une expression */
BlocOuExpr : AffectExprOpt    	{$$=$1;}
           | Bloc     		{$$=$1;}
           ;

ListParamOpt : LParam       	{$$=$1;}
              | /* epsilon */   {$$=NIL(SVAR);}
              ;

LParam : Param        		{$$=$1;}
        | Param','LParam    	{$1->suivant=$3; $$=$1;}
        ;

Param : ID':' IDCLASS     {$$= makeListVar($1,getClasseBis(listeDeClass,$3),0,NIL(Tree));} /* 0 = var non static */
          ;   
          
ListExtendsOpt : EXTENDS IDCLASS'('ListOptArg')'
{

	$$=getClasse(listeDeClass, $2);	/* peut etre besoin de getClasseBis? */
	char* message = NEW(SIZE_ERROR,char);
	if($$ == NULL){
		/* la classe n'existe pas: erreur */
		printf("Probleme au niveau de la declaration de la classe\n");
    		sprintf(message,"Erreur la classe %s n'existe pas",$2);
		pushErreur(message,classActuel,NULL,NULL);
	}
	else{

		classActuel->appel_constructeur_mere = $4;

		/* on ajoute a la classe mere les param passees dans ListOptArg */
		/* Exemple : class PointColore(xc: Integer, yc:Integer, c: Couleur) extends Point(xc, yc) ==> on dit que les param xc 
			et yc de Point ont les valeurs respectives xc et yc **/
		
	/** C'est de l'eval : FIXME attente d'une fonction qui me rend le TreeP dans le bon ordre --> Amin et Gishan s'en occupe **/
	/*	COMPLETEMENT FAUX CAR L'ARBRE N'EST PAS DANS LE BON SENS
		TreeP listOptArg = $4;
		PVAR paramConstructeur = $$->param_constructeur;
		printf("while\n");
		while(listOptArg->u.children[1]!=NIL(Tree)){
			printf("1\n");
			paramConstructeur->init=listOptArg->u.children[0];
			printf("2\n");
			listOptArg = listOptArg->u.children[1];
			printf("3\n");
			paramConstructeur = paramConstructeur->suivant;
			printf("4\n");
			printf("nb de  children = %d\n", listOptArg->nbChildren);
		}
		printf("if\n");
		if(listOptArg->u.children[0]!=NIL(Tree)){
			paramConstructeur->init=listOptArg->u.children[0];
		}
		printf("-----passer-----\n");
	*/

	}
} 
               | /* epsilon */        {$$=NIL(SCLASS);}
               ;

ListOptArg :        {$$=NIL(Tree);}
     | LArg         {$$=$1;}
           ;


LArg : expr           	{$$=$1;}
     | LArg','expr      {$$=makeTree(LISTEARG, 2, $1,$3);}
     ;

/* Pour info : ID est dans Cible */
expr : PLUS expr %prec unaire   	{ $$=makeTree(PLUSUNAIRE, 1, $2); }
       | MINUS expr %prec unaire  	{ $$=makeTree(MINUSUNAIRE, 1, $2); }
       | expr CONCAT expr   		{ $$=makeTree(CONCATENATION, 2, $1, $3); }
       | expr PLUS expr    	 	{ $$=makeTree(PLUSBINAIRE, 2, $1, $3); }
       | expr MINUS expr    		{ $$=makeTree(MINUSBINAIRE, 2, $1, $3); }
       | expr DIV expr      		{ $$=makeTree(DIVISION, 2, $1, $3); }
       | expr MUL expr      		{ $$=makeTree(MULTIPLICATION, 2, $1, $3); }
       | expr RELOP expr    		{ $$=makeTree(OPCOMPARATEUR, 3 , $1, $3, makeLeafInt(OPERATEUR,$2));}
       | constante      		{ $$=$1; }
       | instanciation      		{ $$=$1; }
       | envoiMessage     		{ $$=$1; }
       | OuRien       			{ $$=$1; }
       ;

OuRien : '(' expr ')'  			{$$=$2;}
       | Cible        			{$$=$1;}
       ;

/*
 * Cas de base C.attributStatique
 * Ou c.x
 */

selection : IDCLASS'.'ID  %prec '.'       {$$=makeTree(SELECTION, 2, makeLeafStr(IDENTIFICATEURCLASS,$1),makeLeafStr(IDENTIFICATEUR,$3));}
          | envoiMessage'.'ID %prec '.'   {$$=makeTree(SELECTION, 2, $1,makeLeafStr(IDENTIFICATEUR,$3));}
          | OuRien '.' ID %prec '.' 	  {$$=makeTree(SELECTION, 2, $1,makeLeafStr(IDENTIFICATEUR,$3));}
         ;

/*
 * regle qui defini soit une constante entiere, soit une constante d type string
 */
constante : CSTS		{$$ = makeLeafStr(CSTSTRING,$1); }
    	  | CSTE  		{$$ = makeLeafInt(CSTENTIER,$1);}
          ;

instanciation : NEWO IDCLASS '(' ListOptArg ')' { $$=makeTree(INSTANCIATION, 2, makeLeafStr(IDENTIFICATEURCLASS,$2), $4); }
              ;

/*
 * Cas de base C.f(...)
 * Cas de base x.f(...)
 */

envoiMessage : IDCLASS '.' ID '(' ListOptArg ')' %prec '.'    
        {$$=makeTree(ENVOIMESSAGE, 3, makeLeafStr(IDENTIFICATEURCLASS,$1),makeLeafStr(IDENTIFICATEUR,$3),$5); }
              | envoiMessage '.' ID'('ListOptArg ')' %prec '.'    
        {$$=makeTree(ENVOIMESSAGE, 3,$1,makeLeafStr(IDENTIFICATEUR,$3),$5); }
              | OuRien '.' ID '(' ListOptArg ')'  %prec '.'   
        {$$=makeTree(ENVOIMESSAGE, 3,$1,makeLeafStr(IDENTIFICATEUR,$3),$5); }
	      | constante '.' ID '(' ListOptArg ')' %prec '.'
	{$$=makeTree(ENVOIMESSAGE, 3,$1,makeLeafStr(IDENTIFICATEUR,$3),$5); }
             ;
