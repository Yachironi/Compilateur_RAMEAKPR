#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "tp.h"
#include "tp_y.h"

bool dansCheckBlocMain = FALSE;
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

  printf("tp.c -> avant res\n");
  res = yyparse();
  if(res==1)
  {
    printf("Aucune verification a faire car syntax error\n");
    exit(0);
  }

  	printf("--------------------------------------------------------------\n");
  	bool checkProg = checkProgramme(programme);
  	printf("--------------------------------------------------------------\n");
  	printf("FIN de la COMPILATION\n");
  	if(!checkProg){
    		printf("Il y a des probleme dans le code !\n");
        afficheListeErreur(listeErreur);
  	}
  	else{
    		//Faire eval ici
  	}

  /*exit(0);*/

  printf("tp.c -> res=%d\n", res);
  if (programme == NULL) {
    printf("tp.c -> Programme est NULL\n");
  }
  else{
    printf("tp.c -> Programme n'est pas NULL\n");
    printf("=======================\n");
    /* Fonction de teste des evalExpr*/
    /*testEval();*/
    /*printf("tp.c -> Affichage de l'arbre : \n");
    pprintTreeMain(programme);*/
    printf("=======================\n");
    
  }
  if (fd != NIL(FILE)) fclose(fd);
    if (res == 0 && errorCode == NO_ERROR) return 0;
    else {
      int res2 = res ? SYNTAX_ERROR : errorCode;
        printf("Error in file. Kind of error: %d\n", res2); 
        return res2;
    }
    printf("FIN COMPILATION\n");
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

