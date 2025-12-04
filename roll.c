#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FLAG_SORT 0x01
#define FLAG_TOTAL 0x02

char help_message[] = "usage: roll [die]\ndie is either a [n], d[n], or [m]d[n]\nmultiple rolls can be done at the same time\n-s to sort the rolls\n";

int comp_int(const void *a, const void *b) {
   return *(int*)a - *(int*)b;
}

enum ROLL_ERROR {
   SUCCESS = 0,
   ZERO_DICE,
   ZERO_FACES,
   MALFORMED_PROMPT
};
enum ROLL_ERROR roll(char *prompt, int flags) {
   int die_count = 1;
   int face_count = 0;
   if(*prompt == '0') {
      while(*++prompt == '0');
      if(*prompt < '1' || '9' < *prompt) return ZERO_DICE;
   }

   while(*prompt != '\0') {
      if('0' <= *prompt && *prompt <= '9') {
         face_count *= 10;
         face_count += *prompt - '0';
      } else if(*prompt == 'd') {
         die_count = face_count;
         if(die_count == 0) die_count = 1;
         face_count = 0;
      } else {
         return MALFORMED_PROMPT;
      }
      prompt++;
   }
   if(face_count == 0) return ZERO_FACES;
   
   int out_len = 0;
   for(int rem = face_count; rem > 0; rem /= 10) out_len++;

   int result[die_count];
   for(int i=0; i<die_count; i++) {
      result[i] = 1 + rand() % face_count;
   } 
   if(flags & FLAG_SORT) {
      qsort(result, die_count, sizeof(*result), comp_int);
   }

   printf("%dd%d:\t", die_count, face_count);
   if(flags & FLAG_TOTAL) {
      int total = 0;
      for(int i=0; i<die_count; i++) total += result[i];
      printf("%d\t(", total);
   }
   printf("%*d", out_len, result[0]);
   for(int i=1; i<die_count; i++) {
      printf(" %*d", out_len, result[i]);
   }
   if(flags & FLAG_TOTAL) printf(")");
   printf("\n");
   return SUCCESS;
}

int main(int argc, char **argv) {
   srand(time(NULL));

   if(argc < 2) {
      printf(help_message);
      return 1;
   }

   int flags = 0;
   for(int i=1; i<argc; i++) {
      if(strcmp(argv[i], "-s") == 0) flags |= FLAG_SORT;
      if(strcmp(argv[i], "-t") == 0) flags |= FLAG_TOTAL;
   }

   enum ROLL_ERROR error;
   for(int i=1; i<argc; i++) {
      if(argv[i][0] == '-') continue; /* ignore flags */
      error = roll(argv[i], flags);
      if(error) {
         if(error == ZERO_DICE) printf("why are you trying to roll zero dice: %s\n", argv[i]);
         if(error == ZERO_FACES) printf("die can't have zero faces: %s\n", argv[i]);
         if(error == MALFORMED_PROMPT) printf("malformed die: %s\n", argv[i]);
      }
   }


   return 0;
}
