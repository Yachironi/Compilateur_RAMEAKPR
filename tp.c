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
  if (programme == NULL) {
  printf("Programme est NULL");
  }else{
printf("=======================\n");
printf("Affichage de l'arbre : \n");
printTree(programme);
printf("=======================\n");}
  if (fd != NIL(FILE)) fclose(fd);
  if (res == 0 && errorCode == NO_ERROR) return 0;
  else {
    int res2 = res ? SYNTAX_ERROR : errorCode;
    printf("Error in file. Kind of error: %d\n", res2); 
    return res2;
  }

  printf("=======================\n");
printf("Affichage de l'arbre : \n");
printTree(programme);
printf("=======================\n");
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
  res->suivant=NIL(SCLASS); /* verifier si ça ne pose pas de pb */
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
  res->suivant = NIL(SMETH);  /* verifier si ça ne pose pas de pb */
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
  res->suivant=NIL(SVAR); /* verifier si ça ne pose pas de pb */
  res->nom=nom;
  res->type=type;
  res->init=init;
  res->categorie=cat; /* si cat=0 ==> var non static. si cat=1 ==> var static */
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
  
  nouvelle->classe = *classe;
  nouvelle->methode = *methode;
  nouvelle->variable = *variable;
  
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


/*
* Fonction permettant le parcours recursif de l'arbre
*/

bool parcourRecursifArbre(TreeP prog){
  if(prog==NULL) return TRUE;
  TreeP tmp = prog; 
  
  switch(tmp->op){
    case PROGRAM :
      bool droite = f(getChild(tmp,1), tmp->op, NULL); 
      bool gauche = f(getChild(tmp,0), tmp->op, NULL);
      
      bool rec1 = parcourRecursifArbre(getChild(tmp,0));
      bool rec2 = parcourRecursifArbre(getChild(tmp,1));
        
      return droite && gauche && rec1 && rec2;
      break; 
  }

}

bool f(TreeP tree,short etiquette,PVAR listeVar){
  
  switch(etiquette){
    case PROGRAM :
      PVAR decl;
      if(getChild(tree,0)!=NULL)
      {
        decl = getChild(tree,0)->u.var;
      }
      else{
        decl = NULL;
      }
      /* Vérifier que la liste d'instruction est bien correcte */
      if(decl==NULL)
      {
        if(getChild(tree,1)==NULL)
        {
          if(getChild(tree,2)==NULL)
          {
            return TRUE;
          }
          else
          {
            return checkExpr(getChild(getChild(tree,2),0),listeVar, tree->op);
          }
        }
        else
        {
          return checkListInstr(getChild(tree,1),listeVar,tree->op);
        }
      }
      else
      {
        if(getChild(tree,1)==NULL) 
        {
          return FALSE;
        }
        else
        {
          /* Si dans checkExpr ou dans checkList l'arbre est null, on retourne TRUE*/
          bool res = checkExpr(getChild(getChild(tree,2),0),listeVar,tree->op);
          return checkListInstr(getChild(tree,1),listeVar,tree->op) && res;
        }
      }
    break;

    case LISTCLASS : 
        bool res1 = checkListClassBloc(getChild(tree,0),listeVar,tree->op);
        //bool res2 = checkListClassBloc(getChild(tree,0),listeVar,tree->op);
    break;

    default : 
      return FALSE;
  }
}

