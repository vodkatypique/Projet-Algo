/*! \file compileBST.c
 *  \brief	   This implements the applyPatch program.
 *  \author    Lucie Pansart
 *  \author    Jean-Louis Roch
 *  \version   1.0
 *  \date      30/9/2016
 *  \warning   Usage: compileBST n originalFile 
 *  \copyright GNU Public License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include <stdio.h>
#include<stdlib.h>
#include <string.h>

#define LONGUEURBRACKET 50


/**
 * \struct OBST
 * \brief Objet Arbre Binaire Optimal
 *
 * OBST est un objet qui reprensente un arbre binaire optimal, 
 * ou bien un sous arbre binaire (qui est lui aussi optimal par construction)
 */
typedef struct OBST {
  int KEY;  /*! valeur portée par la feuille racine de l'arbre binaire optimal */
  struct OBST * left, * right;  /*! sous arbres gauche et droit */
}
OBST;

float **C; ///cost matrix
float *W; ///weight matrix
int **R; ///root matrix
float *p; ///frequencies
long NUMBER_OF_KEYS; ///number of keys in the tree
int *KEYS;
OBST * ROOT;

/**
 * \fn void calculerMatrices (void)
 * \brief Fonction de calcule des matrices utiles : 
 * cout, racine et poids
 *
 * \return void
 */
void calculerMatrices() {
  float x, min;
  int i, j, k, h, m;
  ///Construct weight matrix W
    W[0] = 0;
    for (i = 1; i <= NUMBER_OF_KEYS; i++)
      W[i] = W[i - 1] + p[i];
  ///Construct cost matrix C and root matrix R
  for (i = 0; i <= NUMBER_OF_KEYS; i++)
    C[i][i] = 0;
  for (i = 0; i <= NUMBER_OF_KEYS - 1; i++) {
    j = i + 1;
    C[i][j] = W[j]-W[i];
    R[i][j] = j;
  }
  for (h = 2; h <= NUMBER_OF_KEYS; h++)
    for (i = 0; i <= NUMBER_OF_KEYS - h; i++) {
      j = i + h;
      m = R[i][j - 1];
      min = C[i][m - 1] + C[m][j];
      for (k = m + 1; k <= R[i + 1][j]; k++) {
        x = C[i][k - 1] + C[k][j];
        if (x < min) {
          m = k;
          min = x;
        }
      }
      C[i][j] = W[j]-W[i] + min;
      R[i][j] = m;
    }
}

/**
 * \fn OBST* construireArbre (int i, int j)
 * \brief Fonction de construction de l'arbre contenant 
 * les feuilles d'indices i a j
 * \param i borne inferieure
 * \param j borne superieure
 * \return Racine de l'arbre binaire optimal contenant les feuilles d'indice i a j
 */
OBST * construireArbre(int i, int j) {
  OBST * p;
  if (i == j) {
    p = NULL;
  } else {
    p = malloc(sizeof(OBST));
    
    p -> KEY = KEYS[R[i][j]];
    p -> left = construireArbre(i, R[i][j] - 1); //sous arbre gauche
    p -> right = construireArbre(R[i][j], j); //sous arbre droit
  }
  return p;
}

/**
 * \fn void remplirSortie (OBST * ROOT, int nivel, int elt, char ** sortie)
 * \brief Fonction de qui rempli un tampon utiliser pour stocker la futur sortie ecran
 * \param ROOT racine de l'arbre binaire optimal traité
 * \param nivel niveau ("etage") actuel dans le traitement de l'arbre binaire 
 * \param elt nombre d'element dans l'arbre complet
 * \param sortie tampon de sortie
 * \return void
 */
void remplirSortie(OBST * ROOT, int nivel, int elt, char ** sortie) {  
  if (nivel == 0) {
    //fp = fopen("test.txt", "w+");
    fprintf(stdout, "static int BSTroot = %d;\nstatic int BSTtree[%d][2] = {\n", ROOT -> KEY, elt);
    //fclose(fp);
  }
  
  if (ROOT != 0) {
    char * valeur = (char * ) malloc(LONGUEURBRACKET * sizeof(char));
    //valeur[0] = '\0';
    
    if (ROOT -> left != NULL) {
      sprintf(valeur + strlen(valeur), "{%d, ", ROOT -> left -> KEY);
    } else {
      sprintf(valeur + strlen(valeur), "{-1, ");
    }
    if (ROOT -> right != NULL) {
      sprintf(valeur + strlen(valeur), "%d}", ROOT -> right -> KEY);
    } else {
      sprintf(valeur + strlen(valeur), "-1}");
    }
    strcpy(sortie[ROOT -> KEY], valeur);
    remplirSortie(ROOT -> right, nivel + 1, elt, sortie);
    remplirSortie(ROOT -> left, nivel + 1, elt, sortie);

  }
}

/**
 * \fn void afficherSortie (char** sortie)
 * \brief Fonction de qui affiche le tampon a l'ecran
 * \param sortie tampon contenant les sortie dans le bon ordre d'affichage
 * \return void
 */
void afficherSortie(char ** sortie) {
  for (int i = 0; i < NUMBER_OF_KEYS - 1; i++) {
    fprintf(stdout, "%s,\n", sortie[i]);
  }
  fprintf(stdout, "%s };\n", sortie[NUMBER_OF_KEYS - 1]);

}

