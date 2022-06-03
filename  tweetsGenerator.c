#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wordexp.h>
#include <assert.h>
#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000

typedef struct WordStruct {
    char *word;
    struct WordProbability *prob_list;
    int value;
    int n;

} WordStruct;

typedef struct WordProbability {
    struct WordStruct *word_struct_ptr;
    int P_value;
} WordProbability;

/************ LINKED LIST ************/
typedef struct Node {
    WordStruct *data;
    struct Node *next;
} Node;

typedef struct LinkList {
    Node *first;
    Node *last;
    int size;
} LinkList;

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
 //array to save all the words in the file
WordStruct** temp=NULL;

//add the word_struct in the link list
int add(LinkList *link_list, WordStruct *data)
{
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL)
    {
        return 1;
    }
    *new_node = (Node){data, NULL};

    if (link_list->first == NULL)
    {
        link_list->first = new_node;
        link_list->last = new_node;
    }
    else
    {
        link_list->last->next = new_node;
        link_list->last = new_node;
    }

    link_list->size++;
    return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
 //get rand number to use it in the functions (get first number , get next number ,
int get_random_number(int max_number)
{
    return  rand()%max_number;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' .
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
 //get a first random word from the text py using get random number function
WordStruct *get_first_random_word(LinkList *dictionary)
{
    if(dictionary==NULL){
        exit(EXIT_FAILURE);
    }
    int dic_size=dictionary->size;
    int rand_num= get_random_number(dic_size);
   int temp_rand_num=rand_num;
   int not_point=1;
   Node * tempNode=dictionary->first;
    while (not_point!=0) {
        while (temp_rand_num != 0) {
            tempNode=tempNode->next;
            temp_rand_num--;
        }
        if(tempNode->data->word[strlen(tempNode->data->word)-1]!='.'&&tempNode->data->prob_list!=NULL){
            not_point=0;
        }else{
            tempNode=dictionary->first;
            temp_rand_num= get_random_number(dic_size);
        }
    }
    return tempNode->data;
}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
//get a second random word from the probability list for the first word py using first random word and get random number functions
WordStruct *get_next_random_word(WordStruct *word_struct_ptr){
    int max_num=word_struct_ptr->value;
    int rand_num= get_random_number(max_num);
    int the_value=0;
    for (int i = 0; i < word_struct_ptr->n; ++i) {
        the_value+=word_struct_ptr->prob_list[i].P_value;
        if(rand_num<the_value){

            return word_struct_ptr->prob_list[i].word_struct_ptr;

        }
    }
    return word_struct_ptr->prob_list[word_struct_ptr->n-1].word_struct_ptr;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
 // use the first random word and the second random word to build a random sentence
int generate_sentence(LinkList *dictionary){
    if(dictionary==NULL){
        exit(EXIT_FAILURE);
    }
    WordStruct* first_word=NULL;
    WordStruct * second_word=NULL;
    WordStruct * temp_word=NULL;
    int count=2;
    int sen_length=MAX_SENTENCE_LENGTH;

first_word= get_first_random_word(dictionary);
    printf("%s ",first_word->word);
    sen_length=(sen_length)- (strlen(first_word->word)+1);
second_word= get_next_random_word(first_word);
  printf("%s ",second_word->word);
    sen_length=(sen_length)- (strlen(second_word->word)+1);
temp_word=second_word;
    int max=MAX_WORDS_IN_SENTENCE_GENERATION-2;
   while (second_word->word[strlen(second_word->word)-1]!='.'&&max!=0&&temp_word->prob_list!=NULL&&sen_length!=0){
        second_word= get_next_random_word(temp_word);
        temp_word=second_word;
        printf("%s ",second_word->word);
       sen_length=(sen_length)- (strlen(second_word->word)+1);

        count++;
        max--;
    }
    printf(" \n");



    return count;
}

/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
 // adding word struct pointers to the probability list for every word in the link list
int add_word_to_probability_list(WordStruct *first_word,WordStruct *second_word){
    int find=1;
    if(first_word->prob_list==NULL){
        first_word->n=1;
        first_word->prob_list=(WordProbability *) malloc(sizeof (WordProbability));
        first_word->prob_list[0].word_struct_ptr=second_word;
        first_word->prob_list[0].P_value=1;
    }else {
         find = 1;
        for (int k = 0; k < first_word->n; ++k) {
            if (strcmp(first_word->prob_list[k].word_struct_ptr->word, second_word->word) ==0) {
                first_word->prob_list[k].word_struct_ptr=second_word;
                first_word->prob_list[k].P_value++;
                find = 0;
                break;
            }
        }
        if (find == 1) {
            first_word->n++;
            first_word->prob_list = realloc(first_word->prob_list,sizeof (WordProbability)* first_word->n);
            first_word->prob_list[first_word->n - 1].word_struct_ptr = second_word;
            first_word->prob_list[first_word->n - 1].P_value = 1;
        }

    }
    return find;
  }

  /**
    * Read word from the given file. Add every unique word to the dictionary.
    * Also, at every iteration, update the prob_list of the previous word with
    * the value of the current word.
    * @param fp File pointer
    * @param words_to_read Number of words to read from file.
    *                      If value is bigger than the file's word count,
    *                      or if words_to_read == -1 than read entire file.
    * @param dictionary Empty dictionary to fill
    */
// fill the all the word in the link list with the requirements that we should do it
  void fill_dictionary(FILE *fp, int words_to_read, LinkList *dictionary){

       temp=(WordStruct **)malloc(sizeof(WordStruct*)*(words_to_read));
      Node* tempNode;
      int size;
      int to_read=words_to_read;
      fseek(fp, 0L, SEEK_END);
      size = ftell(fp);
      fseek(fp, 0L, SEEK_SET);
      char* str=(char *) malloc((size+1)*sizeof (char));
      char *token;
      int ret_val = fread(str,size,1,fp);
      assert(ret_val == 1);
      str[size]='\0';
      if(dictionary==NULL){
          printf("error \n");
          exit(EXIT_FAILURE);
      }

    // divide the file and put every single word in struct and put the struct in index
      token = strtok(str, " \t\r\n\v\f");
      int i=0;
      while( token != NULL&&to_read!=0 ) {
          temp[i]=(WordStruct *)malloc(sizeof(WordStruct));
          temp[i]->word=(char *) malloc((MAX_WORD_LENGTH)*sizeof (char ));
          temp[i]->prob_list=NULL;
          temp[i]->value=0;
          temp[i]->n=0;
          strcpy(temp[i]->word, token);
          if(dictionary->first==NULL){
              temp[i]->value=1;
              add(dictionary,temp[i]);
          }else{
               tempNode=dictionary->first;
              int found=1;
              while (tempNode!=NULL){
                  if(strcmp(tempNode->data->word,token)==0){
                      tempNode->data->value++;

                      found=0;
                      break;
                  }
                  tempNode= tempNode->next;
              }
              if (found==1){
                  temp[i]->value=1;
                  add(dictionary,temp[i]);

              }


          }

           to_read--;
          i++;
           token = strtok(NULL," \t\r\n\v\f");

      }
        // fill the probability list for every word in the link list
      Node * tempNode2=dictionary->first;
      while (tempNode2!=NULL){
          for (int j = 0; j <words_to_read ; ++j) {
              if (strcmp(tempNode2->data->word, temp[j]->word) == 0){
                  if(temp[j]->word[strlen(temp[j]->word)-1]=='.'){
                      tempNode2->data->prob_list=NULL;
                      break;
                  } else{
                      Node *temp_n;
                      if(j!=words_to_read-1) {
                          if (temp[j + 1]->prob_list == NULL &&temp[j + 1]->word[strlen(temp[j + 1]->word) - 1] != '.') {
                              for (temp_n = dictionary->first;temp_n != NULL && temp_n->data != NULL; temp_n = temp_n->next) {
                                  if (strcmp(temp_n->data->word, temp[j + 1]->word) == 0) {
                                      add_word_to_probability_list(tempNode2->data, temp_n->data);
                                      break;
                                  }
                              }
                          }else{
                          add_word_to_probability_list(tempNode2->data, temp[j + 1]);
                      }
                      }
                  }
              }
          }


          tempNode2=tempNode2->next;

      }


     free(str);
  }

 /**
  * Free the given dictionary and all of it's content from memory.
  * @param dictionary Dictionary to free
  */
//free all the nods in the link list and free the link list {did not free the word struct
// because i already free it in the array
  void free_dictionary(LinkList *dictionary){
      if(dictionary==NULL){
          exit(EXIT_FAILURE);
      }
      Node * n_temp=dictionary->first;
      Node * n_temp2;
        int x=0;
     while (n_temp!=NULL){
         x++;
         n_temp2=n_temp->next;

         free(n_temp);
         n_temp=n_temp2;
     }
     free(dictionary);
  }

                                         /**
                                          * @param argc
                                          * @param argv 1) Seed
                                          *             2) Number of sentences to generate
                                          *             3) Path to file
                                          *             4) Optional - Number of words to read
                                          */
  int main(int argc, char *argv[])
  {
      if(argc>5||argc<4){
          printf("there is error in the inputs \n");
          exit(EXIT_FAILURE);
      }
      int seed_value,sentence_num,word_num;
      seed_value = atoi(argv[1]);
      sentence_num= atoi(argv[2]);
      if (argv[4]!=NULL){
          word_num= atoi(argv[4]);
      } else{
          word_num=-1;
      }
      srand(seed_value);
     char *token;
     int size,num=0;
      LinkList* dictionary=(LinkList*) malloc(sizeof (LinkList));

      dictionary->first=NULL;
      dictionary->last=NULL;
      dictionary->size=0;
      FILE *p_fd;
      p_fd = fopen(argv[3],"r") ;
      if(p_fd == NULL){
          perror("open  file  failed\n ");
          exit(EXIT_FAILURE);
      }
      fseek(p_fd, 0L, SEEK_END);
      size = ftell(p_fd);
      fseek(p_fd, 0L, SEEK_SET);
      char* str=(char *) malloc((size+1)*sizeof (char));
      int ret_val = fread(str,size,1,p_fd);
      assert(ret_val == 1);
        str[size]='\0';

      token = strtok(str, " \t\r\n\v\f");
      while( token != NULL) {
          num++;
        token = strtok(NULL," \t\r\n\v\f");
          }
      if(word_num>num||word_num==-1){
        word_num=num;
      }
        fill_dictionary(p_fd,word_num,dictionary);




      for (int i = 0; i < sentence_num; ++i) {
          printf("Tweet %d: ",i+1);
           generate_sentence(dictionary);

      }
//free the all word struct in the array
     for (int i = 0; i <word_num; ++i) {
         free(temp[i]->word);
         if(temp[i]->prob_list!=NULL)
          free(temp[i]->prob_list);

         free(temp[i]);
     }
     free(temp);
      free_dictionary(dictionary);
      free(str);
      fclose(p_fd);

      return 0;
  }