bool checkListClassBloc(TreeP tree, PVAR listeVar, short eti){
  /* méthode amine */
  /* Test de blocOpt */
  if(tree==NULL)
  {
    retrun TRUE;
  }
  PCLASS tempClass = tree->u.classe;

  PVAR tmpVarParam = tempClass->param_constructeur;
  PVAR tmpListChamp = tempClass->liste_champs;          
  PVAR tmpHerite = tempClass->champs_herite;   
  PVAR fusion;
  
  if(tmpVarParam==NULL)
  { 
    if(tmpListChamp!=NULL)
    {
      fusion = tmpListChamp; 
      fusion->suivant = tmpHerite;
    }
    else{
      fusion = tmpHerite;
    } 

  else 
  {
    fusion = tmpVarParam;
    
    if(tmpListChamp!=NULL)
    {
      fusion = tmpListChamp; 
      fusion->suivant = tmpHerite;
    }
    else
    {
      fusion = tmpHerite;
    }
  }
  

  /* Vérification de blocOPT, les variables doivent être présente dans la liste fusion 
  * Appeler la methode qui regarde un bloc, on lui passant en paramètre fusion
  */ 
  checkBloc(tempClass->corps_constructeur, fusion);
}

bool checkBloc(TreeP CorpBloc, PVAR listeVar){

TreeP tmp = CorpBloc;

if(getChild(tmp,0)==NULL)
      {
        if(getChild(tmp,1)==NULL)
        {
          if(getChild(tmp,2)==NULL)
          {
            return TRUE;
          }
          else
          {
            return checkExpr(getChild(getChild(tmp,2),0),listeVar, tmp->op);
          }
        }
        else
        {
          return checkListInstr(getChild(tmp,1),listeVar,tmp->op);
        }
      }
      
      else
      {
        if(getChild(tmp,1)==NULL) 
        {
          return FALSE;
        }
        else
        {
          /* Si dans checkExpr ou dans checkList l'arbre est null, on retourne TRUE*/
          bool res = checkExpr(getChild(getChild(tmp,2),0),listeVar,tmp->op);
          return checkListInstr(getChild(tmp,1),listeVar,tmp->op) && res;
        }
      }
}

bool checkExpr(TreeP tree, PVAR listeVar, short etiquette){
   /* Vérifier que dans le cas d'un plus bianire, d'un minus binaire, d'une multiplication 
   * d'une division que le type est le même 
   * dans le cas d'une divison, regarder que le deuxième expr n'est pas  NULL ou 0 
   * Pour RELOP, les deux fils doivent avoir le même type
   * Plus unaire, moins unaire ? 
   */ 
}

bool checkExprEnvoiSelecInst(TreeP p, TreeP droit){
  if(droit==NULL){
      return checkExprEnvoiSelecInst(getChild(p,0), getChild(p,1));
  }  
  else{
    if(estCoherent(p,droit)) {
      return checkExprEnvoiSelecInst(getChild(p,0), getChild(p,1));
    }
    else return FALSE;
  }
} 
/* a.b ==> a partie gauche, b partie droite */


bool classeContient(PCLASS classe,TreeP droite)
{/*
  switch(droite->op)
  {
    case SELECTION:
    PVAR tmp = classe->liste_champs;
    PVAR tmpHerite = classe->champs_herite;
    PVAR fusion = tmp;

    fusion->suivant = tmpHerite;

    while(fusion!=NULL)
    {
      if(strcmp(fusion->nom,droite->u.var->nom)==0)
        return TRUE;
      fusion = fusion->suivant;
    }

    break;

    case ENVOIMESSAGE:

    PMETH tmp = classe->liste_methodes;
    PMETH tmpHerite = classe->override;
    PMETH fusion = tmp;

    fusion->suivant = tmpHerite;

    while(fusion!=NULL)
    {
      if(strcmp(fusion->nom,droite->u.methode->nom)==0)
        return TRUE;
      fusion = fusion->suivant;
    }
    break;

    default : 
      return FALSE;
  }*/
  return FALSE;
}

bool contientClasseInst(PVAR class, TreeP droite){
  PVAR tmp =  class->liste_champs; 
  PVAR tmpHerite = class->champs_herite;
  PVAR fusion = tmp;
  fusion->suivant = tmpHerite;

  while(fusion!=NULL){
    while(droite->u.var!= NULL){
      if(strcmp(fusion->nom,droite->u.var->nom)==0){
        return TRUE;
      }
      droite->u.var = droite->u.var->suivant;
    }
    fusion = fusion->suivant;
  }
  return FALSE;
}


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

/*
 * Verifie que la classe PCLASS existe belle et bien dans la lis
 */
bool containsClasse(PCLASS classe)
{
  PCLASS tmp = listeDeClass;

  while(tmp!=NULL)
  {

    if(strcmp(classe->nom,tmp->nom)==0)
      return TRUE;

    tmp = tmp->suivant;
  }
  return FALSE;
}

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
  /*
   * Parcourir les attributs de la classe actuel & verifier qu'il n'y a aucune qui se ressemble !
   */
  PVAR tmp = classe->liste_champs;

  while(tmp!=NULL)
  {
    /*
     * Appel de la fonction de Gishan qui verifier une instruction
     * Integer x; ... et bien d'autres chose
     */
    if(!checkListOptArg(tmp))
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur l'attribut %s est mal forme",tmp->nom);
      pushErreur(message,NULL,NULL,tmp);
      return FALSE;
    }
    /* /!!!\ Ici il s'arete des qu'un attribut est faux*/
    tmp = tmp->suivant;
  }
}

