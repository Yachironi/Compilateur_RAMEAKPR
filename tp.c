#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "tp.h"
#include "tp_y.h"

/*
 * Toute cette premiere partie n'a (normalement) pas besoin d'etre modifiee
 */

extern int yyparse();
extern int yylineno;

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
  res = yyparse();
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

/** construit une structure classe (pouvant etre une liste de classe) */
PCLASS makeClasse(PCLASS listeClass,char *nom,PVAR param_constructeur,TreeP corps_constructeur,PMETH liste_methodes,PVAR liste_champs, PCLASS classe_mere, int isExtend){
	PCLASS res = NEW(1, SCLASS);
	res->suivant=NIL(SCLASS);	/* verifier si ça ne pose pas de pb */
	res->nom=nom;
	res->param_constructeur=param_constructeur;
	res->corps_constructeur=corps_constructeur;
	res->liste_methodes=liste_methodes;
	res->liste_champs=liste_champs;
	res->classe_mere=classe_mere;	
	res->isExtend=isExtend;
	if(listeClass==NULL){
		listeClass=res;
	}else{
		PCLASS parcour=listeClass;
		while(parcour->suivant!=NULL){
			parcour=parcour->suivant;	
		}
		parcour->suivant=res;
	}
	return res;
}

/* Renvoi la classe avec un nom donnée */
PCLASS getClasse(PCLASS listeClass,char *nom){
	PCLASS parcour=listeClass;
	while((parcour!=NULL)&&(strcmp(parcour->nom,nom)!=0)){
		parcour=parcour->suivant;	
	}
	return parcour;
}

/* Creation de la structure Methode */
PMETH makeMethode(char *nom, int OverrideOuStaticOpt,TreeP corps,PCLASS typeRetour,PVAR params, PCLASS home){
	PMETH res=NEW(1, SMETH);
	res->suivant = NIL(SMETH);	/* verifier si ça ne pose pas de pb */
	res->nom=nom;
	res->corps=corps;
	res->params=params;
	res->typeRetour=typeRetour;
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
	res->suivant=NIL(SVAR);	/* verifier si ça ne pose pas de pb */
	res->nom=nom;
	res->type=type;
	res->init=init;
	res->categorie=cat;	/* si cat=0 ==> var non static. si cat=1 ==> var static */
	return res;
}

/*
 * Seconde partie probablement a modifier
 */


/* Avant evaluation, verifie si tout id qui apparait dans tree a bien ete declare
 * et est donc dans lvar.
 * On impose que ca soit le cas y compris si on n'a pas besoin a l'evaluation de
 * la valeur de cet id.
 */
bool checkScope(TreeP tree, VarDeclP lvar) {
  
  VarDeclP tmp = lvar;
  if(tree->nbChildren==0)
  {
    bool contains = TRUE;
    while(tmp!=NULL)
    {
      if(strcmp(tmp->name,tree->u.str)!=0)
        return FALSE;
      tmp = tmp->next;
    }
    return contains;
  }

  int i = 0;
  for(i=0;i<tree->nbChildren;i++)
  {
    bool b = checkScope(getChild(tree,i),lvar);
    if(!b)
      return FALSE;
    i++;
  }

  return FALSE;
}

/*
 * Verifier la liste de classe qui peut etre vide
 * True : OK -> s'il n'y a aucune classe la verification a reussi
 * False : KO
*/
bool checkLClassOpt()
{
  int i = 0;
  if(listeDeClass==NULL)
    return TRUE;
  else
  {
    /*return checkClass(getChild(tree,0)) && checkLClassOpt(tree);*/
    PCLASS listTmp = listeDeClass;
    while(listTmp!=NULL)
    {
      /*checkClass : Ne verifie qu'une classe -> methode non recusive*/
      checkClass(listTmp);
      listTmp = listTmp->suivant;
    }
    
  }

  return FALSE;
}

