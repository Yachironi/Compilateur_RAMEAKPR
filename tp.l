/** ANALYSE LEXICALE **/

/** Expressions régulières **/

Digit [0-9]
Integer {Digit}+
Letter  [a-zA-Z]
LD  ({Letter}|{Digit})
Guill "\""
NotGuillAntislash [^"\\\n]
NotAntislash [^\\]
AntislashAll \\.
String  ({Guill}(({AntislashAll})|{NotGuillAntislash})*{Guill})
Star \*
NotStar [^*]
NotStarSlash [^*/]
Delim [ \n\t\r]
Symbol [;,(){}:.]
All [.]

Antislash [\\]
Commentaire ("/"{Star}({NotStar}|({Star}+{NotStarSlash}))*{Star}+"/")

/*
 * ("/"{Star}({NotStar}|({Star}+{NotStarSlash}))*{Star}+"/")
 * expr : ("/"{Star}{{NotStar}|({Star}+{NotStarSlash})}*{Star}+"/")
 *
*/

%{
#include "tp.h"
#include "tp_y.h"
#include <string.h> /* pour strdup */

extern void setError(int code);

extern YYSTYPE yylval;
extern int errorCode;


/* Il est plus economique de ne pas creer une expression reguliere pour chaque
 * mot-clef mais plutot de les reconnaitre dans un premier temps comme des
 * identificateurs et de faire la distinction par un bout de code
 */
int keywordOrId() {
  /* si ce n'est pas un mot-clef, sauvegarder le texte de l'identificateur
   * et renvoyer le token associe aux identificateurs
   */
  yylval.S = strdup(yytext);
  if(strcmp(yylval.S,"if")==0)
    return IF;
  if(strcmp(yylval.S,"else")==0)
    return ELSE;
  if(strcmp(yylval.S,"then")==0)
    return THEN;
  if(strcmp(yylval.S,"class")==0)
    return CLASS;
  if(strcmp(yylval.S,"var")==0)
    return VAR;
  if(strcmp(yylval.S,"extends")==0)
    return EXTENDS;
  if(strcmp(yylval.S,"is")==0)
    return IS;
  if(strcmp(yylval.S,"static")==0)
    return STATIC;
  if(strcmp(yylval.S,"override")==0)
    return OVERRIDE;
  if(strcmp(yylval.S,"returns")==0)
    return RETURNS;
  if(strcmp(yylval.S,"return")==0)
    return RETURN;
  if(strcmp(yylval.S,"yield")==0)
    return YIELD;
  if(strcmp(yylval.S,"new")==0)
    return NEWO;
  if(strcmp(yylval.S,"def")==0)
    return DEF  ;
  if(yylval.S[0]>='A' && yylval.S[0]<='Z')
    return IDCLASS;

  return(ID);
}


void lexError(char c) {
  fprintf(stderr, "Error: Skipping invalid character '%c' on line %d\n",
          c, yylineno);
  setError(LEXICAL_ERROR);
}


/* les codes symboliques pour les unites lexicales (CST, IF, PLUS, ...) sont
 * definis dans tp_y.h et serviront aussi d'etiquettes pour les sommets de
 * l'arbre de syntaxe abstraite qui sera construit par la suite du processus
 * de compilation.
 *
 * Les "return" qui apparaissent dans les actions associees aux expressions
 * reguleres peuvent renvoyer:
 * - soit le code symbolique d'une U.L.
 * - soit un simple caractere
 * L'eventuelle partie "valeur" de l'U.L. doit etre stockee dans une des
 * variantes de la variable yylval (definie comme etant de type YYSTYPE)
 * afin que les outils compagnons de l'analyseur lexical puisse l'y recuperer.
 * Attention: le contenu de yylval va etre ecrase a chaque appel a l'analyseur
 * lexical et doit donc etre sauvegarde si besoin.
 */

%}

%%
{Integer}       { yylval.I = strtol(yytext, NULL, 0); return(CSTE); }
{Letter}{LD}*   { return keywordOrId(); }
{String}	{ yylval.S = strdup(yytext); return(CSTS);}
"+"             { return(PLUS); }
"-"             { return(MINUS); }
"*"             { return(MUL); }
"/"             { return(DIV); }
"="             { yylval.I = EQ; return(RELOP); } /*ici on stock l'opérateur VU et on le donne à Bison*/
"<>"            { yylval.I = NE; return(RELOP); }
"<"             { yylval.I = LT; return(RELOP); }
"<="            { yylval.I = LE; return(RELOP); }
">"             { yylval.I = GT; return(RELOP); }
">="            { yylval.I = GE; return(RELOP); }
{Symbol}        { yylval.C = yytext[0]; return(yytext[0]); }
":="            { return AFFECT; }
"&"		{ return(CONCAT);}
{Delim}         { ; }
{Commentaire}	{ ; }
.      		{ printf("Erreur lexical sur : %s\n",yytext);}
%%
