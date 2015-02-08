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

  /*
   * TEST
   */
   printf("Debut\n");
   TreeP gauche = makeLeafInt(CSTENTIER,1);
   printf("a\n");
   TreeP droite = makeLeafStr(IDENTIFICATEUR,"a");
   printf("b\n");

   TreeP lesdeuxexpression = makeTree(OPCOMPARATEUR, 2, gauche, droite);
   
   printf("c\n");

   printf("nb %d\n",(getChild(lesdeuxexpression,1)==NULL)?TRUE:FALSE );
   printf("d\n");
   /*PCLASS getType(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl);*/
   PCLASS resultat = getType(lesdeuxexpression,NULL,NULL,NULL,NULL);
   printf("sortie\n");
   if(resultat!=NULL)
   {
    printf("Type %s\n",resultat->nom);
   }
   else
   {
    printf("Erreur de type\n");
   }

   //afficheListeErreur(listeErreur);

   
    printf("FIN DES TEST\n");
    afficheListeErreur(listeErreur);


    /*
      | ListDeclVar IS LInstruction YieldOpt  {$$=makeTree(CONTENUBLOC,3,$1,$3,$4);}  


      ListDeclVar : VAR StaticOpt ID ':' IDCLASS AffectExprOpt ';' LDeclChampsOpt 
            {
            $$=makeListVar($3,getClasse(listeDeClass,$5),$2,$6); 
            $$->suivant=$8;
            }

      AffectExprOpt : AFFECT expr ';'   {$$=makeTree(ETIQUETTE_AFFECT, 1, $2);}
              |       {$$=NIL(Tree);}
              ;
     */

    /*
    * TEST 2
    */
    /*
     PCLASS makeClasse(PCLASS listeClass,char *nom,PVAR param_constructeur,TreeP corps_constructeur,PMETH liste_methodes,PVAR liste_champs, PCLASS classe_mere, int isExtend);
      PMETH makeMethode(char *nom, int OverrideOuStaticOpt,TreeP corps,PCLASS typeRetour,PVAR params, PCLASS home);
      PVAR makeListVar(char *nom,PCLASS type,int cat,TreeP init);
      */

    PCLASS point = NEW(1,SCLASS);
    point->nom = calloc(100,sizeof(char));
    strcpy(point->nom,"Point");

    PCLASS point2D = NEW(1,SCLASS);
    point2D->nom = calloc(100,sizeof(char));
    strcpy(point2D->nom,"Point2D");

    PCLASS point3D = NEW(1,SCLASS);
    point3D->nom = calloc(100,sizeof(char));
    strcpy(point3D->nom,"Point3D");

    PCLASS point4D = NEW(1,SCLASS);
    point4D->nom = calloc(100,sizeof(char));
    strcpy(point4D->nom,"Point4D");
    

    PVAR p = NEW(1,SVAR);
    p->nom = calloc(100,sizeof(char));
    strcpy(p->nom,"b");
    p->type = point2D;


    PVAR p1 = NEW(1,SVAR);
    p1->nom = calloc(100,sizeof(char));
    strcpy(p1->nom,"c");
    p1->type = point3D;

    PVAR p2 = NEW(1,SVAR);
    p2->nom = calloc(100,sizeof(char));
    strcpy(p2->nom,"d");
    p2->type = point4D;
    

    point->liste_champs = p;
    point2D->liste_champs =p1; 
    point3D->liste_champs=p2;

    TreeP ourien = makeLeafStr(IDENTIFICATEUR,"a");
    
    TreeP selection = makeTree(SELECTION, 2,ourien,makeLeafStr(IDENTIFICATEUR,"b"));
    TreeP selection1 = makeTree(SELECTION, 2,selection,makeLeafStr(IDENTIFICATEUR,"c"));
    TreeP selection2 = makeTree(SELECTION, 2,selection1,makeLeafStr(IDENTIFICATEUR,"d"));

    PMETH f = NEW(1,SMETH);
    f->nom = calloc(100,sizeof(char));
    strcpy(f->nom,"f");
    f->home = point4D;
    
    PCLASS Integer = NEW(1,SCLASS);
    Integer->nom = calloc(100,sizeof(char));
    strcpy(Integer->nom,"Integer");
    PVAR paramsMethode = makeListVar("p",Integer,0,NIL(Tree));
    /*paramsMethode->suivant = makeListVar("toto",point,0,NIL(Tree));*/

    /*
     
ListParamOpt : LParam       {$$=$1;}
              |    {$$=NIL(SVAR);}
              ;

LParam : Param        {$$=$1 ;}
        | Param','LParam    {$1->suivant=$3; $$=$1;}
        ;

Param : ID':' IDCLASS     {$$= makeListVar($1,getClasse(listeDeClass,$3),0,NIL(Tree));}
          ; 
     */

    f->params = paramsMethode;

    PVAR tmpP = f->params;

 

    f->typeRetour = point;
    f->isStatic = FALSE;
    f->isRedef = FALSE;
    f->corps = makeTree(CONTENUBLOC,3,NULL,NULL,NULL);

    point4D->liste_methodes = f;

    TreeP argumentAppel = lesdeuxexpression;

    /*
    LArg : expr           {$$ = $1;}
     | LArg','expr              {$$=makeTree(LISTEARG, 2, $1,$3);}
     ;
     */

    TreeP envoiMessage = makeTree(ENVOIMESSAGE, 3,selection2,makeLeafStr(IDENTIFICATEUR,"f"),argumentAppel);

    TreeP selectionBis = makeTree(SELECTION, 2,envoiMessage,makeLeafStr(IDENTIFICATEUR,"b"));

    /*
     envoiMessage : IDCLASS '.' ID '(' ListOptArg ')' %prec '.'    
        { $$=makeTree(ENVOIMESSAGE, 3, makeLeafStr(IDENTIFICATEURCLASS,$1),makeLeafStr(IDENTIFICATEUR,$3),$5); }
              | envoiMessage '.' ID'('ListOptArg ')' %prec '.'    
        { $$=makeTree(ENVOIMESSAGE, 3,$1,makeLeafStr(IDENTIFICATEUR,$3),$5); }
              | OuRien '.' ID '(' ListOptArg ')'  %prec '.'   
        { $$=makeTree(ENVOIMESSAGE, 3,$1,makeLeafStr(IDENTIFICATEUR,$3),$5); }
             ;

     */


    PVAR listDeclVar = makeListVar("a",point,FALSE,NULL);
    //xslistDeclVar 

    /*
      * Liste d'instructions obligatoires 
      */
    /*LInstruction : Instruction LInstructionOpt  {$$=makeTree(LIST_INSTRUCTION, 2, $1, $2);}
                  ;*/

    TreeP instruction = makeTree(LIST_INSTRUCTION, 2, selectionBis,NULL);
    TreeP contenu = makeTree(CONTENUBLOC,3,listDeclVar,instruction,NULL);
    resultat = getType(contenu,NULL,NULL,NULL,listDeclVar);
   

    if(resultat!=NULL)
    {
      printf("Type %s\n",resultat->nom);
    }
     else
     {
      printf("Erreur de type\n");
     }

    afficheListeErreur(listeErreur);

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

    PCLASS tmp = listeClass;

    listeClass = res;
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
    /*FIXME : Liste d'erreur en LIFO : l'insertion est a l'envers la*/
    ErreurP tmp = listeErreur;
    listeErreur = nouvelle;
    nouvelle->suivant = tmp;
  }
}

