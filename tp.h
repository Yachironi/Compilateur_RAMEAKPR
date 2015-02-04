#ifndef __TP__
#define __TP__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* deux macros pratiques, utilisees dans les allocations */
#define NEW(howmany, type) (type *) calloc((unsigned) howmany, sizeof(type))
#define NIL(type) (type *) 0

#define TRUE 1
#define FALSE 0 

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

#define MSG_VOID "void"

/* Codes d'erreurs */
#define NO_ERROR	0
#define USAGE_ERROR	1
#define LEXICAL_ERROR	2
#define SYNTAX_ERROR    3
#define CONTEXT_ERROR	4
#define EVAL_ERROR	5
#define UNEXPECTED	10

#define SIZE_ERROR 100

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
} Tree, *TreeP;

/* la structure ci-dessous permet de memoriser des listes variable/valeur */
typedef struct _Decl
{ char *name;
  int val;
  struct _Decl *next;
} VarDecl, *VarDeclP;


/* Structure d'une classe */
struct _Class{
  char *nom;            	/* nom de la classe */
  PVAR param_constructeur;    	/*  paramètres du constructeur de la classe */
  TreeP corps_constructeur;   	/* corps du constructeur de la classe sous la forme d'un arbre (d'expression) */
  PMETH liste_methodes;     	/* liste des méthodes de la classe */
  PMETH override;             /* liste de méthodes override */			/** A GERERRRRR **/
  PVAR liste_champs;        	/* liste des champs de la classe */ 
  PVAR champs_herite;         /* atrributs heritees */				/** A GERERRRRR **/
  PCLASS classe_mere;       	/* classe mère éventuelle de la classe */
  int isExtend;             	/* 1 si la classe est une classe fille, 0 sinon */
  PCLASS suivant;		/* suivant permettant de faire une liste */
};

PCLASS listeDeClass; 	/* Liste de toutes les classe declarees dans le programme */

/**
  J'ai noté qqch, mais me souviens plus trop de ce que ça voulait dire 
  --> "représentation des arguments des constructeurs de la super classe => type : ??"
**/

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
  /* ... : j'ai noté ça les 3 points, vous l'avez aussi? */
};

/*
Je crois qu'il faut faire une structure pour catégorie (dans VAR) avec :
  champ static -> 1
  champ non static -> 2
  param méthode -> 3
  variable locale à un bloc -> 4
*/

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

/**/


/* Type pour la valeur de retour de Flex et les actions de Bison
 * le premier champ est necessaire pour flex
 * les autres correspondent aux variantes utilisees dans les actions
 * associees aux productions de la grammaire. 
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

#define YYSTYPE YYSTYPE

/* construction des declarations */
VarDeclP makeVar(char *name);
VarDeclP declVar(char *name, TreeP tree, VarDeclP currentScope);

/* construction pour les arbres */
TreeP makeLeafStr(short op, char *str);
TreeP makeLeafInt(short op, int val);
TreeP makeLeafVar(short op, PVAR var);
TreeP makeLeafClass(short op, PCLASS classe);
TreeP makeLeafMeth(short op, PMETH methode);
TreeP makeTree(short op, int nbChildren, ...);

/* evaluateur d'expressions */
int evalMain(TreeP tree, VarDeclP lvar);
VarDeclP evalDecls (TreeP tree);

/* ecriture formatee */
void pprintVar(VarDeclP decl, TreeP tree);
void pprintValueVar(VarDeclP decl);
void pprint(TreeP tree);
void pprintMain(TreeP tree);
void pprintTreeN(TreeP tree, char *op, int nbChild);
void pprintListMethode(PMETH meth);
void pprintMethode(PMETH meth);
void pprintListClasse(PCLASS classe);
void pprintClasse(PCLASS classe);
void pprintListVAR(PVAR var);
void pprintVAR(PVAR var);

/* methode rajoute */
PCLASS makeClasse(PCLASS listeClass,char *nom,PVAR param_constructeur,TreeP corps_constructeur,PMETH liste_methodes,PVAR liste_champs, PCLASS classe_mere, int isExtend);
PMETH makeMethode(char *nom, int OverrideOuStaticOpt,TreeP corps,PCLASS typeRetour,PVAR params, PCLASS home);
PVAR makeListVar(char *nom,PCLASS type,int cat,TreeP init);
PCLASS getClasse(PCLASS listeClass,char *nom);

bool checkAppelMethode(TreeP listOptArg,PVAR paramMeth, int isAppelConstructeurMere);

/*
 * Methode check
 */
bool checkLClassOpt();
bool checkClass(PCLASS classe);
bool checkHeritage(PCLASS classe);
bool classExtendsDeclareeAvant(PCLASS actuelle,PCLASS heritee);
bool checkConstructeur(PCLASS classe);
bool checkListAttribut(PCLASS classe);
bool checkListMethode(PCLASS classe);
bool checkMethodeStatic(PMETH methode);


void printTree(TreeP tree); /* Methode pour imprimer toute l'arbre */
void evalProgramme(TreeP programme);
void evalContenuBloc(TreeP bloc);
PVAR evalListDeclVar(TreeP listDeclVar);
void pushErreur(char* message,PCLASS classe,PMETH methode,PVAR variable);

bool checkExprEnvoiSelecInst(TreeP p, TreeP droit);
bool estCoherent(TreeP gauche, TreeP droite);
bool classeContient(PCLASS classe,TreeP droite);

#endif
