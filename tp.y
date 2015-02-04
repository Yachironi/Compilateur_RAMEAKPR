
/** ANALYSE SYNTAXIQUE + GRAMMAIRE **/

/* FIXME Ajouter une methode qui copie les methodes de la classe mere dans l'attribut classe->override
 * SI EXTENDS
 * Aussi copier les attributs
 */

/* les tokens ici sont ceux supposes etre renvoyes par l'analyseur lexical
 * A adapter par chacun en fonction de ce qu'il a ecrit dans tp.l
 * Bison ecrase le contenu de tp_y.h a partir de la description de la ligne
 * suivante. C'est donc cette ligne qu'il faut adapter si besoin, pas tp_y.h !
 */
%token CLASS VAR EXTENDS IS DEF OVERRIDE RETURNS YIELD IF THEN ELSE NEWO PLUS MINUS RELOP AFFECT MUL DIV CST STRING CONCAT
%token <S> ID CSTS IDCLASS RETURN STATIC
%token <I> CSTE

/* indications de precedence d'associativite. Les operateurs sur une meme
 * ligne (separes par un espace) ont la meme priorite. Les ligns sont donnees
 * par precedence croissante d'operateurs.
 */
%left CONCAT
%nonassoc RELOP
%left PLUS MINUS
%left MUL DIV 
%left '.'
%left unaire

/* voir la definition de YYSTYPE dans main.h 
 * Les indications ci-dessous servent a indiquer a Bison que les "valeurs" $i
 * ou $$ associees a ces non-terminaux doivent utiliser la variante indiquee
 * de l'union YYSTYPE (par exemple la variante D ou S, etc.)
 * La "valeur" associee a un terminal utilise toujours la meme variante
 */



/* %type <C> REL */
%type <T> expr Programme Bloc BlocOpt ContenuBloc YieldOpt Cible Instruction ContenuClassOpt AffectExprOpt BlocOuExpr /*ListExtendsOpt*/ selection constante instanciation envoiMessage LInstruction LInstructionOpt OuRien ListOptArg LArg
%type <V> ListDeclVar LDeclChampsOpt LParam ListParamOpt Param
%type <M> Methode LDeclMethodeOpt
%type <CL> LClassOpt DeclClass ListExtendsOpt
%type <I> OverrideOuStaticOpt StaticOpt
 
/* Initialisation des variables globales*/

%{
#include "tp.h"     /* les definition des types et les etiquettes des noeuds */
PCLASS classActuel; /* Classe en cours d'analyse*/
TreeP programme;

extern int yylex(); /* fournie par Flex */
extern void yyerror();  /* definie dans tp.c */

%}

%% 
 /*
 * Attention: on est dans un analyseur ascendant donc on s'occupe des composants
 * d'une construction avant de traiter la construction elle-meme. Dans le cas
 * d'un IF on traitera donc la condition et les parties THEN et ELSE avant
 * de traiter le 'IF' lui-meme. Comme on ne doit evaluer que l'une des deux
 * branches 'then' ou 'else' selon la valeur de la condition, on ne peut pas
 * evaluer les expressions au fur et a mesure qu'on les rencontre puisqu'on ne
 * sait pas si on fait partie d'un IF ou pas. On doit attendre de connaitre
 * l'expression complete pour savoir si telle partie doit etre evaluee ou non.
 *
 * Les macros d'allocation NEW et de nullite NIL sont definies dans tp.h.
 * Leur usage n'est bien sur pas obligatoire, juste conseille !
 *
 * Les definition des types YYSTYPE, VarDecl, VarDeclP, Tree, TreeP et autres
 * sont dans tp.h
 */

/*
 * Declaration d'une classe => vérifier ce qui est optionnel ou non
 * Les : c'est à mettre entre cote au niveau de ID: ?
 * epsilon est declare au dessus
 */

/*
 * Axiome : Liste de classe optionnel suivi d'un bloc obligatoire
 */ 

Programme : LClassOpt Bloc			{$$=makeTree(PROGRAM,2,makeLeafClass(LISTCLASS,$1),$2);programme=$$;}