/*
* Fonction permettant le parcours recursif de l'arbre
*/

bool parcourRecursifArbre(TreeP prog){
  if(prog==NULL) return TRUE;
  TreeP tmp = prog; 
  bool droite,gauche,rec1,rec2;
  switch(tmp->op){
    case PROGRAM :
      droite = f(getChild(tmp,1), tmp->op, NULL); 
      gauche = f(getChild(tmp,0), tmp->op, NULL);
      
      rec1 = parcourRecursifArbre(getChild(tmp,0));
      rec2 = parcourRecursifArbre(getChild(tmp,1));
        
      return droite && gauche && rec1 && rec2;
      break; 
  }

  return FALSE;
}

bool f(TreeP tree,short etiquette,PVAR listeVar){
  PVAR decl;
  bool res;
  switch(etiquette){
    case PROGRAM :
      
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
            /*return checkExpr(getChild(getChild(tree,2),0),listeVar, tree->op);*/
          }
        }
        else
        {
          /*return checkListInstr(getChild(tree,1),listeVar,tree->op);*/
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
          /*bool res = checkExpr(getChild(getChild(tree,2),0),listeVar,tree->op);
          return checkListInstr(getChild(tree,1),listeVar,tree->op) && res;*/
        }
      }
    break;

    case LISTCLASS : 
        /*FIXME : ???? que faire ?res = checkListClassBloc(getChild(tree,0),listeVar,tree->op);*/
        /*bool res2 = checkListClassBloc(getChild(tree,0),listeVar,tree->op);*/
    break;

    default : 
      return FALSE;
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
  /* FIXME : checkBlock !!! ici ce n'est pas une methode -> creer fausse methode constructeur*/
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
  return FALSE;
}

