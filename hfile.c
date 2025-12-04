#include <stdio.h>
#include <unistd.h>

void create_include_guard(char *out, size_t size, char *path) {
   /* extract only the filename, rather than the whole path */
   char *name = path;
   while(*path != '\0') {
      if(*path == '/') name = &path[1];
      path++;
   }

   int i;
   for(i=0; i<size-1; i++) {
      if(name[i] == '\0') break;
      else if('a' <= name[i] && name[i] <= 'z') out[i] = name[i] - 'a' + 'A';
      else if(name[i] == '.') out[i] = '_';
      else out[i] = name[i];
   }
   name[++i] = '\0';
}

int main(int argc, char **argv) {
   if(argc < 2) {
      printf("usage: hfile [path]\n");
      return 1;
   }

   char *source_path = argv[1];
   char target_path[2048];
   char include_guard[2048];

   int i;
   for(i=0; i<sizeof(target_path); i++) {
      target_path[i] = source_path[i];
      if(source_path[i] == '.') break;
      if(source_path[i] == '\0') {
         target_path[++i] = '.';
         break;
      }
   }
   target_path[++i] = 'h'; /* the .h suffix */
   create_include_guard(include_guard, sizeof(include_guard), target_path);

   if(access(target_path, F_OK) == 0) {
      char response[2];
      printf("file %s exists. Are you sure you want to replace it? (y/n)\n", target_path);
      fgets(response, 2, stdin);
      if(response[0] != 'y' && response[0] != 'Y') return 0;
   }

   FILE *in = fopen(source_path, "r");
   if(!in) {
      printf("file %s could not be opened\n", source_path);
      return 1;
   }
   FILE *out = fopen(target_path, "w");
   if(!out) {
      printf("file %s could not be created\n", target_path);
      fclose(in);
      return 1;
   }

   fprintf(out, 
           "#ifndef %s\n"
           "#define %s\n",
           include_guard, include_guard);
   int brace_depth = 0;
   char c;
   bool space_storage = false;
   while((c = fgetc(in)) != EOF) {
      if(brace_depth > 0) {
         if(c == '{') brace_depth++;
         if(c == '}') brace_depth--;
         continue;
      }
      if(c == '}') {
         printf("unbalanced brackets detected.. aborting..");
         fclose(in);
         fclose(out);
         remove(target_path);
         return 0;
      }
      if(c == ' ') {
         space_storage = true;
      } else if(c == '{') {
         brace_depth++;
         space_storage = false;
         fputc(';', out);
      } else {
         if(space_storage) {
            fputc(' ', out);
            space_storage = false;
         }
         fputc(c, out);
      }
   }
   fprintf(out,
           "#endif /* %s */",
           include_guard);

   fclose(in);
   fclose(out);
   return 0;
}