/*
 * Etudie une classe en particulier
 * True : OK
 * False : KO

 struct _Class{
  char *nom;                      nom de la classe
  PVAR param_constructeur;        paramètres du constructeur de la classe
  TreeP corps_constructeur;       corps du constructeur de la classe sous la forme d'un arbre (d'expression)
  PMETH liste_methodes;           liste des méthodes de la classe
  PVAR liste_champs;              liste des champs de la classe 
  PCLASS classe_mere;             classe mère éventuelle de la classe 
  int isExtend;                   si la classe herite ou pas 
  PCLASS suivant;                 suivant permettant de faire une liste 

  Question 1: ou vous avez inserez le constructeur de la classe Mere ?
              Quand on fait un heritage il me semble qu'il appel le constructeur classe mere
              Comme en C++
              J'ai vu que vous l'aviez stocker dans un arbre au niveau de la regle ListExtendsOpt
              Du coup un parcours de l'arbre est obligatoire ? ou tout est dans corps_constructeur ?
};
*/
bool checkClass(PCLASS classe)
{
  /*
   * bool checkHeritage(classe);
   * bool checkListAttribut(SCLASS classe);
   * bool checkListMethode(SCLASS classe);
    ----
   * bool checkCorp(SMETH methode)
    ----
   * bool checkListMethodeStatic(SCLASS classe); // n'utilise pas genre les attribut de classe comme en java
  */

  /* Le nom de la classe doit avoir une majuscule*/
  bool nomMaj = FALSE;

  if(classe->nom!=NULL && (classe->nom[0] >= 'A' && classe->nom[0] <= 'Z'))
    nomMaj = TRUE;

  /*TRUE : OK FALSE : NOK*/
  bool heritage = FALSE;
  /*FIXME : boucle qui verifie que la classe mere n'est pas dans la liste d'erreur !!!
   * Si c'est le cas -> renvoye faux directement => et ajouter une erreur du style
   * "Corrigez la classe Mere %s avant",class->classe_mere->nom (utilisez sprintf)
   */
  if(classe->isExtend)
  {
    heritage = checkHeritage(classe);
  }
  else
  {
    heritage = TRUE;
  }

  bool constructeur = checkConstructeur(classe);

  bool attribut = checkListAttribut(classe);

  bool methode = checkListMethode(classe);

  return (nomMaj && heritage && constructeur && attribut && methode);
}

/*
 * Verifie qu'il n'y a pas de cycle d'héritage
 * VRAI : OK (si la classe n'herite d'aucune classe on considere qu'il n'y a pas de cycle)
 * FAUX : classe mere non declare avant
 */
bool checkHeritage(PCLASS classe)
{
  return classExtendsDeclareeAvant(classe,classe->classe_mere);
}

/*
 * Verifie que la classe heritee est declare avant la classe actuelle
 * Car si on fait class actuelle extends class herite il y a une necessite de 
 * declaration au dessus (de la classe heritee)
 */
bool classExtendsDeclareeAvant(PCLASS actuelle,PCLASS heritee)
{
  /*classe mere inexistante*/
  if(heritee==NULL)
    return TRUE;
  int i = 0;
  PCLASS listTmp = listeDeClass;
  while(listTmp!=NULL && strcmp(actuelle->nom,listTmp->nom)!=0)
  {
    /*On a trouve la classe elle est bien declaree avant*/
    if(strcmp(heritee->nom,listTmp->nom)==0)
    {
      return TRUE;
    }

    listTmp = listTmp->suivant;
  }

  return FALSE;
}

bool checkConstructeur(PCLASS classe)
{
  PVAR tmp = classe->param_constructeur;
}

bool checkListAttribut(PCLASS classe)
{
  PVAR tmp = classe->liste_champs;

  while(tmp!=NULL)
  {
    /*
     * Appel de la fonction de Gishan qui verifier une instruction
     * Integer x; ... et bien d'autres chose
     */
    if(!checkListOptArg(tmp))
      return FALSE;
    /* /!!!\ Ici il s'arete des qu'un attribut est faux*/
    tmp = tmp->suivant;
  }
}

/*
 * 

  struct _Method{
  char *nom;
  int isStatic;  1 si vrai, 0 si non 
  int isRedef;  1 si vrai, 0 si non
  TreeP corps;
  PCLASS typeRetour;
  PVAR params;
  PMETH suivant;
  PCLASS home;
};


 */
