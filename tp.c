#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "tp.h"
#include "tp_y.h"

bool dansCheckBlocMain = FALSE;

extern int yyparse();
extern int yylineno;
extern TreeP programme;
extern PCLASS classActuel;


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
   * + ajout des methodes predefinies toString (-> integer) et println & print (-> string)
   */

  /* Creation des classes predefinies */
  PCLASS Integer = makeClasse("Integer", NULL, NULL, NULL, NULL, NULL, 0);
  PCLASS String = makeClasse("String", NULL, NULL, NULL, NULL, NULL, 0);
  PCLASS Void = makeClasse("Void", NULL, NULL, NULL, NULL, NULL, 0);

  /* Creation des methodes predefinies */
  PMETH toString = makeMethode("toString", 0, NIL(Tree), String, NIL(SVAR), Integer);
  PMETH println = makeMethode("println", 0, NIL(Tree), String, NIL(SVAR), String);
  PMETH print = makeMethode("print", 0, NIL(Tree), String, NIL(SVAR), String);
  toString->suivant = NULL;
  print->suivant = NULL;
  println->suivant = print;

  /* Ajout des methodes dans leurs classes respectives */
  Integer->liste_methodes = toString;
  String->liste_methodes = println;

  /* Ajout de ces classes predefinies dans la liste de classe */
  Void->suivant = NULL;
  String->suivant = Void;
  Integer->suivant = String;
  listeDeClass = Integer;

  /* debut du main */
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

  printf("Debut yyparse()\n");
  res = yyparse();
  printf("Fin yyparse()\n");
  if(res==1)
  {
  printf("Aucune verification a faire car syntax error\n");
    exit(0);
  }
  printf("Lancement de .... checkProgramme(programme)\n");
  bool checkProg = checkProgramme(programme);
  printf("Fin de la compilation\n");
  afficheListeErreur(listeErreur);

  if (programme == NULL) {
    printf("tp.c -> Programme est NULL\n");
    exit(0);
  }
  else
  {
    /* Fonction de teste des evalExpr*/
    /*testEval();*/
    /*printf("tp.c -> Affichage de l'arbre : \n");*/

	/* Remarque : si vous voulez afficher l'arbre du programme, décommentez l'appel de pprintTreeMain */
    	/*pprintTreeMain(programme);*/
  }
  if (fd != NIL(FILE)) fclose(fd);
    if (res == 0 && errorCode == NO_ERROR) return 0;
    else {
      int res2 = res ? SYNTAX_ERROR : errorCode;
      printf("Error in file. Kind of error: %d\n", res2);
        return res2;
    }
  
  return 0;
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

/* Permet de creer une classe qui contient uniquement un nom */
PCLASS makeDefClasse(char *nom){
  PCLASS res = NEW(1, SCLASS);
  res->nom=nom;
  return res;
}

/** construit une structure classe (pouvant etre une liste de classe) apres l'appel de makeDefClasse */
PCLASS makeClasseApresDef(PCLASS res,PVAR param_constructeur,TreeP corps_constructeur,PMETH liste_methodes,PVAR liste_champs, PCLASS classe_mere, int isExtend){
  res->param_constructeur=param_constructeur;
  res->corps_constructeur=corps_constructeur;
  res->liste_methodes=liste_methodes;
  res->liste_champs=liste_champs;
  res->classe_mere=classe_mere;
  res->isExtend=isExtend;
  /* res->suivant=NULL;*/ /* verifier si ça ne pose pas de pb */
  return res;
}

/* Construit entierement une classe (sans avoir a appeler makeDefClasse et/ou makeClasseApresDef */
PCLASS makeClasse(char *nom, PVAR param_constructeur,TreeP corps_constructeur,PMETH liste_methodes,PVAR liste_champs, PCLASS classe_mere, int isExtend){
  PCLASS res = NEW(1, SCLASS);
  res->nom=nom;
  res->param_constructeur=param_constructeur;
  res->corps_constructeur=corps_constructeur;
  res->liste_methodes=liste_methodes;
  res->liste_champs=liste_champs;
  res->classe_mere=classe_mere;
  res->isExtend=isExtend;
  /* res->suivant=NULL;*/ /* verifier si ça ne pose pas de pb */
  return res;
}

bool varEstDansListe(PVAR listeVar, char *nom){
	PVAR tmp = listeVar;
	while(tmp!=NULL){
		if(strcmp(tmp->nom, nom) == 0){
			return TRUE;
		}
		tmp = tmp->suivant;
	}
	return FALSE;
}

PVAR getVar(PVAR var, char* nom){
  PVAR tmp = var;

	while(tmp != NULL){
		if(strcmp(tmp->nom, nom)==0){
			/* return makeListVar(tmp->nom, tmp->type, tmp->categorie, tmp->init); */
			return tmp;
		}
		tmp = tmp->suivant;
	}
	return NULL;
}

/* Renvoi le pointeur de classe avec un nom donnée */
PCLASS getClasse(PCLASS listeClass,char *nom){

  PCLASS parcour=listeClass;
  while((parcour!=NULL)&&(strcmp(parcour->nom,nom)!=0)){
    parcour=parcour->suivant;
  }
if(parcour == NULL){
    char *message = calloc(SIZE_ERROR,sizeof(char));
    sprintf(message,"Classe inexistante");
    pushErreur(message,NULL,NULL,NULL);
    return NULL;
  }
  else{
    /*parcour->suivant = NULL;*/
    return parcour;
  }
}

/** Renvoie une copie de la classe cherchee */
PCLASS getClasseBis(PCLASS listeClass,char *nom)
{
  PCLASS retour = getClasse(listeClass,nom);

  if(retour ==NULL)
  {
    char *message = calloc(SIZE_ERROR,sizeof(char));
    sprintf(message,"Classe inexistante");
    pushErreur(message,NULL,NULL,NULL);
    return NULL;
  }
  SCLASS classe = *retour;
  PCLASS classeFin = NEW(1,SCLASS);
  *classeFin = classe;
  classeFin->suivant = NULL;
  return classeFin;
}

PMETH getMethodeBis(PMETH meth, char *nom){
	PMETH tmp = meth;
	while(tmp != NULL){
		if(strcmp(tmp->nom, nom)==0){
			PMETH methode = makeMethode(tmp->nom, 0, tmp->corps, tmp->typeRetour, tmp->params, tmp->home);
			methode->isRedef = tmp->isRedef;
			methode->isStatic = tmp->isStatic;
			return methode;
		}
	}
	return NULL;
}

/** Renvoie un pointeur de la methode recherchee */
PMETH getMethode(PCLASS classe, PMETH methode){
  if(classe==NULL || methode==NULL)
  {
    return NULL;
  }
  PMETH tmp_liste_methodes_classe = getClasseBis(listeDeClass,classe->nom)->liste_methodes;
  PMETH tmp_liste_methode = methode;
  if(methode == NULL) return NULL;
  while(tmp_liste_methodes_classe != NULL){
    /* si 2 methodes ont le meme noms, les memes classes de retour (meme noms) et memes param ==> meme methode */
    if(strcmp(tmp_liste_methodes_classe->nom, tmp_liste_methode->nom)==0 && tmp_liste_methodes_classe->typeRetour!=NULL && tmp_liste_methode->typeRetour!=NULL && strcmp(tmp_liste_methodes_classe->typeRetour->nom, tmp_liste_methode->typeRetour->nom)==0 && memeVar(tmp_liste_methodes_classe->params, tmp_liste_methode->params)==TRUE ){
      return tmp_liste_methodes_classe;
    }
    tmp_liste_methodes_classe = tmp_liste_methodes_classe->suivant;
  }
  return NULL;
}

/* Renvoie vrai si une classe est dans une liste de classe */
bool estDansListClasse(PCLASS listeClasse, char *nom){
  PCLASS parcour=listeClasse;
  while((parcour!=NULL)&&(strcmp(parcour->nom,nom)!=0)){
    parcour=parcour->suivant;
  }
  if(parcour == NULL){
    char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 20");
    return FALSE;
  }
  else{
    return TRUE;
  }
}

/* Renvoi vrai si la methode est dans la classe, faux sinon */
bool methodeDansClasse(PCLASS classe, PMETH methode){
  if(classe==NULL || methode==NULL)
  {
    char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 21");
    return FALSE;
  }
  PMETH tmp_liste_methodes_classe = getClasseBis(listeDeClass,classe->nom)->liste_methodes;
  PMETH tmp_liste_methode = methode;
  if(methode == NULL) return FALSE;

  while(tmp_liste_methodes_classe != NULL){
    /* si 2 methodes ont le meme noms, les memes classes de retour (meme noms) et memes param ==> meme methode */
    if(strcmp(tmp_liste_methodes_classe->nom, tmp_liste_methode->nom)==0 && strcmp(tmp_liste_methodes_classe->typeRetour->nom, tmp_liste_methode->typeRetour->nom)==0 && memeVar(tmp_liste_methodes_classe->params, tmp_liste_methode->params)==TRUE ){
      return TRUE;
    }
    tmp_liste_methodes_classe = tmp_liste_methodes_classe->suivant;
  }

  return FALSE;
}

/* Renvoi vrai si var1 = var2 */
bool memeVar(PVAR var1, PVAR var2){
  if(var1 == NIL(SVAR)){
    if(var2 == NIL(SVAR)) return TRUE;
    return FALSE;
  }
  if(var2 == NIL(SVAR)) return FALSE;
  if(strcmp(var1->nom, var2->nom)!=0) return FALSE;
  if(strcmp(var1->type->nom, var2->type->nom) != 0) return FALSE;
  if(var1->categorie != var2->categorie)  return FALSE;
  return memeVar(var1->suivant, var2->suivant);
}

/* Creation de la structure Methode */
PMETH makeMethode(char *nom, int OverrideOuStaticOpt,TreeP corps,PCLASS typeRetour,PVAR params, PCLASS home){
  
  PMETH res=NEW(1, SMETH);
  /*res->suivant = NIL(SMETH);*/  /* verifier si ça ne pose pas de pb */
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
  /*res->suivant=NIL(SVAR); verifier si ça ne pose pas de pb */
  res->nom=nom;
  res->type=type;
  res->init=init;
  res->categorie=cat; /* si cat=0 ==> var non static. si cat=1 ==> var static */
  return res;
}

/*
 * Creer une erreur et l'ajoute a la pile
 */
void pushErreur(char* message,PCLASS classe,PMETH methode,PVAR variable)
{

  if(message==NULL)
    return;
  ErreurP nouvelle = NEW(1,Erreur);
  nouvelle->message = NEW(SIZE_ERROR,char);
  strcpy(nouvelle->message,message);

  if(classe!=NULL) nouvelle->classe = *classe;
  if(methode!=NULL) nouvelle->methode = *methode;
  if(variable!=NULL) nouvelle->variable = *variable;

  if(listeErreur==NULL)
  {
    listeErreur = nouvelle;
  }
  else
  {

    ErreurP tmp = listeErreur;
    listeErreur = nouvelle;
    nouvelle->suivant = tmp;
  }
}

/*
* Fonction permettant le parcours de l'arbre : check le programme en sa totalite
*/
bool checkProgramme(TreeP prog){
  if(prog==NULL) return FALSE;
  /* Acces statique au bloc Main */
  TreeP bloc = getChild(prog,1);
  bool  checkLC= FALSE;
  SCLASS tmp;
  PCLASS liste = NULL;
  if(listeDeClass == NULL)
  {
    checkLC = TRUE;
  }
  else
  {
    tmp = *listeDeClass;
    liste = NEW(1,SCLASS);
    *liste = tmp;
  }

  dansCheckBlocMain = FALSE;

  if(!checkLC)
  {
     while(liste!=NULL)
     {
      checkLC = checkClass(bloc,prog,liste, NULL, NULL) && checkLC;
      liste = liste->suivant;
     }
  }

  bool blockMain = FALSE;
  dansCheckBlocMain = TRUE;
  if(getChild(bloc,0)!=NULL)
  {
    blockMain = checkBloc(bloc,prog,NULL, NULL, getChild(bloc,0)->u.var);
  }
  else
  {
    blockMain = checkBloc(bloc,prog,NULL, NULL, NULL);
  }
  if(listeDeClass==NULL)
  {
    return blockMain;
  }
  return blockMain && checkLC;
}

