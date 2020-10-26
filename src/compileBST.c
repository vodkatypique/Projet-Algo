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

#define NMAX 6000
#define LONGUEURBRACKET 50

typedef struct OBST {
  int KEY;
  struct OBST * left, * right;
}
OBST;
float **C; //cost matrix   //faire malloc
float *W; //weight matrix
int **R; //root matrix
float *p; //frequencies
long NUMBER_OF_KEYS; //number of keys in the tree
int *KEYS;
OBST * ROOT;

void calculerMatrices() {
  float x, min;
  int i, j, k, h, m;
  //Construct weight matrix W
    W[0] = 0;
    for (i = 1; i <= NUMBER_OF_KEYS; i++)
      W[i] = W[i - 1] + p[i];
  //Construct cost matrix C and root matrix R
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

OBST * construireArbre(int i, int j) {
  //printf("tota");
  OBST * p;
  if (i == j) {
    p = NULL;
  } else {
    p = malloc(sizeof(OBST));
    
    //printf("toto");
    p -> KEY = KEYS[R[i][j]];
    //printf("totI");
    p -> left = construireArbre(i, R[i][j] - 1); //sous arbre gauche
    //printf("totW");
    p -> right = construireArbre(R[i][j], j); //sous arbre droit
    //printf("%d", p->KEY);
  }
  return p;
}

void remplirSortie(OBST * ROOT, int nivel, int elt, char ** sortie) {
  //int i;
  //FILE * fp;
  //printf("toto");
  
  if (nivel == 0) {
    //fp = fopen("test.txt", "w+");
    fprintf(stdout, "static int BSTroot = %d;\nstatic int BSTtree[%d][2] = {\n", ROOT -> KEY, elt);
    //fclose(fp);
  }
  
  //printf("titi");
  if (ROOT != 0) {
    char * valeur = (char * ) malloc(LONGUEURBRACKET * sizeof(char));
    //valeur[0] = '\0';
    
    if (ROOT -> left != NULL) {
      //fprintf(fp, "{%d, ", ROOT->left->KEY);
      sprintf(valeur + strlen(valeur), "{%d, ", ROOT -> left -> KEY);
    } else {
      //fprintf(fp, "{-1, ");
      sprintf(valeur + strlen(valeur), "{-1, ");
    }
    if (ROOT -> right != NULL) {
      //fprintf(fp, "%d}\n", ROOT->right->KEY);
      sprintf(valeur + strlen(valeur), "%d}", ROOT -> right -> KEY);
    } else {
      //fprintf(fp, "-1}\n");
      sprintf(valeur + strlen(valeur), "-1}");
    }
    strcpy(sortie[ROOT -> KEY], valeur);
    //fclose(fp);
    //printf("tata");  
    remplirSortie(ROOT -> right, nivel + 1, elt, sortie);
    //printf("%d\n", ROOT->KEY);
    remplirSortie(ROOT -> left, nivel + 1, elt, sortie);

  }
}

void afficherSortie(char ** sortie) {
  //FILE * fp;
  for (int i = 0; i < NUMBER_OF_KEYS - 1; i++) {
    //fp = fopen("test.txt", "a");
    fprintf(stdout, "%s,\n", sortie[i]);
    //fclose(fp);
  }
  //p = fopen("test.txt", "a");
  fprintf(stdout, "%s };\n", sortie[NUMBER_OF_KEYS - 1]);
  //fclose(fp);

}
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
  
  int i;//, k;


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

   C = (float**)malloc((NUMBER_OF_KEYS+1)* sizeof(float*)); //cost matrix   //faire malloc
   for (int i = 0; i < (NUMBER_OF_KEYS+1); i++)
   {
     C[i] = (float*)malloc((NUMBER_OF_KEYS+1)* sizeof(float));
   }
   
  W = (float*)malloc((NUMBER_OF_KEYS+1)* sizeof(float)); //weight matrix


  R = (int**)malloc((NUMBER_OF_KEYS+1)* sizeof(int*)); //root matrix
   for (int i = 0; i < (NUMBER_OF_KEYS+1); i++)
   {
     R[i] = (int*)malloc((NUMBER_OF_KEYS+1)* sizeof(int));
   }

  p = (float*)malloc((NUMBER_OF_KEYS+1)* sizeof(float)); //frequencies

  KEYS = (int*)malloc((NUMBER_OF_KEYS+1)* sizeof(int));

  int espace = 1;
  char ch, buffer[32];
  int x = 0, y = 1;


 
  while (1) {
    ch = fgetc(fs);
    //printf("titi");


    if (ch == EOF) {
      //printf("totO");
      KEYS[y] = atoi(buffer);
      //printf("%d", KEYS[y]);
      break;
    }

    else if (ch == ' ') {
      //printf("tATA");

      if (espace == 0) {
        espace = 1; //empecher les espaces multiples cause de bug

        KEYS[y] = atoi(buffer);
        //printf("%d", KEYS[y]);
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
