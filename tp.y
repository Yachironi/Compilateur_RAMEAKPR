/** ANALYSE SYNTAXIQUE + GRAMMAIRE **/

/* les tokens ici sont ceux supposes etre renvoyes par l'analyseur lexical
 * A adapter par chacun en fonction de ce qu'il a ecrit dans tp.l
 * Bison ecrase le contenu de tp_y.h a partir de la description de la ligne
 * suivante. C'est donc cette ligne qu'il faut adapter si besoin, pas tp_y.h !
 */
%token CLASS VAR EXTENDS IS STATIC DEF OVERRIDE RETURNS RETURN YIELD IF THEN ELSE NEW PLUS MINUS RELOP AFFECT MUL DIV CST IdClass
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

Programme : LClassOpt Bloc

LClassOpt : DeclClass LClassOpt
            | /* epsilon */

/* TODO
 * 
 * ListBlock + ListDecl
 * 
*/

DeclClass : CLASS IdClass'('ListIdentOpt')' ListExtendsOpt ListBloc IS {ListDecl}
            ;

ListIdentOpt : LI
              | /* epsilon */
              ;

LI : ID':' IdClass
    | ID':' IdClass','LI
;
ListExtendsOpt : EXTENDS IdClass'('ListOpt')'
              | /* epsilon */
              ;
ListOpt :  | LArg;
LArg : expr | LArg','expr;

/*
 * RAJOUTER Au meme niveau ADD etc ... tout en haut
 * pour envoiMessage : fonction() + 5; ==> fonction prioritaire par rapport a 5

  E : E+E
    | Fonc%prec
 Pareil pour + unaire - unaire
 */
expr : ID
       | PLUS expr %prec unaire
       | expr PLUS expr /* $$ = make_tree(etiquette,nbfils,...liste_filse..) */
       | expr MINUS expr /* $$ = make_tree('-',nbfils,...liste_filse..) */
       | expr DIV expr
       | expr MUL expr
       | expr RELOP expr
       | selection
       | constante /* $$ = make_feuille(...) */
       | '('expr')'
       | instanciation
       | envoiMessage
       ;

       /* On peut faire : (new c()).kkchose ou new c().kkchose*/

selection : IdClass'.'ID
          | ID'.'ID
          | envoiMessage'.'ID
          | selection'.'ID
         ;

constante : CSTS | CSTE
          ;

instanciation : NEW IdClass'('ListOpt')'
              ;

/**
 * Verfier derniere liste envoi Message car on ne l'avait pas avant je l'ai rajoute
 */
envoiMessage : IdClass'.'ID'('ListOpt')'
              | ID'.'ID'('ListOpt')'
              | envoiMessage'.'ID'('ListOpt')'
              | selection'.'ID'('ListOpt')'
             ;

/* "programme" est l'axiome de la grammaire */
/*programme : declL BEG expr END  FAUX : il n'y a pas de BEGIN & END
;*/

/* Une liste eventuellement vide de declarations de variables */
/*declL : | 
        | decl declL;*/


/* une declaration de variable ou de fonction, terminee par un ';'. 
 * AFFECT = ":="
*/
/*decl :
ID AFFECT expr ';'
;$/

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
  * FAUX A CORRIGER : à compléter
  * /!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\
  */

/*expr :
  IF bexpr THEN expr ELSE expr
    { $$ = makeTree(IF, 3, $2, $4, $6); }
| expr PLUS expr
    { $$ = makeTree(PLUS, 2, $1, $3); }
| expr MINUS expr
    { $$ = makeTree(MINUS, 2, $1, $3); }
| expr MUL expr
    { $$ = makeTree(MUL, 2, $1, $3); }
| expr DIV expr
    { $$ = makeTree(DIV, 2, $1, $3); }
| CST
    { $$ = makeLeafInt(CST, $1); }
| ID
    { $$ = makeLeafStr(ID, $1); }
| '(' expr ')'      //meme traitement que pour le + unaire.
    { $$ = $2; }
;

bexpr : expr REL expr 
    { $$ = makeTree($2, 2, $1, $3); }
| '(' bexpr ')'
    { $$ = $2; }
;*/

/*
 * TODO : On n'a pas envie d'écrire au niveau des expression <= > < >= etc ..
 * du coup RELOP est une notion qui nous permet de les généraliser dans une seul entité
 * Ici même on récuper grace à lex l'opérateur mis en jeu ! => go en bas du tp.l
 */
REL : RELOP { $$ = yylval.C; }
;