bool checkBloc(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl){
  char* message = NEW(SIZE_ERROR,char);
  /*bloc vide => OK*/
  if(arbre == NIL(Tree) || arbre==NULL)
  {
    return TRUE;
  }

  if(arbre->op == CONTENUBLOC)
  {
    if(listeDecl != NULL && getChild(arbre,1)==NULL)
    {
      sprintf(message,"la liste d'instruction est vide tandis que la liste de declaration ne l'est pas");
      pushErreur(message,courant,methode,listeDecl);
      return FALSE;
    }
    bool res = checkListDeclaration(getChild(arbre,0),ancien,courant,methode,listeDecl);
    if(res == FALSE)
    {
      sprintf(message,"la liste de declaration contient des erreurs");
      pushErreur(message,courant,methode,listeDecl);
      return FALSE;
    }
    res = checkListInstruction(getChild(arbre,1),arbre,courant,methode,listeDecl) & res;

    if(methode!=NULL && getChild(arbre,2)!=NULL)
    {
      PCLASS yield = getType(getChild(getChild(arbre,2),0),getChild(arbre,2),courant,methode,listeDecl);
      if(!equalsType(methode->typeRetour,yield))
      {
        if(methode->home!=NULL)
        {
          sprintf(message,"verifier le yield dans la methode %s de la classe %s ",methode->nom,methode->home->nom);
        }
        else
        {
          sprintf(message,"verifier le yield dans la methode %s ",methode->nom);
        }
        pushErreur(message,courant,methode,listeDecl);
        res = FALSE;
      }
    }
    return res;
  }
  else if(arbre->op == ETIQUETTE_AFFECT)
  {
    PCLASS typeCible = getType(getChild(arbre,0),NULL,courant,methode,listeDecl);
    PCLASS typeExpr = getType(getChild(arbre,1),NULL,courant,methode,listeDecl);
    if(!equalsType(typeCible,typeExpr))
    {
      sprintf(message,"Erreur affectation entre un type et un type %s et %s",typeCible!=NULL?typeCible->nom:" ... null/inexistant",typeExpr!=NULL?typeExpr->nom:" ... null");
      pushErreur(message,courant,methode,listeDecl);
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  return FALSE;
}
bool checkListInstruction(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  /* arriver a la fin des instructions */
  if(arbre==NULL)
  {
    return TRUE;
  }
  char* message = NEW(SIZE_ERROR,char);
  if(arbre->op == CONTENUBLOC)
  {
    bool resultat = FALSE;
    resultat = checkBloc(arbre, NULL, courant,methode,listeDecl);
    if(!resultat)
    {
      sprintf(message,"Instruction : erreur dans contenu bloc");
      pushErreur(message,courant,methode,listeDecl);
    }
    return resultat;
  }

  if(arbre->nbChildren==0)
  {
    return (getType(arbre,NULL,courant,methode,listeDecl)!=NULL);
  }

  TreeP autreinstructions = getChild(arbre,1);
  TreeP instruction = getChild(arbre,0);

  PCLASS type = NULL;
  PCLASS type2 = NULL;

  bool resultat = FALSE;


  SVAR copieListDecl;
  PVAR nouvellelisteDecl = NULL;
  PVAR parcourL =  NULL;

  switch(instruction->op)
  {
    case EXPRESSIONRETURN :
      if(methode == NULL)
      {
        sprintf(message,"Bloc main ne peut avoir de return");
        pushErreur(message,courant,methode,listeDecl);
        resultat = FALSE;
      }
      else
      {
        type = getType(getChild(instruction,0),instruction,courant,methode,listeDecl);
        resultat = type!=NULL;
        if(!equalsType(methode->typeRetour,type))
        {
          sprintf(message,"Erreur de return : %s type = %s et retour %s",methode!=NULL?methode->nom:" ... null",(methode!=NULL && methode->typeRetour!=NULL)?methode->typeRetour->nom:" ... null",type!=NULL?type->nom:" ... null");
          pushErreur(message,courant,methode,listeDecl);
          return FALSE;
        }
        if(!resultat)
        {
          sprintf(message,"Erreur de return : %s ",methode!=NULL?methode->nom:"... null");
          pushErreur(message,courant,methode,listeDecl);
        }
      }
    break;

    case ETIQUETTE_AFFECT :
      type = getType(getChild(instruction,0),instruction,courant,methode,listeDecl);
      type2 = getType(getChild(instruction,1),instruction,courant,methode,listeDecl);
      if(!equalsType(type,type2))
      {
        if(type!=NULL)
        {
          if(type2!=NULL)
          {          
            sprintf(message,"Instruction : affectation incorrecte entre deux types differents %s & %s courant = %s",type->nom,type2->nom,methode!=NULL?methode->nom:"");
          }
          else
          {
            sprintf(message,"Instruction : affectation incorrecte entre deux types differents : %s",type->nom);
          }
        }
        else
        {
          if(type2!=NULL)
          {
            sprintf(message,"Instruction : affectation incorrecte entre %s deux types differents %s ---- %d ",type2->nom, getChild(instruction,0)->u.str,getChild(instruction,1)->op);
          }
          else
          {
            sprintf(message,"Instruction : affectation incorrecte entre deux types differents %s ---- %d ", getChild(instruction,0)->u.str,getChild(instruction,1)->op);
          }
        }
        pushErreur(message,courant,methode,listeDecl);
        resultat = FALSE;
      }
      else
      {
        resultat = TRUE;
      }
      if(!resultat)
      {
        sprintf(message,"Instruction : erreur d'affectation");
        pushErreur(message,courant,methode,listeDecl);
      }
      break;

        case IFTHENELSE :

            if(listeDecl!=NULL)
            {
              copieListDecl = *listeDecl;
              nouvellelisteDecl = NEW(1,SVAR);
              *nouvellelisteDecl = copieListDecl;
            }
            /*nouvellelisteDecl = listDecl a ce stade la*/
            type = getType(getChild(instruction,0),instruction,courant,methode,nouvellelisteDecl);


            if(!equalsType(type,getClasseBis(listeDeClass,"Integer")))
            {
              sprintf(message,"Instruction : la condition dans le if n'est pas un Integer");
              pushErreur(message,courant,methode,nouvellelisteDecl);
              return FALSE;
            }

            if(getChild(instruction,1)->op==CONTENUBLOC || getChild(instruction,1)->op==ETIQUETTE_AFFECT)
            {
              if(getChild(instruction,1)->op==CONTENUBLOC && getChild(getChild(instruction,1),0)!=NULL)
              {

                parcourL = nouvellelisteDecl;
                if(parcourL!=NULL)
                {
                  while(parcourL->suivant!=NULL)
                  {
                    parcourL = parcourL->suivant;
                  }
                  if(getChild(getChild(instruction,1),0)->u.var!=NULL)
                  {
                    nouvellelisteDecl =  fusionne(nouvellelisteDecl,getChild(getChild(instruction,1),0)->u.var);
                  }
                  else
                    parcourL->suivant = NULL;
                }
                else
                {
                  if(getChild(getChild(instruction,1),0)->u.var!=NULL)
                  {
                    copieListDecl = *getChild(getChild(instruction,1),0)->u.var;
                    nouvellelisteDecl = NEW(1,SVAR);
                    *nouvellelisteDecl = copieListDecl;
                  }
                }

              }
              resultat = checkBloc(getChild(instruction,1),NULL,courant,methode,nouvellelisteDecl);
            }
            else
            {

              resultat = checkListInstruction(getChild(instruction,1),instruction,courant,methode,nouvellelisteDecl);

            }

            if(getChild(instruction,2)->op==CONTENUBLOC || getChild(instruction,2)->op==ETIQUETTE_AFFECT)
            {
              if(getChild(instruction,2)->op==CONTENUBLOC && getChild(getChild(instruction,2),0)!=NULL)
              {
                parcourL = nouvellelisteDecl;
                if(parcourL!=NULL)
                {
                  while(parcourL->suivant!=NULL)
                  {
                    parcourL = parcourL->suivant;
                  }

                  if(getChild(getChild(instruction,2),0)->u.var!=NULL)
                  {
                    nouvellelisteDecl =  fusionne(nouvellelisteDecl,getChild(getChild(instruction,2),0)->u.var);
                  }
                  else
                    parcourL->suivant = NULL;
                }
                else
                {
                  if(getChild(getChild(instruction,2),0)->u.var!=NULL)
                  {
                    copieListDecl = *getChild(getChild(instruction,2),0)->u.var;
                    nouvellelisteDecl = NEW(1,SVAR);
                    *nouvellelisteDecl = copieListDecl;
                  }
                }
              }
              resultat = checkBloc(getChild(instruction,2),NULL,courant,methode,nouvellelisteDecl) && resultat;

            }
            else
            {

              resultat = checkListInstruction(getChild(instruction,2),instruction,courant,methode,nouvellelisteDecl) && resultat;

            }
            if(!resultat)
            {
              sprintf(message,"Instruction : erreur de if then else");
              pushErreur(message,courant,methode,nouvellelisteDecl);
            }
        break;

        case RETURN_VOID :
          resultat = equalsType(getClasseBis(listeDeClass,"Void"),methode->typeRetour);

          if(!resultat)
          {
            sprintf(message,"Instruction : erreur de return void");
            pushErreur(message,courant,methode,listeDecl);
          }
        break;

        case PLUSUNAIRE :
        case MINUSUNAIRE :
        case CONCATENATION :
        case PLUSBINAIRE :
        case MINUSBINAIRE :
        case DIVISION :
        case MULTIPLICATION :
        case OPCOMPARATEUR :
        case CSTSTRING :
        case CSTENTIER :
        case INSTANCIATION :
        case ENVOIMESSAGE :
        case SELECTION :
        case IDENTIFICATEUR :
          type = getType(instruction,NULL,courant,methode,listeDecl);
          if(type==NULL)
          {
            resultat = FALSE;
          }
          else
          {
            if(arbre->op==EXPRESSIONRETURN && methode!=NULL)
            {
              resultat = equalsType(methode->typeRetour,type);
            }
            else
            {
              resultat = TRUE;
            }

          }
          if(!resultat)
          {
            if(type!=NULL)
            {
              sprintf(message,"type : %s  Instruction : erreur d'instruction %d",type->nom,instruction->op);
            }
            else
            {
              sprintf(message,"Instruction : erreur d'instruction");
            }
            pushErreur(message,courant,methode,listeDecl);
          }
        break;


        default :
        printf("L'etiquette : %d n'est pas pris en compte ", instruction->op);
        resultat = FALSE;
     }
  return checkListInstruction(autreinstructions,instruction,courant,methode,listeDecl)&& resultat;
}


PVAR fusionne(PVAR declarationPrecedente,PVAR declarationSousBloc)
{
  if(declarationPrecedente==NULL)
  {
    SVAR copie = *declarationSousBloc;
    PVAR resultat = NEW(1,SVAR);
    *resultat = copie;
    return resultat;
  }
  else if(declarationSousBloc==NULL)
  {
    SVAR copie = *declarationPrecedente;
    PVAR resultat = NEW(1,SVAR);
    *resultat = copie;
    return resultat;
  }
  else
  {
    SVAR copieDP = *declarationPrecedente;
    PVAR resultatDP = NEW(1,SVAR);
    *resultatDP = copieDP;

    PVAR tmpDP = resultatDP;

    SVAR copieDSB = *declarationSousBloc;
    PVAR resultatDSB = NEW(1,SVAR);
    *resultatDSB = copieDSB;

    PVAR tmpDSB = resultatDSB;

    bool ajouter = TRUE;

    PVAR nouvellelisteDecl = NULL;

    while(tmpDP!=NULL)
    {
      ajouter = TRUE;
      tmpDSB = resultatDSB;
      while(tmpDSB!=NULL)
      {
        if(strcmp(tmpDP->nom,tmpDSB->nom)==0)
        {
          ajouter = FALSE;
          break;
        }
        tmpDSB = tmpDSB->suivant;
      }
      if(ajouter)
      {
        SVAR copieAjout = *tmpDP;
        PVAR copieAjoutP = NEW(1,SVAR);
        *copieAjoutP = copieAjout;
        copieAjoutP->suivant = NULL;
        nouvellelisteDecl = ajouterPVAR(copieAjoutP, nouvellelisteDecl);
      }
      tmpDP = tmpDP->suivant;
    }

    PVAR tmpDSBParcours = resultatDSB;
    while(tmpDSBParcours!=NULL)
    {
      SVAR copieAjout = *tmpDSBParcours;
      PVAR copieAjoutP = NEW(1,SVAR);
      *copieAjoutP = copieAjout;
      copieAjoutP->suivant = NULL;
      nouvellelisteDecl = ajouterPVAR(copieAjoutP, nouvellelisteDecl);
      tmpDSBParcours = tmpDSBParcours->suivant;
    }

    return nouvellelisteDecl;
  }
}

PVAR ajouterPVAR(PVAR nouvelle, PVAR liste)
{
  if(liste==NULL)
  {
    SVAR copie = *nouvelle;
    PVAR resultat = NEW(1,SVAR);
    *resultat = copie;
    return resultat;
  }
  else if(nouvelle==NULL)
  {
    SVAR copie = *liste;
    PVAR resultat = NEW(1,SVAR);
    *resultat = copie;
    return resultat;
  }
  else
  {
    SVAR copie = *nouvelle;
    PVAR resultat = NEW(1,SVAR);
    *resultat = copie;

    resultat->suivant = liste;
    return resultat;
  }

}

bool checkListDeclaration(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{

  PVAR tmp = listeDecl;

  while(tmp!=NULL)
  {
    if(tmp->type==NULL)
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur la classe de l'attribut %s n'existe pas",tmp->nom);
      pushErreur(message,NULL,NULL,tmp);
      return FALSE;
    }

    if(tmp->init!=NIL(Tree) || tmp->init!=NULL)
    {
      PCLASS type = getType(getChild(tmp->init,0),arbre,courant,methode,listeDecl);
      if(type!=NULL && !equalsType(tmp->type,type))
      {
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Erreur affectation d'un %s par un %s",tmp->type->nom,type->nom);
        pushErreur(message,courant,methode,NULL);
      }
    }
    tmp = tmp->suivant;
  }

  return TRUE;
}
bool checkClass(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  /* Le nom de la classe doit avoir une majuscule*/
  bool nomMaj = FALSE;

  if(courant->nom!=NULL && (courant->nom[0] >= 'A' && courant->nom[0] <= 'Z'))
    nomMaj = TRUE;

  /*TRUE : OK FALSE : NOK*/
  bool heritage = FALSE;
  
  if(courant->isExtend)
  {
    if(courant->classe_mere==NULL)
    {
      heritage = FALSE;
    }
    else
    {
      heritage = checkHeritage(courant);
    }
  }
  else
  {
    heritage = TRUE;
  }

  bool attribut = checkListAttribut(arbre,ancien,courant,methode,listeDecl);

  if(!attribut)
  {
    return FALSE;
  }
  bool methodeC = checkListMethode(arbre,ancien,courant,methode,listeDecl)/*TRUE*/;

  return (nomMaj && heritage && attribut && methodeC);
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
  char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 23");
  return FALSE;
}

bool checkListAttribut(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  if(courant!=NULL && !verifAttributClasse(courant))
  {
    char *message = calloc(SIZE_ERROR,sizeof(char));
    if(courant!=NULL)
    {
      sprintf(message,"Attributs duppliqué dans la classe %s ",courant->nom);
    }
    else
    {
      sprintf(message,"Attributs duppliqué");
    }
    return FALSE;
  }
  /*
   * Parcourir les attributs de la classe actuel & verifier qu'il n'y a aucune qui se ressemble !
   */
  PVAR tmp = courant->liste_champs;

  while(tmp!=NULL)
  {
    if(tmp->type==NULL)
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur la classe de l'attribut %s n'existe pas",tmp->nom);
      pushErreur(message,NULL,NULL,tmp);
      return FALSE;
    }

    if(tmp->init!=NIL(Tree) || tmp->init!=NULL)
    {
      PCLASS type = getType(getChild(tmp->init,0),arbre,courant,methode,listeDecl);
      if(type!=NULL && !equalsType(tmp->type,type))
      {
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Erreur affectation d'un %s par un %s",tmp->type->nom,type->nom);
        pushErreur(message,courant,methode,NULL);
      }
    }
    tmp = tmp->suivant;
  }

  return TRUE;
}

bool checkListMethode(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{

  if(courant->isExtend)
  {

    char * nomC = calloc(100,sizeof(char));
    sprintf(nomC,"constructeur %s",courant->classe_mere->nom);

    PMETH methodeFakeConstructeur = NEW(1,SMETH);
    methodeFakeConstructeur->corps = courant->corps_constructeur;
    methodeFakeConstructeur->nom = calloc(100,sizeof(char));
    sprintf(methodeFakeConstructeur->nom,"constructeur %s",classActuel->nom);
    methodeFakeConstructeur->params = courant->param_constructeur;
    /**/
    bool constCorrecte = compareParametreMethode(courant->classe_mere->param_constructeur,courant->appel_constructeur_mere,courant,methodeFakeConstructeur,NULL,nomC);
    constCorrecte = checkBloc(methodeFakeConstructeur->corps,NULL,courant,methodeFakeConstructeur,listeDecl) && constCorrecte;

    if(!constCorrecte)
    {
      char *message = calloc(100,sizeof(char));
      sprintf(message,"Erreur d'appel constructeur : %s mal appelee",classActuel->nom);
      pushErreur(message,classActuel,NULL,NULL);
      return FALSE;
    }
  }
  else if(strcmp(courant->nom,"Integer")!=0 && strcmp(courant->nom,"String")!=0 && strcmp(courant->nom,"Void")!=0)
  {
    char * nomC = calloc(100,sizeof(char));
    sprintf(nomC,"constructeur %s",courant->nom);

    PMETH methodeFakeConstructeur = NEW(1,SMETH);
    methodeFakeConstructeur->corps = courant->corps_constructeur;
    methodeFakeConstructeur->nom = calloc(100,sizeof(char));
    sprintf(methodeFakeConstructeur->nom,"constructeur %s",classActuel->nom);
    methodeFakeConstructeur->params = courant->param_constructeur;

    /* Pour liste de declaration, faire une fusion de liste champs et param constructeur */
    bool constCorrecte = checkBloc(courant->corps_constructeur,NULL,courant,methodeFakeConstructeur,listeDecl);

    if(!constCorrecte)
    {
      char *message = calloc(100,sizeof(char));
      sprintf(message,"Erreur d'appel constructeur : %s mal appelee",classActuel->nom);
      pushErreur(message,classActuel,NULL,NULL);
      return FALSE;
    }

  }
  if(courant->liste_methodes==NULL)
  {
    return TRUE;
  }
  SMETH copie = *courant->liste_methodes;
  PMETH tmp = NEW(1,SMETH);
  *tmp = copie;

  while(tmp!=NULL)
  {
    if(!checkMethode(arbre,ancien,courant,tmp,listeDecl))
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur la methode %s est mal construite",tmp->nom);
      pushErreur(message,NULL,tmp,NULL);
      return FALSE;
    }

    tmp = tmp->suivant;
  }

  return TRUE;
}

/*Verification d'une methode de classe ou static*/
bool checkMethode(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{
    if(methode->typeRetour==NULL)
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur type de retour de la methode %s inconnu",methode->nom);
      pushErreur(message,classActuel,methode,NULL);
      return FALSE;
    }
    bool statique = FALSE;
    bool redef = FALSE;
    bool typeRetour = (methode->typeRetour!=NULL);
    bool pvar = checkListOptArg(methode->params,methode);
    if(methode->isStatic)
    {
      statique = TRUE;
      redef = TRUE;
    }
    else
    {
      statique = TRUE;
      if(methode->isRedef)
      {
        redef = methodeDansClasse(methode->home->classe_mere,methode);
        if(!redef)
        {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"Erreur dans la methode override %s de la classe %s",methode!=NULL?methode->nom:"... null",(methode->home!=NULL && methode->home->classe_mere!=NULL)?methode->home->classe_mere->nom:" ... null");
          pushErreur(message,classActuel,methode,NULL);
          return FALSE;
        }
      }
      else
      {
        redef = TRUE;
      }
    }
    bool bloc = FALSE;
    if(methode->corps==NULL)
    {
      bloc = TRUE;
    }
    else
    {
      if(methode->corps->op == ETIQUETTE_AFFECT)
      {
        PCLASS retourAffect = NULL;
        if(strcmp(methode->home->nom,courant->nom)==0)
        {
          retourAffect = getType(getChild(methode->corps,0),arbre,courant,methode,NULL);
          if(equalsType(methode->typeRetour,retourAffect))
          {
            bloc = TRUE;
          }
        }
        else
        {
          bloc = TRUE;
        }
      }
      else if(getChild(methode->corps,0)==NULL)
      {
        if(strcmp(methode->home->nom,courant->nom)==0)
        {
          bloc = checkBloc(methode->corps,arbre,courant,methode,NULL);
        }
        else
        {
          bloc = TRUE;
        }
      }
      else
      {
        if(strcmp(methode->home->nom,courant->nom)==0)
        {
           bloc = checkBloc(methode->corps,arbre,courant,methode,getChild(methode->corps,0)->u.var);
        }
        else
        {
          bloc = TRUE;
        }
      }
     }

    return (bloc&&typeRetour&&statique&&redef&&pvar);
}

