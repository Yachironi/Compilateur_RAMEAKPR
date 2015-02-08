#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "tp.h"
#include "tp_y.h"

/*
 * FIXME : ajouter une methode makeErreur(...)
 */

/*
 * Toute cette premiere partie n'a (normalement) pas besoin d'etre modifiee
 */

extern int yyparse();
extern int yylineno;
extern TreeP programme;
extern PCLASS classActuel;

int eval(TreeP tree, VarDeclP decls);
TreeP getChild(TreeP tree, int rank);

/* Niveau de 'verbosite'.
 * Par defaut, n'imprime que le resultat et les messages d'erreur
 */
bool verbose = FALSE;

/* Evaluation ou pas. Par defaut, on evalue les expressions */
bool noEval = FALSE;

/* code d'erreur a retourner */
int errorCode = NO_ERROR;

FILE *fd = NIL(FILE);

/* Appel:
 *   tp [-option]* programme.txt
 * Les options doivent apparaitre avant le nom du fichier du programme.
 * Options: -[eE] -[vV] -[hH?]
 */

int main(int argc, char **argv) {
	listeDeClass = NULL;
	/* Ajout des classes predefinies : Integer, String et Void 
	 *	Remarque -> les constructeurs (et ses parametres) + liste de methode + liste de champ + classe mere sont à NULL
	 */

	PCLASS Integer = makeClasse("Integer", NULL, NULL, NULL, NULL, NULL, 0);
	PCLASS String = makeClasse("String", NULL, NULL, NULL, NULL, NULL, 0);
	PCLASS Void = makeClasse("Void", NULL, NULL, NULL, NULL, NULL, 0);
	Void->suivant = NULL;
	String->suivant = Void;
	Integer->suivant = String;
	listeDeClass = Integer;

	int fi;
	int i, res;
	if (argc == 1) {
		fprintf(stderr, "Syntax: tp -e -v program.txt\n");
		exit(USAGE_ERROR);
	}
	for(i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'v': case 'V':
					verbose = TRUE; continue;
				case 'e': case 'E':
					noEval = TRUE; continue;
				case '?': case 'h': case 'H':
					fprintf(stderr, "Syntax: tp -e -v program.txt\n");
					exit(USAGE_ERROR);
				default:
					fprintf(stderr, "Error: Unknown Option: %c\n", argv[i][1]);
					exit(USAGE_ERROR);
     			}
    		} else break;
  	}

	if (i == argc) {
 		fprintf(stderr, "Error: Program file is missing\n");
		exit(USAGE_ERROR);
	}

	if ((fi = open(argv[i++], O_RDONLY)) == -1) {
		fprintf(stderr, "Error: Cannot open %s\n", argv[i-1]);
		exit(USAGE_ERROR);
	}

	/* redirige l'entree standard sur le fichier... */
	close(0); dup(fi); close(fi);

  /* Lance l'analyse syntaxique de tout le source, en appelant yylex au fur
   * et a mesure. Execute les actions semantiques en parallele avec les
   * reductions.
   * yyparse renvoie 0 si le source est syntaxiquement correct, une valeur
   * differente de 0 en cas d'erreur syntaxique (eventuellement dues a des
   * erreurs lexicales).
   * Comme l'interpretation globale est automatiquement lancee par les actions
   * associees aux reductions, une fois que yyparse a termine il n'y
   * a plus rien a faire (sauf fermer les fichiers)
   * Si le code du programme contient une erreur, on bloque l'evaluation.
   * S'il ny a que des erreurs contextuelles on essaye de ne pas s'arreter
   * a la premiere mais de continuer l'analyse pour en trovuer d'autres, quand
   * c'est possible.
   */

	printf("tp.c -> avant res\n");
	res = yyparse();
	printf("tp.c -> res=%d\n", res);
	if (programme == NULL) {
		printf("tp.c -> Programme est NULL\n");
	}
	else{
		printf("tp.c -> Programme n'est pas NULL\n");
		printf("=======================\n");
		printf("tp.c -> Affichage de l'arbre : \n");
		pprintTreeMain(programme);
		printf("=======================\n");
		
	}
	if (fd != NIL(FILE)) fclose(fd);
  	if (res == 0 && errorCode == NO_ERROR) return 0;
  	else {
   		int res2 = res ? SYNTAX_ERROR : errorCode;
    		printf("Error in file. Kind of error: %d\n", res2); 
    		return res2;
  	}
}


void setError(int code) {
  errorCode = code;
  if (code != NO_ERROR) { noEval = TRUE; }
}


/* yyerror:  fonction importee par Bison et a fournir explicitement. Elle
 * est appelee quand Bison detecte une erreur syntaxique.
 * Ici on se contente d'un message minimal.
 */
void yyerror(char *ignore) {
  fprintf(stderr, "Syntax error on line: %d\n", yylineno);
}


