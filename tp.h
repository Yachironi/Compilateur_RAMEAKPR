#ifndef __TP__
#define __TP__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern char *strdup(const char *s);

/* deux macros pratiques, utilisees dans les allocations */
#define NEW(howmany, type) (type *) calloc((unsigned) howmany, sizeof(type))
#define NIL(type) (type *) 0

#define TRUE 1
#define FALSE 0 

#define CATEGORIE_STATIC 1

/* Etiquettes additionnelles pour les arbres de syntaxe abstraite.
 * Les tokens tels que PLUS, MINUS, etc. servent directement d'etiquette.
 * Attention donc a ne pas donner des valeurs identiques a celles des tokens
 * produits par Bison dans le fichier tp_y.h
 */
#define NE 1
#define EQ 2
#define LT 3
#define LE 4
#define GT 5
#define GE 6
#define IDENTIFICATEUR 7
#define PLUSUNAIRE 8
#define MINUSUNAIRE 9
#define CONCATENATION 10
#define PLUSBINAIRE 11
#define MINUSBINAIRE 12
#define DIVISION 13
#define MULTIPLICATION 14
#define OPCOMPARATEUR 15
#define SELECTION 16
#define CONSTANTE 17
#define EXPRESSION 18
#define INSTANCIATION 19
#define ENVOIMESSAGE 20
#define EXPRESSIONRETURN 21
#define IDENTIFICATEURCLASS 22
#define CSTSTRING 23
#define CSTENTIER 24
#define RETURN_VOID 25
#define EXTENTION 26
#define PARAM 27
#define STATIQUE 28
#define SURCHARGE 29
#define DEFTOKEN 30
#define LISTEMETHODE 31
#define PROGRAM 32
#define LISTCLASS 33
#define CONTENUBLOC 34
#define ETIQUETTE_IS 35
#define ETIQUETTE_YIELD 36
#define ETIQUETTE_AFFECT 37
#define IFTHENELSE 38
#define CONTENUCLASS 39
#define LISTEARG 40
#define LISTEPARAM 41
#define LIST_INSTRUCTION 42
#define LISTEVAR 43
#define OPERATEUR 44
#define EVALUE_STR 45
#define EVALUE_INT 46
#define EVALUE_PVAR 47
#define EVALUE_PCLASS 48
#define EVALUE_PMETH 49
#define EVALUE_TREEP 50

#define MSG_VOID "void"

/* Valeur de l'integer type dans une structure Eval/EvalP */
#define EVAL_STR 0
#define EVAL_INT 1
#define EVAL_PVAR 2
#define EVAL_PCLASS 3
#define EVAL_PMETH 4
#define EVAL_TREEP 5

/* Codes d'erreurs */
#define NO_ERROR	0
#define USAGE_ERROR	1
#define LEXICAL_ERROR	2
#define SYNTAX_ERROR    3
#define CONTEXT_ERROR	4
#define EVAL_ERROR	5
#define UNEXPECTED	10

#define SIZE_ERROR 200
#define RED "\033[31m" /* Red */
#define BLACK "\033[30m" /* Black */

typedef struct _Var SVAR, *PVAR;
typedef struct _Method SMETH, *PMETH;
typedef struct _Class SCLASS, *PCLASS;
typedef struct _LClass LCLASS, *PLCLASS;

typedef int bool;

/* la structure d'un arbre (noeud ou feuille) */
typedef struct _Tree {
  short op;         		/* etiquette de l'operateur courant */
  short nbChildren; 		/* nombre de sous-arbres */
  union {
    char *str;     		/* valeur de la feuille si op = ID ou STR */
    int val;        		/* valeur de la feuille si op = CST */
    PVAR var;	    		/* valeur de la feuille si op = VAR */
    PCLASS classe;	   	/* valeur de la feuille si op = IDENTIFICATEURCLASS */
    PMETH methode;        	/* valeur de la feuille si op = METHODE */
    struct _Tree **children; 	/* tableau des sous-arbres */
  } u;
  /*Utilise pour les check et uniquement les checks*/
  struct _Tree *suivant;
  int isEnvoiMessage;
  int recupType;
} Tree, *TreeP;

/* Structure d'une classe */
struct _Class{
  char *nom;            	/* nom de la classe */
  PVAR param_constructeur;    	/*  paramètres du constructeur de la classe */
  TreeP corps_constructeur;   	/* corps du constructeur de la classe sous la forme d'un arbre (d'expression) */
  PMETH liste_methodes;     	/* liste des méthodes de la classe */