/*
 * Liste de classes optionnelle : Vide ou composee d'au moins une declaration de classe
 */
LClassOpt : DeclClass LClassOpt     {$1->suivant=$2; $$=$1;}
            | /* epsilon */       {$$=NIL(SCLASS);}
            ;

/*
 * Un bloc est defini par une entite avec 2 accolades entourant un contenu
 */
Bloc : '{' ContenuBloc '}'      {$$=$2;}
      ;

/*
 * Le contenu d'un bloc : Une 
 * une liste d'instruction optionnelle suivi d'un Yield option => S'il y a un yield => bloc procedural
 * Ou si on a une List de declaration de valeur => oblige apres le IS d'avoir une Liste d'instruction
 * suivi par un Yield optionnel
 */
 
ContenuBloc : LInstructionOpt YieldOpt        {$$=makeTree(CONTENUBLOC,3,NIL(Tree),$1,$2);}
      | ListDeclVar IS LInstruction YieldOpt  {$$=makeTree(CONTENUBLOC,3,$1,$3,$4);}  
      ;

/*
 * Sert a differencier les deux types de bloc : fonctionnel et procedural
 */

YieldOpt : YIELD expr     {$$=makeTree(ETIQUETTE_YIELD, 1, $2);}
        | /* epsilon */       {$$=NIL(Tree);}


ListDeclVar : VAR StaticOpt ID ':' IDCLASS AffectExprOpt ';' LDeclChampsOpt 
            {
            $$=makeListVar($3,getClasse(listeDeClass,$5),$2,$6); 
            $$->suivant=$8;
            }
            ;

/*
 * Liste d'instructions optionnel 
 */

LInstructionOpt : Instruction LInstructionOpt {$$=makeTree(LIST_INSTRUCTION, 2, $1, $2);} // makeTree ou makeList?
                | /* epsilon */ {$$=NIL(Tree);}
                ;
/*
 * Liste d'instructions obligatoires 
 */
LInstruction : Instruction LInstructionOpt  {$$=makeTree(LIST_INSTRUCTION, 2, $1, $2);}
              ;

/*
 * Une instruction c'est : 
    *  expression;
    * bloc-procedural
    * cible:= expression;
    * if expression then instruction else instruction
 */

Instruction : expr ';'            {$$=$1;}
            | RETURN expr ';'         {$$=makeTree(EXPRESSIONRETURN, 1, $2);}
            | Bloc            {$$=$1;}
            | Cible AFFECT expr ';'       {$$=makeTree(ETIQUETTE_AFFECT, 2, $1, $3);} 
            | IF expr THEN Instruction ELSE Instruction   {$$=makeTree(IFTHENELSE, 3, $2, $4, $6);}
            | RETURN ';'          {$$=makeLeafStr(RETURN_VOID, MSG_VOID);}  /* return void */
            ;
/*
 * La cible de l'affectation ne peut etre qu'un identifiant : 
 * un nom d'un objet x := new Point(1,5)
 * un nom de classe var res : Point := new Point(x, y);
 */

Cible : ID    {$$=makeLeafStr(IDENTIFICATEUR,$1);}
      | selection {$$=$1;}
      ;

/*
 * Bloc optionnel
 */
BlocOpt : Bloc    {$$=$1;}
        | /* epsilon */ {$$=NIL(Tree);}
        ;

/*
 * class nom(param, ...) [extends nom(arg, ...)] [bloc] is {decl, ...}
 */
DeclClass : CLASS IDCLASS '('ListParamOpt')' ListExtendsOpt BlocOpt IS '{'ContenuClassOpt'}' 
    {   
      if(getClasse(listeDeClass, $2) != NULL){
        /* probleme : la classe qu'on souhaite declaree existe deja */
        /* FIXME : gerer l'erreur */

        char* message = NEW(SIZE_ERROR,char);
        sprintf(message,"Erreur la classe %s est deja declare",$2);
        pushErreur(message,classActuel,NULL,NULL);
      }     
      else
      {
        int isExtend; 
        if($6==NIL(SCLASS)){
          isExtend=FALSE;
        }else{
          isExtend=TRUE;
        }
        classActuel=makeClasse(listeDeClass,$2,$4,$7,$10->u.children[1]->u.methode,$10->u.children[0]->u.var,/*getClasse(listeDeClass,$6->u.children[0]->u.str)*/ $6,isExtend);
        $$=classActuel;
      }
    }
    ;