/* Tronc commun pour la construction d'arbre */
TreeP makeNode(int nbChildren, short op) {
  TreeP tree = NEW(1, Tree);
  tree->op = op;
  tree->nbChildren = nbChildren;
  tree->u.children = nbChildren > 0 ? NEW(nbChildren, TreeP) : NIL(TreeP);
  return(tree);
}


/* Construction d'un arbre a nbChildren branches, passees en parametres */
TreeP makeTree(short op, int nbChildren, ...) {
  va_list args;
  int i;
  TreeP tree = makeNode(nbChildren, op); 
  va_start(args, nbChildren);
  for (i = 0; i < nbChildren; i++) { 
    tree->u.children[i] = va_arg(args, TreeP);
  }
  va_end(args);
  return(tree);
}


/* Retourne le rankieme fils d'un arbre (de 0 a n-1) */
TreeP getChild(TreeP tree, int rank) {
  return tree->u.children[rank];
}


/* Constructeur de feuille dont la valeur est une chaine de caracteres
 * (un identificateur de variable).
 */
TreeP makeLeafStr(short op, char *str) {
  TreeP tree = makeNode(0, op);
  tree->u.str = str;
  return(tree);
}


/* Constructeur de feuille dont la valeur est un entier */
TreeP makeLeafInt(short op, int val) {
  TreeP tree = makeNode(0, op); 
  tree->u.val = val;
  return(tree);
}

/* Constructeur de feuille dont la valeur est un VAR */
TreeP makeLeafVar(short op, PVAR var){
 TreeP tree = makeNode(0, op); 
  tree->u.var = var;
  return(tree);
}
/* Constructeur de feuille dont la valeur est une Classe */
TreeP makeLeafClass(short op, PCLASS classe){
 TreeP tree = makeNode(0, op); 
  tree->u.classe = classe;
  return(tree);
}
/* Constructeur de feuille dont la valeur est un methode */
TreeP makeLeafMeth(short op, PMETH methode){
 TreeP tree = makeNode(0, op); 
  tree->u.methode = methode;
  return(tree);
}

/**/
PCLASS makeDefClasse(char *nom){
	PCLASS res = NEW(1, SCLASS);
	res->nom=nom;
	return res;
}

/** construit une structure classe (pouvant etre une liste de classe) */
PCLASS makeClasseApresDef(PCLASS res,PVAR param_constructeur,TreeP corps_constructeur,PMETH liste_methodes,PVAR liste_champs, PCLASS classe_mere, int isExtend){
	res->param_constructeur=param_constructeur;
	res->corps_constructeur=corps_constructeur;
	res->liste_methodes=liste_methodes;
	res->liste_champs=liste_champs;
	res->classe_mere=classe_mere;	
	res->isExtend=isExtend;
	/* res->suivant=NULL;*/	/* verifier si ça ne pose pas de pb */
	return res;
}

PCLASS makeClasse(char *nom, PVAR param_constructeur,TreeP corps_constructeur,PMETH liste_methodes,PVAR liste_champs, PCLASS classe_mere, int isExtend){
	PCLASS res = NEW(1, SCLASS);
	res->nom=nom;
	res->param_constructeur=param_constructeur;
	res->corps_constructeur=corps_constructeur;
	res->liste_methodes=liste_methodes;
	res->liste_champs=liste_champs;
	res->classe_mere=classe_mere;	
	res->isExtend=isExtend;
	/* res->suivant=NULL;*/	/* verifier si ça ne pose pas de pb */
	return res;
}

/* Renvoi la classe avec un nom donnée */
PCLASS getClasse(PCLASS listeClass,char *nom){
	PCLASS parcour=listeClass;
	while((parcour!=NULL)&&(strcmp(parcour->nom,nom)!=0)){
		parcour=parcour->suivant;	
	}
	if(parcour == NULL){
		return NULL;
	}
	else{
		/*parcour->suivant = NULL;*/
		return parcour;
	}
}
/** TODO A VERIFIER */
/* Renvoi vrai si la methode est dans la classe, faux sinon */
bool methodeDansClasse(PCLASS classe, PMETH methode){
	PMETH tmp_liste_methodes_classe = classe->liste_methodes;
	PMETH tmp_liste_methode = methode;
	if(methode == NULL)	return FALSE;

	while(tmp_liste_methodes_classe != NULL){

		/* si 2 methodes ont le meme noms, les memes classes de retour (meme noms) et memes param ==> meme methode */
		if(strcmp(tmp_liste_methodes_classe->nom, tmp_liste_methode->nom)==0 && strcmp(tmp_liste_methodes_classe->typeRetour->nom, tmp_liste_methode->typeRetour->nom)==0 && memeVar(tmp_liste_methodes_classe->params, tmp_liste_methode->params)==TRUE ){
			/* je n'ai pas mis isRedef = 1 si il était = a 0 */
			return TRUE;
		}
		tmp_liste_methodes_classe = tmp_liste_methodes_classe->suivant;
	}
	printf("La methode -%s- n'est pas dans la classe -%s-\n", methode->nom, classe->nom);
	return FALSE;
}

