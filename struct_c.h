// Guillaume m'a dit que qqch est faux dans ça, il faut que je lui demande
#ifndef __STRUCT__ 
#define __STRUCT__
#include "tp.h"


struct _struct_var;
struct _struct_method;
struct _struct_class;

// Structure d'une classe
typedef _struct_class{
	char *nom;						// nom de la classe
	PVAR param_constructeur;		// paramètres du constructeur de la classe
	TreeP corps_constructeur;		// corps du constructeur de la classe sous la forme d'un arbre (d'expression)
	_struct_method *liste_methodes;			// liste des méthodes de la classe
	_struct_var *liste_champs;				// liste des champs de la classe
	PCLASS classe_mere;				// classe mère éventuelle de la classe
} CLASS, *PCLASS;

/**
	J'ai noté qqch, mais me souviens plus trop de ce que ça voulait dire 
	--> "représentation des arguments des constructeurs de la super classe => type : ??"
**/

// Structure d'une méthode
typedef _struct_method{
	char *nom;
	bool isStatic;
	bool isRedef;
	TreeP corps;
	PClasse typeRetour;
	PVAR params;
	PMETH suivant;
	PCLASS home;
} METH, *PMETH;

// Structure d'une variable (pouvant être un paramètre, un champ,... exemple : "int x")
typedef _struct_var{
	char *nom;
	PCLASS type;
	int categorie;
	TreeP init;
	_struct_var *suivant;	// on peut pas mettre directement PVAR?
	// ... : j'ai noté ça les 3 points, vous l'avez aussi?
} VAR, *PVAR;

/*
Je crois qu'il faut faire une structure pour catégorie (dans VAR) avec :
	champ static -> 1
	champ non static -> 2
	param méthode -> 3
	variable locale à un bloc -> 4
*/
#endif