bool checkListOptArg(PVAR var, PMETH methode)
{
    PVAR tmp = var;

    while(tmp!=NULL)
    {
      if(tmp->type==NULL)
      {
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Verifier les argument de la methode %s",methode->nom);
        pushErreur(message,methode->home,methode,NULL);
        return FALSE;
      }
      tmp = tmp->suivant;
    }

    return TRUE;
}


/* Methode permettant de créer un type EvalP de type char* */
EvalP makeEvalStr(char *str){
  EvalP eval = NEW(1, Eval);
  eval->type = EVAL_STR;
  eval->u.str = str;
  return eval;
}

/* Methode permettant de créer un type EvalP de type int */
EvalP makeEvalInt(int val){
  EvalP eval = NEW(1, Eval);
  eval->type = EVAL_INT;
  eval->u.val=val;
  return eval;
}

/* Methode permettant de créer un type EvalP de type PVAR */
EvalP makeEvalVar(PVAR var){
  EvalP eval = NEW(1, Eval);
  eval->type = EVAL_PVAR;
  eval->u.var = var;
  return eval;
}

/* Methode permettant de créer un type EvalP de type PCLASS */
EvalP makeEvalClasse(PCLASS classe){
  EvalP eval = NEW(1, Eval);
  eval->type = EVAL_PCLASS;
  eval->u.classe = classe;
  return eval;
}

/* Methode permettant de créer un type EvalP de type PMETH */
EvalP makeEvalMethode(PMETH methode){
  EvalP eval = NEW(1, Eval);
  eval->type = EVAL_PMETH;
  eval->u.methode = methode;
  return eval;
}

/* Methode permettant de créer un type EvalP de type TreeP */
EvalP makeEvalTree(TreeP tree){
  EvalP eval = NEW(1, Eval);
  eval->type = EVAL_TREEP;
  eval->u.tree = tree; 
  return eval;
}


/* Evaluation par parcours recursif de l'arbre representant une expression.
 * Les valeurs des identificateurs situes aux feuilles de l'arbre sont a
 * rechercher dans la liste decls
 * Attention a n'evaluer que ce qui doit l'etre et au bon moment
 * selon la semantique de l'operateur (cas du IF, etc.)
 */

/** Partie eval **/

/* Ajout de env2 a la fin de environnement */
void updateEnvironnement(PVAR environnement, PVAR env2){
	PVAR tmp_env = environnement;
	while(tmp_env != NULL){
		tmp_env = tmp_env->suivant;
	}
	tmp_env = env2;
}

/* Evaluation globale du programme */
EvalP evalProgramme(TreeP programme){
	/* on a l'attribut listeDeClass qui contient toutes les classes (s'il y en a) --> pas besoin de regarder ListClassOpt */
	return evalContenuBloc(programme->u.children[1], NIL(SVAR));
}

/* Evaluation d'un bloc */
EvalP evalContenuBloc(TreeP bloc, PVAR environnement){

	/* on est dans la regle : ContenuBloc : ListDeclVar IS LInstruction YieldOpt */
	if(bloc->u.children[0] != NIL(Tree)){
		/* Evaluation de toutes les variables -> les "init" de ces variables sont mis à jour */
		evalListDeclVar(bloc->u.children[0]->u.var, bloc->u.children[0]->u.var);

		/* Mise a jour de l'environnement */
		updateEnvironnement(environnement, bloc->u.children[0]->u.var);
	}

	/* eval de LInstruction */
	if(bloc->u.children[1] != NIL(Tree)){
		evalListInstruction(bloc->u.children[1], environnement);	/* L'environnement doit se mettre a jour */
	}
	/* eval de YieldOpt */
	if(bloc->u.children[2] != NIL(Tree)){
		return evalExpr(bloc->u.children[2]->u.children[0], environnement);
	}
	return NIL(Eval);
}

/* Evalue toutes les variables déclarées -> ne pas mettre a jour l'environnement, ca se fait automatiquement (changement dans les PVAR) */
void evalListDeclVar(PVAR listDeclVar, PVAR environnement){
	if(listDeclVar == NIL(SVAR))	return;

	/* TODO : évaluer les parametres passé dans l'extends */
	/*
	if(listDeclVar->type->isExtend == 1){
		if(listDeclVar->type->appel_constructeur_mere != NULL){
			LEvalP eval_extend = evalListArg(listDeclVar->type->appel_constructeur_mere, environnement);
			while(eval_extend != NULL){
				
			}
		}
	}
	*/

	PVAR tmp = listDeclVar;
	EvalP eval = evalExpr(tmp->init->u.children[0], environnement);
	switch(eval->type){
		case EVAL_STR:
			tmp->init = makeLeafStr(EVALUE_STR, eval->u.str);
			break;
		case EVAL_INT:
			tmp->init = makeLeafInt(EVALUE_INT, eval->u.val);
			break;

		case EVAL_PVAR:
			/* Normalement ça devrait être instanciation -> une PVAR est créé, il faut lui donner son nom */
			printf("evalListDeclVar->PVAR\n");
			/*tmp->init = makeLeafVar(EVALUE_PVAR, eval->u.var);*/
			tmp->init = eval->u.var->init;
			break;

		/* Ne devrait pas etre utilisé */
		case EVAL_PCLASS:
			printf("evalListDeclVar->PCLASS\n");
			tmp->init = makeLeafClass(EVALUE_PCLASS, eval->u.classe);
			break;
		case EVAL_PMETH:
			printf("evalListDeclVar->PMETH\n");
			tmp->init = makeLeafMeth(EVALUE_PMETH, eval->u.methode);
			break;
		case EVAL_TREEP:
			printf("evalListDeclVar->TREEP\n");
			break;
		default:
			printf("evalListDeclVar->ERREUR\n");
			break;
	}

	/* Evaluation du reste de la liste des variables */
	evalListDeclVar(tmp->suivant, environnement);
}
EvalP evalInstruction(TreeP instruction, PVAR environnement){

/* TODO update de l'environnement */

	TreeP tmp = instruction;
	if(tmp == NULL || tmp == NIL(Tree))	return NIL(Eval);

	if(tmp->op == CONTENUBLOC){
		return evalContenuBloc(tmp, environnement);
	}
	else if(tmp->op == ETIQUETTE_AFFECT){
		PVAR cible;
		/* tmp->u.children[0] = eval de cible = ID ou selection */
		if(tmp->u.children[0]->op == IDENTIFICATEUR){
			cible = getVar(environnement, tmp->u.children[0]->u.str);
		}
		else{
			cible = evalSelection(tmp->u.children[0], environnement)->u.var;
		}
		/* affectation : tmp->u.children[1] = eval de expr */
		EvalP eval_affect = evalExpr(tmp->u.children[1], environnement);
		switch(eval_affect->type){
			case EVAL_INT:
				cible->init = makeLeafInt(EVALUE_INT, eval_affect->u.val);
				break;
			case EVAL_STR:
				cible->init = makeLeafStr(EVALUE_STR, eval_affect->u.str);
				break;
			case EVAL_PVAR:
				cible->init = makeLeafVar(EVALUE_PVAR, eval_affect->u.var);
				break;
			case EVAL_PCLASS:
				cible->init = makeLeafClass(EVALUE_PCLASS, eval_affect->u.classe);
				break;
			case EVAL_PMETH:
				cible->init = makeLeafMeth(EVALUE_PMETH, eval_affect->u.methode);
				break;
			case EVAL_TREEP:
				printf("Dans evalInstruction -> l'eval de l'affectation de l'expr = TreeP\n");
				break;
			default:
				printf("Dans evalInstruction -> etiquette introuvable (affect)\n");
				break;
		}

		/* Pas besoin de retourner qqch */
		return NIL(Eval);
	}
	else if(tmp->op == IFTHENELSE){
		return evalIf(tmp, environnement);
	}
	else if(tmp->op == EXPRESSIONRETURN){
		return evalExpr(tmp, environnement);
	}
	else if(tmp->op == RETURN_VOID){
		return NIL(Eval);
	}
	/* expression */
	else{
		/*EvalP eval = */evalExpr(tmp, environnement);
		/* Pas besoin de retourner qqch */
		return NIL(Eval);
	}

}
/* Evalue une liste d'instruction */
void evalListInstruction(TreeP Linstruction, PVAR environnement){
	evalInstruction(Linstruction->u.children[0], environnement);
	/* Eval du reste de la liste */
	if(Linstruction->nbChildren == 1){
		evalListInstruction(Linstruction->u.children[0], environnement);
	}
	else if(Linstruction->nbChildren == 2){
		evalListInstruction(Linstruction->u.children[1], environnement);
	}
}