/* Contenu d'une classe : elle peut contenir une liste des champs et/ou des methodes */
ContenuClassOpt : LDeclChampsOpt LDeclMethodeOpt  {$$=makeTree(CONTENUCLASS,2,makeLeafVar(LISTEVAR,$1),makeLeafMeth(LISTEMETHODE,$2));}
    ;

/* 
 * var [static] nom : type [:= expression]; 
 */
LDeclChampsOpt : VAR StaticOpt ID ':' IDCLASS AffectExprOpt ';' LDeclChampsOpt  
        {$$=makeListVar($3,getClasse(listeDeClass,$5),$2,$6); $$->suivant=$8;}
              |     {$$=NIL(SVAR);}
              ;

/* Renvoie un entier : 1 si la methode est static, 0 sinon */
StaticOpt : STATIC  {$$=1;}
          |     {$$=0;}
          ;

/* Affectation optionnelle d'une expression */
AffectExprOpt : AFFECT expr ';'   {$$=makeTree(ETIQUETTE_AFFECT, 1, $2);}
              |       {$$=NIL(Tree);}
              ;

/* Declaration d'une Methode :
 * def [override | static] nom (param, ...) returns Classe bloc
 * def [override | static] nom (param, ...) returns Classe := expression
 */

Methode: DEF OverrideOuStaticOpt ID '(' ListParamOpt ')' RETURNS IDCLASS BlocOuExpr 
      {$$=makeMethode($3,$2,$9,getClasse(listeDeClass,$8),$5,classActuel);}
  ;

/* Liste de methode */
LDeclMethodeOpt : Methode LDeclMethodeOpt {$1->suivant=$2; $$=$1;}
              |         {$$=NIL(SMETH);}
              ;

/* Renvoie un entier : 0 si la methode n'est ni statique, ni override; 1 si elle est override et 2 si elle est static */ 
OverrideOuStaticOpt : OVERRIDE     {$$=1;}
                      | STATIC     {$$=2;}
                      | /* epsilon */    {$$=0;}
                      ;

/* Soit un bloc, soit une expression */
BlocOuExpr : AffectExprOpt    {$$=$1;}
           | Bloc     {$$=$1;}
           ;

ListParamOpt : LParam       {$$=$1;}
              | /* epsilon */     {$$=NIL(SVAR);}
              ;

LParam : Param        {$$=$1 ;}
        | Param','LParam    {$1->suivant=$3; $$=$1;}
        ;

Param : ID':' IDCLASS     {$$= makeListVar($1,getClasse(listeDeClass,$3),0,NIL(Tree));} /* 0 = var non static */
          ;   

/** Julien : j'ai essaye de renvoyer directement une classe **/
/* Pour Amin : il faut remplacer les commentaires par les verifs que tu dois faire */
ListExtendsOpt : EXTENDS IDCLASS'('ListOptArg')'
  {
    $$=getClasse(listeDeClass, $2);
    if($$ == NULL){
      /* la classe n'existe pas: erreur */
      char* message = NEW(SIZE_ERROR,char);
      sprintf(message,"Erreur la classe %s n'existe pas",$2);
      pushErreur(message,classActuel,NULL,NULL);
    }
    else{
      /* appeler une fonction qui verifie si ListOptArg est coherent avec la classe ($$) */
      appelConstructeurEstCorrecte($4,$$);
    }
  } /*$$=makeTree(EXTENTION, 2, makeLeafStr(IDENTIFICATEURCLASS,$2),$4);}*/
               | /* epsilon */        {$$=NIL(/*Tree*/ SCLASS);}
               ;