PVAR getVar(PVAR var, char* nom){
  PVAR tmp = var;

	while(tmp != NULL){
		if(strcmp(tmp->nom, nom)==0){
			return makeListVar(tmp->nom, tmp->type, tmp->categorie, tmp->init);
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

/* Renvoie l'attribut d'une classe */
/*PVAR getVAR(PCLASS classe, char* nomVar){
if(classe==NULL || nomVar==NULL){
	return NULL;
	}
PVAR parc = classe->liste_champs;
while((strcmp(parc->nom,nomVar)!=0)&&(parc->suivant!=NULL)){
	parc=parc->suivant;
	}
if(strcmp(parc->nom,nomVar)==0){
	return parc;
	} else {
return NULL;
	}
}*/

/** Renvoie un pointeur de la methode recherchee */	
PMETH getMethode(PCLASS classe, PMETH methode){
  if(classe==NULL || methode==NULL)
  {
    return NULL;
  }
  PMETH tmp_liste_methodes_classe = getClasseBis(listeDeClass,classe->nom)->liste_methodes;
  PMETH tmp_liste_methode = methode;
  if(methode == NULL) return FALSE;

  while(tmp_liste_methodes_classe != NULL){
    /* si 2 methodes ont le meme noms, les memes classes de retour (meme noms) et memes param ==> meme methode */
    if(strcmp(tmp_liste_methodes_classe->nom, tmp_liste_methode->nom)==0 && strcmp(tmp_liste_methodes_classe->typeRetour->nom, tmp_liste_methode->typeRetour->nom)==0 && memeVar(tmp_liste_methodes_classe->params, tmp_liste_methode->params)==TRUE ){
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
  res->typeRetour=typeRetour; /*FIXME faire un vrai copie? Mais pb : typeRetour n'est pas "totalement fini" */
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
    
    ErreurP tmp = listeErreur;
    listeErreur = nouvelle;
    nouvelle->suivant = tmp;
  }
}

/*
* Fonction permettant le parcours de l'arbre
*/

bool checkProgramme(TreeP prog){
  if(prog==NULL) return FALSE;
  printf("Entree 1\n");
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
  if(bloc!=NULL)
  {
    printf("Entree 2.0\n");
  /* FIXME : transformer getChild(bloc,0) en PVAR */
   printf("Arbre ----------------------------: %d et %s \n", bloc->op,getChild(bloc,0)!=NULL?"True":"False");
  }
  bool blockMain = FALSE;
  if(getChild(bloc,0)!=NULL)
  {
    blockMain = checkBloc(bloc,prog,NULL, NULL, getChild(bloc,0)->u.var);
  }
  else
  {
    blockMain = checkBloc(bloc,prog,NULL, NULL, NULL);
  }
  printf("J4AI FINIS CHECK BLO ===================================================================\n");
  if(listeDeClass==NULL)
  {
    return blockMain;
  }

 printf("Entree 3\n");
  if(!checkLC)
  {
     while(liste!=NULL)
     {
     printf("Entree 4\n");
     checkLC = checkClass(bloc,prog,liste, NULL, NULL) && checkLC;
     printf("Sortie 4\n");
      liste = liste->suivant;
     }
  }
  return blockMain && checkLC;

}

bool checkBloc(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl){
  char* message = NEW(SIZE_ERROR,char);
  printf(" JE VAIS ALLER FAIRE MON PUTAIN DE BLOC\n");
  if(arbre == NIL(Tree) || arbre==NULL)
  {
    printf("C'EST LA FIN DES HARICOTS\n");
    
    return TRUE;    /* arriver a la fin des instructions */
  }
  printf("--------------------%d\n", arbre->op);
  printf("a.a.a.3\n");
  
  if(arbre->op == CONTENUBLOC)
  {
    printf("caca\n");
    if(listeDecl != NULL && getChild(arbre,1)==NULL)
    {
      sprintf(message,"la liste d'instruction est vide tandis que la liste de declaration ne l'est pas");
      pushErreur(message,courant,methode,listeDecl);
      return FALSE;
    }
    printf("lalalala\n");
    bool res = checkListDeclaration(getChild(arbre,0),ancien,courant,methode,listeDecl);
    if(res == FALSE)
    {
      sprintf(message,"la liste de declaration contient des erreurs");
      pushErreur(message,courant,methode,listeDecl);
      return FALSE;
    }
    res = checkListInstruction(getChild(arbre,1),ancien,courant,methode,listeDecl) & res;
    return res;
  }
  else if(arbre->op == ETIQUETTE_AFFECT)
  {
    PCLASS type = getType(getChild(arbre,0),NULL,courant,methode,NULL);
    return type!=NULL;
  }
}
bool checkListInstruction(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  if(arbre==NULL)
  {
    return TRUE;
  }
  char* message = NEW(SIZE_ERROR,char);
  if(arbre->op == CONTENUBLOC)
  {
    bool resultat = FALSE;
    printf("checkListInstruction return 7\n");
    resultat = checkBloc(arbre, NULL, courant,methode,listeDecl);
    if(!resultat)
    {
      sprintf(message,"Instruction : erreur dans contenu bloc");
      pushErreur(message,courant,methode,listeDecl);
    }
    return resultat;
  }
  
  
    printf("je suis ici autre \n");
  
    printf("getChild arbre 1 n'est pas null %d \n",arbre==NULL);
    printf("EYIQUETTE %d \n",arbre->op);
    TreeP autreinstructions = getChild(arbre,1);

    printf("La \n");

    TreeP instruction = getChild(arbre,0);
    
    printf("AVAnt \n");
    printf("ETIQUETE du pere : %d \n",arbre->op );

    if(arbre->nbChildren==0)
    {
      return (getType(arbre,NULL,courant,methode,listeDecl)!=NULL);
    }

     PCLASS type = NULL;
     PCLASS type2 = NULL;
     bool instruction1 =FALSE;
     bool instruction2 =FALSE;
     
     bool resultat = FALSE;
     bool blocRetour = FALSE;
     TreeP ifInstruction1 = NULL;
     TreeP ifInstruction2 = NULL;
     bool if1 = FALSE;
     bool if2 = FALSE;
     
    
     switch(instruction->op)
     {
      
        case EXPRESSIONRETURN :
        printf("m.expr\n");
          if(methode == NULL)
          {
            sprintf(message,"Bloc main ne peut avoir de return");
            pushErreur(message,courant,methode,listeDecl);
            resultat = FALSE;
            printf("checkblock check false 1\n");
          }
          else
          {
            
            type = getType(getChild(instruction,0),instruction,courant,methode,listeDecl);
            if(type==NULL)
            {
              printf("Methode ppppppppppppppppppppppppppppppppppppppppp %s \n",methode->nom);
              printf("Instruction Fils %d : \n",getChild(instruction,0)->op);
              printf("Instruction Fils %s : \n",getChild(instruction,0)->u.str);
            }
            resultat = type!=NULL;
  
            if(!equalsType(type,methode->typeRetour))
            {
              sprintf(message,"Erreur de return : %s type = %s et retour  ",methode->nom,methode->typeRetour->nom);
              pushErreur(message,courant,methode,listeDecl);
              printf("checkblock check false 2\n");
              return FALSE;
            }
            printf("checkListInstruction return 3.1\n");
            

            if(!resultat)
            {
              sprintf(message,"Erreur de return : %s ",methode->nom);
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
                  sprintf(message,"Instruction : affectation incorrecte entre deux types differents %s & %s",type->nom,type2->nom);
                }
                else
                  sprintf(message,"Instruction : affectation incorrecte entre deux types differents : %s",type->nom);
              }
              else
              {
                sprintf(message,"Instruction : affectation incorrecte entre deux types differents");
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
            type = getType(getChild(instruction,0),instruction,courant,methode,listeDecl);
            printf("j'ai passer le getType \n");

            if(!equalsType(type,getClasseBis(listeDeClass,"Integer")))
            {
              sprintf(message,"Instruction : la condition dans le if n'est pas un Integer");
              pushErreur(message,courant,methode,listeDecl);
              printf("checkListInstruction return 4\n");
              printf("checkListInstruction check false 4\n");
              return FALSE;
            }
            
            if(getChild(instruction,1)->op==CONTENUBLOC || getChild(instruction,1)->op==ETIQUETTE_AFFECT)
            {
              /* FIXME2.0 fusionner listDecl et celui du bloc si c'est un bloc */
              resultat = checkBloc(getChild(instruction,1),NULL,courant,methode,listeDecl);
            }
            else
            {
              resultat = checkListInstruction(getChild(instruction,1),NULL,courant,methode,listeDecl);
            }

            if(getChild(instruction,2)->op==CONTENUBLOC || getChild(instruction,2)->op==ETIQUETTE_AFFECT)
            {
              /* FIXME2.0 fusionner listDecl et celui du bloc si c'est un bloc */
              resultat = checkBloc(getChild(instruction,2),NULL,courant,methode,listeDecl) && resultat;
            }
            else
            {
              resultat = checkListInstruction(getChild(instruction,2),NULL,courant,methode,listeDecl) && resultat;
            }
            if(!resultat)
            {
              sprintf(message,"Instruction : erreur de if then else");
              pushErreur(message,courant,methode,listeDecl);
            }
        break;

        case RETURN_VOID : 

        printf("checkListInstruction return 5.5\n");
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
          printf("je suis la dans ma petite expression %d \n",instruction->op);
          type = getType(instruction,NULL,courant,methode,listeDecl);
          if(type==NULL)
          {
            sprintf(message,"Instruction incorrecte %s",instruction->u.str);
            pushErreur(message,courant,methode,listeDecl);
            printf("checkblock check false 5 %d\n",instruction->op);
            resultat = FALSE;
          }
          else
          {
            resultat = TRUE;
          }
          if(!resultat)
          {
            if(type!=NULL)
            {
              sprintf(message,"type : %s  Instruction : erreur d'instruction %d",type->nom,instruction->op);
            }
            else
            {
              sprintf(message,"Instruction : erreur d'instruction %d",instruction->op);
            }
            pushErreur(message,courant,methode,listeDecl);
          }
        break; 
        

        default : 
        printf("L'etiquette : %d n'est pas pris en compte ", instruction->op);
        resultat = FALSE;
     }
  return checkListInstruction(autreinstructions,NULL,courant,methode,listeDecl)&& resultat;
}
bool checkListDeclaration(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{

  return TRUE;
}
bool checkClass(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
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
 printf("Entree 5\n");
  bool nomMaj = FALSE;

  if(courant->nom!=NULL && (courant->nom[0] >= 'A' && courant->nom[0] <= 'Z'))
    nomMaj = TRUE;

  /*TRUE : OK FALSE : NOK*/
  bool heritage = FALSE;
  /*FIXME : boucle qui verifie que la classe mere n'est pas dans la liste d'erreur !!!
   * Si c'est le cas -> renvoye faux directement => et ajouter une erreur du style
   * "Corrigez la classe Mere %s avant",class->classe_mere->nom (utilisez sprintf)
   */
  printf("Entree 6\n");
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
 printf("Entree 7\n");
  bool attribut = checkListAttribut(arbre,ancien,courant,methode,listeDecl);
 printf("Entree 8\n");
  bool methodeC = checkListMethode(arbre,ancien,courant,methode,listeDecl)/*TRUE*/;
 printf("Entree 9\n");
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
 printf("Entree 7.1\n");
  if(!verifAttributClasse(courant))
  {
    char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 24");
    return FALSE;
  }
 printf("Entree 7.2\n");
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
      if(type!=NULL && !equalsType(type,tmp->type))
      {
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Erreur affectation d'un %s par un %s",tmp->type->nom,type->nom);
        pushErreur(message,courant,methode,NULL);
      }
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

bool checkListMethode(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  

  if(courant->isExtend)
  {
   printf("Gestion de la classe %s \n",courant->nom);
    char * nomC = calloc(100,sizeof(char));
    sprintf(nomC,"constructeur %s",courant->classe_mere->nom);   
        
    PVAR tmp = courant->classe_mere->param_constructeur;

   printf("\n\n\n\n\n\n\n\n_______DEBUT__________\n\n\n\n\n\n\n\n");
   printf("Classe courante : %s : sa classe mere %s\n",courant->nom,courant->classe_mere->nom);
    while(tmp!=NULL)
    {
     printf(BLACK"VALEURRRRRRR = %s \n",tmp->nom );
      tmp = tmp->suivant;
    }
    
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
     printf("FFAALLLSSSEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
      char *message = calloc(100,sizeof(char));
      sprintf(message,"Erreur d'appel constructeur : %s mal appelee",classActuel->nom);
      pushErreur(message,classActuel,NULL,NULL);
      return FALSE;
    }
   printf("TRUUUUUUUEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
   /*FIXME ajouter appel de checkBlock sur le bloc du constructeur*/
   printf("\n\n\n\n\n\n\n\n________FIN_________\n\n\n\n\n\n\n\n");
  }
  printf("Aucune methode avant\n");
  if(courant->liste_methodes==NULL)
  {
    return TRUE;
  }
  printf("Aucune methode apres\n");
  SMETH copie = *courant->liste_methodes;
  PMETH tmp = NEW(1,SMETH);
  *tmp = copie;

  while(tmp!=NULL)
  {
    /* /!!!\ Ici il s'arete des qu'une methode est fausse*/
    /*bool checkBloc(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)*/
    printf("************************ Je suis en train de check la methode : %s \n",tmp->nom );
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
    printf("Je check la methode %s\n",methode->nom);

    /*FIXME bool corps = checkBloc(methode->corps);*/
    /* FIXME : concat des messafes???*/
    printf("1\n");
    bool typeRetour = (methode->typeRetour!=NULL);
    printf("2\n");
    bool pvar = checkListOptArg(methode->params,methode);
    printf("3\n");
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
        /* FIXME : existeMethodeOverride */
        /*int methodeDansClasse(PCLASS classe, PMETH methode){*/
        printf("3.1\n");
        redef = methodeDansClasse(methode->home->classe_mere,methode);
        printf("3.2\n");
        if(!redef)
        {
          char* message = NEW(SIZE_ERROR,char);
          sprintf(message,"Erreur dans la methode override %s de la classe %s",methode->nom,methode->home->classe_mere->nom);
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
     /*bool checkBloc(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)*/
     printf("4 \n");
     printf("METHODE NULL : %d\n",methode==NULL );
     bool bloc = FALSE;
     printf("\n\n\n\n\n\n\n\n\n\n\n\nTon race %s \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n",methode->nom);
     
     
     if(methode->corps==NULL)
     {
      printf("MARHCE PAS\n");
      bloc = TRUE;

     }
     else
     {
      /*bbbbbbbbbb*/
      /*pprintTreeMain(methode->corps);*/

      if(getChild(methode->corps,0)==NULL)
      {
        bloc = checkBloc(methode->corps,arbre,courant,methode,NULL);
      }
      else
      {
        
        printf("fin de la street\n");

        bloc = checkBloc(methode->corps,arbre,courant,methode,getChild(methode->corps,0)->u.var);
      }

       
     }
     
     printf("typeRetour %d statique %d redef %d pvar %d bloc ? : %d\n",typeRetour,statique,redef, pvar, bloc );
     printf("5\n");
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


/* Methode permettant de créer un type EvalP de type char* */
EvalP makeEvalStr(char *str){
  EvalP eval = NEW(1, Eval);
  eval->type = EVAL_STR;
  eval->u.str = str;
  /*eval->u.str = strdup(str);*/
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
  /* FIXME : Pointeur ou nouvelle instance ? */
  /*eval->u.var = makeListVar(var->nom, var->type, var->categorie, var->init);*/
  eval->u.var = var;
  return eval;
}

/* Methode permettant de créer un type EvalP de type PCLASS */
EvalP makeEvalClasse(PCLASS classe){
  EvalP eval = NEW(1, Eval);
  eval->type = EVAL_PCLASS;
  /* FIXME : Pointeur ou nouvelle instance ? */
  /*eval->u.classe = makeClasse(classe->nom, classe->param_constructeur, classe->corps_constructeur, classe->liste_methodes, classe->liste_champs, classe->classe_mere, classe->isExtend);*/
  eval->u.classe = classe;
  return eval;
}

/* Methode permettant de créer un type EvalP de type PMETH */
EvalP makeEvalMethode(PMETH methode){
  EvalP eval = NEW(1, Eval);
  eval->type = EVAL_PMETH;
  /* FIXME : Pointeur ou nouvelle instance ? 
  eval->u.methode = makeMethode(methode->nom, 0, methode->corps, methode->typeRetour, methode->params, methode->home);
  eval->u.methode->isStatic = methode->isStatic;
  eval->u.methode->isRedef = methode->isRedef;
  */
  eval->u.methode = methode;
  return eval;
}

/* Methode permettant de créer un type EvalP de type TreeP */
EvalP makeEvalTree(TreeP tree){
  EvalP eval = NEW(1, Eval);
  eval->type = EVAL_TREEP;
  eval->u.tree = tree;  /* FIXME : Pointeur ou nouvelle instance ? */
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
void evalProgramme(TreeP programme){
	/* on a l'attribut listeDeClass qui contient toutes les classes (s'il y en a) --> pas besoin de regarder ListClassOpt */
	EvalP eval = evalContenuBloc(programme->u.children[1], NIL(SVAR));
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
		evalListInstruction(bloc->u.children[1], environnement);
	}
	/* eval de YieldOpt */
	if(bloc->u.children[2] != NIL(Tree)){
		return evalExpr(bloc->u.children[2]->u.children[0], environnement);	/*FIXME : verifier que environnement 													sauvegarde les modifs */
	}
	return NIL(Eval);
}

/* Evalue toutes les variables déclarées */
void evalListDeclVar(PVAR listDeclVar, PVAR environnement){
  	/** ListDeclVar : VAR StaticOpt ID ':' IDCLASS AffectExprOpt ';' LDeclChampsOpt ==> renvoi PVAR */
	if(listDeclVar == NIL(SVAR))	return;
	PVAR tmp = listDeclVar;
	EvalP eval = evalExpr(tmp->init->u.children[0], environnement);
	switch(eval->type){
		case EVAL_STR:
			tmp->init = makeLeafStr(EVALUE_STR, eval->u.str);
			break;
		case EVAL_INT:
			tmp->init = makeLeafInt(EVALUE_INT, eval->u.val);
			break;

	/** TODO : Attente de savoir ce que rend instanciation, envoiMessage et OuRien**/
		case EVAL_PVAR:
			printf("evalListDeclVar->PVAR\n");
			break;
		case EVAL_PCLASS:
			printf("evalListDeclVar->PCLASS\n");
			break;
		case EVAL_PMETH:
			printf("evalListDeclVar->PMETH\n");
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

/* TODO */
void evalListInstruction(TreeP Linstruction, PVAR environnement){

}


/** Renvoie la longueur d'une chaine **/
int sizeString(char *str){
  int size=0;
  while(str[size] != '\0'){
    size++;
  }
  return size;
}

int getVal(EvalP eval){
	switch(eval->type){
		case EVAL_INT:
			return eval->u.val;
		case EVAL_PVAR:
			if(eval->u.var->init->op == EVALUE_INT){
				return eval->u.var->init->u.children[0]->u.val;
			}
    case EVAL_STR:
      return eval->u.str; 
		default:
			printf("Probleme\n");
			exit(0);			
	}
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
			sizeConcat=(
        sizeString(evalExpr(tree->u.children[0], environnement)->u.str)+
        sizeString(evalExpr(tree->u.children[1], environnement)->u.str));
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
				return NIL(Eval);	/* FIXME a changer? */
			}
	
		case IDENTIFICATEUR:
    printf("YOUPPI on est dans ce cas \n");
    printf("=======> Children STR = %s \n",tree->u.str);
			var = getVar(environnement, tree->u.str);
      printf("YOUPPI on est dans ce cas \n");
			if(var == NULL)		return NIL(Eval);
			return makeEvalVar(var);

		case INSTANCIATION:
			return evalInstanciation(tree, environnement);

		case ENVOIMESSAGE:
			return evalEnvoiMessage(tree, environnement);

		case SELECTION :
			return evalSelection(tree, environnement); 

		default: 
			fprintf(stderr, "Erreur! etiquette indefinie: %d\n", tree->op);
			exit(UNEXPECTED);
	} 

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
	int nbArg=0;
	LEvalP tmp_eval = listArg;
	PVAR tmp_param = param;
	while(tmp_eval != NIL(LEval) && tmp_param != NULL){		/* normalement nbEval = nbParam ! */
		switch(tmp_eval->eval->type){
			case EVAL_STR:
				tmp_param->init = makeLeafStr(EVALUE_STR, tmp_eval->eval->u.str);
			case EVAL_INT:
				tmp_param->init = makeLeafInt(EVALUE_INT, tmp_eval->eval->u.val);
			case EVAL_PVAR:
				tmp_param->init = makeLeafVar(EVALUE_PVAR, tmp_eval->eval->u.var);

			/* A ENLEVER NORMALEMENT CA NE DEVRAIT JAMAIS ARRIVE */
			case EVAL_PCLASS:
				tmp_param->init = makeLeafClass(EVALUE_PCLASS, tmp_eval->eval->u.classe);
			case EVAL_PMETH:
				tmp_param->init = makeLeafMeth(EVALUE_PMETH, tmp_eval->eval->u.methode);
			case EVAL_TREEP:
				printf("Dans evalInstanciation -> eval d'un arg est un tree\n");
			default:
				printf("Etiquette non reconnue dans evalInstanciation = %d\n", tmp_eval->eval->type);
				exit(0);
		}
		tmp_eval = tmp_eval->suivant;
		tmp_param = tmp_param->suivant;
	}

	/* TODO : Mettre a jour l'environnement ! --> peut etre fait automatiquement si param est dans environnement */
	
	/* TODO : Appeler le constructeur selon les champs de la classe et les "nouveaux" param du constructeur */
	

	/* Remarque : le nom de l'instance est pour l'instant à NULL et on lui affectera plus tard dans la regle + categorie = 0 par defaut*/
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
			case EVAL_STR:
				init = makeLeafStr(EVALUE_STR, eval_constructeur->u.str);
			case EVAL_PVAR:
				init = makeLeafVar(EVALUE_PVAR, eval_constructeur->u.var);
			default:
				init = NIL(Tree);
		}
		var = makeListVar(NULL, classe, 0, init);
	}
	return makeEvalVar(var);
}

/** FIXME Renvoie une liste d'évaluation -> la liste est dans l'ordre **/
LEvalP evalListArg(TreeP tree, PVAR environnement){
	if(tree == NIL(Tree))	return NIL(LEval);

	/* Le but : récupérer les expr dans l'ordre et faire appel au constructeur de la classe IDCLASS */
	LEvalP listEval;
	/* On a une liste de type LArg, expr*/
	if(tree->op == LISTEARG){
		/* TODO Regarder les evalExpr + demander a Amin & Gishan comment avoir la PVAR d'un ID
			 (exemple : xc -> comment avoir son type et sa valeur)
		--> Reponse : faire un getVar et chercher dans 1) param methode, 2) listDecl, 3) attribut de la classe 
		--> Commentaire de Julien : introduire un PVAR environnement pour régler ce pb?
		  FIXME : verifier si c'est ok
		*/
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

	PMETH methode;	

	/* IDCLASS '.' ID '(' ListOptArg ')' */
	if(tree->u.children[0]->op == IDENTIFICATEURCLASS){
		methode = getMethodeBis(getClasse(listeDeClass, tree->u.children[0]->u.str)->liste_methodes, tree->u.children[1]->u.str);
		if(methode == NULL){
			printf("Problème dans evalEnvoiMessage : méthode introuvable dans la classe \n");
			exit(0);
		}

	}
	/* envoiMessage '.' ID'('ListOptArg ')' */
	else if(tree->u.children[0]->op == ENVOIMESSAGE){
		/* faire appel a evalEnvoiMessage sur tree->u.children[0] */
	
	}
	/* constante '.' ID '(' ListOptArg ')' */
	else if(tree->u.children[0]->op == CSTSTRING){

	}
	/* constante '.' ID '(' ListOptArg ')' */
	else if(tree->u.children[0]->op == CSTENTIER){

	}
	/* ID '.' ID '(' ListOptArg ')'  */
	else if(tree->u.children[0]->op == IDENTIFICATEUR){

	}

	/* selection '.' ID '(' ListOptArg ')' */
	else if(tree->u.children[0]->op == SELECTION){

	}
	/* expr '.' ID '(' ListOptArg ')' */
	else{
		
	}

	/* TODO : Faire l'eval maintenant avec ListOptArg : tree->u.children[2] ==> liste des arguments (parametres) de la methode */

	return NIL(Eval); /* A ENLEVER */
}

EvalP evalSelection(TreeP tree, PVAR environnement){
	return NIL(Eval);
	/*
	// IDCLASS'.'ID 
	if(tree->u.children[0]->op == IDENTIFICATEURCLASS){

		PCLASS classeTMP = getClasse(listeClass,tree->u.children[0]->u.str);
		PVAR varTMP = getVAR(classeTMP,tree->u.children[1]);
		return makeEvalVar(varTMP);
	}	
	// envoiMessage'.'ID
	else if(tree->u.children[0]->op == ENVOIMESSAGE){
		PCLASS classeTMP = getClasse(listeClass,tree->u.children[0]->nom);  //envoiMessage renvoi un PVAR
		PVAR varTMP = getVAR(classeTMP,tree->u.children[1]);
		return makeEvalVar(varTMP);

	}
	// OuRien '.' ID : OuRien = expr ou Cible (=ID ou selection)
	else if(tree->u.children[0]->op == IDENTIFICATEUR){
		// ID '.' ID
	}
	else if(tree->u.children[0]->op == SELECTION){
		// selection '.' ID
	}
	else{
		// normalement c'est expr '.' ID  
	}
  
	*/
}



/** A NE PAS CONSIDERER : C'EST LE EVAL DU TP **/
/*
int eval(TreeP tree, VarDeclP decls) {
  if (tree == NIL(Tree)) { exit(UNEXPECTED); }
  switch (tree->op) {
    case ID:
      return evalVar(tree, decls);
    case CSTE:
      return tree->u.val;
    case CSTS:
      return tree->u.str;

    case EQ:
      return (eval(getChild(tree, 0), decls) == eval(getChild(tree, 1), decls));
    case NE:
      return (eval(getChild(tree, 0), decls) != eval(getChild(tree, 1), decls));

    case PLUS:
      return (eval(getChild(tree, 0), decls) + eval(getChild(tree, 1), decls));
    case MINUS:
      return (eval(getChild(tree, 0), decls) - eval(getChild(tree, 1), decls));

    case MUL:
      return (eval(getChild(tree, 0), decls) * eval(getChild(tree, 1), decls));
    case DIV:
      if(eval(getChild(tree, 1), decls)!=0){
        return (eval(getChild(tree, 0), decls) / eval(getChild(tree, 1), decls));
      }
      else{
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
  }
  else {
    res = eval(tree, lvar); 
    printf("\n/-Result: %d-/\n", res);
  }
  return errorCode;
}
*/

/** FIN DE CE QUI N'EST PAS A CONSIDERER **/





PCLASS getType(TreeP arbre, TreeP ancien, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  if(courant!=NULL)
  {
    courant = getClasseBis(listeDeClass,courant->nom);
  }
  if(arbre==NULL)
  {
    char* message = NEW(SIZE_ERROR,char);
    printf("Arbre est vide\n");
    sprintf(message,"Arbre est vide");
    pushErreur(message,classActuel,methode,NULL);
    return NULL;
  }
  PCLASS tmpDebug = NULL;
  PCLASS integer = NEW(1,SCLASS);PCLASS string = NEW(1,SCLASS);
  printf("1 - 3\n");
  printf("arbre NIL : ? %d\n",arbre==NULL?TRUE:FALSE );
  printf("Etiquette %d\n",arbre->op );
  printf("2\n");
   /* Dans le cas d'une selection, récupérer le dernier élèment */ 
  PCLASS type = NULL;
  PCLASS type2 = NULL;
  LTreeP liste = NULL;
  LTreeP tmpL = NULL;
  bool instCorrecte = FALSE;
  char* message = NEW(SIZE_ERROR,char);
  char *nomC = NULL;
  char * nomClass =NULL;
  PCLASS tmp = NULL;
  LTreeP listeTmp =NULL;
  PCLASS typeDe = NULL;

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
        else
        {
          sprintf(message,"Le type a cote de %s n'est pas reconnu",type->nom);
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
      printf("CONCATENATIONNNNNNNNNNNNNNNNNNNNNNNNNNNN\n   %d", getChild(arbre,1)->op);
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

       printf("0\n");
        liste = transFormSelectOuEnvoi(arbre,liste);
       printf("1 2\n");             
       printf("2\n");
       printf("3----\n");
        return estCoherentEnvoi(liste, courant, methode,listeDecl);
       printf("4 a\n");
     printf("Apres .....\n");
    break;

    case ENVOIMESSAGE :

        printf("HELLO 1\n");
        printf("HELLO 4\n");
        liste = transFormSelectOuEnvoi(arbre,liste);
        return estCoherentEnvoi(liste, courant, methode,listeDecl);
        printf("HELLO 6\n");
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
      printf("MALABAR AUSSI\n");
      tmpDebug = getTypeAttribut(arbre->u.str, courant, methode, listeDecl,FALSE,FALSE);
      printf("CACACACACACA 1\n");
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
        /*printf("op : %s", getChild(arbre,0)->u.str);*/
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
  printf("JE FAIS EST COHERENT ENVOIE  \n");
  if(classe != NULL)  
  {
    printf("LE NOM DE LA CLASSE AVEC CETTE ENVOIT DE MESSAGE EST : %s\n", classe->nom);
  }
  printf("estCoherentEnvoi1\n");
  LTreeP tmp = liste;
  /*while(tmp!=NULL)
  {
    printf("VALUE : %d \n", tmp->elem->op);
    tmp = tmp->suivant;
  }*/
  PCLASS init = NULL;
  printf("rentrer\n");

  bool isStatic = FALSE;
  bool agerer = FALSE;
  printf(" la valeur de bizarre est  %d, \n", tmp->elem->recupType);
  char* message = NEW(SIZE_ERROR,char);
  
  if(liste!=NULL || tmp->elem!=NULL)
  {    
    printf("liste me casse les couilles mais c'est abuser\n");  
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
      printf("1.1\n");

      if(classe!=NULL && (strcmp(tmp->elem->u.str,"super")==0))
      {
        printf("1.super\n");          
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
        printf("1.this\n");
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
         printf("1.rien\n");
          if((classe==NULL) && ((strcmp(tmp->elem->u.str,"this")==0)||(strcmp(tmp->elem->u.str,"super")==0)))
          {
            sprintf(message,"Erreur this ou super present dans le bloc main");
            pushErreur(message,classe,methode,NULL);
            return NULL; 
          }
          else
          {
          printf("getTypeAttribut de %s \n",tmp->elem->u.str);
          /*ici true*/
          init = getTypeAttribut(tmp->elem->u.str, classe, methode, listeDecl, FALSE, FALSE);

          printf("getTypeAttribut fin\n");
          }
      } 
      if(init == NULL)
      {
          printf("1.1.1\n");
         /*FIXME dans le gettypeAttribut rajouter les cas pour "string" et 1*/
          char* message = NEW(SIZE_ERROR,char);
          printf("%s inconnu \n",tmp->elem->u.str);
          sprintf(message,"%s inconnu ",tmp->elem->u.str);
          pushErreur(message,classe,methode,NULL);
          return NULL; 
      }
      else
      {
          printf("INIT = %s\n", init->nom);
      }
          printf("1.2\n");
  }
  else if(tmp->elem->op == IDENTIFICATEURCLASS)
  {
        printf("je suis pas la \n");
        isStatic = TRUE;

        agerer = TRUE;
        printf("2.1\n");
        init = getClasseBis(listeDeClass, tmp->elem->u.str);
        if(init==NULL)
        {
          char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Inconnu %s",tmp->elem->u.str);
          pushErreur(message,classe,methode,NULL);
          return NULL; 
        }
       printf("2.2\n");
  }
  else if(tmp->elem->op == INSTANCIATION)
  {
      printf("3.1\n");
      char * nomClass = getChild(tmp->elem,0)->u.str;
      printf("3.2\n");
      PCLASS tmp = getClasseBis(listeDeClass,nomClass);
      printf("3.3\n");
      if(tmp == NULL)
      {
        printf("3.4\n");
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
      printf("je fais CSTSTRING\n");
      init = getClasseBis(listeDeClass,"String");
  }
  else if(tmp->elem->op == CSTENTIER)
  {
      printf("je fais le CTSENTIER\n");
      init = getClasseBis(listeDeClass,"Integer");
  }
  else if(tmp->elem->recupType==1)
  {
      printf("L'element est un type bizareeeee ===============\n");
      init = getType(tmp->elem,NULL,classe,methode,listeDecl);
      if(init != NULL)
      {
        printf("Toi la tu me soul hein : %s", init->nom);
      }
  }
    short etiquette = tmp->elem->op;
    printf("3.6\n");
    PCLASS tempoAffiche;
    while(tmp!=NULL)
    {
        printf("je suis dans ma petite boucle while \n");

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
            printf("il y a un soucis\n");
            return NULL; 
        }
        
        if(tmp->elem->suivant!=NULL || tmp->elem->isEnvoiMessage==TRUE)
        {
            /*printf("tmp->elem->suivant %s \n",tmp->elem->suivant->op);*/

            printf("J'AI UNE FONCTION CACA :%s \n",tmp->elem->u.str);
            printf("25-25-25\n");

           
            if(init==NULL)
            {
               printf("INIT EST NULL\n");
               sprintf(message,"EnvoiMessage : il y a un probleme au niveau de celui ci");
               pushErreur(message,classe,methode,NULL);
               printf(" mais c'est null\n");
               return NULL; 
            }
            else
            {
              if(tmp->elem->suivant == NULL)
              {
                printf("PROBLEME : LISTE ARG POUR LUI EST NUL");
              }
              init = appartient(init,tmpElem,TRUE,methode,listeDecl,tmp,etiquette,isStatic,agerer);
            }
            printf("apres 25-25-25 \n");
             
            if(init==NULL)
            {   
                printf("INI EST UN GROS ENFOIRE\n");
                char* message = NEW(SIZE_ERROR,char);
                if(tempoAffiche == NULL)
                {
                  sprintf(message,"EnvoiMessage : il y a un probleme au niveau de celui ci");
                }

                else
                {
                  sprintf(message,"Erreur la methode %s n'appartient pas a %s",tmp->elem->u.str,tempoAffiche->nom);

                }
                pushErreur(message,classe,methode,NULL);
                return NULL; 
            }               
                tempoAffiche = init;

        }
        else if(tmp->elem->isEnvoiMessage==FALSE)
        { 

            printf("VALUE : %s \n",tmp->elem->u.str);
            printf("26_26_26\n");
            init = appartient(init,tmpElem,FALSE,methode,listeDecl,tmp,etiquette, isStatic,agerer);
            if(init == NULL)
            {
                char* message = NEW(SIZE_ERROR,char);
                if(tmp->elem == NULL)
                {
                  printf("je suis ici\n");
                  sprintf(message,"Probleme envoie de message",tmp->elem->u.str,tempoAffiche->nom);
                }
                else if(tmp->elem != NULL && tempoAffiche != NULL)
                {
                  printf("je suis plutot la\n");
                  printf("%s ===> \n",tmp->elem->u.str);
                  printf("%s ===> \n",tempoAffiche->nom);
                  sprintf(message,"Erreur la varibable  %s n'appartient pas a %s",tmp->elem->u.str,tempoAffiche->nom); 
                }
                       
                pushErreur(message,classe,methode,NULL);
                return NULL; 
                
            }
            tempoAffiche = init;
        }
        printf("etiquette avant\n");
        etiquette = tmp->elem->op;
        printf("etiquette apres\n");
    }
    return init;  
}



PCLASS appartient(PCLASS mere, TreeP fille, bool isEnvoiMessage, PMETH methode, PVAR listeDecl, LTreeP tmp,short etiquette, bool isStatic, bool agerer)
{
  printf("cake lol \n");
  /*printf("Appartient : %s\t%s\n",mere->nom, fille->u.str);*/
   
  if(mere!=NULL)
  {
    mere = getClasseBis(listeDeClass,mere->nom);
  }
  
  if(fille==NULL || mere==NULL){
    printf("ERREUR \n");
    char* message = NEW(SIZE_ERROR,char);
    sprintf(message,"fonction tp.c appartient : Erreur Arbre");
    pushErreur(message,mere,methode,NULL);
    return NULL;
  }

  if(isEnvoiMessage)
  {
   printf("1_1_1_1_1\n");
    
    PMETH listMeth = NULL;
    if(mere!=NULL)
    {
      printf("avant-----q-s-qs--s-q %s -> \n",mere->nom);
    }
    else
    {
      printf("avant-----q-s-qs--s-q ");
    }

    if(mere->liste_methodes!=NULL)
    {
      printf("les listes ne sont pas null \n");
      SMETH copieConforme = *mere->liste_methodes;
      listMeth = NEW(1,SMETH);
      *listMeth = copieConforme;
    }
    else
    {
        printf("222\n");
        printf("La classe %s n'a pas de methode \n",mere->nom );
        
        listMeth = mere->liste_methodes; 
        if(mere->liste_champs==NULL)
        {
          printf("TOUJOUR PAS La classe %s n'a pas de methode \n",mere->nom );
        }
        else
        {
          printf("OUAH MAGIQUE\n");
        }
           
    }
    bool isVerifOk = FALSE;
    while(listMeth!=NULL)
    {
      printf("NOMMETH %s\n",listMeth->nom );
      printf("Je compare methode : %s et filles %s\n", listMeth->nom,fille->u.str);
      if(strcmp(listMeth->nom,fille->u.str)==0)
      {
        /* Verifie si les parametre de de listMeth->param & fille */
       printf("Allez isVerif  : ? %s \n",listMeth->nom);
       printf("l'etiquette transmis ici : %d\n", etiquette);
       /*tmp -> elem -> suiavnt ??? */
       isVerifOk = getTypeMethode(listMeth->nom,mere,etiquette,tmp->elem->suivant,methode,listeDecl,isStatic)!=NULL?TRUE:FALSE;
       printf("Fin isVerif %d \n",isVerifOk);

        if(isVerifOk)
        {
          
          return listMeth->typeRetour;
        }
        else
        {
          printf("MON VERIF N'EST PAS BON--------------------------------------------\n");
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
    printf("liste meth =\n");
    sprintf(message,"Erreur init 1\n");
    pushErreur(message,NULL,NULL,NULL);
    return NULL;
  }
  else
  {
     return getTypeAttribut(tmp->elem->u.str, mere, methode, listeDecl,isStatic, agerer);    
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
 printf("1.1.1\n");
  bool estDansParamMeth = FALSE;
  bool estDansListeDecl =  FALSE;
  bool estDansAttributClasse = FALSE;
  PCLASS res = NULL;
  
  if(methode != NULL)
  {
   printf("1.1.2\n");
    
    PVAR paramParcours = methode->params;
    char** variable = calloc(1,sizeof(char*));
    int i = 0;
    while(paramParcours!=NULL)
    {
     printf("cacaca\n");
      variable[i] = calloc(100,sizeof(char));
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
    


    PVAR param = methode->params;

    while(param!=NULL)
    {
     printf("1.1.3\n");
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
   printf("1.1.4\n");
  }
  
  if(listeDecl!=NULL)
  {
   printf("1.1.5\n");

    PVAR listDeclParcours = listeDecl;
    char** variable = calloc(1,sizeof(char*));
    int i = 0;
    printf("1.1.5.1\n");
    while(listDeclParcours!=NULL)
    {
      printf("Avant Affichage\n");
      printf("%s\n", listDeclParcours->nom);
      variable[i] = calloc(100,sizeof(char));
      strcpy(variable[i],listDeclParcours->nom);
      i++;
      listDeclParcours = listDeclParcours->suivant;
    }
    printf("1.1.5.2\n");
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
    printf("1.1.5.3\n");
    PVAR listDeclaration = listeDecl; 
    printf("1.1.5.3.1\n");
    while(listDeclaration!=NULL)
    {
      printf("1.1.5.4\n");
      if(strcmp(nom,listDeclaration->nom)==0 && estDansParamMeth==TRUE)
      {
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Erreur l'attribut %s est redeclaree 1",nom);
        pushErreur(message,classe,methode,NULL);
        return NULL;
      }
      else if(strcmp(nom,listDeclaration->nom)==0 && estDansParamMeth==FALSE){
        printf("1.1.5.5\n");
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
   printf("1.1.6\n");
  }

  printf("1.1.6.1\n");
  /*printf("IS CLASSE NULLLLLLLLLLLL %d\n",classe->liste_champs==NULL );*/
  /*printf("classe->liste_champs null ? %d\n",classe==NULL?TRUE:FALSE);*/
  if(classe!=NULL && classe->liste_champs != NULL)
  {
   printf("1.1.7\n");

    PVAR listeClasseParcours = classe->liste_champs;
    char** variable = calloc(1,sizeof(char*));
    int i = 0;

    printf("\n\n\n DEBUT LISTE DECL \n\n\n");
    while(listeClasseParcours!=NULL)
    {
      printf(" : : : : %s\n",listeClasseParcours->nom );
      variable[i] = calloc(100,sizeof(char));
      strcpy(variable[i],listeClasseParcours->nom);
      i++;
      listeClasseParcours = listeClasseParcours->suivant;
    }
    printf("\n\n\nFIN LISTE DECL \n\n\n");
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

   PVAR listeClasse = classe->liste_champs;
   printf("1.1.8\n");
    while(listeClasse!=NULL)
    {
     printf("1.1.9\n");
      if(strcmp(nom,listeClasse->nom)==0 && (estDansListeDecl==TRUE))
      {
       printf("1.1.10\n");
        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Erreur l'attribut %s est redeclaree %s",nom,classe->nom);
        pushErreur(message,classe,methode,NULL);
        return NULL;
       printf("1.1.11\n");
      }
      else if(strcmp(nom,listeClasse->nom)==0 && estDansListeDecl==FALSE){
        if(!agerer)
        {
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
         printf("1.1.13\n");
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
  if(res==NULL)
  {
   printf("gros soucis\n");
  }
  else{
   printf("lolololo\n");
  }
 printf("1.1.6.2\n");
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
      if(strcmp(variable[i],variable[j])==0)
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
    printf("la classe dans le gettYPEmETHODE EST :%s", classe->nom);
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
   printf("a.1\n");
    if(precedant == IDENTIFICATEURCLASS)
    {
     printf("a.2\n");
      if(strcmp(nom,tmp->nom)==0 && tmp->isStatic && isStatic)
      {
        bool verifOk = compareParametreMethode(tmp->params,appelMethode, classe,methode, listeDecl, nom);
        if(verifOk)
        {
          return tmp->home;
        }
      }
    }
    else if(precedant == IDENTIFICATEUR)
    {
     printf("a.3\n");
      if(strcmp(nom,tmp->nom)==0 && !tmp->isStatic && !isStatic)
      {
       printf("a.4\n");
        bool verifOk = compareParametreMethode(tmp->params,appelMethode, classe,methode, listeDecl, nom);
       printf("a.5\n");
        if(verifOk)
        {
          return tmp->home;
        }
      }
    }
    else if(precedant == CSTSTRING || precedant == CSTENTIER)
    {
      /*GISHAN NEEEEE*/
      printf("AIGHT\n");
      bool verifOk = compareParametreMethode(tmp->params,appelMethode, classe,methode, listeDecl, nom);
      printf("a.66\n");
      if(verifOk)
      {
        return tmp->home;
      }
    }
    else
    {
      bool verifOk = compareParametreMethode(tmp->params,appelMethode, classe,methode, listeDecl, nom);
      printf("a.66\n");
      printf("VERIFICATION : %d\n",verifOk);
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
   printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");	
   char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 26");
    return FALSE;
  }
  else if (appelMethode==NULL && declaration==NULL)
  {
   printf("BDQBBQBQBQBBQBQBBQBQBQBQBQBQBQBBQ\n");;
    return TRUE;
  }

  /*Transformer a->b->c*/
  PCLASS liste = NULL;
 printf("a.6\n");

  liste = transformerAppel(appelMethode,liste,classe,methode,listeDecl);

  PCLASS pointeur = liste;
  if(classe!=NULL)
    printf("Classe courante %s \n",classe->nom );
  while(pointeur!=NULL)
  {
    printf("CLASSSSE APP %s\n",pointeur->nom );
    pointeur = pointeur->suivant;
  }

  if(liste==NULL)
  {
    char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 29");
    return FALSE;
  }
 printf("a.7\n");

  /*printf("--D1-- lelena : %s\n ----F1--\n",liste->nom);
 printf("--D2-- lelena : %s\n ----F2---\n",liste->suivant->nom);*/

  PCLASS tmp = liste;
  PVAR tmpDeclarationOfficiel = declaration;

  if(tmp==NULL && tmpDeclarationOfficiel==NULL)
  {
    return TRUE;
  }
 printf("..1.. \n");
  SCLASS contenuTMP = *tmp;
 printf("..2.. \n");
  PCLASS tmp2 = NEW(1,SCLASS);
  *tmp2 = contenuTMP;
 printf("..3.. \n");
  int cpt = 0;
  
  while(tmp2!=NULL)
  {
    cpt++;
   printf("AppelMethode : %s\n", tmp2->nom);
    tmp2 = tmp2->suivant;
  }
 printf("AppelMethode contient : %d\n", cpt);
 printf("..4.. \n");

  SVAR contenuDeclaration = *tmpDeclarationOfficiel;
 printf("..5.. \n");
  PVAR tmpDeclarationOfficiel2 = &contenuDeclaration;
 printf("..6.. \n");
  int cptDeclaration = 0;
  while(tmpDeclarationOfficiel2!=NULL)
  {
    cptDeclaration++;
   printf("ParamOfficiel : %s\n",tmpDeclarationOfficiel2->type->nom);
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
     char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 29");
      return FALSE;
    }

    if(!equalsType(tmpDeclarationOfficiel->type,tmp))
    {
     printf("return 4\n");
     char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 30");
      return FALSE;
    }

    tmpDeclarationOfficiel = tmpDeclarationOfficiel->suivant;
    tmp = tmp->suivant;
  }
 printf("BYE BYE\n");
  return TRUE;

}

PCLASS transformerAppel(TreeP appelMethode,PCLASS liste, PCLASS courant, PMETH methode, PVAR listeDecl)
{
  if(courant!=NULL)
  {
    courant = getClasseBis(listeDeClass,courant->nom);
  }
 printf("ETIQUETE   ----------- op : %d\n", appelMethode->op);
  if(liste==NULL)
  {
   printf("LISTE EST NULL \n");
    if(appelMethode->op!=LISTEARG)
    {
     printf("je suis ici 2 -- 2\n");
      PCLASS getTypeRetour = getType(appelMethode,appelMethode, courant, methode, listeDecl);
      if(getTypeRetour==NULL)
      {
        char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 3");
        pushErreur(message,NULL,NULL,NULL);
        return NULL;
      }
      getTypeRetour = getClasseBis(listeDeClass,getTypeRetour->nom);
      /*printf("1 ?????? \n");
      printf("getTypeRetour : %s\n",getTypeRetour->nom);*/
      return getTypeRetour;
    }
    else
    {
     printf("ABCABC\n");
      PCLASS getTypeRetour = getType(getChild(appelMethode,1),appelMethode, courant, methode, listeDecl);
      /*printf("2 ?????? \n");
      printf("getTypeRetour : %s\n",getTypeRetour->nom);*/
      if(getTypeRetour==NULL)
      {
        char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 4");
        pushErreur(message,NULL,NULL,NULL);
        return NULL;
      }
      getTypeRetour = getClasseBis(listeDeClass,getTypeRetour->nom);
      PCLASS POINTEUR = getTypeRetour; 
      while(POINTEUR!=NULL)
      {
        printf("GET TYPE RETOUR LISTE NULL= %s\n",getTypeRetour->nom);
        POINTEUR = POINTEUR->suivant;
      }

      liste = getTypeRetour;

    }
  }
  else
  {
    if(appelMethode->op!=LISTEARG)
    {
     printf("XYZXYZ 2\n");
      SCLASS tmp = *liste;
      PCLASS getTypeRetour = getType(appelMethode,NULL, courant, methode, listeDecl);
      /*printf("3 ?????? \n");
     printf("getTypeRetour : %s\n",getTypeRetour->nom);*/

      
      if(getTypeRetour!=NULL)
      {
        getTypeRetour = getClasseBis(listeDeClass,getTypeRetour->nom);
        liste = getTypeRetour;
        liste->suivant = NEW(1,SCLASS);
        *liste->suivant = tmp;
        PCLASS POINTEUR = liste;
        while(POINTEUR!=NULL)
        {
          printf("LISTE GET TYPE = %s\n",getTypeRetour->nom);
          POINTEUR = POINTEUR->suivant;
        }
        return liste;
      }
      else
      {
        char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 5");
        pushErreur(message,NULL,NULL,NULL);
        return NULL;
      }
      
      
    }
    else
    {
     printf("ABCABC 2\n");
      SCLASS tmp = *liste;

     printf("Classe courante : %s\n",courant->nom);

      /*printf("CONTENU = %s\n",getChild(appelMethode,1)->u.var);*/
      PCLASS getTypeRetour = getType(getChild(appelMethode,1),appelMethode, courant, methode, listeDecl);
      if(getTypeRetour!=NULL)
      {
        getTypeRetour = getClasseBis(listeDeClass,getTypeRetour->nom);
        liste = getTypeRetour;
        liste->suivant = NEW(1,SCLASS);
        *liste->suivant = tmp;
        /*return liste;*/
      }
      else
      {
        char *message = calloc(SIZE_ERROR,sizeof(char));sprintf(message,"Erreur init 5");
        pushErreur(message,NULL,NULL,NULL);
        return NULL;
      }
      
    }
  }
  return transformerAppel(getChild(appelMethode,0),liste,courant,methode,listeDecl);
}


/* Cree une liste chainee lorsqu'il y a une selection ou un envoi de message */

LTreeP transFormSelectOuEnvoi(TreeP arbre, LTreeP liste)
{
  
  printf("arbre -> op %d\n",arbre->op );
  if(liste==NULL){
  printf("h.1\n");
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
    printf("h.2\n");
    listeTree tmp = *liste;
    printf("h.3 -> %d \n",arbre->nbChildren);

    if(arbre->nbChildren==0)
    {
      return liste;
    }
    /* Faire la verif à ce niveau aussi */
    liste->elem = getChild(arbre,1);
    printf("h.4\n");
    liste->suivant = NEW(1,listeTree);
    *liste->suivant = tmp;
    printf("h.5\n");
    if(arbre->nbChildren == 3)
    {
      printf("h.6\n");
      liste->elem->suivant = getChild(arbre,2);
      liste->elem->isEnvoiMessage = TRUE;
    }
    else
    {
      printf("h.7\n");
      liste->elem->isEnvoiMessage = FALSE;
    }
    printf("h.8\n");
  }
  /*aminamin*/
  printf("Debul %d getChild(arbre,0) %d \n",arbre->op,getChild(arbre,0)->op);
  if(getChild(arbre,0)->op==IDENTIFICATEUR || getChild(arbre,0)->op==IDENTIFICATEURCLASS || getChild(arbre,0)->op==CSTSTRING || getChild(arbre,0)->op==CSTENTIER)
  {
    if(liste!=NULL)
    {
      printf("J'AI FINIS MA TRANSFORMOTION \n");
      listeTree tmp = *liste;
      liste->elem = getChild(arbre,0);
      liste->suivant = NEW(1,listeTree);
      *liste->suivant = tmp;
    } 
    printf("JE RETOURNE MAS TRANSFORMATION \n");
    return liste;
  }
  
  else if(getChild(arbre,0)->op==SELECTION || getChild(arbre,0)->op==ENVOIMESSAGE)
  {

    return transFormSelectOuEnvoi(getChild(arbre,0),liste);
  }
  else{
    printf("LE OP EST : %d",getChild(arbre,0)->op);
    printf("======================liste est bizare ==========================\n");
    if(liste!=NULL)
    {
      listeTree tmp = *liste;
      liste->elem = getChild(arbre,0);
      liste->elem->recupType = 1;
      liste->suivant = NEW(1,listeTree);
      *liste->suivant = tmp;
    } 
    LTreeP parcours = liste;
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

/* FIXME : equalsAffectation : verifie si Point = Point2D correcte (OUI) et inversemment */

void testEval(){
  /*
  expr : PLUS expr %prec unaire     { $$=makeTree(PLUSUNAIRE, 1, $2); }
       | MINUS expr %prec unaire    { $$=makeTree(MINUSUNAIRE, 1, $2); }
       | expr CONCAT expr       { $$=makeTree(CONCATENATION, 2, $1, $3); }
       | expr PLUS expr       { $$=makeTree(PLUSBINAIRE, 2, $1, $3); }
       | expr MINUS expr        { $$=makeTree(MINUSBINAIRE, 2, $1, $3); }
       | expr DIV expr          { $$=makeTree(DIVISION, 2, $1, $3); }
       | expr MUL expr          { $$=makeTree(MULTIPLICATION, 2, $1, $3); }
       | expr RELOP expr        { $$=makeTree(OPCOMPARATEUR, 3 , $1, $3, makeLeafInt(OPERATEUR,$2));}
       | constante          { $$=$1; }
       | instanciation          { $$=$1; }
       | envoiMessage         { $$=$1; }
       | OuRien             { $$=$1; }
       ;
  */

/*
 * Creation d'arbres bidule de teste Exemple :  x:= a + b
 */
 int a=8,b=6;
TreeP treeTestPLUS = makeTree(PLUSBINAIRE, 2,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b));
TreeP treeTestMINUS = makeTree(MINUSBINAIRE, 2,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b));
TreeP treeTestDIV = makeTree(DIVISION, 2,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b));
TreeP treeTestMUL = makeTree(MULTIPLICATION, 2,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b));
TreeP treeTestNE = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,NE));
TreeP treeTestEQ = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,EQ));
TreeP treeTestGT = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,GT));
TreeP treeTestGE = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,GE));
TreeP treeTestLT = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,LT));
TreeP treeTestLE = makeTree(OPCOMPARATEUR, 3,  makeLeafInt(CSTENTIER,a), makeLeafInt(CSTENTIER,b),makeLeafInt(OPERATEUR,LE));
/*  a-b+44 */
TreeP treeTestPLUSImbrique = makeTree(PLUSBINAIRE, 2,  treeTestMINUS, makeLeafInt(CSTENTIER,44));
/*
 * Creation d'arbres bidule de teste Exemple :  x:= "yas"&"ser";
 */
char* ch1="yas"; char* ch2="ser"; char* ch3=" RABI";
TreeP treeTestCONCATENATION = makeTree(CONCATENATION, 2,  makeLeafStr(CSTSTRING,ch1), makeLeafStr(CSTSTRING,ch2)); 
TreeP treeTestCONCATENATIONImbrique = makeTree(CONCATENATION, 2, treeTestCONCATENATION, makeLeafStr(CSTSTRING,ch3)); 
/* Creation d'arbre avec envirenoment pour les identificateurs */


printf("============= DEB Eval TEST =============\n");
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

printf("============= FIN Eval TEST =============\n");

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