/* Evalue un if/then/else */
EvalP evalIf(TreeP tree, PVAR environnement){
	EvalP eval_condition = evalExpr(tree->u.children[0], environnement);
	if(eval_condition->type != EVAL_INT){
		printf("Probleme dans EvalIf : l'évaluation de la condition (expression) n'est pas un int\n");
		return NIL(Eval);
	}
	/* la condition est OK -> on va dans THEN */
	if(eval_condition->u.val != 0){
		return evalInstruction(tree->u.children[1], environnement);
	}

	/* la condition est KO -> on va dans ELSE */
	else{
		return evalInstruction(tree->u.children[2], environnement);
	}
}


/** Renvoie la longueur d'une chaine **/
int sizeString(char *str){
	int size=0;
	while(str[size] != '\0'){
		size++;
	}
	return size;
}

/** Renvoie la longueur d'un int **/
int sizeInt(int val){
	int cpt=1;
	while((val/10)>0){
		val = val/10;
		cpt++;
	}
	return cpt;
}

/* Prend un parametre de type EvalP et renvoie sa valeur (entier) s'il en a une */
int getVal(EvalP eval){
	if(eval->type == EVAL_INT){
		return eval->u.val;
	}
	else if(eval->type == EVAL_PVAR){
		if(eval->u.var->init->op == EVALUE_INT){
			return eval->u.var->init->u.children[0]->u.val;
		}
	}
	/* yasser -> faux normalement
	else if(eval->type == EVAL_STR){
		return eval->u.str;
	}
*/	else{
		printf("Probleme dans getVal\n");
		exit(0);
	}
	return 0;
}

/** Methode eval d'une expression **/
EvalP evalExpr(TreeP tree, PVAR environnement){
	/*printf(" ======> (ICI) <=====\n");*/
	if(tree == NIL(Tree))	return NIL(Eval);
	char* chaine;
	int sizeConcat;
	PVAR var;
	int val1, val2;
	switch (tree->op) {
		case PLUSUNAIRE:
			return evalExpr(tree->u.children[0], environnement);
		case MINUSUNAIRE:
			return makeEvalInt(0-evalExpr(tree->u.children[0], environnement)->u.val);
		case CONCATENATION:
			sizeConcat=(sizeString(evalExpr(tree->u.children[0], environnement)->u.str)+sizeString(evalExpr(tree->u.children[1], environnement)->u.str));
		    	chaine = calloc(sizeConcat, sizeof(char));
			chaine = strdup(evalExpr(tree->u.children[0], environnement)->u.str);
			strcat(chaine, evalExpr(tree->u.children[1], environnement)->u.str);
			return makeEvalStr(chaine);
		case PLUSBINAIRE:
			val1=getVal(evalExpr(tree->u.children[0], environnement));
			val2=getVal(evalExpr(tree->u.children[1], environnement));
			return makeEvalInt(val1 + val2);
		case MINUSBINAIRE:
			val1=getVal(evalExpr(tree->u.children[0], environnement));
			val2=getVal(evalExpr(tree->u.children[1], environnement));
			return makeEvalInt(val1 - val2);
		case DIVISION:
			val1=getVal(evalExpr(tree->u.children[0], environnement));
			val2=getVal(evalExpr(tree->u.children[1], environnement));
			return makeEvalInt(val1 / val2);
		case MULTIPLICATION:
			val1=getVal(evalExpr(tree->u.children[0], environnement));
			val2=getVal(evalExpr(tree->u.children[1], environnement));
			return makeEvalInt(val1 * val2);
		case CSTENTIER:
			return makeEvalInt(tree->u.val);
		case CSTSTRING:
			return makeEvalStr(tree->u.str);

		case OPCOMPARATEUR:
			if(tree->u.children[2]->u.val == EQ){
				val1=getVal(evalExpr(tree->u.children[0], environnement));
				val2=getVal(evalExpr(tree->u.children[1], environnement));
				return makeEvalInt(val1 == val2);
			}
			else if(tree->u.children[2]->u.val == NE){

				val1=getVal(evalExpr(tree->u.children[0], environnement));
				val2=getVal(evalExpr(tree->u.children[1], environnement));
				return makeEvalInt(val1 != val2);
			}
			else if(tree->u.children[2]->u.val == GT){
				val1=getVal(evalExpr(tree->u.children[0], environnement));
				val2=getVal(evalExpr(tree->u.children[1], environnement));
				return makeEvalInt(val1 > val2);
			}
			else if(tree->u.children[2]->u.val == GE){
				val1=getVal(evalExpr(tree->u.children[0], environnement));
				val2=getVal(evalExpr(tree->u.children[1], environnement));
				return makeEvalInt(val1 >= val2);
			}
			else if(tree->u.children[2]->u.val == LT){
				val1=getVal(evalExpr(tree->u.children[0], environnement));
				val2=getVal(evalExpr(tree->u.children[1], environnement));
				return makeEvalInt(val1 < val2);
			}
			else if(tree->u.children[2]->u.val == LE){
				val1=getVal(evalExpr(tree->u.children[0], environnement));
				val2=getVal(evalExpr(tree->u.children[1], environnement));
				return makeEvalInt(val1 <= val2);
			}
			else{
				/* Probleme */
				printf("Probleme dans EvalExpr au niveau du comparateur -> etiquette inconnue\n");
				return NIL(Eval);
			}

		case IDENTIFICATEUR:
			
			var = copyVar(getVar(environnement, tree->u.str)); /* TODO pointeur ou copie? */
      			printf("YOUPPI on est dans ce cas \n");
			if(var == NULL)		return NIL(Eval);
			/*return makeEvalVar(var);*/
			/* si l'id n'a pas d'affectation pour le moment */
			if(var->init == NULL){
				return NIL(Eval);	/* TODO : ou PVAR? */
			}
			/* si l'id a deja ete evalue */
			else if(var->init->op == EVALUE_STR){
				return makeEvalStr(var->init->u.str);
			}
			else if(var->init->op == EVALUE_INT){
				return makeEvalInt(var->init->u.val);
			}
			else if(var->init->op == EVALUE_PVAR){
				return makeEvalVar(var->init->u.var);
			}
			else if(var->init->op == EVALUE_PCLASS){
				return makeEvalClasse(var->init->u.classe);
			}
			else if(var->init->op == EVALUE_PMETH){
				return makeEvalMethode(var->init->u.methode);
			}
			else if(var->init->op == EVALUE_TREEP){
				return makeEvalTree(var->init->u.children[0]);
			}
			/* l'id a une affectation qui n'a pas encore ete evalue */
			else{
				/*printf("Probleme dans evalExpr -> ID ==> sa valeur n'est pas evalué\n");
				return NIL(Eval);
				*/
				return evalExpr(var->init, environnement);	/* on evalue l'affectation */
			}
		case INSTANCIATION:
			return evalInstanciation(tree, environnement);

		case ENVOIMESSAGE:
			return evalEnvoiMessage(tree, environnement);

		case SELECTION :
			return evalSelection(tree, environnement);

		/*
		default:
			fprintf(stderr, "Erreur! etiquette indefinie: %d\n", tree->op);
			exit(UNEXPECTED);
		*/
	}
	printf("Dans EvalExpr -> aucune etiquette trouvé\n");
	return NIL(Eval);
}

PVAR copyVar(PVAR var){
	if(var == NULL)	return NULL;
	PVAR tmp_var = var;
	PVAR copy = makeListVar(tmp_var->nom, tmp_var->type, tmp_var->categorie, tmp_var->init);
	PVAR tmp_copy = copy;
	tmp_var = tmp_var->suivant;
	tmp_copy = tmp_copy->suivant;
	while(tmp_var!=NULL){
		tmp_copy = makeListVar(tmp_var->nom, tmp_var->type, tmp_var->categorie, tmp_var->init);
		tmp_var = tmp_var->suivant;
		tmp_copy = tmp_copy->suivant;
	}
	tmp_copy = NULL;
	return copy;
}

/** Exemple d'instanciation : new Point(xc, yc) **/
EvalP evalInstanciation(TreeP tree, PVAR environnement){
	if(tree == NIL(Tree))	return NIL(Eval);

	/* Recherche de la classe IDCLASS */
	PCLASS classe = getClasse(listeDeClass, tree->u.children[0]->u.str);
	if(classe == NULL){
		printf("Probleme dans evalInstanciation : IDCLASS = NULL\n");
		exit(0);
	}

	/* Evaluation des champs de la classe s'il y a besoin */
	evalListDeclVar(classe->liste_champs, classe->liste_champs);

	/* TODO : ajouter dans environnement classe->liste_champs ? */
	/* updateEnvironnement(environnement, classe->liste_champs); */

	/* Création d'une copie des params du constructeur pour leur passer leurs valeurs */
	PVAR param = copyVar(classe->param_constructeur);

	/* Obtention de l'évaluation des params constructeur dans l'ordre */
	LEvalP listArg = evalListArg(tree->u.children[1], environnement);

	/* Attribution des eval de la listOptArg a ces params */
	LEvalP tmp_eval = listArg;
	PVAR tmp_param = param;
	while(tmp_eval != NIL(LEval) && tmp_param != NULL){		/* normalement nbEval = nbParam ! */
		switch(tmp_eval->eval->type){
			case EVAL_STR:
				tmp_param->init = makeLeafStr(EVALUE_STR, tmp_eval->eval->u.str);
				break;
			case EVAL_INT:
				tmp_param->init = makeLeafInt(EVALUE_INT, tmp_eval->eval->u.val);
				break;
			case EVAL_PVAR:
				tmp_param->init = makeLeafVar(EVALUE_PVAR, tmp_eval->eval->u.var);
				break;

			/* A ENLEVER NORMALEMENT CA NE DEVRAIT JAMAIS ARRIVE */
			case EVAL_PCLASS:
				tmp_param->init = makeLeafClass(EVALUE_PCLASS, tmp_eval->eval->u.classe);
				break;
			case EVAL_PMETH:
				tmp_param->init = makeLeafMeth(EVALUE_PMETH, tmp_eval->eval->u.methode);
				break;
			case EVAL_TREEP:
				printf("Dans evalInstanciation -> eval d'un arg est un tree\n");
				break;
			default:
				printf("Etiquette non reconnue dans evalInstanciation = %d\n", tmp_eval->eval->type);
				exit(0);
		}
		tmp_eval = tmp_eval->suivant;
		tmp_param = tmp_param->suivant;
	}

	/* TODO : Mettre a jour l'environnement ! --> peut etre fait automatiquement si param est dans environnement sinon mettre param dans environnement */

	/* TODO fait?: Appeler le constructeur selon les champs de la classe et les "nouveaux" param du constructeur */
	PVAR var;
	/* Constructeur vide */
	if(classe->corps_constructeur == NIL(Tree)){
		/* Le contenu de l'instance = les variables de la classe qui ont été évalué précédemment */
		var = makeListVar(NULL, classe, 0, makeLeafVar(EVALUE_PVAR, classe->liste_champs));
	}
	/* Constructeur non vide */
	else{
		/* appel de evalContenuBloc pour évaluer le constructeur de la classe */
		EvalP eval_constructeur = evalContenuBloc(classe->corps_constructeur, environnement);
		TreeP init;
		switch(eval_constructeur->type){
			case EVAL_INT:
				init = makeLeafInt(EVALUE_INT, eval_constructeur->u.val);
				break;
			case EVAL_STR:
				init = makeLeafStr(EVALUE_STR, eval_constructeur->u.str);
				break;
			case EVAL_PVAR:
				init = makeLeafVar(EVALUE_PVAR, eval_constructeur->u.var);
				break;
			default:
				init = NIL(Tree);
				break;
		}
		var = makeListVar(NULL, classe, 0, init);
	}
	/* Remarque : le nom de l'instance est pour l'instant à NULL -> on lui affectera plus tard dans la regle */
	return makeEvalVar(var);
}

/** Renvoie une liste d'évaluation -> la liste est dans l'ordre **/
LEvalP evalListArg(TreeP tree, PVAR environnement){
	if(tree == NIL(Tree))	return NIL(LEval);

	/* Le but : récupérer les expr dans l'ordre et faire appel au constructeur de la classe IDCLASS */
	LEvalP listEval = NULL;
	/* On a une liste de type LArg, expr*/
	if(tree->op == LISTEARG){
		LEvalP listEvalPrec = NULL;
		listEval->eval = evalExpr(tree->u.children[1], environnement);
		listEval->suivant = NULL;
		TreeP tmp = tree;
		tmp = tmp->u.children[0];
		while(tmp != NIL(Tree) && tmp->op == LISTEARG){
			listEvalPrec->eval = evalExpr(tree->u.children[1], environnement);
			listEvalPrec->suivant = listEval;
			listEval = listEvalPrec;	/* TODO a verifier */
			tmp = tmp->u.children[0];
		}
		/* Dans cette condition, on tombe sur une expr (la derniere) */
		if(tmp != NIL(Tree)){
			listEvalPrec->eval = evalExpr(tree->u.children[1], environnement);
			listEvalPrec->suivant = listEval;
		}
		/* Ici, listEvalPrec ne devrait pas etre null -> elle represente la liste des arguments dans l'ordre */
		return listEvalPrec;

	}
	/* on a pas une liste mais une expression */
	else{
		listEval->eval = evalExpr(tree, environnement);
		listEval->suivant = NULL;
		return listEval;
	}
}