bool checkListAttribut(PCLASS classe)
{

  if(!verifAttributClasse(classe))
  {
    return FALSE;
  }
  /*
   * Parcourir les attributs de la classe actuel & verifier qu'il n'y a aucune qui se ressemble !
   */
  PVAR tmp = classe->liste_champs;

  while(tmp!=NULL)
  {
    if(tmp->type==NULL)
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur la classe de l'attribut %s n'existe pas",tmp->nom);
      pushErreur(message,NULL,NULL,tmp);
      return FALSE;
    }
    /*
     * Appel de la fonction de Gishan qui verifier une instruction
     * Integer x; ... et bien d'autres chose
     */
     /* FIXME : changer checkListOptArg par getType */
    /*if(!checkListOptArg(tmp))
    {
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur l'attribut %s est mal forme",tmp->nom);
      pushErreur(message,NULL,NULL,tmp);
      return FALSE;
    }*/
    /* /!!!\ Ici il s'arete des qu'un attribut est faux*/
    tmp = tmp->suivant;
  }

  return TRUE;
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
  SMETH copie = *classe->liste_methodes;
  PMETH tmp = NEW(1,SMETH);
  *tmp = copie;

  while(tmp!=NULL)
  {
    /* /!!!\ Ici il s'arete des qu'une methode est fausse*/
    if(!checkMethode(tmp))
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


bool checkMethode(PMETH methode)
{
    bool statique = FALSE;
    bool redef = FALSE;
    printf("Je check la methode %s\n",methode->nom);

    /*FIXME bool corps = checkBloc(methode->corps);*/
    /* FIXME : concat des messafes*/
    bool corps = TRUE;
    bool typeRetour = (methode->typeRetour!=NULL);
    bool pvar = checkListOptArg(methode->params);

    if(methode->isStatic)
    {
      redef = TRUE;
      statique = checkMethodeStatic(methode);

      if(!statique)
      {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"Erreur dans la methode statique %s",methode->nom);
          pushErreur(message,classActuel,methode,NULL);
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
          sprintf(message,"Erreur la methode %s de la classe %s",methode->nom,methode->home->classe_mere->nom);
          pushErreur(message,classActuel,methode,NULL);
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
   return FALSE;   

}

bool checkCorp(PMETH methode)
{
  /*checkBloc()*/
  return FALSE;   
}

bool checkListOptArg(PVAR var)
{
  /*FIXME : verifier que deux var n'ont pas meme parametre ou que le type retour = NULL*/
    return FALSE;   
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
  {
    char* message = NEW(SIZE_ERROR,char);
    sprintf(message,"Arbre est vide");
    pushErreur(message,classActuel,methode,NULL);
    return NULL;
  }
   PCLASS integer = NEW(1,SCLASS);PCLASS string = NEW(1,SCLASS);
   printf("1\n");
   printf("arbre NIL : ? %d\n",arbre==NULL?TRUE:FALSE );
   printf("Etiquette %d\n",arbre->op );
   printf("2\n");
   /* Dans le cas d'une selection, récupérer le dernier élèment */ 
  PCLASS type = NULL;
  PCLASS type2 = NULL;
  LTreeP liste = NULL;
  LTreeP tmpL = NULL;
  char* message = NEW(SIZE_ERROR,char);

  switch(arbre->op){
    case MINUSUNAIRE:
    case PLUSUNAIRE :
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
    printf("type = NULL ? %s type2 = NULL ? %s\n",type->nom,type2->nom );
    if(equalsType(type,type2)){
      printf("EST TU ICI ?\n");
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
    
    if(equalsType(type,type2) && strcmp(type->nom,"Integer")==0){
      
      return type;
    }
    else
    {
      if(type!=NULL)
      {
        if(type2!=NULL)
        {
          sprintf(message,"Erreur operation de comparaison entre un objet de type %s et %s",type->nom,type2->nom);
        }
      }
      else
      {
        sprintf(message,"Erreur de type operation de comparaison");
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

      if(ancien!=NULL && (ancien->op == SELECTION || ancien->op == ENVOIMESSAGE))
      {
        return getType(getChild(arbre,0),arbre,courant,methode,listeDecl);
      }
      else
      {
        printf("0\n");
        liste = transFormSelectOuEnvoi(arbre,liste);
        printf("1\n");             
        printf("2\n");
        printf("3----\n");
        return estCoherentEnvoi(liste, courant, methode,listeDecl);
        printf("4\n");
      }
      printf("Apres .....\n");
    break;

    case ENVOIMESSAGE : 
        if(ancien!=NULL &&(ancien->op == SELECTION || ancien->op == ENVOIMESSAGE))
        {
          return getType(getChild(arbre,0),arbre,courant,methode,listeDecl);
        }
        else
        {
          liste = transFormSelectOuEnvoi(arbre,liste);
          tmpL = liste;
          while(tmpL!=NULL)
          {

            if(!tmpL->elem->isEnvoiMessage)
            {
              printf("Attribut : Pour moi %s c'est null\n", tmpL->elem->u.str);
            }
            else
            {
              printf("Methode : Pour moi %s c'est pas null\n", tmpL->elem->u.str);
            }

            tmpL = tmpL->suivant;
          }
          return estCoherentEnvoi(liste, courant, methode,listeDecl);
        } 
      break;

    case CSTENTIER:
        if(integer->nom==NULL)
        {
        integer->nom = calloc(100,sizeof(char));
        strcpy(integer->nom,"Integer");
        }
        return integer;
      break;
    
    case CSTSTRING:
        if(string->nom==NULL)
        {
        string->nom = calloc(100,sizeof(char));
        strcpy(string->nom,"String");
        }
        return string;
      break;

    case IDENTIFICATEUR:
      return getTypeAttribut(arbre->u.str, courant, methode, listeDecl);
    break;

    case IDENTIFICATEURCLASS:
      return getClasse(listeDeClass,arbre->u.str);
    break;

    case CONTENUBLOC:
        return getType(getChild(arbre,1),arbre,courant,methode,listeDecl);
      break;

    case LIST_INSTRUCTION:
        return getType(getChild(arbre,0),arbre,courant,methode,listeDecl);
      break;

  }
  return NULL;
}

PCLASS estCoherentEnvoi(LTreeP liste, PCLASS classe, PMETH methode, PVAR listeDecl){

    LTreeP tmp = liste;
    PCLASS init = NULL;
    printf("rentrer\n");


    if(liste==NULL || tmp->elem==NULL)
    {
        char* message = NEW(SIZE_ERROR,char);
        if(methode!=NULL && classe!=NULL)
        {
            sprintf(message,"Erreur la methode %s est mal forme - Classe : %s",methode->nom,classe->nom);
        }
        else
        {
            sprintf(message,"Erreur envoi de message");
        }
        pushErreur(message,classe,methode,NULL);
        return NULL;
    }
    printf("--2---\n");
    if(tmp->elem->op == IDENTIFICATEUR)
    {
        printf("1.1\n");

        if(classe!=NULL && (strcmp(tmp->elem->u.str,"super")==0))
        {
          /*FIXME verifier que la methode n'est pas statique et n'est pas NULL*/
          init = classe->classe_mere;
        }
        else if(classe!=NULL && (strcmp(tmp->elem->u.str,"this")==0))
        {
          /* FIXME pareille que le FIXME au dessus*/
          init = classe;
        }
        else
        {
          init = getTypeAttribut(tmp->elem->u.str, classe, methode, listeDecl);
        } 

        

        if(init == NULL)
        {
            printf("1.1.1\n");
            char* message = NEW(SIZE_ERROR,char);
            sprintf(message,"%s inconnu au bataillon",tmp->elem->u.str);
            pushErreur(message,classe,methode,NULL);
            return NULL;
        }
        printf("1.2\n");
    }
    else if(tmp->elem->op == IDENTIFICATEURCLASS)
    {
      printf("2.1\n");
        init = getClasse(listeDeClass, tmp->elem->u.str);
        printf("2.2\n");
    }
    else if(tmp->elem->op == INSTANCIATION)
    {
        printf("3.1\n");
        char * nomClass = getChild(tmp->elem,0)->u.str;
        printf("3.2\n");
        PCLASS tmp = getClasse(listeDeClass,nomClass);
        printf("3.3\n");
        if(tmp == NULL)
        {
          printf("3.4\n");
            char* message = NEW(SIZE_ERROR,char);
            sprintf(message,"La classe %s n'est pas declare ",nomClass);
            pushErreur(message,classe,methode,NULL);
            return NULL;
            printf("3.5\n");
        };
    }
    
    short etiquette = tmp->elem->op;
    printf("3.6\n");
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
        if(tmp->elem->op == IDENTIFICATEURCLASS || tmp->elem->op == INSTANCIATION || strcmp(tmp->elem->u.str,"super")==0 || strcmp(tmp->elem->u.str,"this")==0)
        {
            char* message = NEW(SIZE_ERROR,char);
            sprintf(message," Identificateur de classe, instanciation, super ou this en plein milieu : %s",tmp->elem->u.str);
            pushErreur(message,classe,methode,NULL);
            printf("il y a un soucis\n");
            return NULL;
        }
        
        if(tmp->elem->suivant!=NULL || tmp->elem->isEnvoiMessage==TRUE)
        {
            //printf("tmp->elem->suivant %s \n",tmp->elem->suivant->op);
            printf("25-25-25\n");
            init = appartient(init,tmpElem,TRUE,methode,listeDecl,tmp,etiquette);
            if(init==NULL)
            {
                char* message = NEW(SIZE_ERROR,char);
                sprintf(message,"Erreur la methode %s n'appartient pas a %s",tmp->elem->u.str,tempoAffiche->nom);
                pushErreur(message,classe,methode,NULL);
                return NULL;
            }
            tempoAffiche = init;
        }
        else if(tmp->elem->isEnvoiMessage==FALSE)
        { 
            printf("26_26_26\n");
            init = appartient(init,tmpElem,FALSE,methode,listeDecl,tmp,etiquette);
            if(init ==NULL)
            {
                char* message = NEW(SIZE_ERROR,char);
                sprintf(message,"Erreur la varibable %s n'appartient pas a %s",tmp->elem->u.str,tempoAffiche->nom);
                pushErreur(message,classe,methode,NULL);
                return NULL;
            }
            tempoAffiche = init;
        }
        etiquette = tmp->elem->op;
    }
    return init;  
}


PCLASS appartient(PCLASS mere, TreeP fille, bool isEnvoiMessage, PMETH methode, PVAR listeDecl, LTreeP tmp,short etiquette){
  printf("Appartient : %s\t%s\n",mere->nom, fille->u.str);
  
  if(fille==NULL || mere==NULL){
    char* message = NEW(SIZE_ERROR,char);
    sprintf(message,"fonction tp.c appartient : Erreur Arbre");
    pushErreur(message,mere,methode,NULL);
    return NULL;
  }

  if(isEnvoiMessage)
  {
    printf("1_1_1_1_1\n");
    PMETH listMeth = mere->liste_methodes;
    while(listMeth!=NULL)
    {
      printf("Je compare methode : %s et filles %s\n", listMeth->nom,fille->u.str);
      if(strcmp(listMeth->nom,fille->u.str)==0)
      {
        /* Verifie si les parametre de de listMeth->param & fille */
        printf("Allez isVerif ?\n");
        bool isVerifOk = getTypeMethode(listMeth->nom,mere,etiquette,tmp->elem->suivant,methode,listeDecl)!=NULL?TRUE:FALSE;
        printf("Fin isVerif\n");
        if(isVerifOk)
        {
          return listMeth->typeRetour;
        }
        else
        {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"Erreur la methode %s est mal appele ou n'existe pas",tmp->elem->u.str);
          pushErreur(message,mere,methode,NULL);
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
     return getTypeAttribut(tmp->elem->u.str, mere, methode, listeDecl);    
  }
}


/* Retourne le type d'une varibable suivant les paramètre de la méthode, 
* de la classe et de la liste de déclaration 
*/
PCLASS getTypeAttribut(char* nom, PCLASS classe, PMETH methode, PVAR listeDecl){
  printf("nom : %s\n",nom);
  printf("1.1.1\n");
  bool estDansParamMeth = FALSE;
  bool estDansListeDecl =  FALSE;
  bool estDansAttributClasse = FALSE;
  PCLASS res = NULL;
  
  if(methode != NULL)
  {
    printf("1.1.2\n");
  PVAR param = methode->params;

    while(param!=NULL)
    {
      printf("1.1.3\n");
      if(strcmp(nom,param->nom)==0)
      {
        estDansParamMeth = TRUE;
        res = param->type;
        break;
      }

    param = param->suivant;
    }
    printf("1.1.4\n");
  }
  
  if(listeDecl!=NULL)
  {
    printf("1.1.5\n");
  PVAR listDeclaration = listeDecl; 
  
    while(listDeclaration!=NULL)
    {
      if(strcmp(nom,listDeclaration->nom)==0 && estDansParamMeth==TRUE)
      {
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Erreur l'attribut %s est redeclaree",nom);
        pushErreur(message,classe,methode,NULL);
        return NULL;
      }
      else if(strcmp(nom,listDeclaration->nom)==0 && estDansParamMeth==FALSE){
        estDansListeDecl = TRUE;
        res = listDeclaration->type;
        break;
      }
      listDeclaration = listDeclaration->suivant;
    }
    printf("1.1.6\n");
  }
  printf("1.1.6.1\n");
  /*printf("classe->liste_champs null ? %d\n",classe==NULL?TRUE:FALSE);*/
  if(classe!=NULL && classe->liste_champs != NULL)
  {
    printf("1.1.7\n");
    PVAR listeClasse = classe->liste_champs;
    printf("1.1.8\n");
    while(listeClasse!=NULL)
    {
      printf("1.1.9\n");
      if(strcmp(nom,listeClasse->nom)==0 && (estDansListeDecl==TRUE))
      {
        printf("1.1.10\n");
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Erreur l'attribut %s est redeclaree",nom);
        pushErreur(message,classe,methode,NULL);
        return NULL;
        printf("1.1.11\n");
      }
      else if(strcmp(nom,listeClasse->nom)==0 && estDansListeDecl==FALSE){
        estDansAttributClasse = TRUE;
        res = listeClasse->type;
        break;
        printf("1.1.13\n");
      }
      listeClasse = listeClasse->suivant;
    }
  }
  printf("1.1.6.2\n");
  return res;
}

PCLASS getTypeMethode(char * nom, PCLASS classe, short precedant, TreeP appelMethode, PMETH methode, PVAR listeDecl)
{
  if(classe == NULL || nom == NULL){
    char* message = NEW(SIZE_ERROR,char);
    sprintf(message,"tp.c getTypeMethode : Erreur Arbre");
    pushErreur(message,classe,methode,NULL);
    return NULL;
  }
  PMETH tmp = classe->liste_methodes;
  while(tmp!=NULL)
  {
    printf("a.1\n");
    if(precedant == IDENTIFICATEURCLASS)
    {
      printf("a.2\n");
      if(strcmp(nom,tmp->nom)==0 && tmp->isStatic)
      {
        bool verifOk = compareParametreMethode(tmp->params,appelMethode, classe,methode, listeDecl, nom);
        if(verifOk)
        {
          return tmp->home;
        }
        else 
        {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"La methode statique %s est mal appelee",nom);
          pushErreur(message,classe,methode,NULL);
          return NULL;
        }
      }
    }
    else if(precedant == IDENTIFICATEUR)
    {
      printf("a.3\n");
      if(strcmp(nom,tmp->nom)==0 && !tmp->isStatic)
      {
        printf("a.4\n");
        bool verifOk = compareParametreMethode(tmp->params,appelMethode, classe,methode, listeDecl, nom);
        printf("a.5\n");
        if(verifOk)
        {
          return tmp->home;
        }
        else 
        {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"La methode de classe %s est mal appelee",nom);
          pushErreur(message,classe,methode,NULL);
          return NULL;
        }
      }
    }

    tmp = tmp->suivant;
  }
  return NULL;
}

bool compareParametreMethode(PVAR declaration,TreeP appelMethode, PCLASS classe, PMETH methode, PVAR listeDecl, char*nom)
{
  if((appelMethode==NULL && declaration!=NULL)||(appelMethode!=NULL && declaration==NULL))
  {
    printf("return 1\n");
    return FALSE;
  }
  else if (appelMethode==NULL && declaration==NULL)
  {
    return TRUE;
  }

  /*Transformer a->b->c*/
  PCLASS liste = NULL;
  printf("a.6\n");

  liste = transformerAppel(appelMethode,liste,classe,methode, listeDecl);
  printf("a.7\n");
  
  /*printf("--D1-- lelena : %s\n ----F1--\n",liste->nom);
  printf("--D2-- lelena : %s\n ----F2---\n",liste->suivant->nom);*/

  if(appelMethode!=NULL && liste==NULL)
  {
    printf("return 2\n");
    return FALSE;
  }

  PCLASS tmp = liste;
  PVAR tmpDeclarationOfficiel = declaration;

  if(tmp==NULL && tmpDeclarationOfficiel==NULL)
  {
    return TRUE;
  }
  printf("..1.. \n");
  SCLASS contenuTMP = *tmp;
  printf("..2.. \n");
  PCLASS tmp2 = &contenuTMP;
  printf("..3.. \n");
  int cpt = 0;
  
  while(tmp2!=NULL)
  {
    cpt++;

    printf("Val 1 : %s\n", tmp2->nom);

    tmp2 = tmp2->suivant;
  }
  printf("..4.. \n");

  SVAR contenuDeclaration = *tmpDeclarationOfficiel;
  printf("..5.. \n");
  PVAR tmpDeclarationOfficiel2 = &contenuDeclaration;
  printf("..6.. \n");
  int cptDeclaration = 0;
  while(tmpDeclarationOfficiel2!=NULL)
  {
    cptDeclaration++;

    tmpDeclarationOfficiel2 = tmpDeclarationOfficiel2->suivant;
  }
  printf("DeclarationOfficiel contient : %d element\n",cptDeclaration );

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
      printf("return 3\n");
      return FALSE;
    }

    if(!equalsType(tmpDeclarationOfficiel->type,tmp))
    {
      printf("return 4\n");
      return FALSE;
    }

    tmpDeclarationOfficiel = tmpDeclarationOfficiel->suivant;
    tmp = tmp->suivant;
  }

  return TRUE;

}

PCLASS transformerAppel(TreeP appelMethode,PCLASS liste, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  if(liste==NULL)
  {
    if(appelMethode->op!=LISTEARG)
    {
      PCLASS getTypeRetour = getType(appelMethode,appelMethode, courant, methode, listeDecl);
      /*printf("1 ?????? \n");
      printf("getTypeRetour : %s\n",getTypeRetour->nom);*/
      return getTypeRetour;
    }
    else
    {
      PCLASS getTypeRetour = getType(getChild(appelMethode,1),appelMethode, courant, methode, listeDecl);
      /*printf("2 ?????? \n");
      printf("getTypeRetour : %s\n",getTypeRetour->nom);*/
      liste = getTypeRetour;
    }
  }
  else
  {
    if(appelMethode->op!=LISTEARG)
    {
      SCLASS tmp = *liste;

      PCLASS getTypeRetour = getType(appelMethode,appelMethode, courant, methode, listeDecl);
      /*printf("3 ?????? \n");
      printf("getTypeRetour : %s\n",getTypeRetour->nom);*/
      liste = getTypeRetour;
      liste->suivant = NEW(1,SCLASS);
      *liste->suivant = tmp;
      return liste;
    }
    else
    {
      SCLASS tmp = *liste;
      PCLASS getTypeRetour = getType(getChild(appelMethode,1),appelMethode, courant, methode, listeDecl);
      /*printf("4 ?????? \n");
      printf("getTypeRetour : %s\n",getTypeRetour->nom);*/
      liste = getTypeRetour;
      liste->suivant = NEW(1,SCLASS);
      *liste->suivant = tmp;
    }
  }
  return transformerAppel(getChild(appelMethode,0),liste,courant,methode,listeDecl);
}


/* Cree une liste chainee lorsqu'il y a une selection ou un envoi de message */

LTreeP transFormSelectOuEnvoi(TreeP arbre, LTreeP liste)
{
  printf("arbre -> op %d\n",arbre->op );
  if(liste==NULL){
    printf("Je suis ici\n");
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


  if(getChild(arbre,0)->op==IDENTIFICATEUR || getChild(arbre,0)->op==IDENTIFICATEURCLASS)
  {
    if(liste!=NULL)
    {
      listeTree tmp = *liste;
      printf("%s\n",tmp.elem->u.str);
      liste->elem = getChild(arbre,0);
      printf("%s\n",liste->elem->u.str);
      printf("%s\n",tmp.elem->u.str);
      liste->suivant = NEW(1,listeTree);
      *liste->suivant = tmp;
    }   
    return liste;
  }
  printf("je suis à la fin\n");
  return transFormSelectOuEnvoi(getChild(arbre,0),liste);
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


bool estCoherent(TreeP gauche,PCLASS actuelle)
{
  char * nomClass = NULL;
  switch(gauche->op){
   
    case SELECTION :
      /* regarder les deux fils de la selection */ 
      
      /*return classeContient(gauche->u.var->type,droite);*/
    break;
    
    case ENVOIMESSAGE : 
      /*return classeContient(gauche->u.methode->home,droite);*/
      /*
      * Vérifier les listOptArg
      */
    break;

    case INSTANCIATION :
      nomClass = getChild(gauche,0)->u.str; 
        /* Vérification si la classe existe */ 
      PCLASS tmp = getClasse(listeDeClass,nomClass);

      if(tmp == NULL) return FALSE; 
        /*Vérifie que la classe de l'instanciation contient la partie droite*/
      /*contientClasseInst(tmp,droite);*/
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

bool equalsType(PCLASS gauche, PCLASS droite)
{
  if(gauche==NULL)
  {
    return FALSE;
  }
  if(droite==NULL){
    return FALSE;
  }
  if(strcmp(gauche->nom,droite->nom)==0)
  {
    return TRUE;
  }
  else
  {

  }
  return FALSE;
}



/** Partie eval **/
void evalProgramme(TreeP programme)
{
  /* on a l'attribut listeDeClass qui contient toutes les classes (s'il y en a) --> pas besoin de regarder ListClassOpt */
  evalContenuBloc(programme->u.children[1]/*->children[0]*/);
}

void evalContenuBloc(TreeP bloc)
{
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
  else
  {
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
printf("Etiquette %d\n",tree->op);
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
      printf(RED "Erreur %d : %s\n" BLACK,i,tmp->message);
      printf(""BLACK);
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
  /* FIXME Verifier qu'il n'y a pas 2 attribut qui on le même nom !!!!!!!!!!!!*/
  PVAR tmp = classe->liste_champs;
  PVAR reste = tmp->suivant;
  while(tmp!=NULL)
  {
    reste = tmp->suivant;

    while(reste!=NULL)
    {
      if(strcmp(reste->nom,tmp->nom)==0)
      {
        return FALSE;
      }
      reste = reste->suivant;
    }
    tmp = tmp->suivant;
  }

  return TRUE;
}

/* FIXME : equalsAffectation : verifie si Point = Point2D correcte (OUI) et inversemment */
