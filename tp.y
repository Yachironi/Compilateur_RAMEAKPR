/** ANALYSE SYNTAXIQUE + GRAMMAIRE **/

/* les tokens ici sont ceux supposes etre renvoyes par l'analyseur lexical
 * A adapter par chacun en fonction de ce qu'il a ecrit dans tp.l
 * Bison ecrase le contenu de tp_y.h a partir de la description de la ligne
 * suivante. C'est donc cette ligne qu'il faut adapter si besoin, pas tp_y.h !
 */
%token CLASS VAR EXTENDS IS STATIC DEF OVERRIDE RETURNS RETURN YIELD IF THEN ELSE NEW PLUS MINUS RELOP AFFECT MUL DIV CST STRING CONCAT
%token <S> ID CSTS IDCLASS/* voir %type ci-dessous pour le sens de <S> et Cie */
%token <I> CSTE


/* indications de precedence d'associativite. Les operateurs sur une meme
 * ligne (separes par un espace) ont la meme priorite. Les ligns sont donnees
 * par precedence croissante d'operateurs.
 */

%left PLUS MINUS
%left MUL DIV 
%left unaire
%left '(' ')'
%left '{' '}'




/* %empty epsilon
*/
/* voir la definition de YYSTYPE dans main.h 
 * Les indications ci-dessous servent a indiquer a Bison que les "valeurs" $i
 * ou $$ associees a ces non-terminaux doivent utiliser la variante indiquee
 * de l'union YYSTYPE (par exemple la variante D ou S, etc.)
 * La "valeur" associee a un terminal utilise toujours la meme variante
 */
%type <C> REL
%type <T> expr Programme LClassOpt Bloc DeclClass  selection avant_selection constante

//%type <T> 