/* Correspond à l'appel d'une methode */
EvalP evalEnvoiMessage(TreeP tree, PVAR environnement){
	if(tree == NIL(Tree))	return NIL(Eval);

  printf("Eval\n");
	PMETH methode;	


	/* TODO : Pour appel d'une méthode, l'environnement est mis à jour mais apres avoir fini, l'environnement est remis
		comme avant car les "changements" sont locales (a la méthode)
	*/

	/* IDCLASS '.' ID '(' ListOptArg ')'  -> METHODE STATIC */
	if(tree->u.children[0]->op == IDENTIFICATEURCLASS){	
    		methode = getMethodeBis(getClasse(listeDeClass, tree->u.children[0]->u.str)->liste_methodes, tree->u.children[1]->u.str);
		if(methode == NULL){
			printf("Problème dans evalEnvoiMessage : méthode introuvable dans la classe \n");
			return NIL(Eval);
		}
	}
	/* envoiMessage '.' ID'('ListOptArg ')' */
	else if(tree->u.children[0]->op == ENVOIMESSAGE){
		char *buffer; 
		/* faire appel a evalEnvoiMessage sur tree->u.children[0] */
		EvalP eval_precedent = evalEnvoiMessage(tree->u.children[0], environnement);
		switch(eval_precedent->type){
			case EVAL_STR:
				methode = getMethodeBis(getClasse(listeDeClass, "String")->liste_methodes, tree->u.children[1]->u.str);
				if(methode == NULL){
					printf("La methode %s de la classe String n'existe pas\n", tree->u.children[1]->u.str);
					return NIL(Eval);
				}
				if(strcmp(methode->nom, "println")==0){
					buffer = calloc(sizeString(eval_precedent->u.str + 1), sizeof(char));
					strcat(buffer,eval_precedent->u.str);
					buffer[sizeString(eval_precedent->u.str)] = '\n';
					buffer[sizeString(eval_precedent->u.str)+1] = '\0';
					/* affichage */
					printf("%s", buffer);
					return makeEvalStr(buffer);
				}
				else if(strcmp(methode->nom, "print")==0){
					printf("%s", eval_precedent->u.str);
					return makeEvalStr(eval_precedent->u.str);
				}
				else{
					printf("ce n'est ni print, ni println\n");
					return NIL(Eval);
				}
				break;
			case EVAL_INT:
				methode = getMethodeBis(getClasse(listeDeClass, "Integer")->liste_methodes, tree->u.children[1]->u.str);
				if(methode == NULL){
					printf("La methode %s de la classe Integer n'existe pas\n", tree->u.children[1]->u.str);
					return NIL(Eval);
				}
				if(strcmp(methode->nom, "toString") == 0){
					buffer = calloc(sizeInt(eval_precedent->u.val), sizeof(char));
					sprintf(buffer, "%d", eval_precedent->u.val);
					/* affichage */
					printf("%s\n", buffer);
					return makeEvalStr(buffer);
				}
				else{
					printf("La méthode toString n'est pas appelé -> pb\n");
					return NIL(Eval);
				}
				break;
			case EVAL_PVAR:
				/* Récupération de la méthode du PVAR ID */
				methode = getMethodeBis(eval_precedent->u.var->type->liste_methodes, tree->u.children[1]->u.str);
				if(methode == NULL){
					printf("dans evalEnvoiMessage, le ID de la methode de la var est NULL\n");
					return NIL(Eval);
				}
				break;
			default:
				printf("Dans evalEnvoiMessage : envoiMessage recursif -> pb sur l'étiquette\n");
				return NIL(Eval);
		}

	}
	/* constante '.' ID '(' ListOptArg ')' */
	else if(tree->u.children[0]->op == CSTSTRING){
		char *buffer;
		methode = getMethodeBis(getClasse(listeDeClass, "String")->liste_methodes, tree->u.children[1]->u.str);
		if(methode == NULL){
			printf("La methode %s de la classe String n'existe pas\n", tree->u.children[1]->u.str);
			return NIL(Eval);
		}
		if(strcmp(methode->nom, "println")==0){
			buffer = calloc(sizeString(tree->u.children[0]->u.str + 1), sizeof(char));
			strcat(buffer,tree->u.children[0]->u.str);
			buffer[sizeString(tree->u.children[0]->u.str)] = '\n';
			buffer[sizeString(tree->u.children[0]->u.str)+1] = '\0';
			/* affichage */
			printf("%s", buffer);
			return makeEvalStr(buffer);
		}
		else if(strcmp(methode->nom, "print")==0){
			printf("%s", tree->u.children[0]->u.str);
			return makeEvalStr(tree->u.children[0]->u.str);
		}
		else{
			printf("ce n'est ni print, ni println\n");
			return NIL(Eval);
		}
	}
	/* constante '.' ID '(' ListOptArg ')' */
	else if(tree->u.children[0]->op == CSTENTIER){
		char *buffer;
		methode = getMethodeBis(getClasse(listeDeClass, "Integer")->liste_methodes, tree->u.children[1]->u.str);
		if(methode == NULL){
			printf("La methode %s de la classe Integer n'existe pas\n", tree->u.children[1]->u.str);
			return NIL(Eval);
		}
		if(strcmp(methode->nom, "toString") == 0){
			buffer = calloc(sizeInt(tree->u.children[0]->u.val), sizeof(char));
			sprintf(buffer, "%d", tree->u.children[0]->u.val);
			/* affichage */
			printf("%s\n", buffer);
			return makeEvalStr(buffer);
		}
		else{
			printf("La méthode toString n'est pas appelé -> pb\n");
			return NIL(Eval);
		}
	}
	/* ID '.' ID '(' ListOptArg ')'  */
	else if(tree->u.children[0]->op == IDENTIFICATEUR){
		/* Recupération du PVAR ID (le 1er) */
		PVAR var=getVar(environnement, tree->u.children[0]->u.str);
		if(var == NULL){
			printf("dans evalEnvoiMessage, le ID de la var = NULL\n");
			return NIL(Eval);
		}
		/* Récupération de la méthode du PVAR ID */
		methode = getMethodeBis(var->type->liste_methodes, tree->u.children[1]->u.str);
		if(methode == NULL){
			printf("dans evalEnvoiMessage, le ID de la methode de la var est NULL\n");
		}

	}

	/* TODO selection '.' ID '(' ListOptArg ')' */
	else if(tree->u.children[0]->op == SELECTION){
		char *buffer;
		EvalP eval_selection = evalSelection(tree->u.children[0], environnement);
		switch(eval_selection->type){
			case EVAL_STR:
				methode = getMethodeBis(getClasse(listeDeClass, "String")->liste_methodes, tree->u.children[1]->u.str);
				if(methode == NULL){
					printf("La methode %s de la classe String n'existe pas\n", tree->u.children[1]->u.str);
					return NIL(Eval);
				}
				if(strcmp(methode->nom, "println")==0){
					buffer = calloc(sizeString(eval_selection->u.str + 1), sizeof(char));
					strcat(buffer,eval_selection->u.str);
					buffer[sizeString(eval_selection->u.str)] = '\n';
					buffer[sizeString(eval_selection->u.str)+1] = '\0';
					/* affichage */
					printf("%s", buffer);
					return makeEvalStr(buffer);
				}
				else if(strcmp(methode->nom, "print")==0){
					printf("%s", eval_selection->u.str);
					return makeEvalStr(eval_selection->u.str);
				}
				else{
					printf("ce n'est ni print, ni println\n");
					return NIL(Eval);
				}
				break;
			case EVAL_INT:
				methode = getMethodeBis(getClasse(listeDeClass, "Integer")->liste_methodes, tree->u.children[1]->u.str);
				if(methode == NULL){
					printf("La methode %s de la classe Integer n'existe pas\n", tree->u.children[1]->u.str);
					return NIL(Eval);
				}
				if(strcmp(methode->nom, "toString") == 0){
					buffer = calloc(sizeInt(eval_selection->u.val), sizeof(char));
					sprintf(buffer, "%d", eval_selection->u.val);
					/* affichage */
					printf("%s\n", buffer);
					return makeEvalStr(buffer);
				}
				else{
					printf("La méthode toString n'est pas appelé -> pb\n");
					return NIL(Eval);
				}
				break;
			case EVAL_PVAR:
				/* Récupération de la méthode du PVAR ID */
				methode = getMethodeBis(eval_selection->u.var->type->liste_methodes, tree->u.children[1]->u.str);
				if(methode == NULL){
					printf("dans evalEnvoiMessage, le ID de la methode de la var est NULL\n");
					return NIL(Eval);
				}
				break;
			default:
				printf("Dans evalEnvoiMessage : selection -> pb sur l'étiquette\n");
				return NIL(Eval);
		}
	}
	/* TODO expr '.' ID '(' ListOptArg ')' */
	else{
		char *buffer;
		EvalP eval_expr = evalExpr(tree->u.children[0], environnement);
		switch(eval_expr->type){
			case EVAL_STR:
				methode = getMethodeBis(getClasse(listeDeClass, "String")->liste_methodes, tree->u.children[1]->u.str);
				if(methode == NULL){
					printf("La methode %s de la classe String n'existe pas\n", tree->u.children[1]->u.str);
					return NIL(Eval);
				}
				if(strcmp(methode->nom, "println")==0){
					buffer = calloc(sizeString(eval_expr->u.str + 1), sizeof(char));
					strcat(buffer,eval_expr->u.str);
					buffer[sizeString(eval_expr->u.str)] = '\n';
					buffer[sizeString(eval_expr->u.str)+1] = '\0';
					/* affichage */
					printf("%s", buffer);
					return makeEvalStr(buffer);
				}
				else if(strcmp(methode->nom, "print")==0){
					printf("%s", eval_expr->u.str);
					return makeEvalStr(eval_expr->u.str);
				}
				else{
					printf("ce n'est ni print, ni println\n");
					return NIL(Eval);
				}
				break;
			case EVAL_INT:
				methode = getMethodeBis(getClasse(listeDeClass, "Integer")->liste_methodes, tree->u.children[1]->u.str);
				if(methode == NULL){
					printf("La methode %s de la classe Integer n'existe pas\n", tree->u.children[1]->u.str);
					return NIL(Eval);
				}
				if(strcmp(methode->nom, "toString") == 0){
					buffer = calloc(sizeInt(eval_expr->u.val), sizeof(char));
					sprintf(buffer, "%d", eval_expr->u.val);
					/* affichage */
					printf("%s\n", buffer);
					return makeEvalStr(buffer);
				}
				else{
					printf("La méthode toString n'est pas appelé -> pb\n");
					return NIL(Eval);
				}

				break;
			case EVAL_PVAR:
				/* Récupération de la méthode du PVAR ID */
				methode = getMethodeBis(eval_expr->u.var->type->liste_methodes, tree->u.children[1]->u.str);
				if(methode == NULL){
					printf("dans evalEnvoiMessage, le ID de la methode de la var est NULL\n");
					return NIL(Eval);
				}
				break;
			default:
				printf("Dans evalEnvoiMessage : expr -> pb sur l'étiquette\n");
				return NIL(Eval);
		}
	}

	if(methode->corps == NULL)	return NIL(Eval);

	/* Evaluation de la liste des arguments avant l'appel de la méthode */
	/* FIXME : peut etre qu'ici l'environnement n'est pas TOUT ! */
	LEvalP evalArg = evalListArg(tree->u.children[2], environnement);

	/* Attribution de leurs evaluations dans la liste des params de la methode */
	LEvalP tmp_eval = evalArg;
	PVAR tmp_param = methode->params;
	while(tmp_eval != NIL(LEval) && tmp_param != NULL){		/* normalement nbEval = nbParam ! */
		switch(tmp_eval->eval->type){
			case EVAL_STR:
				tmp_param->init = makeLeafStr(EVALUE_STR, tmp_eval->eval->u.str);
				break;
			case EVAL_INT:
				tmp_param->init = makeLeafInt(EVALUE_INT, tmp_eval->eval->u.val);
				break;
			case EVAL_PVAR:
				tmp_param->init = makeLeafVar(EVALUE_PVAR, tmp_eval->eval->u.var);
				break;

			/* A ENLEVER NORMALEMENT CA NE DEVRAIT JAMAIS ARRIVE */
			case EVAL_PCLASS:
				tmp_param->init = makeLeafClass(EVALUE_PCLASS, tmp_eval->eval->u.classe);
				break;
			case EVAL_PMETH:
				tmp_param->init = makeLeafMeth(EVALUE_PMETH, tmp_eval->eval->u.methode);
				break;
			case EVAL_TREEP:
				printf("Dans evalInstanciation -> eval d'un arg est un tree\n");
				break;
			default:
				printf("Etiquette non reconnue dans evalInstanciation = %d\n", tmp_eval->eval->type);
				exit(0);
		}
		tmp_eval = tmp_eval->suivant;
		tmp_param = tmp_param->suivant;
	}

	/* TODO ENVIRONNEMENT ? --> mettre tmp_param ????? */

	/* Evaluation du corps de la méthode */
	/* Cas ou le corps de la methode est un bloc */
	if(methode->corps->op == CONTENUBLOC){
		return evalContenuBloc(methode->corps, environnement);
	}
	/* Cas ou le corps de la mthode est une expression */
	else{
		/* On aura un AFFECT expr -> donc il faut allé au fils d'apres */
		return evalExpr(methode->corps->u.children[0], environnement);
	}
}

