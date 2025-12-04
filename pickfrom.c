#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

char *help_message = "usage: pickfrom [-p prompt] <options>";

int32_t main(int32_t argc, char **argv) {
   bool has_prompt = false;

   if(argc < 2) {
      printf("usage: pickfrom [list of options]\n");
      return 1;
   }

   if(strcmp(argv[1], "-p") == 0) {
      if(argc < 4) {
         puts(help_message);
         return 1;
      }
      has_prompt = true;
   }

   srand(time(NULL));
   if(has_prompt) printf("%s: %s\n", argv[2], argv[rand() % (argc-3) + 3]);
   else printf("option picked: %s\n", argv[rand() % (argc-1) + 1]);
   return 0;
}
   