%{
#include "tp.h"     /* les definition des types et les etiquettes des noeuds */

PCLASS classActuel;

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

/*
 * Axiome : Liste de classe optionnel suivi d'un bloc obligatoire
 */ 
Programme : LClassOpt Bloc	{ $$ = makeTree(PROGRAM,2,$1,$2); }
          ;

/*
 * Liste de classes optionnelle : Vide ou composee d'au moins une declaration de classe
 */
LClassOpt : DeclClass LClassOpt	{$$=makeTree(LISTCLASS,2,$1,$2);}
            | /* epsilon */ {$$=NIL(Tree);}
            ;

/*
 * Un bloc est defini par une entite avec 2 accolades entourant un contenu
 */
Bloc : '{' ContenuBloc '}'
      ;

/*
 * Le contenu d'un bloc : Une 
 * une liste d'instruction optionnelle suivi d'un Yield option => S'il y a un yield => bloc procedural
 * Ou si on a une List de declaration de valeur => oblige apres le IS d'avoir une Liste d'instruction
 * suivi par un Yield optionnel
 */
ContenuBloc : LInstructionOpt YieldOpt
      | ListDeclVar IS LInstruction YieldOpt
      ;

/*
 * Sert a differencier les deux types de bloc : fonctionnel et procedural
 */
YieldOpt : YIELD expr;
        | /* epsilon */
        ;

ListDeclVar : VAR StaticOpt ID ':' IDCLASS AffectExprOpt ';' LDeclChampsOpt
            ;

/*
 * Liste d'instructions optionnel 
 */
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
/*
 * La cible de l'affectation ne peut etre qu'un identifiant : 
 * un nom d'un objet x := new Point(1,5)
 * un nom de classe var res : Point := new Point(x, y);
 */
Cible : ID 
      | IDCLASS
      | selection
      ;

/*
 * Bloc optionnel
 */
BlocOpt : Bloc
        | /* epsilon */ 
        ;

/*
 * class nom(param, ...) [extends nom(arg, ...)] [bloc] is {decl, ...}
 */
DeclClass : CLASS IDCLASS '('ListParamOpt')' ListExtendsOpt BlocOpt IS '{'ContenuClassOpt'}' {classActuel=$2}
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


Methode: DEF OverrideOuStaticOpt ID '(' ListParamOpt ')' RETURNS IDCLASS BlocOuExpr 
	{$$ = makeMethode($3,$2,$9,$8,$5,classActuel);} //strcmp($2->u.s,"static")?1:0;  strcmp($2->u.s,"override")?1:0;
	;
	//makeMethode(char *nom,int OverrideOuStaticOpt,TreeP corps,PClasse typeRetour,PVAR params,PCLASS home;);

LDeclMethodeOpt : Methode LDeclMethodeOpt 
		 { $$ = makeTree(LISTEMETHODE,$1,$2) }
              |  { $$=NIL(Tree);}
              ;

OverrideOuStaticOpt : OVERRIDE		 { $$=1; }
                      | STATIC		 { $$=2; }
                      | /* expression */ { $$=NIL(Tree);}
                      ;

BlocOuExpr : AffectExprOpt	{ $$=$1;}
           | Bloc		{ $$=$1;}
           ;


ListParamOpt : LParam 		{ $$=$1; }
              | /* epsilon */ 	{ $$=NIL(Tree);}
              ;

LParam : Param			{ $$=$1 ;}
        | LParam','Param	{ $$=makeTree(LISTE, 2,$1,$3);}
        ;

Param : ID':' IDCLASS	{ $$ = makeTree(PARAM, 2, makeLeafStr(IDENTIFICATEURCLASS,$1->S),makeLeafStr(IDENTIFICATEURCLASS,$3->S)); }
          ;

ListExtendsOpt : EXTENDS IDCLASS'('ListOptArg')'	{ $$=makeTree(EXTENTION, 2, makeLeafStr(IDENTIFICATEURCLASS,$1->S),$4);} // A verifier $1->S
              | /* epsilon */				{ $$=NIL(Tree);}
              ;
ListOptArg :		{ $$=NIL(Tree);}
	   | LArg	{ $$=$1	;}
           ;


LArg : expr		{ $$ = $1}
     | LArg','expr      { $$=makeTree(LISTE, 2, $1,$3);}
     ;





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


expr : ID 				{ $$=makeLeafStr(IDENTIFICATEUR, $1->S); } // yylval.S ou $1->S
       | PLUS expr %prec unaire		{ $$=makeTree(PLUSUNAIRE, 1, $2); }
       | MINUS expr %prec unaire	{ $$=makeTree(MINUSUNAIRE, 1, $2); }
       | expr CONCAT expr		{ $$=makeTree(CONCATENATION, 2, $1, $3); }
       | expr PLUS expr 		{ $$=makeTree(PLUSBINAIRE, 2, $1, $3); }
       | expr MINUS expr 		{ $$=makeTree(MINUSBINAIRE, 2, $1, $3); }
       | expr DIV expr			{ $$=makeTree(DIVISION, 2, $1, $3); }
       | expr MUL expr			{ $$=makeTree(MULTIPLICATION, 2, $1, $3); }
       | expr REL expr			{ $$=makeTree(OPCOMPARATEUR, 2, $1, $3); }
       | selection			{ $$=$1; }
       | constante 			{ $$=$1; }
       | '(' expr ')'			{ $$=$2; }//{ $$=makeTree(EXPRESSIONPAREN, 3, '(',$2, ')'); }
       | instanciation			{ $$=$1; }
       | envoiMessage			{ $$=$1; }
       | RETURN expr ';'		{ $$=makeTree(EXPRESSIONRETURN, 1, $2); }
       ;

/*
 * Cas de base C.attributStatique
 * Ou c.x
 */


avant_selection : IDCLASS		{ $$=makeLeafStr(IDENTIFICATEURCLASS, $1->S); }
	| ID				{ $$=makeLeafStr(IDENTIFICATEUR, $1->S); }
	| envoiMessage			{ $$=$1;}
	| selection			{ $$=$1;}
	| '('instanciation')'		{ $$=$2;}
	;


selection : avant_selection '.' ID	{ $$=makeTree(SELECTION, 2, $1, makeLeafStr(IDENTIFICATEUR,$3->S));}
	;

/*
selection : IDCLASS'.'ID
          | ID'.'ID
          | envoiMessage'.'ID
          | selection'.'ID
          | '('instanciation')' '.' ID
         ;
*/


constante : CSTS  { $$ = makeLeafStr(CSTSTRING,yylval.S); } // yylval.S ou $1 
	  | CSTE  { $$ = makeLeafInt(CSTENTIER,yylval.I); }
          ;
/*
 * new C(...)
 * TODO !!!!!! expression fini par un ';' ???
 */


instanciation : NEW IDCLASS '(' ListOptArg ')' { $$=makeTree(INSTANCIATION, 2, makeLeafStr(IDENTIFICATEURCLASS,$2->S), $4); }
              ;

/*
 * Cas de base C.f(...)
 * Cas de base x.f(...)
 */



envoiMessage : IDCLASS '.' ID '(' ListOptArg ')'		{ $$=makeTree(ENVOIMESSAGE, 3, makeLeafStr(IDENTIFICATEURCLASS,$1->S),makeLeafStr(IDENTIFICATEUR,$3->S),$5); }
              | ID '.' ID '(' ListOptArg ')'   			{ $$=makeTree(ENVOIMESSAGE, 3, makeLeafStr(IDENTIFICATEUR,$1),makeLeafStr(IDENTIFICATEUR,$3),$5); }
              | envoiMessage '.' ID'('ListOptArg ')'  	 	{ $$=makeTree(ENVOIMESSAGE, 3,$1,makeLeafStr(IDENTIFICATEUR,$3->S),$5); }
              | selection '.' ID '(' ListOptArg ')' 	 	{ $$=makeTree(ENVOIMESSAGE, 3,$1,makeLeafStr(IDENTIFICATEUR,$3->S),$5); }
              | '('instanciation ')' '.' ID '('ListOptArg ')'   { $$=makeTree(ENVOIMESSAGE, 3,$2,makeLeafStr(IDENTIFICATEUR,$5->S),$7); }
             ;

/** On peut pas faire ça? :

envoiMessage : selection '(' ListOptArg ')
		;
**/

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