/* TODO */
bool memeTreeP(TreeP tree1, TreeP tree2){
	/*
	if(tree1 == NIL(Tree)){
		if(tree2 == NIL(Tree))	return TRUE;
		return FALSE;
	}
	if(tree2 == NIL(Tree))				return FALSE;
	if(tree1->op != tree2->op)			return FALSE;
	if(tree1->nbChildren != tree1->nbChildren)	return FALSE;

	if(tree1->nbChildren>0){
		int i;
		for(i=0; i<tree1->nbChildren; i++){
			if(memeTreeP(tree1->u.children[i], tree2->u.children[i]) == FALSE){
				return FALSE;
			}
		}
	}
	else{ */
		/* tester les autres membres de l'union */
		/*
			  union {
 			   char *str;     		
   			   int val;        		
     			   PVAR var;	    		
    			   PCLASS classe;	   
    			   PMETH methode;     
 			 } u;
		*//*
	}*/
	return TRUE;
}

/* TODO */
bool memeVar(PVAR var1, PVAR var2){
	if(var1 == NIL(SVAR)){
		if(var2 == NIL(SVAR))	return TRUE;
		return FALSE;
	}
	if(var2 == NIL(SVAR))	return FALSE;
	if(strcmp(var1->nom, var2->nom)!=0)	return FALSE;
	if(strcmp(var1->type->nom, var2->type->nom) != 0)	return FALSE; 
	if(var1->categorie != var2->categorie)	return FALSE;	
	return memeVar(var1->suivant, var2->suivant);
}

/* Creation de la structure Methode */
PMETH makeMethode(char *nom, int OverrideOuStaticOpt,TreeP corps,PCLASS typeRetour,PVAR params, PCLASS home){
	PMETH res=NEW(1, SMETH);
	/*res->suivant = NIL(SMETH);*/	/* verifier si ça ne pose pas de pb */
	res->nom=nom;
	res->corps=corps;
	res->params=params;
	res->typeRetour=typeRetour;	/*FIXME faire un vrai copie? Mais pb : typeRetour n'est pas "totalement fini" */
	res->home=home;
	/* ni static, ni override */
	if(OverrideOuStaticOpt == 0){
		res->isStatic = 0;
		res->isRedef = 0;
	}
	/* override mais pas static */
	else if(OverrideOuStaticOpt == 1){
		res->isStatic = 0;
		res->isRedef = 1;
	}
	/* static mais pas override */
	else{
		res->isStatic = 1;
		res->isRedef = 0;
	}
	return res;
}

/* Creer une var pouvant etre un parametre, un champ, .. */
PVAR makeListVar(char *nom,PCLASS type,int cat,TreeP init){
	PVAR res=NEW(1,SVAR);
	/*res->suivant=NIL(SVAR);	verifier si ça ne pose pas de pb */
	res->nom=nom;
	res->type=type;
	res->init=init;
	res->categorie=cat;	/* si cat=0 ==> var non static. si cat=1 ==> var static */
	return res;
}

/*
 * Seconde partie probablement a modifier
 */

/*
 * Creer une erreur et l'ajoute a la pile
 */
void pushErreur(char* message,PCLASS classe,PMETH methode,PVAR variable)
{

  ErreurP nouvelle = NEW(1,Erreur);
  nouvelle->message = NEW(SIZE_ERROR,char);
  strcpy(message,nouvelle->message);
	
  if(classe!=NULL) nouvelle->classe = *classe;		
  if(methode!=NULL) nouvelle->methode = *methode;
  if(variable!=NULL) nouvelle->variable = *variable;
	
  
  if(listeErreur==NULL)
  {
    listeErreur = nouvelle;
  }
  else
  {
    /*FIXME : Liste d'erreur en LIFO : l'insertion est a l'envers la*/
    ErreurP tmp = listeErreur;
    listeErreur = nouvelle;
    nouvelle->suivant = tmp;
  }
}

/* Avant evaluation, verifie si tout id qui apparait dans tree a bien ete declare
 * et est donc dans lvar.
 * On impose que ca soit le cas y compris si on n'a pas besoin a l'evaluation de
 * la valeur de cet id.
 */



/*
 * Methode pour imprimer toute l'arbre
 
 PVAR appelConstructeurEstCorrecteRecursif(TreeP args,PCLASS mere)
{
  TreeP tmp = args;

  return NULL;
}

bool appelConstructeurEstCorrecte(TreeP args,PCLASS mere)
{
  PVAR p = appelConstructeurEstCorrecteRecursif(args,mere);
}


*/





void printTree(TreeP tree){
 if (! verbose ) return;
printf("Etiquette %d",tree->op);
 switch (tree->op) {
  case LISTCLASS:pprintListClasse(tree->u.classe); break;
  default:
    fprintf(stderr, "Erreur! pprint : etiquette d'operator inconnue: %d\n", 
	    tree->op);
    setError(UNEXPECTED);
  }
int i;
for (i = 0; i < tree->nbChildren; i++) { 
    printTree(tree->u.children[i]);
  }
}