ListOptArg :            {$$=NIL(Tree);}
     | LArg         {$$=$1;}
           ;


LArg : expr           {$$ = $1;}
     | LArg','expr              {$$=makeTree(LISTEARG, 2, $1,$3);}
     ;

/*
 * pour envoiMessage : fonction() + 5; ==> fonction prioritaire par rapport a 5 A VOIRRRRRRRRRRRRRRRRRRRRRRRRRR (Amin et Gishan)
 * Reponse Amin : on a vu avec le prof en fait le + est associtif gauche donc pas de probleme => on avait conclu ca avec Gishan
  E : E+E
    | Fonc%prec
 Pareil pour + unaire - unaire

  * Une expression est : 
     * identificateur
     * selection
     * constante
     * (expression)
     * instanciation
     * envoi de message
     * expression arithmetique ou de comparaison
     * return expression
 */

/* !!!!!!!!!!!!!!!!!!! TODO !!!!!!!!!!!!!!!!!!!!!! ID passe dans OuRien->Cible */

/* Pour info : ID est dans Cible */
expr : PLUS expr %prec unaire   { $$=$2; }
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
       ;

OuRien : '(' expr ')'     {$$=$2;}
       | Cible        {$$=$1;}
       ;

/*
 * Cas de base C.attributStatique
 * Ou c.x
 */

selection : IDCLASS'.'ID  %prec '.'   
      {$$=makeTree(SELECTION, 2, makeLeafStr(IDENTIFICATEURCLASS,$1),makeLeafStr(IDENTIFICATEUR,$3));}
          | envoiMessage'.'ID %prec '.' {$$=makeTree(SELECTION, 2, $1,makeLeafStr(IDENTIFICATEUR,$3));}
          | OuRien '.' ID %prec '.' {$$=makeTree(SELECTION, 2, $1,makeLeafStr(IDENTIFICATEUR,$3));}
         ;

/*
 * regle qui defini soit une constante entiere, soit une constante d type string
 */
constante : CSTS  { $$ = makeLeafStr(CSTSTRING,$1); }
    | CSTE  { $$ = makeLeafInt(CSTENTIER,$1); }
          ;

instanciation : NEWO IDCLASS '(' ListOptArg ')' { $$=makeTree(INSTANCIATION, 2, makeLeafStr(IDENTIFICATEURCLASS,$2), $4); }
              ;

/*
 * Cas de base C.f(...)
 * Cas de base x.f(...)
 */


envoiMessage : IDCLASS '.' ID '(' ListOptArg ')' %prec '.'    
        { $$=makeTree(ENVOIMESSAGE, 3, makeLeafStr(IDENTIFICATEURCLASS,$1),makeLeafStr(IDENTIFICATEUR,$3),$5); }
              | envoiMessage '.' ID'('ListOptArg ')' %prec '.'    
        { $$=makeTree(ENVOIMESSAGE, 3,$1,makeLeafStr(IDENTIFICATEUR,$3),$5); }
              | OuRien '.' ID '(' ListOptArg ')'  %prec '.'   
        { $$=makeTree(ENVOIMESSAGE, 3,$1,makeLeafStr(IDENTIFICATEUR,$3),$5); }
             ;

/* les appels ci-dessous creent un arbre de syntaxe abstraite pour l'expression
 * arithmetique. On rappelle que la methode est ascendante, donc les arbres
 * des operandes sont deja construits au moment de rajouter le noeud courant.
 * Dans la premiere regle, par exemple, $2, $4 et $6 representent donc
 * les arbres qui sont les composants d'un if-then-else.
 * la fonction makeTree est definie dans tp.c et prend un nombre variables
 * d'arguments (au moins 2). Le premier est l'etiquette du noeud a construire,
 * le second est le nombre de fils.
 */

/*
 * TODO : On n'a pas envie d'écrire au niveau des expression <= > < >= etc ..
 * du coup RELOP est une notion qui nous permet de les généraliser dans une seul entité
 * Ici même on récuper grace à lex l'opérateur mis en jeu ! => go en bas du tp.l
 */