/**
 * \fn void calculEtConstructionArbre (void)
 * \brief Fonction de qui calcul les matrices puis constructi l'arbre binaire optimal contenant tout les elements
 * \return void
 */
void calculEtConstructionArbre() {
  calculerMatrices();
  ROOT = construireArbre(0, NUMBER_OF_KEYS);
}

/**
 * Main function
 * \brief Main function
 * \param argc  A count of the number of command-line arguments
 * \param argv  An argument vector of the command-line arguments.
 * \warning Must be called with a positive long integer, n,  and a filename, freqFile, as commandline parameters and in the given order.
 * \returns { 0 if succeeds and prints C code implementing an optimal ABR on stdout; exit code otherwise}
 */
int main (int argc, char *argv[]) {
  long n = 0 ; // Number of elements in the dictionary
  FILE *fs = NULL ; // File that contains n positive integers defining the relative frequence of dictinary elements 
  
  int i;


  if(argc != 3){
    fprintf(stderr, "!!!!! Usage: ./compileBST n  originalFile !!!!!\n");
      exit(EXIT_FAILURE); /* indicate failure.*/
  }

  { // Conversion of parameter n in a long 
    int codeRetour = EXIT_SUCCESS;
    char *posError;
    long resuLong;
    n = atol(argv[1] ) ;
   
    assert(argc >= 2);
    // Conversion of argv[1] en long
    resuLong = strtol(argv[1], &posError, 10);
    // Traitement des erreurs
    switch (errno)
    {
      case EXIT_SUCCESS :
         // Conversion du long en int
         if (resuLong > 0)
         {
            n = (long)resuLong;
            fprintf(stderr, "Number of elements in the dictionary: %ld\n", n);         
         }
         else
         {
            (void)fprintf(stderr, "%s cannot be converted into a positive integer matching the number of elements in the dicionary.\n", argv[1]) ; 
            codeRetour = EXIT_FAILURE;
         }
      break;
      
      case EINVAL :
         perror(__func__);
         (void)fprintf(stderr, "%s does not match a long integer value. \n", argv[1]);
         codeRetour = EXIT_FAILURE;
      break;
      
      case ERANGE :
         perror(__func__);
         (void)fprintf(stderr, "%s does not fit in a long int :\n" "out of bound [%ld;%ld]\n",
                       argv[1], LONG_MIN, LONG_MAX);
         codeRetour = EXIT_FAILURE;
      break;
      default :
         perror(__func__);
         codeRetour = EXIT_FAILURE;
    } // switch (errno)
    if  (codeRetour != EXIT_SUCCESS) return codeRetour ;
  }

  fs = fopen(argv[2] , "r" );
  if (fs==NULL) {fprintf (stderr, "!!!!! Error opening originalFile !!!!!\n"); exit(EXIT_FAILURE);}

  // TO BE COMPLETED 
   NUMBER_OF_KEYS = n;

   C = (float**)malloc((NUMBER_OF_KEYS+1)* sizeof(float*));
   for (int i = 0; i < (NUMBER_OF_KEYS+1); i++)
   {
     C[i] = (float*)malloc((NUMBER_OF_KEYS+1)* sizeof(float));
   }
   
  W = (float*)malloc((NUMBER_OF_KEYS+1)* sizeof(float));


  R = (int**)malloc((NUMBER_OF_KEYS+1)* sizeof(int*));
   for (int i = 0; i < (NUMBER_OF_KEYS+1); i++)
   {
     R[i] = (int*)malloc((NUMBER_OF_KEYS+1)* sizeof(int));
   }

  p = (float*)malloc((NUMBER_OF_KEYS+1)* sizeof(float));

  KEYS = (int*)malloc((NUMBER_OF_KEYS+1)* sizeof(int));

  int espace = 1;
  char ch, buffer[32];
  int x = 0, y = 1;


 
  while (1) {
    ch = fgetc(fs);


    if (ch == EOF) {
      KEYS[y] = atoi(buffer);
      break;
    }

    else if (ch == ' ') {

      if (espace == 0) {
        espace = 1; ///empecher les espaces multiples cause de bug

        KEYS[y] = atoi(buffer);
        y++;
        memset(buffer, 0, 32);
        x = 0;
        continue;
      }

    } else {
      espace = 0;
      buffer[x] = ch;
      x++;
    }
  }

  char ** sortie = malloc(NUMBER_OF_KEYS * sizeof(char * ));
  for (int i = 0; i < NUMBER_OF_KEYS; i++) {
    sortie[i] = malloc(LONGUEURBRACKET * sizeof(char));
    }

  int somme = 0;
  for (int i = 1; i <= NUMBER_OF_KEYS; i++) {
    somme += KEYS[i];
  }

  for(i = 1; i <= NUMBER_OF_KEYS; i++)
   {
    p[i] = (float)KEYS[i]/somme;
    KEYS[i] = i-1;
   }


  calculEtConstructionArbre();
  remplirSortie(ROOT, 0, NUMBER_OF_KEYS, sortie);
  afficherSortie(sortie);


  fclose(fs);


  return 0;
}
