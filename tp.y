
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
%token CLASS VAR EXTENDS IS DEF OVERRIDE RETURNS YIELD IF THEN ELSE NEWO PLUS MINUS RELOP AFFECT MUL DIV CONCAT
%token <S> ID CSTS IDCLASS RETURN STATIC
%token <I> CSTE

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



/* %type <C> REL */
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
 /*JULIEN : {$$=makeTree(CONTENUBLOC,3,NIL(SVAR),$1,$2);} -> a {$$=makeTree(CONTENUBLOC,3,NIL(Tree),$1,$2);} */
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
		$$=makeListVar($3,getClasse(listeDeClass,$5),$2,$6);
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
		if(getClasse(listeDeClass, $2) != NULL){
			printf("Erreur dans DeclClass avec idClass=%s, elle existe deja\n", $2);
			char* message = NEW(SIZE_ERROR,char);
			sprintf(message,"Erreur la classe %s est deja declare",$2);
			/* TODO A MODIF pushErreur(message,classActuel,NULL,NULL); */
			$$ = NULL; 	/* FIXME bon? */
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
		printf("Erreur dans DefClass\n");
	}
	else {
		int isExtend; 
		/** cas ou une classe n'herite pas d'une classe mere **/
       		if($5==NIL(SCLASS)){
          		isExtend=0;
        	}
		/** cas ou une classe herite pas d'une classe mere **/
		else{
          		isExtend=1;
       		 }

        	$1=makeClasseApresDef($1,$3,$6,$9->u.children[1]->u.methode,$9->u.children[0]->u.var, $5,isExtend);

		/* FIXME VERIFIER SI LA MISE A JOUR EST CORRECTEMENT EFFECTUER (cf TypeREtour) */

		/*      Pour nous simplifier la tache, on copie les attributs et methodes 
		 *	de la classe mere dans les attributs et methodes de la classe fille 
		 */
		/** FIXME A VERIFIER **/
		if(isExtend == 1){ 
			/* Ajout des attributs ($5=classe mere)*/ 
			if($5->liste_champs != NULL){	/* cas ou la classe mere a d'attributs */
				/* cas ou la classe fille a des attributs -> faut ceux de la mere ajouter a la fin */
				if(classActuel->liste_champs!=NULL){
					PVAR tmp_liste_champs = classActuel->liste_champs;
					while(tmp_liste_champs->suivant != NULL){
						tmp_liste_champs=tmp_liste_champs->suivant;
					}
					/* ajout des attributs (en fin de liste) */
					tmp_liste_champs->suivant = $5->liste_champs;
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
					PMETH tmp_liste_a_ajoutee = NULL;
					/* Parcours des methodes de la classe mere */
					while(tmp_liste_methodes_classMere != NULL){

						/* FIXME A FINIR */ 
						/* Condition pour que la classe mere puisse etre ajoutee dans la classe fille */
						if(tmp_liste_methodes_classMere->isStatic == 0 && methodeDansClasse(classActuel, 									tmp_liste_methodes_classMere)==FALSE){

							/* TODO A verifier */ 

							if(tmp_liste_a_ajoutee == NULL){
								printf("1ere methode =%s\n", tmp_liste_methodes_classMere->nom);
								liste_a_ajoutee = makeMethode(tmp_liste_methodes_classMere->nom, 0/*a modif*/, tmp_liste_methodes_classMere->corps, tmp_liste_methodes_classMere->typeRetour, tmp_liste_methodes_classMere->params, classActuel);
								tmp_liste_a_ajoutee = liste_a_ajoutee;
							}
							else{
								printf("Autre ajout\n");
								liste_a_ajoutee = makeMethode(tmp_liste_methodes_classMere->nom, 0/*a modif*/, tmp_liste_methodes_classMere->corps, tmp_liste_methodes_classMere->typeRetour, tmp_liste_methodes_classMere->params, classActuel);
							}
							liste_a_ajoutee = liste_a_ajoutee->suivant;
							/*liste_a_ajoutee->suivant = NULL;*/
						}
						tmp_liste_methodes_classMere = tmp_liste_methodes_classMere->suivant;
					}
					printf("====\nListe a ajoute = \n");
					while(tmp_liste_a_ajoutee != NULL){
						printf("tmp_liste_a_ajoutee=%s\n", tmp_liste_a_ajoutee->nom);
						tmp_liste_a_ajoutee=tmp_liste_a_ajoutee->suivant;
					}
					printf("====\n");
					tmp_liste_methodes->suivant = tmp_liste_a_ajoutee;
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
       			{$$=makeListVar($3,getClasse(listeDeClass,$5),$2,$6); $$->suivant=$8;}
              |     	{$$=NIL(SVAR);}
              ;

/* Renvoie un entier : 1 si la methode est static, 0 sinon */
StaticOpt : STATIC 	{$$=1;}
          |     	{$$=0;}
          ;

/* Affectation optionnelle d'une expression */
AffectExprOpt : AFFECT expr ';'   {$$=makeTree(ETIQUETTE_AFFECT, 1, $2);}
              | {$$=NIL(Tree);}
              ;

/* Declaration d'une Methode :
 * def [override | static] nom (param, ...) returns Classe bloc
 * def [override | static] nom (param, ...) returns Classe := expression
 */

Methode: DEF OverrideOuStaticOpt ID '(' ListParamOpt ')' RETURNS IDCLASS BlocOuExpr 
      {$$=makeMethode($3,$2,$9,getClasse(listeDeClass,$8),$5,classActuel);}
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

Param : ID':' IDCLASS     {$$= makeListVar($1,getClasse(listeDeClass,$3),0,NIL(Tree));} /* 0 = var non static */
          ;   
          
ListExtendsOpt : EXTENDS IDCLASS'('ListOptArg')'
{
    $$=getClasse(listeDeClass, $2);
    printf("le nom de la classe est : %s",$$->nom);
    char* message = NEW(SIZE_ERROR,char);
    if($$ == NULL)
    {
      /* la classe n'existe pas: erreur */
      sprintf(message,"Erreur la classe %s n'existe pas",$2);
      pushErreur(message,classActuel,NULL,NULL);
    }
    else
    {
   
      char * nomC = calloc(100,sizeof(char));
      sprintf(nomC,"constructeur %s",$$->nom);   
      printf(RED"Debug DEBUT \n\n\n\n\n\n\n\n");
      printf("NOMC ======= %s \n ",nomC);
      PVAR tmp = $$->param_constructeur;
      while(tmp!=NULL)
      {
        printf("--------------------- %s\n ",tmp->nom);
        tmp = tmp->suivant;
      }
      printf("caca1\n");

      PMETH methodeFakeConstructeur = NEW(1,SMETH);
      methodeFakeConstructeur->nom = calloc(100,sizeof(char));
      sprintf(methodeFakeConstructeur->nom,"constructeur %s",classActuel->nom);
      methodeFakeConstructeur->params = classActuel->param_constructeur;
      printf("caca2\n");
      bool constCorrecte = compareParametreMethode($$->param_constructeur,$4,classActuel,methodeFakeConstructeur,NULL,nomC);
      printf("Debug FIN\n");
      /*exit(0);*/
      

      if(!constCorrecte)
      {
        sprintf(message,"Erreur d'appel constructeur : %s mal appelee",classActuel->nom);
        pushErreur(message,classActuel,NULL,NULL);
      }    
    }
} /*$$=makeTree(EXTENTION, 2, makeLeafStr(IDENTIFICATEURCLASS,$2),$4);}*/

               | /* epsilon */        {$$=NIL(/*Tree*/ SCLASS);}
               ;
ListOptArg :        {$$=NIL(Tree);}
     | LArg         {$$=$1;}
           ;


LArg : expr           	{$$=$1;}
     | LArg','expr      {$$=makeTree(LISTEARG, 2, $1,$3);}
     ;

/* Pour info : ID est dans Cible */
expr : PLUS expr %prec unaire   { $$=makeTree(PLUSUNAIRE, 1, $2); }
       | MINUS expr %prec unaire  { $$=makeTree(MINUSUNAIRE, 1, $2); }
       | expr CONCAT expr   { $$=makeTree(CONCATENATION, 2, $1, $3); }
       | expr PLUS expr     { $$=makeTree(PLUSBINAIRE, 2, $1, $3); }
       | expr MINUS expr    { $$=makeTree(MINUSBINAIRE, 2, $1, $3); }
       | expr DIV expr      { $$=makeTree(DIVISION, 2, $1, $3); }
       | expr MUL expr      { $$=makeTree(MULTIPLICATION, 2, $1, $3); }
       | expr RELOP expr    { $$=makeTree(OPCOMPARATEUR, 2, $1, $3); }
       | constante      { $$=$1; }
       | instanciation      { $$=$1; }
       | envoiMessage     { $$=$1; }
       | OuRien       { $$=$1; }
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
    	  | CSTE  		{$$ = makeLeafInt(CSTENTIER,$1); }
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

/* les appels ci-dessous creent un arbre de syntaxe abstraite pour l'expression
 * arithmetique. On rappelle que la methode est ascendante, donc les arbres
 * des operandes sont deja construits au moment de rajouter le noeud courant.
 * Dans la premiere regle, par exemple, $2, $4 et $6 representent donc
 * les arbres qui sont les composants d'un if-then-else.
 * la fonction makeTree est definie dans tp.c et prend un nombre variables
 * d'arguments (au moins 2). Le premier est l'etiquette du noeud a construire,
 * le second est le nombre de fils.
 */

/*
 * TODO : On n'a pas envie d'écrire au niveau des expression <= > < >= etc ..
 * du coup RELOP est une notion qui nous permet de les généraliser dans une seul entité
 * Ici même on récuper grace à lex l'opérateur mis en jeu ! => go en bas du tp.l
 */