/* Renvoie un PVAR */
EvalP evalSelection(TreeP tree, PVAR environnement){
	PCLASS classe;
	PVAR var;

	/* IDCLASS'.'ID */
	if(tree->u.children[0]->op == IDENTIFICATEURCLASS){
		classe = getClasse(listeDeClass, tree->u.children[0]->u.str);
		if(classe == NULL){
			printf("Dans evalSelection -> la classe est null (IDCLASS.ID)\n");
			return NIL(Eval);
		}
		var=getVar(classe->liste_champs, tree->u.children[1]->u.str);
		if(var == NULL){
			printf("Dans evalSelection -> la var est null (IDCLASS.ID)\n");
			return NIL(Eval);
		}
	}
	/* envoiMessage'.'ID */
	else if(tree->u.children[0]->op == ENVOIMESSAGE){
		EvalP eval_envoiMsg = evalEnvoiMessage(tree->u.children[0], environnement);
		if(eval_envoiMsg == NIL(Eval)){
			printf("Dans evalSelection -> l'eval de l'envoi de msg est null\n");
			return NIL(Eval);
		}
		switch(eval_envoiMsg->type){
			case EVALUE_INT:
				classe = getClasse(listeDeClass, "Integer");
				break;
			case EVALUE_STR:
				classe = getClasse(listeDeClass, "String");
				break;
			case EVALUE_PVAR:
				classe = eval_envoiMsg->u.var->type;
				break;
			default:
				printf("dans EvalSelection -> etiquette non reconnue pour envoiMessage.ID\n");
				return NIL(Eval);

		}
		if(classe == NULL){
			printf("Dans evalSelection -> la classe est null (envoiMessage.ID)\n");
			return NIL(Eval);
		}
		var=getVar(classe->liste_champs, tree->u.children[1]->u.str);
		if(var == NULL){
			printf("Dans evalSelection -> la var est null (envoiMessage.ID)\n");
			return NIL(Eval);
		}
	}
	/* ID '.' ID */
	else if(tree->u.children[0]->op == IDENTIFICATEUR){
		PVAR var_id = getVar(environnement, tree->u.children[0]->u.str);
		if(var_id == NULL){
			printf("Dans evalSelection -> var_id = NULL (ID.ID)\n");
			return NIL(Eval);
		}
		classe = var_id->type;
		if(classe == NULL){
			printf("Dans evalSelection -> la classe est null (ID.ID)\n");
			return NIL(Eval);
		}
		var=getVar(classe->liste_champs, tree->u.children[1]->u.str);
		if(var == NULL){
			printf("Dans evalSelection -> la var est null (ID.ID)\n");
			return NIL(Eval);
		}
	}
	/* selection '.' ID */
	else if(tree->u.children[0]->op == SELECTION){
		EvalP eval = evalSelection(tree->u.children[0], environnement);
		if(eval == NIL(Eval)){
			return NIL(Eval);
		}
		if(eval->type != EVAL_PVAR){
			printf("Dans evalSelection -> l'eval de la selection n'est pas un PVAR\n");
			return NIL(Eval);
		}
		PVAR var_selection = eval->u.var;
		if(var_selection == NULL){
			printf("Dans evalSelection -> la var de la selection est null\n");
			return NIL(Eval);
		}
		classe = var_selection->type;
		if(classe == NULL){
			printf("Dans evalSelection -> la classe est null (selection.ID)\n");
			return NIL(Eval);
		}
		var=getVar(classe->liste_champs, tree->u.children[1]->u.str);
		if(var == NULL){
			printf("Dans evalSelection -> la var est null (selection.ID)\n");
			return NIL(Eval);
		}
	}
	/* expr '.' ID */
	else{
		EvalP eval_expr = evalExpr(tree->u.children[0], environnement);
		if(eval_expr == NIL(Eval)){
			printf("Dans evalSelection -> l'eval de l'expr est null\n");
			return NIL(Eval);
		}
		switch(eval_expr->type){
			case EVALUE_INT:
				classe = getClasse(listeDeClass, "Integer");
				break;
			case EVALUE_STR:
				classe = getClasse(listeDeClass, "String");
				break;
			case EVALUE_PVAR:
				classe = eval_expr->u.var->type;
				break;
			default:
				printf("dans EvalSelection -> etiquette non reconnue pour expr.ID\n");
				return NIL(Eval);

		}

		if(classe == NULL){
			printf("Dans evalSelection -> la classe est null (expr.ID)\n");
			return NIL(Eval);
		}
		var=getVar(classe->liste_champs, tree->u.children[1]->u.str);
		if(var == NULL){
			printf("Dans evalSelection -> la var est null (expr.ID)\n");
			return NIL(Eval);
		}
	}
	return makeEvalVar(var);
}


PCLASS getType(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  if(courant!=NULL)
  {
    courant = getClasseBis(listeDeClass,courant->nom);
  }
  if(arbre==NULL)
  {
    char* message = NEW(SIZE_ERROR,char);
    sprintf(message,"Arbre est vide");
    pushErreur(message,classActuel,methode,NULL);
    return NULL;
  }
  PCLASS tmpDebug = NULL;

   /* Dans le cas d'une selection, récupérer le dernier élèment */
  PCLASS type = NULL;
  PCLASS type2 = NULL;
  LTreeP liste = NULL;
  bool instCorrecte = FALSE;
  char* message = NEW(SIZE_ERROR,char);
  char *nomC = NULL;
  char * nomClass =NULL;
  PCLASS tmp = NULL;
  switch(arbre->op)
  {
    case MINUSUNAIRE:
    case PLUSUNAIRE:
      type = getType(getChild(arbre,0),arbre,courant,methode,listeDecl);

      if(type!=NULL && strcmp(type->nom,"Integer")!=0)
      {
        sprintf(message,"Erreur plus unaire n'est pas applicable sur %s car ce n'est pas un entier",type->nom);
        pushErreur(message,type,NULL,NULL);
        return NULL;
      }
      else if(type==NULL)
      {
        sprintf(message,"Erreur plus unaire de type");
        pushErreur(message,type,NULL,NULL);
        return NULL;
      }
      else
      {
        return type;
      }
    break;

    case PLUSBINAIRE :
    case MINUSBINAIRE :
    case MULTIPLICATION :
    case DIVISION :

    type = getType(getChild(arbre,0),arbre,courant,methode,listeDecl);
    type2 = getType(getChild(arbre,1),arbre,courant,methode,listeDecl);
    if(equalsType(type,type2)){
      return type;
    }
    else
    {
      if(type!=NULL)
      {
        if(type2!=NULL)
        {
          sprintf(message,"Erreur operation arithmetique entre un objet de type %s et %s",type->nom,type2->nom);
        }
        else
        {
          sprintf(message,"Le type a cote de %s n'est pas reconnu",type->nom);
        }
      }
      else
      {
        sprintf(message,"Erreur de type operation arithmetique");
      }
      pushErreur(message,classActuel,methode,NULL);
      return NULL;
    }

    break;

    case OPCOMPARATEUR :
    type = getType(getChild(arbre,0),arbre,courant,methode,listeDecl);
    type2 = getType(getChild(arbre,1),arbre,courant,methode,listeDecl);
    if(equalsType(type,type2) && (strcmp(type->nom,"Integer")==0 || strcmp(type->nom,"String")==0)){
      return getClasseBis(listeDeClass,"Integer");
    }
    else
    {
      if(type!=NULL)
      {
        if(type2!=NULL)
        {
          sprintf(message,"Erreur operation de comparaison entre un objet de type %s et %s",type->nom,type2->nom);
        }
        else
        {
          sprintf(message,"Le type a cote de %s n'est pas reconnu",type->nom);
        }
      }
      else
      {
        if(type2!=NULL)
        {
          sprintf(message,"Erreur de type operation de comparaison %s ----- %s ",type2->nom,getChild(arbre,0)->u.str);
        }
        else
        {
          sprintf(message,"Erreur de type operation de comparaison");
        }
      }
      pushErreur(message,classActuel,methode,NULL);
      return NULL;
    }
    break;

    case CONCATENATION :

      type = getType(getChild(arbre,0),arbre,courant,methode,listeDecl);
      type2 = getType(getChild(arbre,1),arbre,courant,methode,listeDecl);
      if(type!=NULL && strcmp(type->nom,"String")!=0){
        sprintf(message,"Erreur l'attribut %s n'est pas un string",type->nom);
        pushErreur(message,type,NULL,NULL);
        return NULL;
      }


      if(type2!=NULL && strcmp(type2->nom,"String")!=0){
        sprintf(message,"Erreur l'attribut %s n'est pas un string",type2->nom);
        pushErreur(message,type,NULL,NULL);
        return NULL;
      }

      return type;
      break;

    case SELECTION :
        liste = transFormSelectOuEnvoi(arbre,liste);
        return estCoherentEnvoi(liste, courant, methode,listeDecl);
    break;

    case ENVOIMESSAGE :
        liste = transFormSelectOuEnvoi(arbre,liste);
        return estCoherentEnvoi(liste, courant, methode,listeDecl);
      break;

    case CSTENTIER:
        return getClasseBis(listeDeClass,"Integer");
      break;

    case CSTSTRING:
        return getClasseBis(listeDeClass,"String");
      break;

    case IDENTIFICATEUR:
      if(strcmp(arbre->u.str,"void")==0)
      {
        return getClasseBis(listeDeClass,"Void");
      }
      if(strcmp(arbre->u.str,"this")==0)
      {
        if(courant!=NULL)
        {
          return getClasseBis(listeDeClass,courant->nom);
        }
        else
        {
          return NULL;
        }
      }
      if(strcmp(arbre->u.str,"super")==0 && ancien!=NULL && ancien->op==ETIQUETTE_YIELD && ancien->op!=ENVOIMESSAGE && ancien->op!=SELECTION)
      {
        sprintf(message,"super n'est pas utilisable dans le yield ! ");
        pushErreur(message,type,NULL,NULL);
        return NULL;

      }
      tmpDebug = getTypeAttribut(arbre->u.str, courant, methode, listeDecl,FALSE,FALSE);

      return tmpDebug;
    break;

    case IDENTIFICATEURCLASS:
      return getClasseBis(listeDeClass,arbre->u.str);
    break;

    case CONTENUBLOC:
        return getType(getChild(arbre,1),arbre,courant,methode,listeDecl);
      break;

    case LIST_INSTRUCTION:
        return getType(getChild(arbre,1),arbre,courant,methode,listeDecl);
      break;

    case INSTANCIATION :
        nomClass = getChild(arbre,0)->u.str;
        tmp = getClasseBis(listeDeClass,nomClass);
        if(tmp == NULL)
        {
          sprintf(message,"Erreur d'instanciation : %s n'existe pas",nomClass);
          pushErreur(message,type,NULL,NULL);
        }
        else
        {
          nomC = calloc(100,sizeof(char));
          sprintf(nomC,"constructeur %s",nomClass);

          instCorrecte = compareParametreMethode(tmp->param_constructeur,getChild(arbre,1),courant,methode,listeDecl,nomC);
          if(!instCorrecte)
          {
            sprintf(message,"Erreur d'instanciation : %s mal appelee",nomClass);
            pushErreur(message,type,NULL,NULL);
          }
          else
          {
            return tmp;
          }
        }
      break;

    default :
    printf("L'etiquette %d n'a pas ete gerer\n", arbre->op);

  }
  return NULL;
}