bool checkListMethode(PCLASS classe)
{
  PMETH tmp = classe->liste_methodes;

  bool statique = FALSE;
  bool redef = FALSE;

  while(tmp!=NULL)
  {
    printf("Je check la methode %s\n",tmp->nom);

    if(tmp->isStatic)
    {
      checkListMethodeStatic(tmp);
    }
    else
    {
      statique = TRUE;
      if(tmp->isRedef)
      {
        redef = existeMethodeClasseMereOverride(classe->classe_mere,tmp);

        if(!redef)
      }
      else
      {
        redef = TRUE;
      }

      /*Verification d'une methode de classe*/
    }
    /*
     * Verifier les parametre de la methode
     * Verifier le corps de la methode
     * if(method.static) checkListMethodeStatic
     */
    tmp = tmp->suivant;
  }

  return (statique&&redef)
}

bool existeMethodeClasseMereOverride(PCLASS mere,PMETH methode)
{
  PMETH methodeOverride = mere->liste_methodes;

  while(methodeOverride!=NULL)
  {
    if(strcmp(methodeOverride->nom,methode->nom)==0)
    {
      return TRUE;
    }

    methodeOverride = methodeOverride->suivant;
  }
  return FALSE;
}

/* 
 * Verifie qu'une methode statique est bien formee et qu'elle n'utilise pas des attribut
 * de classe (meme principe que le java)
 * On ne peut pas faire this-> qq chose ou this.
 */ 
bool checkListMethodeStatic(PMETH methode)
{
  /*
   * Si la classe Point a un attribut x,y
   * Si dans la methode il fait x = 1; -> erreur
   */
}

bool checkCorp(PMETH methode)
{
  /*checkBlock()*/
}

bool checkListOptArg(PVAR var)
{

}

/* Verifie si besoin que nouv n'apparait pas deja dans list. l'ajoute en
 * tete et renvoie la nouvelle liste
 */
VarDeclP addToScope(VarDeclP list, VarDeclP nouv) {
  
  if(nouv==NULL)
    return list;

  if(list==NULL && nouv!=NULL)
    return nouv;
  
  VarDeclP nouvTmp = nouv;
  bool continuer = TRUE;
  while(nouvTmp!=NULL){
    /* liste mise a jour si besoin */
    VarDeclP listTmp = list;
    continuer = TRUE;
    while(listTmp!= NULL && continuer)
    {
      if(strcmp(nouvTmp->name, listTmp->name)==0){
        continuer = FALSE;
      }
      listTmp = listTmp->next;
    }
    if(!continuer)
    {
      VarDeclP listCopie = list;
      list = nouvTmp;
      list->next = listCopie;
    }
    nouvTmp = nouvTmp->next; 
  }
  return list;
}

/* Construit le squelette d'un couple (variable, valeur), sans la valeur. */
VarDeclP makeVar(char *name) {
  VarDeclP res = NEW(1, VarDecl);
  res->name = name; res->next = NIL(VarDecl);
  return(res);
}


/* Associe une variable a l'expression qui definit sa valeur, et procede a 
 * l'evaluation de cette expression, sauf si on est en mode noEval
 */
VarDeclP declVar(char *name, TreeP tree, VarDeclP currentScope) {
  
  if(tree->nbChildren==0)
  {
    if(strcmp(tree->u.str,name)==0)
    {
      strcpy(currentScope->name,name);
      currentScope->val = tree->u.val;
      return currentScope;
    }
    else
    {
      return NULL;
    }
  }

  int i = 0;
  for(i=0;i<tree->nbChildren;i++)
  {
    VarDeclP retour = declVar(name,getChild(tree,i),currentScope);
    if(retour!=NULL)
      return retour;
    i++;
  }

  return NULL;
}

/* Evaluation d'une variable */
int evalVar(TreeP tree, VarDeclP decls) {
  return 0;
}

/* Evaluation d'un if then else. Attention a n'evaluer que la partie necessaire ! */
int evalIf(TreeP tree, VarDeclP decls) {
  return 0;
}

VarDeclP evalAff (TreeP tree, VarDeclP decls) {
  return NIL(VarDecl);
}