/*
 * 
  FIXME ajouter une methode equals(PMETH methode1,methode2) -> verifier qu'elle n'est pas declaree 2 fois
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

  while(tmp!=NULL)
  {
    /* /!!!\ Ici il s'arete des qu'une methode est fausse*/
    if(!checkMethode(tmp))
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur la methode %s est mal forme",tmp->nom);
      pushErreur(message,NULL,tmp,NULL);
      return FALSE;
    }

    tmp = tmp->suivant;
  }

  return TRUE;
}


bool checkMethode(PMETH methode)
{
    bool statique = FALSE;
    bool redef = FALSE;
    printf("Je check la methode %s\n",methode->nom);

    bool corps = checkBloc(methode->corps);
    bool typeRetour = containsClasse(methode->typeRetour);
    bool pvar = checkListOptArg(methode->params);

    if(methode->isStatic)
    {
      statique = checkMethodeStatic(methode);

      if(!statique)
      {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"Erreur dans la methode statique %s",methode->nom);
          pushErreur(message,classActuel,NULL,NULL);
          return FALSE;
      }
    }
    else
    {
      statique = TRUE;
      if(methode->isRedef)
      {
        redef = existeMethodeOverride(methode->home,methode);

        if(!redef)
        {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"Erreur la methode %s n'est pas une redefinition de la classe %s",methode->nom,methode->home->classe_mere->nom);
          pushErreur(message,classActuel,NULL,NULL);
          return FALSE;
        }
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
     return (typeRetour&&corps&&statique&&redef&&pvar);
}

/*
 * Verifie si la methode "methode" est declaree dans la classe mere
 */