PCLASS estCoherentEnvoi(LTreeP liste, PCLASS classe, PMETH methode, PVAR listeDecl)
{
  LTreeP tmp = liste;
  
  PCLASS init = NULL;

  bool isStatic = FALSE;
  bool agerer = FALSE;
  char* message = NEW(SIZE_ERROR,char);

  if(liste!=NULL || tmp->elem!=NULL)
  {
    if(methode!=NULL && classe!=NULL)
    {
      sprintf(message,"Erreur la methode %s est mal forme - Classe : %s",methode->nom,classe->nom);
    }
    else
    {
      sprintf(message,"Erreur envoi de message");
    }
  }
  if(tmp->elem->op == IDENTIFICATEUR)
  {
      agerer = TRUE;
      if(classe!=NULL && (strcmp(tmp->elem->u.str,"super")==0))
      {
        if(methode!=NULL && methode->isStatic)
        {
            sprintf(message,"Erreur super present dans une methode statique");
            pushErreur(message,classe,methode,NULL);
            return NULL;
        }
        else
        {
            init = classe->classe_mere;
        }
      }
      else if(classe!=NULL && (strcmp(tmp->elem->u.str,"this")==0))
      {
        if(methode!=NULL && methode->isStatic)
        {
          sprintf(message,"Erreur this present dans une methode statique");
          pushErreur(message,classe,methode,NULL);
          return NULL;
        }
        else
        {
          init = classe;
        }
      }
      else
      {
          /*si il y a this ou super dans le bloc main -> erreur*/
          if((classe==NULL) && ((strcmp(tmp->elem->u.str,"this")==0)||(strcmp(tmp->elem->u.str,"super")==0)))
          {
            sprintf(message,"Erreur this ou super present dans le bloc main");
            pushErreur(message,classe,methode,NULL);
            return NULL;
          }
          else
          {
            /*ici true*/
            if(dansCheckBlocMain)
            {
              init = getTypeAttribut(tmp->elem->u.str, classe, methode, listeDecl, FALSE, FALSE);
            }
            else
            {
              init = getTypeAttribut(tmp->elem->u.str, classe, methode, listeDecl, FALSE, FALSE);
            }
          }
      }
      if(init == NULL)
      {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"%s inconnu ",tmp->elem->u.str);
          pushErreur(message,classe,methode,NULL);
          return NULL;
      }

  }
  else if(tmp->elem->op == IDENTIFICATEURCLASS)
  {        
    isStatic = TRUE;

        agerer = TRUE;
        init = getClasseBis(listeDeClass, tmp->elem->u.str);
        if(init==NULL)
        {
          char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Inconnu %s",tmp->elem->u.str);
          pushErreur(message,classe,methode,NULL);
          return NULL;
        }
  }
  else if(tmp->elem->op == INSTANCIATION)
  {
    agerer = TRUE;
      char * nomClass = getChild(tmp->elem,0)->u.str;
      PCLASS tmp = getClasseBis(listeDeClass,nomClass);
      if(tmp == NULL)
      {
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"La classe %s n'est pas declare ",nomClass);
        pushErreur(message,classe,methode,NULL);
        return NULL;
      }
      else
      {
        init = tmp;
      }


  }
  else if(tmp->elem->op == CSTSTRING)
  {
    init = getClasseBis(listeDeClass,"String");
  }
  else if(tmp->elem->op == CSTENTIER)
  {
    init = getClasseBis(listeDeClass,"Integer");
  }
  else if(tmp->elem->recupType==1)
  {
      init = getType(tmp->elem,NULL,classe,methode,listeDecl);
  }
    short etiquette = tmp->elem->op;
    PCLASS tempoAffiche;
    while(tmp!=NULL)
    {
        tmp = tmp->suivant;
        if(tmp == NULL)
        {
            break;
        }
        TreeP tmpElem = tmp->elem;
        tempoAffiche = init;
        if(tmp->elem->op == IDENTIFICATEURCLASS || tmp->elem->op == INSTANCIATION || strcmp(tmp->elem->u.str,"super")==0 || strcmp(tmp->elem->u.str,"this")==0 || tmp->elem->op == CSTENTIER || tmp->elem->op ==CSTSTRING)
        {
            char* message = NEW(SIZE_ERROR,char);
            sprintf(message," Identificateur de classe, instanciation, super ou this en plein milieu : %s",tmp->elem->u.str);
            pushErreur(message,classe,methode,NULL);
            return NULL;
        }

        if(tmp->elem->suivant!=NULL || tmp->elem->isEnvoiMessage==TRUE)
        {
            if(init==NULL)
            {
               sprintf(message,"EnvoiMessage : il y a un probleme au niveau de celui ci");
               pushErreur(message,classe,methode,NULL);
               return NULL;
            }
            else
            {
              init = appartient(init,tmpElem,TRUE,methode,listeDecl,tmp,etiquette,isStatic,agerer);
            }
            if(init==NULL)
            {
                char* message = NEW(SIZE_ERROR,char);
                if(tempoAffiche == NULL)
                {
                  sprintf(message,"EnvoiMessage : il y a un probleme au niveau de celui ci");
                }

                else
                {
                  sprintf(message,"Erreur la methode %s n'appartient pas a %s. Veuillez verifier la classe",tmp->elem->u.str,tempoAffiche->nom);

                }
                pushErreur(message,classe,methode,NULL);
                return NULL;
            }
                tempoAffiche = init;

        }
        else if(tmp->elem->isEnvoiMessage==FALSE)
        {
            init = appartient(init,tmpElem,FALSE,methode,listeDecl,tmp,etiquette, isStatic,agerer);
            if(init == NULL)
            {
                char* message = NEW(SIZE_ERROR,char);
                if(tmp->elem == NULL)
                {
                  sprintf(message,"Probleme envoie de message");
                }
                else if(tmp->elem != NULL && tempoAffiche != NULL)
                {
                  sprintf(message,"Erreur la varibable  %s n'appartient pas a %s. Veuillez verifier la classe",tmp->elem->u.str,tempoAffiche->nom);
                }

                pushErreur(message,classe,methode,NULL);
                return NULL;

            }
            tempoAffiche = init;
        }
        etiquette = tmp->elem->op;
    }
    return init;
}



PCLASS appartient(PCLASS mere, TreeP fille, bool isEnvoiMessage, PMETH methode, PVAR listeDecl, LTreeP tmp,short etiquette, bool isStatic, bool agerer)
{
  if(mere!=NULL)
  {
    mere = getClasseBis(listeDeClass,mere->nom);
  }

  if(fille==NULL || mere==NULL){
    char* message = NEW(SIZE_ERROR,char);
    sprintf(message,"fonction tp.c appartient : Erreur Arbre");
    pushErreur(message,mere,methode,NULL);
    return NULL;
  }
  if(isEnvoiMessage)
  {

    PMETH listMeth = NULL;

    if(mere->liste_methodes!=NULL)
    {
      SMETH copieConforme = *mere->liste_methodes;
      listMeth = NEW(1,SMETH);
      *listMeth = copieConforme;
    }
    else
    {
        listMeth = mere->liste_methodes;
    }

    bool isVerifOk = FALSE;
    while(listMeth!=NULL)
    {
      if(strcmp(listMeth->nom,fille->u.str)==0)
      {
        /* Verifie si les parametre de de listMeth->param & fille */
       isVerifOk = getTypeMethode(listMeth->nom,mere,etiquette,tmp->elem->suivant,methode,listeDecl,isStatic)!=NULL?TRUE:FALSE;

        if(isVerifOk)
        {

          return listMeth->typeRetour;
        }
        else
        {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"Erreur la methode %s est mal appele ou n'existe pas",tmp->elem->u.str);
          pushErreur(message,mere,listMeth,NULL);
          return NULL;
        }
      }
      listMeth = listMeth->suivant;
    }
    char* message = NEW(SIZE_ERROR,char);
    sprintf(message,"Erreur la methode %s n'a pas ete trouvee",tmp->elem->u.str);
    pushErreur(message,mere,methode,NULL);
    return NULL;
  }
  else
  {
      if(dansCheckBlocMain)
      {
        return getTypeAttribut(tmp->elem->u.str, mere, methode, NULL,isStatic, agerer);
      }
      else
      {
        return getTypeAttribut(tmp->elem->u.str, mere, methode, listeDecl,isStatic, agerer);
      }
  }
}

/* Retourne le type d'une varibable suivant les paramètre de la méthode,
* de la classe et de la liste de déclaration
*/
PCLASS getTypeAttribut(char* nom, PCLASS classe, PMETH methode, PVAR listeDecl, bool isStatic, bool agerer){

  if(classe!=NULL)
  {
    classe = getClasseBis(listeDeClass,classe->nom);
  }
  if(nom!=NULL && nom[0]=='"')
  {
    return getClasseBis(listeDeClass,"String");
  }
  if(nom!=NULL && atoi(nom)!=0)
  {
    return getClasseBis(listeDeClass,"Integer");
  }

  bool estDansParamMeth = FALSE;
  bool estDansListeDecl =  FALSE;
  bool estDansAttributClasse = FALSE;
  PCLASS res = NULL;

  if(methode != NULL)
  {

    PVAR paramParcours = methode->params;
    char** variable = calloc(1,sizeof(char*));
    int i = 0;
    while(paramParcours!=NULL)
    {
      variable[i] = calloc(30,sizeof(char));
      strcpy(variable[i],paramParcours->nom);
      i++;
      paramParcours = paramParcours->suivant;
    }
    /*
     * Si on trouve deux variable ayant le meme nom (FALSE)
     */
    if(!checkDoublon(variable,i-1))
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur doublons dans les parametre de la methode %s",methode->nom);
      pushErreur(message,classe,methode,NULL);
      return FALSE;
    }
    int j = 0;
    for (j = 0; j <= i-1 ; j++)
    {
      /*free(variable[j]);*/
    }
    /*free(variable);*/


    PVAR param = methode->params;

    while(param!=NULL)
    {
      if(strcmp(nom,param->nom)==0)
      {
        estDansParamMeth = TRUE;
        SCLASS copie = *param->type;
        PCLASS pointeurCopie = NEW(1,SCLASS);
        *pointeurCopie = copie;
        res = pointeurCopie;
        res->suivant = NULL;
        break;
      }

    param = param->suivant;
    }
  }

  if(listeDecl!=NULL)
  {

    PVAR listDeclParcours = listeDecl;
    char** variable = calloc(1,sizeof(char*));
    int i = 0;
    while(listDeclParcours!=NULL)
    {
      variable[i] = calloc(30,sizeof(char));
      strcpy(variable[i],listDeclParcours->nom);
      i++;
      listDeclParcours = listDeclParcours->suivant;
    }
    /*
     * Si on trouve deux variable ayant le meme nom (FALSE)
     */



    if(!checkDoublon(variable,i-1))
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur doublons dans la liste de declaration");
      pushErreur(message,classe,methode,NULL);
      return FALSE;
    }

    int j = 0;
    for (j = 0; j <= i-1 ; j++)
    {
      /*free(variable[j]);*/
    }
    /*free(variable);*/

    PVAR listDeclaration = listeDecl;

    while(listDeclaration!=NULL)
    {
      if(strcmp(nom,listDeclaration->nom)==0 && estDansParamMeth==TRUE)
      {
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Erreur l'attribut %s est redeclaree 1",nom);
        pushErreur(message,classe,methode,NULL);
        return NULL;
      }
      else if(strcmp(nom,listDeclaration->nom)==0 && estDansParamMeth==FALSE){
        estDansListeDecl = TRUE;
        if(listDeclaration->type==NULL)
        {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"Type inconnu");
          pushErreur(message,classe,methode,NULL);
          return NULL;
        }
        SCLASS copie = *listDeclaration->type;
        PCLASS pointeurCopie = NEW(1,SCLASS);
        *pointeurCopie = copie;
        res = pointeurCopie;
        res->suivant = NULL;
        break;
      }
      listDeclaration = listDeclaration->suivant;
    }
  }

  if(classe!=NULL && classe->liste_champs != NULL)
  {
    PVAR listeClasseParcours = classe->liste_champs;
    char** variable = calloc(1,sizeof(char*));
    int i = 0;

    while(listeClasseParcours!=NULL)
    {
      variable[i] = calloc(30,sizeof(char));
      strcpy(variable[i],listeClasseParcours->nom);
      i++;
      listeClasseParcours = listeClasseParcours->suivant;
    }
    /*
     * Si on trouve deux variable ayant le meme nom (FALSE)
     */
    if(!checkDoublon(variable,i-1))
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur doublons dans les attribut de la classe %s",classe->nom);
      pushErreur(message,classe,methode,NULL);
      return FALSE;
    }
    int j = 0;
    for (j = 0; j <= i-1 ; j++)
    {
      /*free(variable[j]);*/
    }
    /*free(variable);*/
   PVAR listeClasse = classe->liste_champs;
    while(listeClasse!=NULL)
    {
      if(strcmp(nom,listeClasse->nom)==0 && (estDansListeDecl==TRUE))
      {
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Erreur l'attribut %s est redeclaree %s",nom,classe->nom);
        pushErreur(message,classe,methode,NULL);
        return NULL;
      }
      else if(strcmp(nom,listeClasse->nom)==0 && estDansListeDecl==FALSE){
        if(!agerer)
        {
          if(methode!=NULL && methode->isStatic && listeClasse->categorie!=CATEGORIE_STATIC)
          {
            char* message = NEW(SIZE_ERROR,char);
            sprintf(message,"this implicite dans %s",methode->nom);
            pushErreur(message,classe,methode,NULL);
            return NULL;
          }
          estDansAttributClasse = TRUE;
          SCLASS copie = *listeClasse->type;
          PCLASS pointeurCopie = NEW(1,SCLASS);
          *pointeurCopie = copie;
          res = pointeurCopie;
          res->suivant = NULL;
          break;
        }
        else if((listeClasse->categorie==CATEGORIE_STATIC && isStatic)||(listeClasse->categorie!=CATEGORIE_STATIC && !isStatic))
        {
          estDansAttributClasse = TRUE;
          SCLASS copie = *listeClasse->type;
          PCLASS pointeurCopie = NEW(1,SCLASS);
          *pointeurCopie = copie;
          res = pointeurCopie;
          res->suivant = NULL;
          break;
        }
        else
        {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"Erreur l'attribut %s n'a pas le droit d'etre utilise de cette maniere",nom);
          pushErreur(message,classe,methode,NULL);
          return NULL;
        }
      }
      listeClasse = listeClasse->suivant;
    }
  }
  return res;
}

/*
 * Si VRAI : aucun doublon
 */
bool checkDoublon(char** variable,int n)
{
  int i = 0;
  int j = 0;
  for (i = 0; i <= n; i++)
  {
    for (j = i+1; j <= n; j++)
    {
      if(variable[i]!=NULL && variable[j]!=NULL && strcmp(variable[i],variable[j])==0)
      {
        char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 24");
        return FALSE;
      }
    }
  }
  return TRUE;
}

PCLASS getTypeMethode(char * nom, PCLASS classe, short precedant, TreeP appelMethode, PMETH methode, PVAR listeDecl, bool isStatic)
{
  if(classe!=NULL)
  {
    classe = getClasseBis(listeDeClass,classe->nom);
  }
  if(classe == NULL || nom == NULL){
    char* message = NEW(SIZE_ERROR,char);
    sprintf(message,"tp.c getTypeMethode : Erreur Arbre");
    pushErreur(message,classe,methode,NULL);
    return NULL;
  }

  PMETH tmp = classe->liste_methodes;
  while(tmp!=NULL)
  {
    if(precedant == IDENTIFICATEURCLASS)
    {
      if(strcmp(nom,tmp->nom)==0 && tmp->isStatic && isStatic)
      {
        bool verifOk = compareParametreMethode(tmp->params,appelMethode, classe,methode, listeDecl, nom);
        if(verifOk)
        {
          return tmp->home;
        }
      }
    }
    else if(precedant == IDENTIFICATEUR || precedant==INSTANCIATION)
    {
      if(strcmp(nom,tmp->nom)==0 && !tmp->isStatic && !isStatic)
      {
        bool verifOk = compareParametreMethode(tmp->params,appelMethode, classe,methode, listeDecl, nom);
        if(verifOk)
        {
          return tmp->home;
        }
      }
    }
    else if(precedant == CSTSTRING || precedant == CSTENTIER)
    {
      bool verifOk = compareParametreMethode(tmp->params,appelMethode, classe,methode, listeDecl, nom);
      if(verifOk)
      {
        return tmp->home;
      }
    }
    else
    {
      bool verifOk = compareParametreMethode(tmp->params,appelMethode, classe,methode, listeDecl, nom);
      if(verifOk)
      {
        return tmp->home;
      }
    }

    tmp = tmp->suivant;
  }
  char *message = calloc(SIZE_ERROR,sizeof(char));
  sprintf(message,"La methode de classe ou statique %s est mal appelee",nom);
  pushErreur(message,NULL,NULL,NULL);
  return NULL;
}