  PVAR liste_champs;        	/* liste des champs de la classe */ 

  PCLASS classe_mere;       	/* classe mère éventuelle de la classe */
  int isExtend;             	/* 1 si la classe est une classe fille, 0 sinon */
  PCLASS suivant;		/* suivant permettant de faire une liste */

  TreeP appel_constructeur_mere; /*uniquement pour les checks*/
};

PCLASS listeDeClass; 	/* Liste de toutes les classe declarees dans le programme */

/* Structure d'une méthode */
struct _Method{
  char *nom;
  int isStatic; /* 1 si vrai, 0 si non */
  int isRedef;  /* 1 si vrai, 0 si non */
  TreeP corps;
  PCLASS typeRetour;
  PVAR params;
  PMETH suivant;
  PCLASS home;
};

/* Structure d'une variable (pouvant être un paramètre, un champ,... exemple : "int x") */
 struct _Var{
  char *nom;
  PCLASS type;
  int categorie;	/* si categorie = 1 ==> static, si categorie = 0 ==> non static */
  TreeP init;		/* initialisation de la variable */
  PVAR suivant;
};

typedef struct _Erreur
{
  char* message;
  SCLASS classe; /*La classe en question*/
  SMETH methode; /*La methode en question si le probleme vient de la */
  SVAR variable; /*La variable en jeu*/
  int ligne;
  struct _Erreur *suivant;
} Erreur, *ErreurP;

ErreurP listeErreur;

typedef struct _Eval
{
	int type;
	char* nom;
	union {
		char *str;	/* type = EVAL_STR */
		int val;      	/* type = EVAL_INT */
		PVAR var;	/* type = EVAL_VAR */
	 	PCLASS classe;	/* type = EVAL_PCLASS */
		PMETH methode;  /* type = EVAL_PMETH */
		TreeP tree;	/* type = EVAL_TREEP */
  	} u;
} Eval, *EvalP;

typedef struct _ListEval{
	EvalP eval;
	struct _ListEval *suivant;
}LEval, *LEvalP;

EvalP makeEvalStr(char *str);
EvalP makeEvalInt(int val);
EvalP makeEvalVar(PVAR var);
EvalP makeEvalClasse(PCLASS classe);
EvalP makeEvalMethode(PMETH methode);
EvalP makeEvalTree(TreeP tree);

/* Type pour la valeur de retour de Flex et les actions de Bison
 * le premier champ est necessaire pour flex
 * les autres correspondent aux variantes utilisees dans les actions
 * associees aux productions de la gra
mmaire. 
*/
typedef union
{ char C;
  char *S;
  int I; 
  TreeP T;
  PVAR V;
  PCLASS CL;
  PMETH M;
} YYSTYPE;

/* Structure representant une liste de tree, utile pour la vérification
* des selections et des envois de message
*/
typedef struct _listeTree {
  TreeP elem;
  struct _listeTree *suivant;
}listeTree, *LTreeP;

#define YYSTYPE YYSTYPE


/* construction pour les arbres */
TreeP makeLeafStr(short op, char *str);
TreeP makeLeafInt(short op, int val);
TreeP makeLeafVar(short op, PVAR var);
TreeP makeLeafClass(short op, PCLASS classe);
TreeP makeLeafMeth(short op, PMETH methode);
TreeP makeTree(short op, int nbChildren, ...);

/* evaluateur d'expressions */
TreeP getChild(TreeP tree, int rank);

/* ecriture formatee */
void pprintTreeMain(TreeP tree);
void printVar(PVAR var);
void printClasse(PCLASS classe);
void printMethode(PMETH methode);