bool existeMethodeOverride(PCLASS home,PMETH methode)
{
  PMETH methodeOverride = home->override;
  if(methodeOverride==NULL)
    return TRUE;
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
bool checkMethodeStatic(PMETH methode)
{
  /*
   * Si la classe Point a un attribut x,y
   * Si dans la methode il fait x = 1; -> erreur
   * Verifier qu'elle n'utilise j'amais this ni super
   */

}

bool checkCorp(PMETH methode)
{
  /*checkBloc()*/
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

bool checkAppelMethode(TreeP listOptArg,PVAR paramMeth, int isAppelConstructeurMere)
{
  /* FIXME a l'arrive d'une leaf IDENTIFICATEUR il faut que je cherche si l'attribut est 
   * Dans les attributs de classe, la liste de declaration du bloc de la methode ou les parametre
   * Si c'est un appel de constructeur -> regarder dans this.param_constructeur aussi
   */

   if(args==NULL && paramMeth==NULL)
    return TRUE;

  PVAR copie = paramMeth;

  return checkAppelMethodeR(listOptArg,paramMeth,isAppelConstructeurMere);

}

bool checkAppelMethodeR(TreeP listOptArg,PVAR paramMeth, int isAppelConstructeurMere)
{
  /* FIXME a l'arrive d'une leaf IDENTIFICATEUR il faut que je cherche si l'attribut est 
   * Dans les attributs de classe, la liste de declaration du bloc de la methode ou les parametre
   * Si c'est un appel de constructeur -> regarder dans this.param_constructeur aussi
   */
}

/*expr : PLUS expr %prec unaire   { $$=makeTree(PLUSUNAIRE, 1, $2); }
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
       ;*/

PCLASS getType(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  if(arbre==NULL)
    return NULL;
   
   /* Dans le cas d'une selection, récupérer le dernier élèment */ 
  PCLASS type;
  PCLASS type2;
  LTreeP liste;
  char* message = NEW(SIZE_ERROR,char);
  
  switch(arbre->op){
    case MINUSUNAIRE:
    case PLUSUNAIRE :
      type = getType(getChild(arbre,0),arbre,courant);
      
      if(type!=NULL && strcmp(type->nom,"Integer")!=0)
      {
        sprintf(message,"Erreur l'attribut %s n'est pas un entier",type->nom);
        pushErreur(message,type,NULL,NULL);
        return NULL;
      }
      else
      {
        return type;
      }
    break;

    case PLUSBINAIRE : 
    case MINUSUNAIRE : 
    case MULTIPLICATION :
    case DIVISION : 
    case OPCOMPARATEUR :
    type = getType(getChild(arbre,0),arbre,courant);
    type2 = getType(getChild(arbre,1),arbre,courant);
    if(equalsType(type,type2) && strcmp(type->nom,"Integer")==0){
      return type;
    }
    else
    {
      return NULL;
    }
    break; 

    case CONCATENATION : 
      type = getType(getChild(arbre,0),arbre,courant);
      type2 = getType(getChild(arbre,1),arbre,courant);
      
      if(type!=NULL && strcmp(type->nom,"String")!=0){  
        sprintf(message,"Erreur l'attribut %s n'est pas un entier",type->nom);
        pushErreur(message,type,NULL,NULL);
        return NULL;
      } 
      
      if(type2!=NULL && strcmp(type2->nom,"String")!=0)){
        sprintf(message,"Erreur l'attribut %s n'est pas un entier",type2->nom);
        pushErreur(message,type,NULL,NULL);
        return NULL;   
      }
      return type;
      break;

    case SELECTION : 
      if(ancien!=NULL && (ancien->op == SELECTION || ancien->op == ENVOIMESSAGE))
      {
        return getType(getChild(arbre,0),arbre,courant);
      }
      else
      {
        transFormSelectOuEnvoi(arbre,liste);
        /* A changer estCoherent */  
        estCoherent(liste, courant);
      }
    break;

    case ENVOIMESSAGE : 
        if(ancien!=NULL &&(ancien->op == SELECTION || ancien->op == ENVOIMESSAGE))
        {
            return getType(getChild(arbre,0),arbre,courant);
        }
        else
        {
        transFormSelectOuEnvoi(arbre,liste);
        estCoherentEnvoi(liste, courant, methode,listeDecl);
        } 

  }
}

bool estCoherentEnvoi(LTreeP liste, PCLASS classe, PMETH methode, PVAR listeDecl){
  LTreeP tmp = liste;
  PCLASS init = NULL;
  if(liste==NULL || tmp->elem==NULL)
  {
    return FALSE;
  }
  if(tmp->elem->op == IDENTIFICATEUR)
  {
    init = getTypeAttribut(tmp->elem->u.str, classe, methode, listeDecl);
    if(init == NULL)
    {
      return FALSE;
    }
  }
  else if(tmp->elem->op == IDENTIFICATEURCLASS)
  {
    init = getClasse(listeDeClass, tmp->elem->u.str);
  }
  else if(tmp->elem->op == INSTANCIATION)
  {
      char * nomClass = getChild(tmp->elem,0)->u.str; 

      PCLASS tmp = getClasse(listeDeClass,nomClass);

      if(tmp == NULL) return FALSE; 
  }  
  
  short etiquette = tmp->elem->op;

  while(tmp!=NULL)
  {
    tmp = tmp->suivant;
    if(tmp == NULL)
    {
      return TRUE;
    }
    TreeP tmpElem = tmp->elem;

    if(tmp->elem->op == IDENTIFICATEURCLASS || tmp->elem->op == INSTANCIATION)
    { 
      return FALSE;    
    }

    if(tmp->elem->suivant!=NULL)
    {
      init = appartient(init,tmpElem,TRUE,methode,listeDecl,tmp,etiquette);
      if(init==NULL)
      {
      return FALSE;
      }
    }
    else 
    {
      init = appartient(init,tmpElem,FALSE,methode,listeDecl,tmp,etiquette);
      if(init ==NULL)
      {
      return FALSE;
      }
    }
    etiquette = tmp->elem->op;
  }
  return TRUE;

}

PCLASS appartient(PCLASS mere, TreeP fille, bool isEnvoiMessage, PMETH methode, PVAR listeDecl, LTreeP tmp,short etiquette){
  if(fille==NULL || mere==NULL){
    return TRUE;
  }

  if(isEnvoiMessage)
  {
    PMETH listMeth = mere->liste_methodes;
    while(listMeth!=NULL)
    {
      if(strcmp(listMeth->nom,fille->u.str)==0)
      {
        /* Verifie si les parametre de de listMeth->param & fille */
        bool isVerifOk = getTypeMethode(listMeth->nom,mere,etiquette,tmp->elem->suivant,methode,listeDecl);

        if(isVerifOk)
        {
          return listMeth->typeRetour;
        }
        else
        {
          return NULL;
        }
      }
      listMeth = listMeth->suivant;
    }
    return NULL;
  }
  else
  {
    return getTypeAttribut(tmp->elem->u.str, mere, methode, listeDecl);
  }
}


/* Retourne le type d'une varibable suivant les paramètre de la méthode, 
* de la classe et de la liste de déclaration 
*/
PCLASS getTypeAttribut(char* nom, PCLASS classe, PMETH methode, PVAR listeDecl){
  bool estDansParamMeth = FALSE;
  bool estDansListeDecl =  FALSE;
  bool estDansAttributClasse = FALSE;
  PCLASS res = NULL;
  
  if(methode != NULL)
  {
  PVAR param = methode->params;

    while(param!=NULL)
    {
      if(strcmp(nom,param->nom)==0)
      {
        estDansParamMeth = TRUE;
        res = param->type;
        break;
      }
    param = param->suivant;
    }
  }
  
  if(listeDecl!=NULL)
  {
  PVAR listDeclaration = listeDecl; 
  
    while(listDeclaration!=NULL)
    {
      if(strcmp(nom,listDeclaration->nom)==0 && estDansParamMeth==TRUE)
      {
        return NULL;
      }
      else if(strcmp(nom,listDeclaration->nom)==0 && estDansParamMeth==FALSE){
        estDansListeDecl = TRUE;
        res = listDeclaration->type;
        break;
      }
      listDeclaration = listDeclaration->suivant;
    }
  }
  
  if(classe->liste_champs != NULL)
  {
    PVAR listeClasse = classe->liste_champs;

    while(listeClasse!=NULL)
    {
      if(strcmp(nom,listeClasse->nom)==0 && (estDansListeDecl==TRUE))
      {
        return NULL;
      }
      else if(strcmp(nom,listeClasse->nom)==0 && estDansListeDecl==FALSE){
        estDansAttributClasse = TRUE;
        res = listeClasse->type;
        break;
      }
      listeClasse = listeClasse->suivant;
    }
  }
  return res;
}

PCLASS getTypeMethode(char * nom, PCLASS classe, short precedant, TreeP appelMethode, PMETH methode, PVAR listeDecl){
  if(classe == NULL || nom == NULL){
    return NULL;
  }
  PMETH tmp = classe->liste_methodes;
  while(tmp!=NULL)
  {
    if(precedant == IDENTIFICATEURCLASS)
    {
      if(strcmp(nom,tmp->nom)==0 && tmp->isStatic)
      {
        /* FIXME :  Vérifier qu'elles ont les mêmes parametres */ 
        bool verifOk = compareParametreMethode(tmp->params,appelMethode, methode, listeDecl);
        if(verifOk)
        {
          return tmp->home;
        }
        else {return NULL;}
      }
    }
    else if(precedant == IDENTIFICATEUR)
    {
      if(strcmp(nom,tmp->nom)==0 && !tmp->isStatic)
      {
        bool verifOk = FALSE;
        if(verifOk)
        {
          return tmp->home;
        }
        else {return NULL;}
      }
    }

    tmp = tmp->suivant;
  }
  return NULL;
}

bool compareParametreMethode(PVAR declaration,TreeP appelMethode, PMETH methode, PVAR listeDecl)
{
  if((appelMethode==NULL && declaration!=NULL) || (appelMethode!=NULL && declaration==NULL))
  {
    return FALSE;
  }
  else if (appelMethode==NULL && declaration==NULL)
  {
    return TRUE;
  }

  /*Transformer a->b->c*/
  PCLASS liste = NULL;
  transformerAppel(appelMethode,liste);

  PCLASS tmp = liste;
  while(tmp!=NULL)
  {

    tmp = tmp->suivant;
  }

}

void transformerAppel(TreeP appelMethode,PCLASS liste)
{
  if(appelMethode==NULL) {return NULL;}/*Cas impossible normalement juste au cas ou*/

  if(getChild(appelMethode,0)==NULL)
  {
    PCLASS tmp = liste;
    getType(TreeP arbre, NULL, PCLASS courant, PMETH methode, PVAR listeDecl)
    liste = getType(getChild(appelMethode,0),appelMethode);
    liste->suivant = tmp;
  }

  if(liste==NULL)
  {
    liste = appelMethode;
  }
  else
  {
    PCLASS tmp = liste;
    liste = getChild(appelMethode,1);
    liste->suivant = tmp;
  }

  transformerAppel(getChild(appelMethode,0),liste);
}

/* Cree une liste chainee lorsqu'il y a une selection ou un envoi de message */
void transFormSelectOuEnvoi(TreeP arbre, LTreeP liste){
  if(getChild(arbre,0)==NULL)
  {
    LTreeP tmp = liste;
    liste = arbre;
    liste->suivant = tmp;
    return;
  }
  if(liste==NULL){
    liste = getChild(arbre,1);
    liste->elem->suivant = getChild(arbre,2);
  }
  else{
    LTreeP tmp = liste;
    liste = getChild(arbre,1);
    liste->suivant = tmp;
    liste->elem->suivant = getChild(arbre,2);

  }
  transFormSelectOuEnvoi(getChild(arbre,0),liste);
}


bool estCoherent(TreeP gauche, TreeP droite){
  
  switch(gauche->op){
   
    case SELECTION :
      /* regarder les deux fils de la selection */ 
      
      return classeContient(gauche->u.var->type,droite);
    break;
    
    case ENVOIMESSAGE : 
      return classeContient(gauche->u.methode->home,droite);
      /*
      * Vérifier les listOptArg
      */
    break;

    case INSTANCIATION :
      char * nomClass = getChild(gauche,0)->u.str; 
        /* Vérification si la classe existe */ 
      PCLASS tmp = getClasse(listeDeClass,nomClass);

      if(tmp == NULL) return false; 
        /*Vérifie que la classe de l'instanciation contient la partie droite*/
      contientClasseInst(tmp,droite);
      /*
      * vérifier les listOptArgOpt 
      */
    break;

    case IDENTIFICATEUR :
      /* Condition d'arrêt, retrun True que si appartient à la partie droite */ 
    break; 

    case IDENTIFICATEURCLASS : 
      /* Condition d'arrêt, retrun True que si appartient à la partie droite */ 
    break; 
  }
}

bool equalsType(PCLASS gauche, PCLASS droit){
  if(gauche==NULL)
  {
    return FALSE;
  }
  if(droite==NULL){
    return FALSE;
  }
  return (strcmp(gauche->nom,droit->nom)==0);
}



/** Partie eval **/
void evalProgramme(TreeP programme){
  /* on a l'attribut listeDeClass qui contient toutes les classes (s'il y en a) --> pas besoin de regarder ListClassOpt */
  evalContenuBloc(programme->u.children[1]/*->children[0]*/);
}

void evalContenuBloc(TreeP bloc){
  /* on est dans la regle : ContenuBloc : LInstructionOpt YieldOpt */
  if(bloc->u.children[0] == NIL(Tree)){
    if(bloc->u.children[1] != NIL(Tree)){
      /* eval de LInstruction */
    }
    if(bloc->u.children[2] != NIL(Tree)){
      /* eval de Yield => expr*/
    }
  }
  /* on est dans la regle : ContenuBloc : ListDeclVar IS LInstruction YieldOpt */
  else{
    /* eval de ListDeclVar */
    PVAR listDeclVar = evalListDeclVar(bloc->u.children[0]);

    /* eval de LInstruction */
    if(bloc->u.children[1] != NIL(Tree)){

    }
    /* eval de YieldOpt */
    if(bloc->u.children[2] != NIL(Tree)){

    }   
  }
}

PVAR evalListDeclVar(TreeP listDeclVar){
  /** ListDeclVar : VAR StaticOpt ID ':' IDCLASS AffectExprOpt ';' LDeclChampsOpt ==> renvoi PVAR */
  /* listDeclVar->var = 1ere var et toute la liste*/

  /* Que ca a faire??? */
  PVAR var = listDeclVar->u.var;
  return var;
}

/*
 * Methode pour imprimer toute l'arbre
 */

void printTree(TreeP tree){
/* if (! verbose ) return;*/
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
