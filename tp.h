#ifndef __TP__
#define __TP__

#include <stdlib.h>

/* deux macros pratiques, utilisees dans les allocations */
#define NEW(howmany, type) (type *) calloc((unsigned) howmany, sizeof(type))
#define NIL(type) (type *) 0

#define TRUE 1
#define FALSE 0 

typedef struct _struct_var;
struct _struct_method;
struct _struct_class;

typedef int bool;

/* la structure d'un arbre (noeud ou feuille) */
typedef struct _Tree {
  short op;         /* etiquette de l'operateur courant */
  short nbChildren; /* nombre de sous-arbres */
  union {
    char *str;      /* valeur de la feuille si op = ID ou STR */
    int val;        /* valeur de la feuille si op = CST */
    struct _Tree **children; /* tableau des sous-arbres */
  } u;
} Tree, *TreeP;


/* la structure ci-dessous permet de memoriser des listes variable/valeur */
typedef struct _Decl
{ char *name;
  int val;
  struct _Decl *next;
} VarDecl, *VarDeclP;


/* AJOUT DU Struct.h*/

/* Structure d'une classe */
struct _struct_class{
  char *nom;            // nom de la classe
  PVAR param_constructeur;    // paramètres du constructeur de la classe
  TreeP corps_constructeur;   // corps du constructeur de la classe sous la forme d'un arbre (d'expression)
  _struct_method *liste_methodes;     // liste des méthodes de la classe
  _struct_var *liste_champs;        // liste des champs de la classe
  PCLASS classe_mere;       // classe mère éventuelle de la classe
} SCLASS, *PCLASS;

/**
  J'ai noté qqch, mais me souviens plus trop de ce que ça voulait dire 
  --> "représentation des arguments des constructeurs de la super classe => type : ??"
**/

// Structure d'une méthode
struct _struct_method{
  char *nom;
  int isStatic; //1 si vrai, 0 si non
  int isRedef;  //1 si vrai, 0 si non
  TreeP corps;
  PCLASS typeRetour;
  PVAR params;
  PMETH suivant;
  PCLASS home;
} SMETH, *PMETH;

// Structure d'une variable (pouvant être un paramètre, un champ,... exemple : "int x")
 struct _struct_var{
  char *nom;
  PCLASS type;
  int categorie;
  TreeP init;
  struct _struct_var *suivant; // on peut pas mettre directement PVAR?
  // ... : j'ai noté ça les 3 points, vous l'avez aussi?
} SVAR, *PVAR;

/*
Je crois qu'il faut faire une structure pour catégorie (dans VAR) avec :
  champ static -> 1
  champ non static -> 2
  param méthode -> 3
  variable locale à un bloc -> 4
*/

/**/


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


#define LIST 8

/* Codes d'erreurs */
#define NO_ERROR	0
#define USAGE_ERROR	1
#define LEXICAL_ERROR	2
#define SYNTAX_ERROR    3
#define CONTEXT_ERROR	4
#define EVAL_ERROR	5
#define UNEXPECTED	10


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
} YYSTYPE;

#define YYSTYPE YYSTYPE

/*
	TODO :
	- Créer la méthode makeMethode
	- Créer la méthode makeMethode
*/


/* construction des declarations */
VarDeclP makeVar(char *name);
VarDeclP declVar(char *name, TreeP tree, VarDeclP currentScope);

/* construction pour les arbres */
TreeP makeLeafStr(short op, char *str);
TreeP makeLeafInt(short op, int val);
TreeP makeTree(short op, int nbChildren, ...);

/* evaluateur d'expressions */
int evalMain(TreeP tree, VarDeclP lvar);
VarDeclP evalDecls (TreeP tree);

/* ecriture formatee */
void pprintVar(VarDeclP decl, TreeP tree);
void pprintValueVar(VarDeclP decl);
void pprint(TreeP tree);
void pprintMain(TreeP);

// methode rajoute
PCLASS makeClasse(char *nom,PVAR param_constructeur,TreeP corps_constructeur,PMETH *liste_methodes,PVAR *liste_champs, PCLASS classe_mere);
PMETH makeMethode(char *nom, int OverrideOuStaticOpt,TreeP corps,PCLASS typeRetour,PVAR params);

#endif