/* methode rajoute */
PCLASS makeDefClasse(char *nom);
PCLASS makeClasseApresDef(PCLASS classe, PVAR param_constructeur,TreeP corps_constructeur,PMETH liste_methodes,PVAR liste_champs, PCLASS classe_mere, int isExtend);
PCLASS makeClasse(char *nom, PVAR param_constructeur,TreeP corps_constructeur,PMETH liste_methodes,PVAR liste_champs, PCLASS classe_mere, int isExtend);
PMETH makeMethode(char *nom, int OverrideOuStaticOpt,TreeP corps,PCLASS typeRetour,PVAR params, PCLASS home);
PVAR makeListVar(char *nom,PCLASS type,int cat,TreeP init);
PCLASS getClasse(PCLASS listeClass,char *nom);
bool estDansListClasse(PCLASS listeClasse, char *nom);
bool methodeDansClasse(PCLASS classe, PMETH methode);
bool memeVar(PVAR var1, PVAR var2);
PCLASS getClasseBis(PCLASS listeClass,char *nom);
PMETH getMethode(PCLASS classe, PMETH methode);
int getVal(EvalP eval);
PVAR copyVar(PVAR var);
PMETH getMethodeBis(PMETH meth, char *nom);
bool varEstDansListe(PVAR listeVar, char *nom);
PVAR getVar(PVAR var, char* nom);
int sizeString(char *str);
int sizeInt(int val);


/*
 * Methode check
 */
bool checkAppelMethode(TreeP listOptArg,PVAR paramMeth, int isAppelConstructeurMere);
bool checkClass(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl);
bool checkHeritage(PCLASS classe);
bool classExtendsDeclareeAvant(PCLASS actuelle,PCLASS heritee);
bool checkListAttribut(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl);
bool verifAttributClasse(PCLASS classe);
bool checkListMethode(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl);
bool checkMethode(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl);
bool checkExprEnvoiSelecInst(TreeP p, TreeP droit);
bool classeContient(PCLASS classe,TreeP droite);

/** Methode eval **/
EvalP evalProgramme(TreeP programme);
EvalP evalContenuBloc(TreeP bloc, PVAR environnement);
void evalListDeclVar(PVAR listDeclVar, PVAR environnement);
void evalListInstruction(TreeP Linstruction, PVAR environnement);
EvalP evalInstruction(TreeP instruction, PVAR environnement);
EvalP evalExpr(TreeP tree, PVAR environnement);
EvalP evalSelection(TreeP tree, PVAR environnement);
EvalP evalEnvoiMessage(TreeP tree, PVAR environnement);
EvalP evalInstanciation(TreeP tree, PVAR environnement);
LEvalP evalListArg(TreeP tree, PVAR environnement);
EvalP evalIf(TreeP tree, PVAR environnement);
void updateEnvironnement(PVAR environnement, PVAR env2);

/* Ajout d'une erreur */
void pushErreur(char* message,PCLASS classe,PMETH methode,PVAR variable);

/*
 * Nouvelle fonction
 */
PCLASS getType(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl);
bool equalsType(PCLASS gauche, PCLASS droit);
PCLASS estCoherentEnvoi(LTreeP liste, PCLASS classe, PMETH methode, PVAR listeDecl);
LTreeP transFormSelectOuEnvoi(TreeP arbre, LTreeP liste);
PCLASS getTypeAttribut(char* nom, PCLASS classe, PMETH methode, PVAR listeDecl, bool isStatic, bool agerer);
PCLASS appartient(PCLASS mere, TreeP fille, bool isEnvoiMessage, PMETH methode, PVAR listeDecl, LTreeP tmp,short etiquette, bool isStatic, bool agerer);
PCLASS transformerAppel(TreeP appelMethode,PCLASS liste,PCLASS classe,PMETH methode, PVAR listeDecl);
PCLASS getTypeMethode(char * nom, PCLASS classe, short precedant, TreeP appelMethode, PMETH methode, PVAR listeDecl, bool isStatic);
bool compareParametreMethode(PVAR declaration,TreeP appelMethode, PCLASS classe,PMETH methode, PVAR listeDecl, char* nom);
LTreeP transformeParam(TreeP arbre, LTreeP liste);
void afficheListeErreur(ErreurP listeE);
bool verifAttributClasse(PCLASS classe);
bool checkBloc(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl);
bool checkListInstruction(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl);
bool checkProgramme(TreeP prog);
bool checkDoublon(char** variable,int n);
bool checkListDeclaration(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl);
PVAR fusionne(PVAR declarationPrecedente,PVAR declarationSousBloc);
PVAR ajouterPVAR(PVAR nouvelle, PVAR liste);
bool isHeritage(PCLASS gauche, PCLASS droite);
 /*
  * A voire
  */
bool contientClasseInst(PVAR classe, TreeP droite);
bool checkListOptArg(PVAR var, PMETH methode);



#endif
