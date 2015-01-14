/** ANALYSE SYNTAXIQUE + GRAMMAIRE **/

/* les tokens ici sont ceux supposes etre renvoyes par l'analyseur lexical
 * A adapter par chacun en fonction de ce qu'il a ecrit dans tp.l
 * Bison ecrase le contenu de tp_y.h a partir de la description de la ligne
 * suivante. C'est donc cette ligne qu'il faut adapter si besoin, pas tp_y.h !
 */
%token CLASS VAR EXTENDS IS STATIC DEF OVERRIDE RETURNS RETURN YIELD IF THEN ELSE NEW PLUS MINUS RELOP AFFECT MUL DIV CST IDCLASS STRING DEF
%token <S> ID	CSTS/* voir %type ci-dessous pour le sens de <S> et Cie */
%token <I> CSTE


/* indications de precedence d'associativite. Les operateurs sur une meme
 * ligne (separes par un espace) ont la meme priorite. Les ligns sont donnees
 * par precedence croissante d'operateurs.
 */

%left PLUS MINUS
%left MUL DIV 
%left unaire

/* %empty epsilon
*/
/* voir la definition de YYSTYPE dans main.h 
 * Les indications ci-dessous servent a indiquer a Bison que les "valeurs" $i
 * ou $$ associees a ces non-terminaux doivent utiliser la variante indiquee
 * de l'union YYSTYPE (par exemple la variante D ou S, etc.)
 * La "valeur" associee a un terminal utilise toujours la meme variante
 */
%type <C> REL
//%type <T> 

%{
#include "tp.h"     /* les definition des types et les etiquettes des noeuds */

extern int yylex();	/* fournie par Flex */
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

<<<<<<< HEAD
/*
 * Axiome : Liste de classe optionnel suivi d'un bloc obligatoire
 */ 
=======

>>>>>>> 807b491a9a7667c9316df96e62c47e509e3b669f
Programme : LClassOpt Bloc
          ;

/*
 * Liste de classes optionnelle : Vide ou composee d'au moins une declaration de classe
 */
LClassOpt : DeclClass LClassOpt
            | /* epsilon */
            ;

/*
 * Un bloc est defini par une entite avec 2 accolades entourant un contenu
 */
Bloc : '{' ContenuBloc '}'
      ;

/*
 * Le contenu d'un bloc : 
 */
ContenuBloc : LInstructionOpt YieldOpt
      | ListDeclVar IS LInstruction YieldOpt
      ;

YieldOpt : YIELD expr;
        | /* epsilon */
        ;

LInstructionOpt : Instruction LInstructionOpt
                | /* epsilon */
                ;

LInstruction : Instruction LInstructionOpt
              ;
/*
 * Une instruction c'est : 
    *  expression;
    * bloc-procedural
    * cible:= expression;
    * if expression then instruction else instruction
 */
Instruction : expr ';'
            | Bloc
            | Cible AFFECT expr ';' 
            | IF expr THEN Instruction ELSE Instruction
            | RETURN ';'
            ;
Cible : ID 
      | IDCLASS
      ;

BlocOpt : Bloc
        | /* epsilon */ 
        ;

/*
 * class nom(param, ...) [extends nom(arg, ...)] [bloc] is {decl, ...}
 */
DeclClass : CLASS IDCLASS'('ListParamOpt')' ListExtendsOpt BlocOpt IS '{'ContenuClassOpt'}'
            ;

ContenuClassOpt : LDeclChampsOpt LDeclMethodeOpt;

/* 
 * var [static] nom : type [:= expression]; 
 */
LDeclChampsOpt : VAR StaticOpt ID ':' IDCLASS AffectExprOpt ';' LDeclChampsOpt
              |
              ;

StaticOpt : STATIC
          | 
          ;

AffectExprOpt : AFFECT expr;
              |
              ;
/*
 * def [override | static] nom (param, ...) returns Classe bloc
 * def [override | static] nom (param, ...) returns Classe := expression
 */
LDeclMethodeOpt : DEF OverrideOuStaticOpt ID '(' ListParamOpt ')' RETURNS IDCLASS BlocOuExpr LDeclMethodeOpt
              |
              ;

OverrideOuStaticOpt : OVERRIDE
                      | STATIC
                      | /* expression */
                      ;

BlocOuExpr : AffectExprOpt
            | Bloc
            ;

ListParamOpt : LParam
              | /* epsilon */
              ;

LParam : Param
        | LParam','Param
        ;

Param : ID':' IDCLASS
          ;

ListExtendsOpt : EXTENDS IDCLASS'('ListOptArg')'
              | /* epsilon */
              ;
ListOptArg :  | LArg;
LArg : expr | LArg','expr;

/*
 * RAJOUTER Au meme niveau ADD etc ... tout en haut
 * pour envoiMessage : fonction() + 5; ==> fonction prioritaire par rapport a 5

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
expr : ID
       | PLUS expr %prec unaire
       | MINUS expr %prec unaire
       | expr PLUS expr /* $$ = make_tree(etiquette,nbfils,...liste_filse..) */
       | expr MINUS expr /* $$ = make_tree('-',nbfils,...liste_filse..) */
       | expr DIV expr
       | expr MUL expr
       | expr REL expr
       | selection
       | constante /* $$ = make_feuille(...) */
       | '('expr')'
       | instanciation
       | envoiMessage
       | RETURN expr ';'
       ;

selection : IDCLASS'.'ID
          | ID'.'ID
          | envoiMessage'.'ID
          | selection'.'ID
          | '('instanciation')' '.' ID
         ;

constante : CSTS | CSTE
          ;

instanciation : NEW IDCLASS'('ListOptArg')'
              ;

envoiMessage : IDCLASS'.'ID'('ListOptArg')'
              | ID'.'ID'('ListOptArg')'
              | envoiMessage'.'ID'('ListOptArg')'
              | selection'.'ID'('ListOptArg')'
              | '('instanciation')' '.' ID'('ListOptArg')'
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
REL : RELOP { $$ = yylval.C; }
;