bool compareParametreMethode(PVAR declaration,TreeP appelMethode, PCLASS classe, PMETH methode, PVAR listeDecl, char*nom)
{
  if(classe!=NULL)
  {
    classe = getClasseBis(listeDeClass,classe->nom);
  }
  if((appelMethode==NULL && declaration!=NULL)||(appelMethode!=NULL && declaration==NULL))
  {
   char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 26");
    return FALSE;
  }
  else if (appelMethode==NULL && declaration==NULL)
  {
    return TRUE;
  }

  /*Transformer a->b->c*/
  PCLASS liste = NULL;

  liste = transformerAppel(appelMethode,liste,classe,methode,listeDecl);

  if(liste==NULL)
  {
    char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 29");
    return FALSE;
  }

  PCLASS tmp = liste;
  PVAR tmpDeclarationOfficiel = declaration;

  if(tmp==NULL && tmpDeclarationOfficiel==NULL)
  {
    return TRUE;
  }
  SCLASS contenuTMP = *tmp;
  PCLASS tmp2 = NEW(1,SCLASS);
  *tmp2 = contenuTMP;
  int cpt = 0;

  while(tmp2!=NULL)
  {
    cpt++;
    tmp2 = tmp2->suivant;
  }

  SVAR contenuDeclaration = *tmpDeclarationOfficiel;
  PVAR tmpDeclarationOfficiel2 = &contenuDeclaration;
  int cptDeclaration = 0;
  while(tmpDeclarationOfficiel2!=NULL)
  {
    cptDeclaration++;
    tmpDeclarationOfficiel2 = tmpDeclarationOfficiel2->suivant;
  }

  if(cpt!=cptDeclaration)
  {
    char* message = NEW(SIZE_ERROR,char);
    sprintf(message,"Erreur la methode %s attend %d parametre(s) et vous lui avez passez %d parametre(s)",nom,cptDeclaration,cpt);
    pushErreur(message,NULL,NULL,NULL);
    return FALSE;
  }

  while(tmp!=NULL)
  {
    if(tmpDeclarationOfficiel==NULL)
    {
      return FALSE;
    }

    if(!equalsType(tmpDeclarationOfficiel->type,tmp))
    {
      return FALSE;
    }

    tmpDeclarationOfficiel = tmpDeclarationOfficiel->suivant;
    tmp = tmp->suivant;
  }
  return TRUE;

}

PCLASS transformerAppel(TreeP appelMethode,PCLASS liste, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  if(courant!=NULL)
  {
    courant = getClasseBis(listeDeClass,courant->nom);
  }
  if(liste==NULL)
  {
    if(appelMethode->op!=LISTEARG)
    {
      PCLASS getTypeRetour = getType(appelMethode,appelMethode, courant, methode, listeDecl);
      if(getTypeRetour==NULL)
      {
        char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 3");
        pushErreur(message,NULL,NULL,NULL);
        return NULL;
      }
      getTypeRetour = getClasseBis(listeDeClass,getTypeRetour->nom);
      return getTypeRetour;
    }
    else
    {
      PCLASS getTypeRetour = getType(getChild(appelMethode,1),appelMethode, courant, methode, listeDecl);
    
      if(getTypeRetour==NULL)
      {
        char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 4");
        pushErreur(message,NULL,NULL,NULL);
        return NULL;
      }
      getTypeRetour = getClasseBis(listeDeClass,getTypeRetour->nom);

      liste = getTypeRetour;
    }
  }
  else
  {
    if(appelMethode->op!=LISTEARG)
    {
      SCLASS tmp = *liste;
      PCLASS getTypeRetour = getType(appelMethode,NULL, courant, methode, listeDecl);

      if(getTypeRetour!=NULL)
      {
        getTypeRetour = getClasseBis(listeDeClass,getTypeRetour->nom);
        liste = getTypeRetour;
        liste->suivant = NEW(1,SCLASS);
        *liste->suivant = tmp;
        
        return liste;
      }
      else
      {
        return NULL;
      }
    }
    else
    {
      SCLASS tmp = *liste;

      PCLASS getTypeRetour = getType(getChild(appelMethode,1),appelMethode, courant, methode, listeDecl);
      if(getTypeRetour!=NULL)
      {
        getTypeRetour = getClasseBis(listeDeClass,getTypeRetour->nom);
        liste = getTypeRetour;
        liste->suivant = NEW(1,SCLASS);
        *liste->suivant = tmp;
      }
      else
      {
        return NULL;
      }

    }
  }
  return transformerAppel(getChild(appelMethode,0),liste,courant,methode,listeDecl);
}


/* Cree une liste chainee lorsqu'il y a une selection ou un envoi de message */

LTreeP transFormSelectOuEnvoi(TreeP arbre, LTreeP liste)
{

  if(liste==NULL){

  liste = NEW(1,struct _listeTree);
  liste->elem = getChild(arbre,1);
    if(arbre->nbChildren == 3)
    {
      liste->elem->suivant = getChild(arbre,2);
      liste->elem->isEnvoiMessage = TRUE;
    }
    else
    {
      liste->elem->isEnvoiMessage = FALSE;
    }
  }
  else
  {
    listeTree tmp = *liste;

    if(arbre->nbChildren==0)
    {
      return liste;
    }
    liste->elem = getChild(arbre,1);
    liste->suivant = NEW(1,listeTree);
    *liste->suivant = tmp;
    if(arbre->nbChildren == 3)
    {
      liste->elem->suivant = getChild(arbre,2);
      liste->elem->isEnvoiMessage = TRUE;
    }
    else
    {
      liste->elem->isEnvoiMessage = FALSE;
    }
  }
  /*aminamin*/
  if(getChild(arbre,0)->op==IDENTIFICATEUR || getChild(arbre,0)->op==IDENTIFICATEURCLASS || getChild(arbre,0)->op==CSTSTRING || getChild(arbre,0)->op==CSTENTIER)
  {
    if(liste!=NULL)
    {
      listeTree tmp = *liste;
      liste->elem = getChild(arbre,0);
      liste->suivant = NEW(1,listeTree);
      *liste->suivant = tmp;
    }
    return liste;
  }

  else if(getChild(arbre,0)->op==SELECTION || getChild(arbre,0)->op==ENVOIMESSAGE)
  {

    return transFormSelectOuEnvoi(getChild(arbre,0),liste);
  }
  else{
    if(liste!=NULL)
    {
      listeTree tmp = *liste;
      liste->elem = getChild(arbre,0);
      liste->elem->recupType = 1;
      liste->suivant = NEW(1,listeTree);
      *liste->suivant = tmp;
    }
    return liste;
  }

}

LTreeP transformeParam(TreeP arbre, LTreeP liste)
{
  if(liste==NULL){
    liste->elem = getChild(arbre,1);
  }
  else{
    listeTree tmp = *liste;
    liste->elem = getChild(arbre,1);
    liste->suivant = NEW(1,listeTree);
    *liste->suivant = tmp;
  }

  if(getChild(arbre,0)->op==IDENTIFICATEUR)
  {
    if(liste!=NULL)
    {
    listeTree tmp = *liste;
    liste->elem = getChild(arbre,0);
    liste->suivant = NEW(1,listeTree);
    *liste->suivant = tmp;
    }

    return liste;
  }
  return transformeParam(getChild(arbre,0),liste);
}

bool equalsType(PCLASS gauche, PCLASS droite)
{
  if(gauche!=NULL)
  {
    gauche = getClasseBis(listeDeClass,gauche->nom);
  }
  if(droite!=NULL)
  {
    droite = getClasseBis(listeDeClass,droite->nom);
  }
  if(gauche==NULL)
  {
    char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 31");
    return FALSE;
  }
  if(droite==NULL){
    char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 32");
    return FALSE;
  }
  if(strcmp(gauche->nom,droite->nom)==0)
  {
    return TRUE;
  }
  else
  {
    if(!droite->isExtend)
    {
      return FALSE;
    }
    if(equalsType(gauche,droite->classe_mere))
    {
      return TRUE;
    }
    else
    {
      return isHeritage(gauche,droite->classe_mere);
    }
  }
  return FALSE;
}

bool isHeritage(PCLASS gauche, PCLASS droite)
{
  if(!droite->isExtend)
  {
    return (strcmp(gauche->nom,droite->nom)==0);
  }
  else
  {
    return isHeritage(gauche,droite->classe_mere);
  }
}



/*
 * Methode pour imprimer toute l'arbre
 */

void printTree(TreeP tree)
{
/* if (! verbose ) return;*/
printf("Etiquette %d\n",tree->op);
 switch (tree->op) {
  case LISTCLASS:printClasse(tree->u.classe); break;
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

void afficheListeErreur(ErreurP listeE)
{
  if(listeE==NULL)
  {
    printf("Aucune erreur : OK\n");
    return;
  }
  else
  {
    ErreurP tmp = listeE;
    int i = 1;
    while(tmp!=NULL)
    {
     printf("Erreur %d : %s\n",i,tmp->message);
     printf(BLACK);
      tmp = tmp->suivant;
      i++;
    }
  }
}
/*
 * True : si la classe n'a pas des attributs en doublons
 * False : NOK
 */
bool verifAttributClasse(PCLASS classe)
{
  if(classe->liste_champs==NULL)
  {
    return TRUE;
  }

  PVAR tmp = classe->liste_champs;
  PVAR reste = tmp->suivant;
  while(tmp!=NULL)
  {
    reste = tmp->suivant;

    while(reste!=NULL)
    {
      if(strcmp(reste->nom,tmp->nom)==0)
      {
        char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 33");
        return FALSE;
      }
      reste = reste->suivant;
    }
    tmp = tmp->suivant;
  }

  return TRUE;
}


void testEval(){
/*
 * Creation d'arbres bidule de teste Exemple :  x:= a + b
 */
 /*int a=8,b=6;
TreeP treeTestPLUS = makeTree(PLUSBINAIRE, 2,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b));
TreeP treeTestMINUS = makeTree(MINUSBINAIRE, 2,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b));
TreeP treeTestDIV = makeTree(DIVISION, 2,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b));
TreeP treeTestMUL = makeTree(MULTIPLICATION, 2,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b));
TreeP treeTestNE = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,NE));
TreeP treeTestEQ = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,EQ));
TreeP treeTestGT = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,GT));
TreeP treeTestGE = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,GE));
TreeP treeTestLT = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,LT));
TreeP treeTestLE = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,LE));*/
/*  a-b+44 */
/*TreeP treeTestPLUSImbrique = makeTree(PLUSBINAIRE, 2,  treeTestMINUS, makeLeafInt(CSTENTIER,44));*/
/*
 * Creation d'arbres bidule de teste Exemple :  x:= "yas"&"ser";
 */
/*char* ch1="yas"; char* ch2="ser"; char* ch3=" RABI";
TreeP treeTestCONCATENATION = makeTree(CONCATENATION, 2,  makeLeafStr(CSTSTRING,ch1), makeLeafStr(CSTSTRING,ch2));
TreeP treeTestCONCATENATIONImbrique = makeTree(CONCATENATION, 2, treeTestCONCATENATION, makeLeafStr(CSTSTRING,ch3));*/
/* Creation d'arbre avec envirenoment pour les identificateurs */


/*printf("============= DEB Eval TEST =============\n");
pprintTreeMain(treeTestCONCATENATION);
printf("Resultat treeTestPLUS = %d\n", getVal(evalExpr(treeTestPLUS,NULL)));
printf("Resultat treeTestMINUS = %d\n", getVal(evalExpr(treeTestMINUS,NULL)));
printf("Resultat treeTestDIV = %d\n", getVal(evalExpr(treeTestDIV,NULL)));
printf("Resultat treeTestMUL = %d\n", getVal(evalExpr(treeTestMUL,NULL)));
printf("Resultat treeTestNE = %d\n", getVal(evalExpr(treeTestNE,NULL)));
printf("Resultat treeTestEQ = %d\n", getVal(evalExpr(treeTestEQ,NULL)));
printf("Resultat treeTestGT = %d\n", getVal(evalExpr(treeTestGT,NULL)));
printf("Resultat treeTestGE = %d\n", getVal(evalExpr(treeTestGE,NULL)));
printf("Resultat treeTestLT = %d\n", getVal(evalExpr(treeTestLT,NULL)));
printf("Resultat treeTestLE = %d\n", getVal(evalExpr(treeTestLE,NULL)));
printf("Resultat treeTestCONCATENATION = %s\n", getVal(evalExpr(treeTestCONCATENATION,NULL)));
printf("Resultat treeTestPLUSImbrique = %d\n", getVal(evalExpr(treeTestPLUSImbrique,NULL)));
printf("Resultat treeTestCONCATENATIONImbrique = %s\n", getVal(evalExpr(treeTestCONCATENATIONImbrique,NULL)));

printf("============= FIN Eval TEST =============\n");*/

/*EvalP res;
    if(tree->nbChildren==1) {res = evalExpr(tree->u.children[0],NULL);}
    else {
  printf("===========================================> Nbchildren est = %d \n",tree->nbChildren);
    res = evalExpr(tree->u.children[1],NULL);
  printf("===========================================> Nbchildren est = %d \n",tree->nbChildren);
      }
  printf("Valeur EXPR = %d\n", res->u.val);
    */


}
