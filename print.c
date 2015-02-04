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

/* Impression : )  de la partie declarative du programme */
void pprintVar(VarDeclP decl, TreeP tree) {
  if (! verbose) return;
  printf("%s := ", decl->name);
  pprint(tree);
  /* utile au cas ou l'evaluation se passerait mal ! */
  fflush(NULL);
}


void pprintValueVar(VarDeclP decl) {
  if (! verbose) return;
  if (! noEval) {
    printf(" [Valeur: %d]\n", decl->val);
  } else printf("\n");
}


/* Affichage de la structure d'une expression */

/* Affichage completement parenthese d'une expression binaire */
void pprintTree2(TreeP tree, char *op) {
  printf("(");
  pprint(getChild(tree, 0));
  printf("%s", op);
  pprint(getChild(tree, 1));
  printf(")");
}

/* Affichage completement parenthese d'une expression unaire */
void pprintTree1(TreeP tree, char *op) {
  printf("(%s(", op);
  pprint(getChild(tree, 0));
  printf("))");
}

/* Affichage completement parenthese d'un if then else */
void pprintIf(TreeP tree) {
  printf("(if ");
  pprint(getChild(tree, 0));
  printf(" then ");
  pprint(getChild(tree, 1));
  printf(" else ");
  pprint(getChild(tree, 2));
  printf(")");
}


/* Affichage recursif d'un arbre representant une expression. */
void pprint(TreeP tree) {
  if (! verbose ) return;
  if (tree == NIL(Tree)) { 
    printf("Unknown"); return;
  }
  switch (tree->op) {
  case ID:    printf("%s", tree->u.str); break;
  case CST:   printf("%d", tree->u.val); break;
  case EQ:    pprintTree2(tree, " = "); break;
  case NE:    pprintTree2(tree, " <> "); break;
  case PLUS:   pprintTree2(tree, " + "); break;
  case MINUS:   pprintTree2(tree, " - "); break;
  case IF:    pprintIf(tree); break;
  default:
    /* On signale le probleme mais on ne quitte pas le programme pour autant */
    fprintf(stderr, "Erreur! pprint : etiquette d'operator inconnue: %d\n", 
	    tree->op);
    setError(UNEXPECTED);
  }
}

void pprintMain(TreeP tree) {
  if (! verbose) return;
  printf("Expression principale : ");
  pprint(tree);
  fflush(NULL);
}

/** methodes rajoutees par julien **/

/** permet d'afficher un arbre à n fils **/
void pprintTreeN(TreeP tree, char *op, int nbChild) {
	int i;
	printf("(TreeP(%d fils) : op=%s, childs=", nbChild, op);
	for(i=0; i<nbChild; i++){
		printf("child[%d]=", i+1);
		pprint(getChild(tree, i));
		if(i<nbChild-1){
			printf("-");
		}
	}
	printf(")");
}

/** affiche la liste des methodes **/
void pprintListMethode(PMETH meth){
	pprintMethode(meth);
	printf("->");
	if(meth->suivant != NIL(SMETH)){
		pprintListMethode(meth->suivant);
	}
	else{
		printf("suivant=NIL\n");
	}
}

/** affiche une methode **/
void pprintMethode(PMETH meth){
	printf("methode=(nom=%s, isStatic=%d, isRedef=%d, corps=", meth->nom, meth->isStatic, meth->isRedef);
	pprint(meth->corps);
	printf(", typeRetour=");
	pprintClasse(meth->typeRetour);
	printf(", params=");
	pprintVAR(meth->params);
	printf(", home=");
	pprintClasse(meth->home);
	printf(")");
	
}

/** affiche une liste de classe **/
void pprintListClasse(PCLASS classe){
	pprintClasse(classe);
	printf("->");
	if(classe->suivant != NIL(SCLASS)){
		pprintListClasse(classe->suivant);
	}
	else{
		printf("suivant=NIL\n");
	}
}

/** affiche une classe **/
void pprintClasse(PCLASS classe){
	printf("classe=(nom=%s, param_constructeur=", classe->nom);
	pprintVAR(classe->param_constructeur);
	printf(", corps_constructeur=");
	pprint(classe->corps_constructeur);
	printf(", liste_methodes=");
	pprintListMethode(classe->liste_methodes);
	printf(", liste_champs=");
	pprintListVAR(classe->liste_champs);
	printf(", classe_mere=");
	pprintClasse(classe->classe_mere);
	printf(", isExtend=%d)", classe->isExtend);
}

/** affiche une liste de var (param, ...) **/
void pprintListVAR(PVAR var){
	pprintVAR(var);
	printf("->");
	if(var->suivant != NIL(SVAR)){
		pprintListVAR(var->suivant);
	}
	else{
		printf("suivant=NIL\n");
	}

}

/** affiche un var (param, ...) **/
void pprintVAR(PVAR var){
	printf("VAR=(nom=%s, type=", var->nom);
	pprintClasse(var->type);
	printf(", categorie=%d, init=", var->categorie);
	pprint(var->init);
	printf(")");
}