/* Ici decls correspond au sous-arbre qui est la partie declarative */
VarDeclP evalDecls (TreeP tree) {
  return NIL(VarDecl);
}


/* Evaluation par parcours recursif de l'arbre representant une expression. 
 * Les valeurs des identificateurs situes aux feuilles de l'arbre sont a
 * rechercher dans la liste decls
 * Attention a n'evaluer que ce qui doit l'etre et au bon moment
 * selon la semantique de l'operateur (cas du IF, etc.)
 */
int eval(TreeP tree, VarDeclP decls) {
  if (tree == NIL(Tree)) { exit(UNEXPECTED); }
  switch (tree->op) {
  case ID:
    return evalVar(tree, decls);
  case CST:
    return(tree->u.val);
  case EQ:
    return (eval(getChild(tree, 0), decls) == eval(getChild(tree, 1), decls));
  case NE:
    return (eval(getChild(tree, 0), decls) != eval(getChild(tree, 1), decls));

  /** Distinguer les opérateurs unaires et binaires **/
  case PLUS:
	/* si eval(getChild(tree, 0), decls) == NULL alors unaire?*/
    return (eval(getChild(tree, 0), decls) + eval(getChild(tree, 1), decls));
  case MINUS:
	/* si eval(getChild(tree, 0), decls) == NULL alors unaire?*/
    return (eval(getChild(tree, 0), decls) - eval(getChild(tree, 1), decls));
  /** **/

  case MUL:
    return (eval(getChild(tree, 0), decls) * eval(getChild(tree, 1), decls));
  case DIV:
    if(eval(getChild(tree, 1), decls)!=0)
    {
      return (eval(getChild(tree, 0), decls) / eval(getChild(tree, 1), decls));
    }
    else
    {
      return EVAL_ERROR;
    }
  case IF:
    return evalIf(tree, decls);
  default: 
    fprintf(stderr, "Erreur! etiquette indefinie: %d\n", tree->op);
    exit(UNEXPECTED);
  }
}

int evalMain(TreeP tree, VarDeclP lvar) {
  int res;
  if (noEval) {
    fprintf(stderr, "\nSkipping evaluation step.\n");
  } else {
      res = eval(tree, lvar);
      printf("\n/*Result: %d*/\n", res);
  }
  return errorCode;
}

PVAR appelConstructureEstCorrecteRecursif(TreeP args,PCLASS mere)
{
  TreeP tmp = args;

  return NULL;
}

bool appelConstructureEstCorrecte(TreeP args,PCLASS mere)
{
  PVAR p = appelConstructureEstCorrecteRecursif(args,mere);
}


/** Partie eval **/
void evalMain(TreeP programme){
	/* on a l'attribut listeDeClass qui contient toutes les classes (s'il y en a) --> pas besoin de regarder ListClassOpt */
	evalContenuBloc(programme->children[1]/*->children[0]*/);
}

void evalContenuBloc(TreeP bloc){
	/* on est dans la regle : ContenuBloc : LInstructionOpt YieldOpt */
	if(bloc->children[0] == NIL(Tree)){
		if(bloc->children[1] != NIL(Tree)){
			/* eval de LInstruction */
		}
		if(bloc->children[2] != NIL(Tree)){
			/* eval de Yield => expr*/
		}
	}
	/* on est dans la regle : ContenuBloc : ListDeclVar IS LInstruction YieldOpt */
	else{
		/* eval de ListDeclVar */
		PVAR listDeclVar = evalListDeclVar(bloc->children[0]);

		/* eval de LInstruction */
		if(bloc->children[1] != NIL(Tree)){

		}
		/* eval de YieldOpt */
		if(bloc->children[2] != NIL(Tree)){

		}		
	}
}

PVAR evalListDeclVar(TreeP listDeclVar){
	/** ListDeclVar : VAR StaticOpt ID ':' IDCLASS AffectExprOpt ';' LDeclChampsOpt ==> renvoi PVAR */
	/* listDeclVar->var = 1ere var et toute la liste*/

	/*
	PVAR var = listDeclVar->var;
	return var;
	*/
}


